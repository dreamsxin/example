# OpenResty

```shell
sudo apt-get install build-essential
sudo apt-get install libreadline-dev libncurses5-dev libpcre3-dev libssl-dev perl

wget https://openresty.org/download/openresty-1.11.2.2.tar.gz
wget https://openresty.org/download/openresty-1.11.2.2.tar.gz.asc
gpg --keyserver pgpkeys.mit.edu --recv-key A0E98066
gpg openresty-1.11.2.2.tar.gz.asc

tar -xvf openresty-1.11.2.2.tar.gz
cd openresty-1.11.2.2
./configure -j2 --with-pcre-jit --with-ipv6
make -j2
sudo make install

sudo ufw allow http
sudo ufw status

sudo /usr/local/openresty/bin/openresty -s quit
```

# 完整的 Lua 模块代码和 Nginx 配置

## 1. 完整的 Lua 模块代码 (zset_upstream.lua)

```lua
-- lua_modules/zset_upstream.lua
local redis = require "resty.redis"
local cjson = require "cjson.safe"

local _M = {}
local mt = { __index = _M }

-- Redis 配置
local REDIS_CONFIG = {
    host = "127.0.0.1",
    port = 6379,
    timeout = 1000,
    pool_size = 100
}

-- Redis 键定义
local UPSTREAM_ZSET_KEY = "upstream:servers:zset"      -- ZSET: server -> connections
local UPSTREAM_METADATA_KEY = "upstream:servers:meta"  -- HASH: server -> metadata(JSON)

function _M.new()
    local self = {
        redis_conn = nil,
        upstream_cache = nil,
        cache_ttl = 3 -- 缓存3秒
    }
    return setmetatable(self, mt)
end

-- 连接 Redis
function _M:connect_redis()
    if self.redis_conn then
        return true
    end
    
    local red = redis:new()
    red:set_timeout(REDIS_CONFIG.timeout)
    
    local ok, err = red:connect(REDIS_CONFIG.host, REDIS_CONFIG.port)
    if not ok then
        ngx.log(ngx.ERR, "Failed to connect to Redis: ", err)
        return false
    end
    
    self.redis_conn = red
    return true
end

-- 关闭 Redis 连接
function _M:close_redis()
    if self.redis_conn then
        local ok, err = self.redis_conn:set_keepalive(REDIS_CONFIG.pool_size, REDIS_CONFIG.timeout)
        if not ok then
            ngx.log(ngx.ERR, "Failed to set keepalive: ", err)
        end
        self.redis_conn = nil
    end
end

-- 构建服务器地址
function _M:build_server_addr(host, port)
    if not host or host == "" then
        -- 使用客户端 IP
        host = ngx.var.remote_addr
        ngx.log(ngx.INFO, "Using client IP as host: " .. host)
    end
    
    if not port then
        return nil, "Port is required"
    end
    
    if port < 1 or port > 65535 then
        return nil, "Invalid port number: " .. tostring(port)
    end
    
    return host .. ":" .. port, host
end

-- 解析服务器地址
function _M:parse_server_addr(server_addr)
    local host, port = server_addr:match("^([^:]+):(%d+)$")
    if host and port then
        return host, tonumber(port)
    end
    return nil, nil
end

-- 获取所有启用的服务器（带缓存）
function _M:get_enabled_servers()
    -- 检查缓存
    if self.upstream_cache and ngx.now() - self.upstream_cache.timestamp < self.cache_ttl then
        return self.upstream_cache.servers
    end
    
    if not self:connect_redis() then
        return nil
    end
    
    -- 获取所有服务器的元数据
    local servers_meta, err = self.redis_conn:hgetall(UPSTREAM_METADATA_KEY)
    if not servers_meta or #servers_meta == 0 then
        ngx.log(ngx.WARN, "No upstream servers found in Redis")
        self:close_redis()
        return nil
    end
    
    local enabled_servers = {}
    
    -- 解析元数据，过滤启用的服务器
    for i = 1, #servers_meta, 2 do
        local server_addr = servers_meta[i]
        local meta_json = servers_meta[i + 1]
        
        if meta_json and meta_json ~= ngx.null then
            local meta = cjson.decode(meta_json)
            if meta and meta.enabled ~= false then
                local host, port = self:parse_server_addr(server_addr)
                if host and port then
                    table.insert(enabled_servers, {
                        addr = server_addr,
                        host = host,
                        port = port,
                        weight = meta.weight or 1,
                        max_conns = meta.max_conns or 1000
                    })
                end
            end
        end
    end
    
    -- 更新缓存
    self.upstream_cache = {
        servers = enabled_servers,
        timestamp = ngx.now()
    }
    
    self:close_redis()
    return enabled_servers
end

-- 获取最少连接的服务器
function _M:least_conn_balancer()
    local enabled_servers = self:get_enabled_servers()
    if not enabled_servers or #enabled_servers == 0 then
        ngx.log(ngx.ERR, "No enabled upstream servers found")
        return nil
    end
    
    if not self:connect_redis() then
        return nil
    end
    
    local best_server = nil
    local min_connections = math.huge
    
    -- 使用 ZRANGE 获取连接数最少的几个服务器
    -- 这里获取前5个，然后从中选择可用的
    local candidates, err = self.redis_conn:zrange(UPSTREAM_ZSET_KEY, 0, 4, "WITHSCORES")
    self:close_redis()
    
    if not candidates or #candidates == 0 then
        ngx.log(ngx.ERR, "No servers in ZSET")
        return nil
    end
    
    -- 构建可用服务器查找表
    local enabled_lookup = {}
    for _, server in ipairs(enabled_servers) do
        enabled_lookup[server.addr] = server
    end
    
    -- 从候选中选择第一个可用的服务器
    for i = 1, #candidates, 2 do
        local server_addr = candidates[i]
        local connections = tonumber(candidates[i + 1]) or 0
        
        if enabled_lookup[server_addr] then
            local server = enabled_lookup[server_addr]
            
            -- 检查连接数限制
            if connections < (server.max_conns or 1000) then
                best_server = server
                min_connections = connections
                break
            else
                ngx.log(ngx.WARN, "Server ", server_addr, " reached max connections: ", connections)
            end
        end
    end
    
    if best_server then
        ngx.log(ngx.INFO, "Selected server: ", best_server.addr, " with ", min_connections, " connections")
    else
        ngx.log(ngx.ERR, "No available server found from candidates")
    end
    
    return best_server
end

-- 增加服务器连接数
function _M:increment_connections(server_addr)
    if not self:connect_redis() then
        return false
    end
    
    -- 使用 ZINCRBY 原子增加连接数
    local new_score, err = self.redis_conn:zincrby(UPSTREAM_ZSET_KEY, 1, server_addr)
    self:close_redis()
    
    if not new_score then
        ngx.log(ngx.ERR, "Failed to increment connections for ", server_addr, ": ", err)
        return false
    end
    
    ngx.log(ngx.DEBUG, "Incremented connections for ", server_addr, " to ", new_score)
    return true
end

-- 减少服务器连接数
function _M:decrement_connections(server_addr)
    if not self:connect_redis() then
        return false
    end
    
    -- 使用 ZINCRBY 原子减少连接数
    local new_score, err = self.redis_conn:zincrby(UPSTREAM_ZSET_KEY, -1, server_addr)
    if new_score and tonumber(new_score) < 0 then
        -- 如果连接数小于0，重置为0
        self.redis_conn:zadd(UPSTREAM_ZSET_KEY, 0, server_addr)
        new_score = 0
    end
    
    self:close_redis()
    
    if not new_score then
        ngx.log(ngx.ERR, "Failed to decrement connections for ", server_addr, ": ", err)
        return false
    end
    
    ngx.log(ngx.DEBUG, "Decremented connections for ", server_addr, " to ", new_score)
    return true
end

-- 添加服务器到 upstream
function _M:add_server(host, port, weight, max_conns)
    local server_addr, actual_host = self:build_server_addr(host, port)
    if not server_addr then
        return false, actual_host -- actual_host contains error message
    end
    
    weight = weight or 1
    max_conns = max_conns or 1000
    
    if not self:connect_redis() then
        return false, "Failed to connect to Redis"
    end
    
    -- 检查是否已存在
    local exists, err = self.redis_conn:zscore(UPSTREAM_ZSET_KEY, server_addr)
    if exists and exists ~= ngx.null then
        self:close_redis()
        return false, "Server already exists: " .. server_addr
    end
    
    -- 添加到 ZSET，初始连接数为0
    local zadd_ok, zadd_err = self.redis_conn:zadd(UPSTREAM_ZSET_KEY, 0, server_addr)
    if not zadd_ok then
        self:close_redis()
        return false, "Failed to add server to ZSET: " .. (zadd_err or "unknown error")
    end
    
    -- 存储元数据到 HASH
    local meta = {
        host = actual_host,
        port = port,
        weight = weight,
        max_conns = max_conns,
        enabled = true,
        created_at = ngx.now()
    }
    
    local hset_ok, hset_err = self.redis_conn:hset(UPSTREAM_METADATA_KEY, server_addr, cjson.encode(meta))
    self:close_redis()
    
    if not hset_ok then
        -- 回滚：从 ZSET 中移除
        self:connect_redis()
        self.redis_conn:zrem(UPSTREAM_ZSET_KEY, server_addr)
        self:close_redis()
        return false, "Failed to add server metadata: " .. (hset_err or "unknown error")
    end
    
    -- 清除缓存
    self.upstream_cache = nil
    
    ngx.log(ngx.INFO, "Server added: ", server_addr, " (host: ", actual_host, ", port: ", port, ")")
    return true, server_addr
end

-- 从 upstream 移除服务器
function _M:remove_server(host, port)
    local server_addr, actual_host = self:build_server_addr(host, port)
    if not server_addr then
        return false, actual_host -- actual_host contains error message
    end
    
    if not self:connect_redis() then
        return false, "Failed to connect to Redis"
    end
    
    -- 检查是否存在
    local exists, err = self.redis_conn:zscore(UPSTREAM_ZSET_KEY, server_addr)
    if not exists or exists == ngx.null then
        self:close_redis()
        return false, "Server not found: " .. server_addr
    end
    
    -- 从 ZSET 移除
    local zrem_ok, zrem_err = self.redis_conn:zrem(UPSTREAM_ZSET_KEY, server_addr)
    if not zrem_ok then
        self:close_redis()
        return false, "Failed to remove server from ZSET: " .. (zrem_err or "unknown error")
    end
    
    -- 从元数据 HASH 移除
    local hdel_ok, hdel_err = self.redis_conn:hdel(UPSTREAM_METADATA_KEY, server_addr)
    self:close_redis()
    
    if not hdel_ok then
        ngx.log(ngx.ERR, "Failed to remove server metadata: ", hdel_err)
        return false, "Failed to remove server metadata: " .. (hdel_err or "unknown error")
    end
    
    -- 清除缓存
    self.upstream_cache = nil
    
    ngx.log(ngx.INFO, "Server removed: ", server_addr)
    return true, server_addr
end

-- 启用/禁用服务器
function _M:set_server_status(host, port, enabled)
    local server_addr, actual_host = self:build_server_addr(host, port)
    if not server_addr then
        return false, actual_host -- actual_host contains error message
    end
    
    if not self:connect_redis() then
        return false, "Failed to connect to Redis"
    end
    
    -- 获取当前元数据
    local meta_json, err = self.redis_conn:hget(UPSTREAM_METADATA_KEY, server_addr)
    if not meta_json or meta_json == ngx.null then
        self:close_redis()
        return false, "Server not found: " .. server_addr
    end
    
    local meta = cjson.decode(meta_json)
    if not meta then
        self:close_redis()
        return false, "Failed to decode server metadata"
    end
    
    -- 更新启用状态
    meta.enabled = enabled
    meta.updated_at = ngx.now()
    
    local hset_ok, hset_err = self.redis_conn:hset(UPSTREAM_METADATA_KEY, server_addr, cjson.encode(meta))
    self:close_redis()
    
    if not hset_ok then
        return false, "Failed to update server status: " .. (hset_err or "unknown error")
    end
    
    -- 清除缓存
    self.upstream_cache = nil
    
    ngx.log(ngx.INFO, "Server ", server_addr, " ", enabled and "enabled" or "disabled")
    return true, server_addr
end

-- 获取所有服务器状态
function _M:get_all_servers()
    if not self:connect_redis() then
        return nil, "Failed to connect to Redis"
    end
    
    -- 获取 ZSET 中所有服务器的连接数
    local servers_with_scores, err = self.redis_conn:zrange(UPSTREAM_ZSET_KEY, 0, -1, "WITHSCORES")
    if not servers_with_scores then
        self:close_redis()
        return nil, "Failed to get servers from ZSET: " .. (err or "unknown error")
    end
    
    -- 获取所有服务器的元数据
    local all_meta, err = self.redis_conn:hgetall(UPSTREAM_METADATA_KEY)
    self:close_redis()
    
    if not all_meta or #all_meta == 0 then
        return {}, nil -- 返回空表而不是nil
    end
    
    -- 构建元数据查找表
    local meta_lookup = {}
    for i = 1, #all_meta, 2 do
        local server_addr = all_meta[i]
        local meta_json = all_meta[i + 1]
        if meta_json and meta_json ~= ngx.null then
            meta_lookup[server_addr] = cjson.decode(meta_json)
        end
    end
    
    -- 构建结果
    local result = {}
    for i = 1, #servers_with_scores, 2 do
        local server_addr = servers_with_scores[i]
        local connections = tonumber(servers_with_scores[i + 1]) or 0
        local meta = meta_lookup[server_addr]
        
        if meta then
            local host, port = self:parse_server_addr(server_addr)
            table.insert(result, {
                addr = server_addr,
                host = host,
                port = port,
                connections = connections,
                weight = meta.weight,
                max_conns = meta.max_conns,
                enabled = meta.enabled,
                created_at = meta.created_at,
                updated_at = meta.updated_at
            })
        end
    end
    
    return result, nil
end

-- 重置所有服务器的连接数
function _M:reset_connections()
    if not self:connect_redis() then
        return false, "Failed to connect to Redis"
    end
    
    -- 获取所有服务器
    local servers, err = self.redis_conn:zrange(UPSTREAM_ZSET_KEY, 0, -1)
    if not servers then
        self:close_redis()
        return false, "Failed to get servers: " .. (err or "unknown error")
    end
    
    -- 重置所有服务器的连接数为0
    for _, server_addr in ipairs(servers) do
        local ok, err = self.redis_conn:zadd(UPSTREAM_ZSET_KEY, 0, server_addr)
        if not ok then
            ngx.log(ngx.ERR, "Failed to reset connections for ", server_addr, ": ", err)
        end
    end
    
    self:close_redis()
    
    ngx.log(ngx.INFO, "Reset connections for ", #servers, " servers")
    return true, #servers
end

return _M
```

