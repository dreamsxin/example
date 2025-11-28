```conf
#user  nobody;
worker_processes  1;

error_log  logs/error.log;
error_log  logs/error.log  notice;
error_log  logs/error.log  debug;
error_log  logs/error.log  info;

#pid        logs/nginx.pid;


events {
    worker_connections  1024;
}


http {
    include       mime.types;
    default_type  application/octet-stream;

    #log_format  main  '$remote_addr - $remote_user [$time_local] "$request" '
    #                  '$status $body_bytes_sent "$http_referer" '
    #                  '"$http_user_agent" "$http_x_forwarded_for"';

    #access_log  logs/access.log  main;

    sendfile        on;
    #tcp_nopush     on;

    #keepalive_timeout  0;
    keepalive_timeout  65;

    #gzip  on;

    # 共享内存区域，用于存储后端服务器信息
    lua_shared_dict upstream_servers 10m;
    
    # 初始化定时清理任务
    init_worker_by_lua_block {
        local upstream = require "upstream"
        upstream.init_cleanup_timer()
    }
    
    upstream backend {
        server 0.0.0.1; # 占位服务器
        
        balancer_by_lua_block {
            local upstream = require "upstream"
            upstream.least_conn_balancer()
        }
    }
    
    server {
        listen 80;
        
        # 管理接口
        location /upstream/add {
            content_by_lua_block {
                local upstream = require "upstream"
                upstream.add_server()
            }
        }
        
        location /upstream/remove {
            content_by_lua_block {
                local upstream = require "upstream"
                upstream.remove_server()
            }
        }
        
        location /upstream/status {
            content_by_lua_block {
                local upstream = require "upstream"
                upstream.get_status()
            }
        }
        
        # 业务请求转发
        location / {
            proxy_pass http://backend;
            proxy_set_header Host $host;
            proxy_set_header X-Real-IP $remote_addr;
            proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
            proxy_connect_timeout 2s;
    
            # 请求完成后处理响应
            header_filter_by_lua_block {
                local upstream = require "upstream"
                upstream.handle_proxy_response()
            }
            
            # 请求完成后减少连接数
            log_by_lua_block {
                local upstream = require "upstream"
                if ngx.ctx.selected_server then
                    upstream.decrease_conn_count(ngx.ctx.selected_server)
                end
            }
        }
    }

}
```
```lua
-- upstream.lua
local cjson = require "cjson"
local balancer = require "ngx.balancer"
local ngx_shared = ngx.shared.upstream_servers

local _M = {}

-- 服务器过期时间（秒）
local SERVER_EXPIRE_TIME = 120
-- 清理间隔（秒）
local CLEANUP_INTERVAL = 10

-- Redis配置
local redis_host = "127.0.0.1"
local redis_port = 6379
local redis_db = 0
local redis_key = "upstream:servers"

-- 获取Redis连接
local function get_redis()
    local redis = require "resty.redis"
    local red = redis:new()
    red:set_timeout(1000) -- 1秒超时
    
    local ok, err = red:connect(redis_host, redis_port)
    if not ok then
        return nil, "failed to connect to redis: " .. (err or "unknown")
    end
    
    -- 选择数据库
    local ok, err = red:select(redis_db)
    if not ok then
        return nil, "failed to select redis db: " .. (err or "unknown")
    end

    return red
end

-- 刚启动时，从Redis同步服务器列表
local function sync_from_redis()
    local red, err = get_redis()
    if not red then
        ngx.log(ngx.WARN, "Failed to connect to redis for sync: ", err)
        return
    end
    
    local dict = ngx.shared.upstream_servers
    local cjson = require "cjson"

    -- 删除Redis过期服务器
    red:zremrangebyscore(redis_key, 0, ngx.now() - SERVER_EXPIRE_TIME)

    -- 从Redis获取所有服务器
    local servers = red:zremrangebyscore(redis_key, 0, -1)
    if not servers then
        ngx.log(ngx.WARN, "Failed to get servers from redis: ", err)
        red:set_keepalive(10000, 100)
        return
    end

    red:set_keepalive(10000, 100)
    
    -- 保存到共享字典
    for i, v in ipairs(servers) do
        ngx.log(ngx.DEBUG, "server: ", v)
        local host, port = v:match("^([^:]+):(%d+)$")
        -- local server_info = cjson.decode(v)
        if not host or not port then
            ngx.log(ngx.WARN, "Invalid server info: ", v)
        else
			-- 转换为数字类型（如果需要）
			port = tonumber(port)
            local server_info = {
                host = host,
                port = port,
                conn_count = 0,
                last_heartbeat = ngx.now(),
                created_time = ngx.now()
            }
            local server_key = get_server_key(host, port)
            -- 保存到共享内存
            local success, err = ngx_shared:set(server_key, cjson.encode(server_info), SERVER_EXPIRE_TIME)
            
            if not success then
                ngx.log(ngx.WARN, "Failed to add server to shared memory: ", err)
            else
                ngx.log(ngx.INFO, "Successfully added server to shared memory: ", server_key)
            end
        end
    end
end

-- 保存到Redis并同步
local function save_to_redis(host, port, last_heartbeat)
    local red, err = get_redis()
    if not red then
        ngx.log(ngx.WARN, "Failed to connect to redis: ", err)
        return false
    end
    
    local member = host .. ":" .. port

    if last_heartbeat < 0 then
        -- 从Redis删除服务器
        local ok, err = red:zrem(redis_key, member)
        if not ok then
            ngx.log(ngx.WARN, "Failed to remove server from redis: ", err)
            red:set_keepalive(10000, 100)
            return false
        end
    else 
        -- 保存到Redis
        local ok, err = red:zadd(redis_key, last_heartbeat, member)
        if not ok then
            ngx.log(ngx.WARN, "Failed to save to redis: ", err)
            red:set_keepalive(10000, 100)
            return false
        end
    end

    red:set_keepalive(10000, 100)
    return true
end

-- 获取客户端IP
local function get_client_ip()
    local headers = ngx.req.get_headers()
    local ip = headers["X-Real-IP"] or headers["X-Forwarded-For"] or ngx.var.remote_addr
    -- 处理X-Forwarded-For可能有多个IP的情况
    if type(ip) == "table" then
        ip = ip[1]
    end
    if ip and string.find(ip, ",") then
        ip = string.match(ip, "([^,]+)")
    end
    return ip
end

-- 序列化服务器键
local function get_server_key(host, port)
    return host .. ":" .. port
end

-- 添加或更新服务器
function _M.add_server()
    local args = ngx.req.get_uri_args()
    local host = args.host
    local port = tonumber(args.port)
    
    if not port then
        ngx.status = ngx.HTTP_BAD_REQUEST
        ngx.say('{"error": "port is required"}')
        return
    end
    
    -- 如果host为空，使用客户端IP
    if not host or host == "" then
        host = get_client_ip()
    end
    
    local server_key = get_server_key(host, port)
    
    -- 获取现有服务器信息或创建新的
    local server_info_str = ngx_shared:get(server_key)
    local server_info
    
    if server_info_str then
        server_info = cjson.decode(server_info_str)
        server_info.last_heartbeat = ngx.now()
        ngx.log(ngx.INFO, "Updated server heartbeat: ", server_key)
    else
        server_info = {
            host = host,
            port = port,
            conn_count = 0,
            last_heartbeat = ngx.now(),
            created_time = ngx.now()
        }
        ngx.log(ngx.INFO, "Added new server: ", server_key)
    end
    
    -- 保存到共享内存
    local success, err = ngx_shared:set(server_key, cjson.encode(server_info), SERVER_EXPIRE_TIME)
    
    if not success then
        ngx.status = ngx.HTTP_INTERNAL_SERVER_ERROR
        ngx.say('{"error": "failed to add server: ' .. err .. '"}')
        return
    end

    -- 保存到Redis并同步
    if not save_to_redis(host, port, server_info.last_heartbeat) then
        ngx.log(ngx.WARN, "Failed to save server to redis")
    end
    
    ngx.say('{"success": true, "server": "' .. server_key .. '"}')
end

-- 移除服务器
function _M.remove_server()
    local args = ngx.req.get_uri_args()
    local host = args.host
    local port = tonumber(args.port)
    
    if not port then
        ngx.status = ngx.HTTP_BAD_REQUEST
        ngx.say('{"error": "port is required"}')
        return
    end
    
    if not host or host == "" then
        host = get_client_ip()
    end
    
    local server_key = get_server_key(host, port)
    local success, err = ngx_shared:delete(server_key)
    
    if not success then
        ngx.status = ngx.HTTP_INTERNAL_SERVER_ERROR
        ngx.say('{"error": "failed to remove server: ' .. err .. '"}')
        return
    end

    -- 保存到Redis并同步
    if not save_to_redis(host, port, -1) then
        ngx.log(ngx.WARN, "Failed to remove server from redis: ", err)
    end
    
    ngx.say('{"success": true, "removed": "' .. server_key .. '"}')
end

-- 获取服务器状态
function _M.get_status()
    local servers = {}
    local keys = ngx_shared:get_keys(0)  -- 获取所有键
    
    for _, key in ipairs(keys) do
        local server_info_str = ngx_shared:get(key)
        if server_info_str then
            local server_info = cjson.decode(server_info_str)
            table.insert(servers, server_info)
        end
    end
    
    ngx.header["Content-Type"] = "application/json; charset=utf-8"
    ngx.say(cjson.encode({
        servers = servers,
        total = #servers,
        timestamp = ngx.now()
    }))
end

-- 最少连接数负载均衡
function _M.least_conn_balancer()
    local least_conn_server = nil
    local least_conn_count = math.huge
    local keys = ngx_shared:get_keys(0)
    
    -- 查找连接数最少的服务器
    for _, key in ipairs(keys) do
	ngx.log(ngx.DEBUG, "key", key)
        local server_info_str = ngx_shared:get(key)
	ngx.log(ngx.INFO, "server", server_info_str)
        if server_info_str then
            local server_info = cjson.decode(server_info_str)
            
            -- 检查服务器是否过期
            if ngx.now() - server_info.last_heartbeat <= SERVER_EXPIRE_TIME then
                if server_info.conn_count < least_conn_count then
                    least_conn_count = server_info.conn_count
                    least_conn_server = server_info
                end
            end
        end
    end
    
    if not least_conn_server then
        ngx.log(ngx.ERR, "No available servers found")
        return ngx.exit(ngx.HTTP_SERVICE_UNAVAILABLE)
    end
    
    -- 增加连接数
    least_conn_server.conn_count = least_conn_server.conn_count + 1
    local server_key = get_server_key(least_conn_server.host, least_conn_server.port)
    ngx_shared:set(server_key, cjson.encode(least_conn_server), SERVER_EXPIRE_TIME)
    
    
    -- 记录选择的服务器，用于后续减少连接数
    ngx.ctx.selected_server = server_key
   
    ngx.log(ngx.DEBUG, "server", least_conn_server.host) 
    -- 设置代理
    local ok, err = balancer.set_current_peer(least_conn_server.host, least_conn_server.port)
    if not ok then
        ngx.log(ngx.ERR, "failed to set current peer: ", err)
        -- 如果设置失败，减少连接数
        _M.decrease_conn_count(server_key)
        return ngx.exit(ngx.HTTP_SERVICE_UNAVAILABLE)
    end
end

-- 减少连接数（在请求完成后调用）
function _M.decrease_conn_count(server_key)
    local server_info_str = ngx_shared:get(server_key)
    if server_info_str then
        local server_info = cjson.decode(server_info_str)
        if server_info.conn_count > 0 then
            server_info.conn_count = server_info.conn_count - 1
            ngx_shared:set(server_key, cjson.encode(server_info), SERVER_EXPIRE_TIME)
        end
    end
end

-- 清理过期服务器
function _M.cleanup_expired_servers()
    local keys = ngx_shared:get_keys(0)
    local removed_count = 0
    
    for _, key in ipairs(keys) do
        local server_info_str = ngx_shared:get(key)
        if server_info_str then
            local server_info = cjson.decode(server_info_str)
            -- 检查服务器是否过期
            if ngx.now() - server_info.last_heartbeat > SERVER_EXPIRE_TIME then
                ngx_shared:delete(key)
                removed_count = removed_count + 1
                ngx.log(ngx.INFO, "Removed expired server: ", key)
                -- 保存到Redis并同步
                if not save_to_redis(server_info.host, server_info.port, -1) then
                    ngx.log(ngx.WARN, "Failed to remove server from redis: ", err)
                end
            end
        end
    end
    
    if removed_count > 0 then
        ngx.log(ngx.INFO, "Cleanup removed ", removed_count, " expired servers")
    end
end

-- 初始化定时清理任务
function _M.init_cleanup_timer()
    local handler
    handler = function(premature)
        if premature then
            return
        end
        
        _M.cleanup_expired_servers()
        
        -- 再次设置定时器
        ngx.timer.at(CLEANUP_INTERVAL, handler)
    end
    
    -- 启动定时器
    ngx.timer.at(CLEANUP_INTERVAL, handler)
end

return _M
```
