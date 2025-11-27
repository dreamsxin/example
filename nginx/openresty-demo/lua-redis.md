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
local _M = {}

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

-- 从Redis同步服务器列表
local function sync_from_redis()
    local red, err = get_redis()
    if not red then
        ngx.log(ngx.WARN, "Failed to connect to redis for sync: ", err)
        return
    end
    
    local dict = ngx.shared.upstream_servers
    local cjson = require "cjson"
    
    -- 获取Redis中的所有服务器
    local servers, err = red:hgetall(redis_key)
    if not servers then
        ngx.log(ngx.WARN, "Failed to get servers from redis: ", err)
        red:set_keepalive(10000, 100)
        return
    end
    
    -- 清空本地共享内存
    dict:flush_all()
    
    -- 同步服务器信息
    for i = 1, #servers, 2 do
        local server_key = servers[i]
        local server_data_str = servers[i + 1]
        
        if server_key and server_data_str then
            dict:set(server_key, server_data_str)
        end
    end
    
    red:set_keepalive(10000, 100)
    ngx.log(ngx.INFO, "Successfully synced ", #servers/2, " servers from redis")
end

-- 保存到Redis并同步
local function save_to_redis_and_sync(server_key, server_data_str)
    local red, err = get_redis()
    if not red then
        ngx.log(ngx.WARN, "Failed to connect to redis: ", err)
        return false
    end
    
    -- 保存到Redis
    local ok, err = red:hset(redis_key, server_key, server_data_str)
    if not ok then
        ngx.log(ngx.WARN, "Failed to save to redis: ", err)
        red:set_keepalive(10000, 100)
        return false
    end
    
    -- 发布同步消息
    local ok, err = red:publish("upstream:sync", server_key)
    if not ok then
        ngx.log(ngx.WARN, "Failed to publish sync message: ", err)
    end
    
    red:set_keepalive(10000, 100)
    return true
end

-- 获取客户端IP
local function get_client_ip()
    local headers = ngx.req.get_headers()
    local ip = headers["X-Real-IP"] or headers["x-real-ip"] or headers["X-Forwarded-For"] or headers["x-forwarded-for"] or ngx.var.remote_addr
    return ip
end

-- 记录服务器失败
local function record_server_failure(server_key)
    local dict = ngx.shared.upstream_servers
    local value = dict:get(server_key)
    
    if value then
        local cjson = require "cjson"
        local server_data = cjson.decode(value)
        
        -- 增加失败次数
        server_data.fails = server_data.fails + 1
        server_data.last_fail = ngx.time()
        
        -- 更新到共享内存
        local data_str = cjson.encode(server_data)
        dict:set(server_key, data_str)
        
        -- 同步到Redis
        save_to_redis_and_sync(server_key, data_str)
        
        ngx.log(ngx.WARN, "Recorded failure for server: ", server_key, " fails: ", server_data.fails)
    end
end

-- 添加服务器
function _M.add_server()
    local args = ngx.req.get_uri_args()
    local port = tonumber(args.port)
    local host = args.host or get_client_ip()
    
    if not port or port <= 0 or port > 65535 then
        ngx.status = 400
        ngx.say("Invalid port number")
        return
    end
    
    -- 验证IP格式
    if not host:match("^%d+%.%d+%.%d+%.%d+$") then
        ngx.status = 400
        ngx.say("Invalid host IP format")
        return
    end
    
    local server_key = host .. ":" .. port
    local dict = ngx.shared.upstream_servers
    
    -- 检查服务器是否已存在
    local server_info = dict:get(server_key)
    if server_info then
        ngx.say("Server already exists: " .. server_key)
        return
    end
    
    -- 初始化服务器信息
    local server_data = {
        host = host,
        port = port,
        connections = 0,
        fails = 0,
        last_fail = 0,
        weight = 1,
        added_time = ngx.time(),
        last_success = 0,
        total_requests = 0,
        success_rate = 100
    }
    
    -- 序列化数据
    local cjson = require "cjson"
    local data_str = cjson.encode(server_data)
    
    -- 存储到共享内存
    dict:set(server_key, data_str)
    
    -- 存储到Redis并同步
    save_to_redis_and_sync(server_key, data_str)
    
    ngx.say("Server added successfully: " .. server_key)
end

-- 移除服务器
function _M.remove_server()
    local args = ngx.req.get_uri_args()
    local port = tonumber(args.port)
    local host = args.host or get_client_ip()
    
    if not port then
        ngx.status = 400
        ngx.say("Port parameter is required")
        return
    end
    
    local server_key = host .. ":" .. port
    local dict = ngx.shared.upstream_servers
    
    -- 检查服务器是否存在
    local server_info = dict:get(server_key)
    if not server_info then
        ngx.status = 404
        ngx.say("Server not found: " .. server_key)
        return
    end
    
    -- 从共享内存移除
    dict:delete(server_key)
    
    -- 从Redis移除
    local red, err = get_redis()
    if red then
        local ok, err = red:hdel(redis_key, server_key)
        if not ok then
            ngx.log(ngx.WARN, "Failed to remove from redis: ", err)
        end
        
        -- 发布移除消息
        red:publish("upstream:remove", server_key)
        red:set_keepalive(10000, 100)
    end
    
    ngx.say("Server removed successfully: " .. server_key)
end

-- 获取状态
function _M.get_status()
    local dict = ngx.shared.upstream_servers
    local servers = {}
    
    -- 获取所有服务器
    local keys = dict:get_keys(0)
    local cjson = require "cjson"
    
    for _, key in ipairs(keys) do
        local value = dict:get(key)
        if value then
            local server_data = cjson.decode(value)
            servers[key] = server_data
        end
    end
    
    local status = {
        total_servers = #keys,
        servers = servers,
        timestamp = ngx.time()
    }
    
    ngx.header.content_type = "application/json"
    ngx.say(cjson.encode(status))
end

-- 最少连接数负载均衡算法
function _M.least_conn_balancer()
    local dict = ngx.shared.upstream_servers
    local keys = dict:get_keys(0)
    
    if #keys == 0 then
        ngx.log(ngx.ERR, "No upstream servers available")
        return ngx.exit(502)
    end
    
    local cjson = require "cjson"
    local selected_server = nil
    local min_connections = math.huge
    
    -- 找到连接数最少的服务器
    for _, key in ipairs(keys) do
        local value = dict:get(key)
        if value then
            local server_data = cjson.decode(value)
            
            -- 检查服务器是否健康（失败次数小于3次且距离上次失败超过10秒）
            if server_data.fails < 3 and (ngx.time() - server_data.last_fail) > 10 then
                if server_data.connections < min_connections then
                    min_connections = server_data.connections
                    selected_server = server_data
                    selected_server.key = key
                end
            end
        end
    end
    
    if not selected_server then
        ngx.log(ngx.ERR, "No healthy upstream servers available")
        return ngx.exit(502)
    end
    
    -- 增加连接数和总请求数
    selected_server.connections = selected_server.connections + 1
    selected_server.total_requests = selected_server.total_requests + 1
    
    -- 更新到共享内存
    local data_str = cjson.encode(selected_server)
    dict:set(selected_server.key, data_str)
    
    -- 保存选中的服务器信息
    ngx.ctx.selected_server = selected_server.key
    
    -- 设置后端服务器
    local ok, err = ngx.balancer.set_current_peer(selected_server.host, selected_server.port)
    if not ok then
        ngx.log(ngx.ERR, "Failed to set current peer: ", err)
        -- 记录失败
        record_server_failure(selected_server.key)
        return ngx.exit(502)
    end
end

-- 减少连接数
function _M.decrease_conn_count(server_key)
    local dict = ngx.shared.upstream_servers
    local value = dict:get(server_key)
    
    if value then
        local cjson = require "cjson"
        local server_data = cjson.decode(value)
        
        if server_data.connections > 0 then
            server_data.connections = server_data.connections - 1
            server_data.last_success = ngx.time()
            
            -- 计算成功率
            if server_data.total_requests > 0 then
                server_data.success_rate = math.floor(((server_data.total_requests - server_data.fails) / server_data.total_requests) * 100)
            end
            
            local data_str = cjson.encode(server_data)
            dict:set(server_key, data_str)
        end
    end
end

-- 处理代理响应
function _M.handle_proxy_response()
    local server_key = ngx.ctx.selected_server
    if not server_key then
        return
    end
    
    local status = ngx.status
    
    -- 如果响应状态码是5xx，记录为失败
    if status >= 500 and status < 600 then
        record_server_failure(server_key)
    end
end

-- 健康检查定时器
function _M.init_cleanup_timer()
    local function check_health(premature)
        if premature then
            return
        end
        
        -- 首先从Redis同步数据
        sync_from_redis()
        
        local dict = ngx.shared.upstream_servers
        local keys = dict:get_keys(0)
        local cjson = require "cjson"
        local now = ngx.time()
        
        for _, key in ipairs(keys) do
            local value = dict:get(key)
            if value then
                local server_data = cjson.decode(value)
                
                -- 清理长时间失败的服务器
                if server_data.fails >= 5 and (now - server_data.last_fail) > 300 then
                    dict:delete(key)
                    ngx.log(ngx.INFO, "Removed unhealthy server: ", key)
                    
                    -- 从Redis中移除
                    local red, err = get_redis()
                    if red then
                        red:hdel(redis_key, key)
                        red:set_keepalive(10000, 100)
                    end
                end
                
                -- 重置长时间无请求的服务器失败次数
                if server_data.fails > 0 and (now - server_data.last_fail) > 600 then
                    server_data.fails = 0
                    local data_str = cjson.encode(server_data)
                    dict:set(key, data_str)
                end
            end
        end
    end
    
    -- 每30秒执行一次健康检查
    local ok, err = ngx.timer.every(30, check_health)
    if not ok then
        ngx.log(ngx.ERR, "Failed to create health check timer: ", err)
    end
    
    -- 订阅Redis消息
    local function subscribe_to_redis()
        local red, err = get_redis()
        if not red then
            ngx.log(ngx.ERR, "Failed to connect to redis for subscription: ", err)
            return
        end
        
        local function handle_message(message, channel)
            if channel == "upstream:sync" then
                sync_from_redis()
            elseif channel == "upstream:remove" then
                local dict = ngx.shared.upstream_servers
                dict:delete(message)
            end
        end
        
        -- 订阅相关频道
        red:subscribe("upstream:sync", "upstream:remove")
        
        while true do
            local res, err = red:read_reply()
            if res then
                handle_message(res[3], res[2])
            end
        end
    end
    
    -- 启动订阅线程
    ngx.timer.at(0, subscribe_to_redis)
end

return _M
```