## 2. 完整的 Nginx 配置

```nginx
# nginx.conf
worker_processes auto;
error_log logs/error.log info;

events {
    worker_connections 1024;
}

http {
    lua_package_path "/path/to/lua_modules/?.lua;;";
    lua_shared_dict upstream_cache 10m;
    
    # 初始化 Redis 连接池
    init_worker_by_lua_block {
        require "resty.redis"
    }
    
    # 动态 upstream 定义
    upstream dynamic_backend {
        server 0.0.0.0; # 占位符
        
        balancer_by_lua_block {
            local zset_upstream = require "zset_upstream"
            local balancer = zset_upstream.new()
            
            -- 选择最少连接的服务器
            local server = balancer:least_conn_balancer()
            if not server then
                ngx.log(ngx.ERR, "No available upstream server found")
                ngx.exit(502)
                return
            end
            
            -- 设置选中的后端服务器
            ngx.var.backend_server = server.addr
            
            -- 增加连接数
            local ok = balancer:increment_connections(server.addr)
            if not ok then
                ngx.log(ngx.ERR, "Failed to increment connections for ", server.addr)
                ngx.exit(500)
                return
            end
            
            -- 存储服务器信息用于后续清理
            ngx.ctx.selected_server = server.addr
            ngx.ctx.server_host = server.host
            ngx.ctx.server_port = server.port
            
            ngx.log(ngx.INFO, "Balancer selected: ", server.addr, " with weight ", server.weight)
        }
    }
    
    server {
        listen 80;
        server_name localhost;
        
        # 设置响应头为 JSON 格式
        more_set_headers "Content-Type: application/json; charset=utf-8";
        
        # 代理到动态后端
        location / {
            set $backend_server "";
            proxy_pass http://$backend_server;
            proxy_set_header Host $host;
            proxy_set_header X-Real-IP $remote_addr;
            proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
            proxy_set_header X-Forwarded-Proto $scheme;
            proxy_connect_timeout 5s;
            proxy_read_timeout 30s;
            
            # 请求完成后减少连接数
            log_by_lua_block {
                if ngx.ctx.selected_server then
                    local zset_upstream = require "zset_upstream"
                    local balancer = zset_upstream.new()
                    local ok, err = balancer:decrement_connections(ngx.ctx.selected_server)
                    if not ok then
                        ngx.log(ngx.ERR, "Failed to decrement connections for ", ngx.ctx.selected_server, ": ", err)
                    end
                end
            }
        }
        
        # 管理接口：查看所有服务器状态
        location /upstream/status {
            content_by_lua_block {
                local zset_upstream = require "zset_upstream"
                local cjson = require "cjson.safe"
                local balancer = zset_upstream.new()
                
                local servers, err = balancer:get_all_servers()
                if not servers then
                    ngx.status = 500
                    ngx.say(cjson.encode({
                        code = 500,
                        message = "Failed to get server status: " .. (err or "unknown error")
                    }))
                    return
                end
                
                ngx.say(cjson.encode({
                    code = 200,
                    message = "Success",
                    data = servers
                }))
            }
        }
        
        # 管理接口：添加服务器
        location /upstream/add {
            content_by_lua_block {
                ngx.req.read_body()
                local args, err = ngx.req.get_post_args()
                
                if not args then
                    ngx.status = 400
                    ngx.say('{"code": 400, "message": "Failed to parse request body"}')
                    return
                end
                
                local host = args["host"]
                local port = tonumber(args["port"])
                local weight = tonumber(args["weight"]) or 1
                local max_conns = tonumber(args["max_conns"]) or 1000
                
                -- 参数验证
                if not port then
                    ngx.status = 400
                    ngx.say('{"code": 400, "message": "Missing required parameter: port"}')
                    return
                end
                
                if port < 1 or port > 65535 then
                    ngx.status = 400
                    ngx.say('{"code": 400, "message": "Invalid port number: ' .. port .. '"}')
                    return
                end
                
                local zset_upstream = require "zset_upstream"
                local balancer = zset_upstream.new()
                
                local ok, result = balancer:add_server(host, port, weight, max_conns)
                if ok then
                    ngx.say('{"code": 200, "message": "Server added successfully", "data": {')
                    ngx.say('  "addr": "' .. result .. '",')
                    ngx.say('  "host": "' .. (host or ngx.var.remote_addr) .. '",')
                    ngx.say('  "port": ' .. port .. ',')
                    ngx.say('  "weight": ' .. weight .. ',')
                    ngx.say('  "max_conns": ' .. max_conns .. ',')
                    ngx.say('  "enabled": true')
                    ngx.say('}}')
                else
                    ngx.status = 400
                    ngx.say('{"code": 400, "message": "' .. (result or "Failed to add server") .. '"}')
                end
            }
        }
        
        # 管理接口：移除服务器
        location /upstream/remove {
            content_by_lua_block {
                ngx.req.read_body()
                local args, err = ngx.req.get_post_args()
                
                if not args then
                    ngx.status = 400
                    ngx.say('{"code": 400, "message": "Failed to parse request body"}')
                    return
                end
                
                local host = args["host"]
                local port = tonumber(args["port"])
                
                -- 参数验证
                if not port then
                    ngx.status = 400
                    ngx.say('{"code": 400, "message": "Missing required parameter: port"}')
                    return
                end
                
                if port < 1 or port > 65535 then
                    ngx.status = 400
                    ngx.say('{"code": 400, "message": "Invalid port number: ' .. port .. '"}')
                    return
                end
                
                local zset_upstream = require "zset_upstream"
                local balancer = zset_upstream.new()
                
                local ok, result = balancer:remove_server(host, port)
                if ok then
                    ngx.say('{"code": 200, "message": "Server removed successfully", "data": {')
                    ngx.say('  "addr": "' .. result .. '"')
                    ngx.say('}}')
                else
                    ngx.status = 404
                    ngx.say('{"code": 404, "message": "' .. (result or "Failed to remove server") .. '"}')
                end
            }
        }
        
        # 管理接口：启用/禁用服务器
        location /upstream/toggle {
            content_by_lua_block {
                ngx.req.read_body()
                local args, err = ngx.req.get_post_args()
                
                if not args then
                    ngx.status = 400
                    ngx.say('{"code": 400, "message": "Failed to parse request body"}')
                    return
                end
                
                local host = args["host"]
                local port = tonumber(args["port"])
                local action = args["action"]
                
                -- 参数验证
                if not port then
                    ngx.status = 400
                    ngx.say('{"code": 400, "message": "Missing required parameter: port"}')
                    return
                end
                
                if port < 1 or port > 65535 then
                    ngx.status = 400
                    ngx.say('{"code": 400, "message": "Invalid port number: ' .. port .. '"}')
                    return
                end
                
                if action ~= "enable" and action ~= "disable" then
                    ngx.status = 400
                    ngx.say('{"code": 400, "message": "Invalid action. Must be enable or disable"}')
                    return
                end
                
                local enabled = (action == "enable")
                local zset_upstream = require "zset_upstream"
                local balancer = zset_upstream.new()
                
                local ok, result = balancer:set_server_status(host, port, enabled)
                if ok then
                    local status_msg = enabled and "enabled" or "disabled"
                    ngx.say('{"code": 200, "message": "Server ' .. status_msg .. ' successfully", "data": {')
                    ngx.say('  "addr": "' .. result .. '",')
                    ngx.say('  "enabled": ' .. tostring(enabled))
                    ngx.say('}}')
                else
                    ngx.status = 404
                    ngx.say('{"code": 404, "message": "' .. (result or "Failed to update server status") .. '"}')
                end
            }
        }
        
        # 管理接口：重置连接数
        location /upstream/reset {
            content_by_lua_block {
                local zset_upstream = require "zset_upstream"
                local balancer = zset_upstream.new()
                
                local ok, result = balancer:reset_connections()
                if ok then
                    ngx.say('{"code": 200, "message": "All connection counts reset to zero", "data": {')
                    ngx.say('  "servers_reset": ' .. result)
                    ngx.say('}}')
                else
                    ngx.status = 500
                    ngx.say('{"code": 500, "message": "' .. (result or "Failed to reset connections") .. '"}')
                end
            }
        }
        
        # 健康检查接口
        location /upstream/health {
            content_by_lua_block {
                local zset_upstream = require "zset_upstream"
                local cjson = require "cjson.safe"
                local balancer = zset_upstream.new()
                
                local servers, err = balancer:get_all_servers()
                if not servers then
                    ngx.status = 503
                    ngx.say(cjson.encode({
                        code = 503,
                        message = "No upstream servers available",
                        status = "unhealthy"
                    }))
                    return
                end
                
                local enabled_servers = 0
                local total_servers = #servers
                
                for _, server in ipairs(servers) do
                    if server.enabled then
                        enabled_servers = enabled_servers + 1
                    end
                end
                
                local status = (enabled_servers > 0) and "healthy" or "unhealthy"
                
                ngx.say(cjson.encode({
                    code = 200,
                    message = "Upstream health status",
                    status = status,
                    data = {
                        total_servers = total_servers,
                        enabled_servers = enabled_servers,
                        disabled_servers = total_servers - enabled_servers
                    }
                }))
            }
        }
        
        # 默认响应
        location / {
            if ($request_uri = "/") {
                content_by_lua_block {
                    ngx.say([[
                    <html>
                    <head><title>Dynamic Upstream Manager</title></head>
                    <body>
                        <h1>Dynamic Upstream Manager</h1>
                        <p>Available endpoints:</p>
                        <ul>
                            <li><a href="/upstream/status">GET /upstream/status</a> - 查看服务器状态</li>
                            <li>POST /upstream/add - 添加服务器</li>
                            <li>POST /upstream/remove - 移除服务器</li>
                            <li>POST /upstream/toggle - 启用/禁用服务器</li>
                            <li>POST /upstream/reset - 重置连接数</li>
                            <li><a href="/upstream/health">GET /upstream/health</a> - 健康检查</li>
                        </ul>
                    </body>
                    </html>
                    ]])
                }
            }
        }
    }
}
```

## 3. 初始化脚本

```lua
-- init_redis_zset.lua
local redis = require "resty.redis"
local cjson = require "cjson"

local red = redis:new()
red:set_timeout(5000)

local ok, err = red:connect("127.0.0.1", 6379)
if not ok then
    print("Failed to connect to Redis: ", err)
    os.exit(1)
end

-- 清空现有数据
red:del("upstream:servers:zset")
red:del("upstream:servers:meta")

-- 初始服务器列表
local servers = {
    { host = "192.168.1.10", port = 8080, weight = 1, max_conns = 1000 },
    { host = "192.168.1.11", port = 8080, weight = 2, max_conns = 2000 },
    { host = "192.168.1.12", port = 8080, weight = 1, max_conns = 1500 }
}

-- 添加到 ZSET 和 HASH
for _, server in ipairs(servers) do
    local server_addr = server.host .. ":" .. server.port
    
    -- 添加到 ZSET，初始连接数为0
    local ok, err = red:zadd("upstream:servers:zset", 0, server_addr)
    if not ok then
        print("Failed to add server to ZSET: ", server_addr, " - ", err)
    else
        print("Added server to ZSET: ", server_addr)
    end
    
    -- 添加元数据
    local meta = {
        host = server.host,
        port = server.port,
        weight = server.weight,
        max_conns = server.max_conns,
        enabled = true,
        created_at = os.time()
    }
    
    local ok, err = red:hset("upstream:servers:meta", server_addr, cjson.encode(meta))
    if not ok then
        print("Failed to add server metadata: ", server_addr, " - ", err)
    else
        print("Added server metadata: ", server_addr)
    end
end

-- 验证数据
local servers_count = red:zcard("upstream:servers:zset")
local meta_count = red:hlen("upstream:servers:meta")

print("\nInitialization completed:")
print("ZSET servers count: " .. (servers_count or 0))
print("HASH metadata count: " .. (meta_count or 0))

-- 显示当前服务器状态
print("\nCurrent server status:")
local servers, err = red:zrange("upstream:servers:zset", 0, -1, "WITHSCORES")
if servers then
    for i = 1, #servers, 2 do
        local server_addr = servers[i]
        local connections = servers[i + 1]
        local meta_json = red:hget("upstream:servers:meta", server_addr)
        if meta_json and meta_json ~= ngx.null then
            local meta = cjson.decode(meta_json)
            if meta then
                local status = meta.enabled and "✓" or "✗"
                print(string.format("  %s %s (conn: %s, weight: %d, max: %d)", 
                    status, server_addr, connections, meta.weight, meta.max_conns))
            end
        end
    end
end

red:close()
```

## 4. 部署说明

### 目录结构
```
/usr/local/openresty/
├── nginx/
│   └── conf/
│       └── nginx.conf
├── lua_modules/
│   └── zset_upstream.lua
└── scripts/
    └── init_redis_zset.lua
```

### 启动步骤

1. **启动 Redis**:
```bash
redis-server
```

2. **初始化 Redis 数据**:
```bash
/usr/local/openresty/bin/resty /usr/local/openresty/scripts/init_redis_zset.lua
```

3. **启动 OpenResty**:
```bash
/usr/local/openresty/bin/openresty -p /usr/local/openresty/nginx -c conf/nginx.conf
```

4. **测试接口**:
```bash
# 查看状态
curl http://localhost/upstream/status

# 添加服务器（使用客户端IP）
curl -X POST http://localhost/upstream/add -d "port=8080"

# 添加服务器（指定host）
curl -X POST http://localhost/upstream/add -d "host=192.168.1.20" -d "port=8080"

# 移除服务器
curl -X POST http://localhost/upstream/remove -d "host=192.168.1.20" -d "port=8080"
```

这个完整的实现提供了：
- 基于 ZSET 的最少连接负载均衡
- 动态服务器管理（添加、移除、启用/禁用）
- 完整的错误处理和参数验证
- 自动客户端 IP 检测
- 健康检查接口
- 详细的 API 文档

# 动态 Upstream 管理 API 文档 

## 概述

基于 OpenResty 和 Redis 的动态 Upstream 管理系统的 RESTful API 接口。
该系统支持最少连接负载均衡、动态服务器管理和实时状态监控。

## 认证

当前版本无需认证。

## 接口列表

### 1. 代理请求

将请求代理到后端服务器，自动使用最少连接负载均衡算法选择目标服务器。

- **URL**: `/`
- **方法**: `GET`、`POST`、`PUT`、`DELETE` 等所有 HTTP 方法
- **描述**: 所有非管理接口的请求都会被代理到后端服务器
- **负载均衡**: 自动使用最少连接算法选择服务器

**请求示例**:
```bash
# 普通请求，自动负载均衡
curl -X GET http://localhost/api/users
curl -X POST http://localhost/api/orders -d '{"item": "test"}'
```

**响应**: 后端服务器的响应

### 2. 获取所有服务器状态

获取当前所有后端服务器的详细状态信息。

- **URL**: `/upstream/status`
- **方法**: `GET`
- **描述**: 返回所有服务器的连接数、权重、状态等信息

**响应示例**:
```json
[
  {
    "addr": "192.168.1.10:8080",
    "host": "192.168.1.10",
    "port": 8080,
    "connections": 15,
    "weight": 1,
    "max_conns": 1000,
    "enabled": true,
    "created_at": 1633046400,
    "updated_at": 1633046500
  },
  {
    "addr": "192.168.1.11:8080",
    "host": "192.168.1.11",
    "port": 8080,
    "connections": 8,
    "weight": 2,
    "max_conns": 2000,
    "enabled": true,
    "created_at": 1633046400,
    "updated_at": 1633046600
  }
]
```

**字段说明**:
- `addr`: 服务器完整地址 (host:port)
- `host`: 服务器主机地址
- `port`: 服务器端口
- `connections`: 当前连接数
- `weight`: 权重值
- `max_conns`: 最大连接数限制
- `enabled`: 是否启用
- `created_at`: 创建时间戳
- `updated_at`: 最后更新时间戳

### 3. 添加服务器

向负载均衡池中添加新的后端服务器。

- **URL**: `/upstream/add`
- **方法**: `POST`
- **Content-Type**: `application/x-www-form-urlencoded`

**请求参数**:
| 参数名 | 类型 | 必需 | 默认值 | 描述 |
|--------|------|------|--------|------|
| `host` | string | 否 | 客户端IP | 服务器主机地址，为空时自动使用请求来源 IP |
| `port` | integer | 是 | - | 服务器端口 |
| `weight` | integer | 否 | 1 | 服务器权重，数值越大分配的请求越多 |
| `max_conns` | integer | 否 | 1000 | 最大连接数限制 |

**特殊说明**:
- 如果 `host` 参数为空或未提供，系统将自动使用请求来源的客户端 IP 地址 (`ngx.var.remote_addr`)
- 这在使用客户端自动注册时特别有用

**请求示例**:
```bash
# 1. 明确指定 host 和 port
curl -X POST http://localhost/upstream/add \
  -d "host=192.168.1.14" \
  -d "port=8080" \
  -d "weight=2" \
  -d "max_conns=2000"

# 2. 自动使用客户端 IP (host 为空)
# 假设客户端 IP 是 192.168.1.100，将注册为 192.168.1.100:8080
curl -X POST http://localhost/upstream/add \
  -d "port=8080" \
  -d "weight=1"

# 3. 使用域名
curl -X POST http://localhost/upstream/add \
  -d "host=backend.example.com" \
  -d "port=80" \
  -d "weight=3"
```

**成功响应**:
```json
{
  "code": 200,
  "message": "Server added successfully",
  "data": {
    "addr": "192.168.1.14:8080",
    "host": "192.168.1.14",
    "port": 8080,
    "weight": 2,
    "max_conns": 2000,
    "enabled": true
  }
}
```

**错误响应**:
```json
{
  "code": 400,
  "message": "Missing required parameter: port"
}
```

### 4. 移除服务器

从负载均衡池中移除指定的后端服务器。

- **URL**: `/upstream/remove`
- **方法**: `POST`
- **Content-Type**: `application/x-www-form-urlencoded`

**请求参数**:
| 参数名 | 类型 | 必需 | 描述 |
|--------|------|------|------|
| `host` | string | 是 | 服务器主机地址 |
| `port` | integer | 是 | 服务器端口 |

**请求示例**:
```bash
# 移除指定服务器
curl -X POST http://localhost/upstream/remove \
  -d "host=192.168.1.14" \
  -d "port=8080"

# 移除使用客户端 IP 注册的服务器
curl -X POST http://localhost/upstream/remove \
  -d "host=192.168.1.100" \
  -d "port=8080"
```

**成功响应**:
```json
{
  "code": 200,
  "message": "Server removed successfully"
}
```

**错误响应**:
```json
{
  "code": 404,
  "message": "Server not found: 192.168.1.14:8080"
}
```

### 5. 启用/禁用服务器

启用或禁用指定的后端服务器。禁用后，该服务器将不再接收新的请求。

- **URL**: `/upstream/toggle`
- **方法**: `POST`
- **Content-Type**: `application/x-www-form-urlencoded`

**请求参数**:
| 参数名 | 类型 | 必需 | 描述 |
|--------|------|------|------|
| `host` | string | 是 | 服务器主机地址 |
| `port` | integer | 是 | 服务器端口 |
| `action` | string | 是 | 操作类型: `enable` 或 `disable` |

**请求示例**:
```bash
# 禁用服务器
curl -X POST http://localhost/upstream/toggle \
  -d "host=192.168.1.11" \
  -d "port=8080" \
  -d "action=disable"

# 启用服务器
curl -X POST http://localhost/upstream/toggle \
  -d "host=192.168.1.11" \
  -d "port=8080" \
  -d "action=enable"
```

**成功响应**:
```json
{
  "code": 200,
  "message": "Server disabled successfully"
}
```

**错误响应**:
```json
{
  "code": 404,
  "message": "Server not found: 192.168.1.11:8080"
}
```

### 6. 获取服务器健康状态

获取服务器的健康检查状态（需要扩展健康检查功能）。

- **URL**: `/upstream/health`
- **方法**: `GET`
- **描述**: 返回所有服务器的健康状态（此接口需要额外实现健康检查逻辑）

**响应示例**:
```json
{
  "timestamp": 1633046800,
  "status": "healthy",
  "servers": [
    {
      "addr": "192.168.1.10:8080",
      "host": "192.168.1.10",
      "port": 8080,
      "status": "healthy",
      "response_time": 45,
      "last_check": 1633046795
    },
    {
      "addr": "192.168.1.11:8080",
      "host": "192.168.1.11", 
      "port": 8080,
      "status": "unhealthy",
      "response_time": 0,
      "last_check": 1633046790,
      "error": "Connection timeout"
    }
  ]
}
```

### 7. 重置连接数

重置所有服务器的连接数统计（用于调试和测试）。

- **URL**: `/upstream/reset`
- **方法**: `POST`
- **描述**: 将所有服务器的连接数重置为0

**请求示例**:
```bash
curl -X POST http://localhost/upstream/reset
```

**成功响应**:
```json
{
  "code": 200,
  "message": "All connection counts reset to zero"
}
```

## 状态码说明

| 状态码 | 说明 |
|--------|------|
| 200 | 请求成功 |
| 400 | 请求参数错误 |
| 404 | 资源未找到 |
| 500 | 服务器内部错误 |

## 错误码说明

| 错误码 | 说明 |
|--------|------|
| `MISSING_PARAM` | 缺少必要参数 |
| `INVALID_PORT` | 端口号无效 |
| `SERVER_EXISTS` | 服务器已存在 |
| `SERVER_NOT_FOUND` | 服务器不存在 |
| `REDIS_ERROR` | Redis 操作错误 |
| `INVALID_ACTION` | 无效的操作类型 |

## Lua 代码修改示例

以下是修改后的 `/upstream/add` 接口的 Lua 代码示例：

```lua
location /upstream/add {
    content_by_lua_block {
        ngx.req.read_body()
        local args = ngx.req.get_post_args()
        
        local host = args["host"]
        local port = tonumber(args["port"])
        local weight = tonumber(args["weight"]) or 1
        local max_conns = tonumber(args["max_conns"]) or 1000
        
        -- 参数验证
        if not port then
            ngx.status = 400
            ngx.say('{"code": 400, "message": "Missing required parameter: port"}')
            return
        end
        
        if port < 1 or port > 65535 then
            ngx.status = 400
            ngx.say('{"code": 400, "message": "Invalid port number: ' .. port .. '"}')
            return
        end
        
        -- 如果 host 为空，使用客户端 IP
        if not host or host == "" then
            host = ngx.var.remote_addr
            ngx.log(ngx.INFO, "Using client IP as host: " .. host)
        end
        
        local server_addr = host .. ":" .. port
        
        local zset_upstream = require "zset_upstream"
        local balancer = zset_upstream.new()
        
        local ok = balancer:add_server(server_addr, weight, max_conns)
        if ok then
            ngx.say('{"code": 200, "message": "Server added successfully", "data": {')
            ngx.say('  "addr": "' .. server_addr .. '",')
            ngx.say('  "host": "' .. host .. '",')
            ngx.say('  "port": ' .. port .. ',')
            ngx.say('  "weight": ' .. weight .. ',')
            ngx.say('  "max_conns": ' .. max_conns .. ',')
            ngx.say('  "enabled": true')
            ngx.say('}}')
        else
            ngx.status = 500
            ngx.say('{"code": 500, "message": "Failed to add server"}')
        end
    }
}
```

## 使用示例

### 完整的服务器管理流程

```bash
# 1. 查看当前服务器状态
curl http://localhost/upstream/status | jq .

# 2. 添加新服务器（明确指定 host）
curl -X POST http://localhost/upstream/add \
  -d "host=192.168.1.20" \
  -d "port=8080" \
  -d "weight=3" \
  -d "max_conns=3000"

# 3. 客户端自动注册（使用客户端 IP）
curl -X POST http://localhost/upstream/add \
  -d "port=8080" \
  -d "weight=1"

# 4. 验证服务器已添加
curl http://localhost/upstream/status | jq .

# 5. 临时禁用服务器进行维护
curl -X POST http://localhost/upstream/toggle \
  -d "host=192.168.1.10" \
  -d "port=8080" \
  -d "action=disable"

# 6. 维护完成后重新启用
curl -X POST http://localhost/upstream/toggle \
  -d "host=192.168.1.10" \
  -d "port=8080" \
  -d "action=enable"

# 7. 移除不再使用的服务器
curl -X POST http://localhost/upstream/remove \
  -d "host=192.168.1.20" \
  -d "port=8080"
```

### 批量操作脚本示例

```bash
#!/bin/bash
# manage_servers.sh

BASE_URL="http://localhost"

add_server() {
    local host=$1
    local port=$2
    local weight=${3:-1}
    local max_conns=${4:-1000}
    
    echo "Adding server: $host:$port"
    curl -s -X POST "$BASE_URL/upstream/add" \
        -d "host=$host" \
        -d "port=$port" \
        -d "weight=$weight" \
        -d "max_conns=$max_conns"
    echo
}

remove_server() {
    local host=$1
    local port=$2
    echo "Removing server: $host:$port"
    curl -s -X POST "$BASE_URL/upstream/remove" \
        -d "host=$host" \
        -d "port=$port"
    echo
}

# 批量添加服务器
add_server "192.168.1.101" 8080 1 1000
add_server "192.168.1.102" 8080 2 2000
add_server "192.168.1.103" 8080 1 1500

# 客户端自动注册（使用脚本运行机器的IP）
add_server "" 8080 1 1000

# 查看最终状态
echo "Final server status:"
curl -s "$BASE_URL/upstream/status" | jq .
```

## 注意事项

1. **自动 IP 检测**: 当 `host` 为空时，系统自动使用 `ngx.var.remote_addr` 作为主机地址
2. **端口验证**: 端口号必须在 1-65535 范围内
3. **连接数统计**: 连接数会在请求开始时增加，请求完成后减少
4. **缓存机制**: 服务器列表有3秒缓存，修改后可能不会立即生效
5. **并发安全**: Redis 的原子操作确保连接数统计的准确性
6. **生产建议**: 
   - 添加身份认证
   - 启用 HTTPS
   - 配置适当的超时时间
   - 实施速率限制

## 扩展功能

系统支持以下扩展功能（需要额外实现）：
- 健康检查自动故障转移
- 基于响应时间的动态权重调整
- 服务器分组和路由规则
- 详细的访问日志和监控指标
- 自动扩缩容集成


