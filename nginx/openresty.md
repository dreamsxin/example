# OpenResty

- https://openresty.org/cn/linux-packages.html

`wget -qO - https://openresty.org/package/pubkey.gpg | gpg --dearmor -o /etc/apt/trusted.gpg.d/openresty.gpg`

## 负载均衡
- https://github.com/OpenNJet/OpenNJet/blob/main/lualib/lib/njt/balancer.lua
Name
====

njt.balancer - Lua API for defining dynamic upstream balancers in Lua

```lua
-- Copyright (C) Yichun Zhang (agentzh)


local base = require "resty.core.base"
base.allows_subsystem('http', 'stream')


local ffi = require "ffi"
local C = ffi.C
local ffi_str = ffi.string
local errmsg = base.get_errmsg_ptr()
local FFI_OK = base.FFI_OK
local FFI_ERROR = base.FFI_ERROR
local int_out = ffi.new("int[1]")
local get_request = base.get_request
local error = error
local type = type
local tonumber = tonumber
local max = math.max
local subsystem = njt.config.subsystem
local njt_lua_ffi_balancer_set_current_peer
local njt_lua_ffi_balancer_set_more_tries
local njt_lua_ffi_balancer_get_last_failure
local njt_lua_ffi_balancer_set_timeouts -- used by both stream and http


if subsystem == 'http' then
    ffi.cdef[[
    int njt_http_lua_ffi_balancer_set_current_peer(njt_http_request_t *r,
        const unsigned char *addr, size_t addr_len, int port, char **err);

    int njt_http_lua_ffi_balancer_set_more_tries(njt_http_request_t *r,
        int count, char **err);

    int njt_http_lua_ffi_balancer_get_last_failure(njt_http_request_t *r,
        int *status, char **err);

    int njt_http_lua_ffi_balancer_set_timeouts(njt_http_request_t *r,
        long connect_timeout, long send_timeout,
        long read_timeout, char **err);

    int njt_http_lua_ffi_balancer_recreate_request(njt_http_request_t *r,
        char **err);
    ]]

    njt_lua_ffi_balancer_ =
        C.njt_http_lua_ffi_balancer_set_current_peer

    njt_lua_ffi_balancer_set_more_tries =
        C.njt_http_lua_ffi_balancer_set_more_tries

    njt_lua_ffi_balancer_get_last_failure =
        C.njt_http_lua_ffi_balancer_get_last_failure

    njt_lua_ffi_balancer_set_timeouts =
        C.njt_http_lua_ffi_balancer_set_timeouts

elseif subsystem == 'stream' then
    ffi.cdef[[
    int njt_stream_lua_ffi_balancer_set_current_peer(
        njt_stream_lua_request_t *r,
        const unsigned char *addr, size_t addr_len, int port, char **err);

    int njt_stream_lua_ffi_balancer_set_more_tries(njt_stream_lua_request_t *r,
        int count, char **err);

    int njt_stream_lua_ffi_balancer_get_last_failure(
        njt_stream_lua_request_t *r, int *status, char **err);

    int njt_stream_lua_ffi_balancer_set_timeouts(njt_stream_lua_request_t *r,
        long connect_timeout, long timeout, char **err);
    ]]

    njt_lua_ffi_balancer_set_current_peer =
        C.njt_stream_lua_ffi_balancer_set_current_peer

    njt_lua_ffi_balancer_set_more_tries =
        C.njt_stream_lua_ffi_balancer_set_more_tries

    njt_lua_ffi_balancer_get_last_failure =
        C.njt_stream_lua_ffi_balancer_get_last_failure

    local njt_stream_lua_ffi_balancer_set_timeouts =
        C.njt_stream_lua_ffi_balancer_set_timeouts

    njt_lua_ffi_balancer_set_timeouts =
        function(r, connect_timeout, send_timeout, read_timeout, err)
            local timeout = max(send_timeout, read_timeout)

            return njt_stream_lua_ffi_balancer_set_timeouts(r, connect_timeout,
                                                            timeout, err)
        end

else
    error("unknown subsystem: " .. subsystem)
end


local peer_state_names = {
    [1] = "keepalive",
    [2] = "next",
    [4] = "failed",
}


local _M = { version = base.version }


function _M.set_current_peer(addr, port)
    local r = get_request()
    if not r then
        error("no request found")
    end

    if not port then
        port = 0
    elseif type(port) ~= "number" then
        port = tonumber(port)
    end

    local rc = njt_lua_ffi_balancer_set_current_peer(r, addr, #addr,
                                                     port, errmsg)
    if rc == FFI_OK then
        return true
    end

    return nil, ffi_str(errmsg[0])
end


function _M.set_more_tries(count)
    local r = get_request()
    if not r then
        error("no request found")
    end

    local rc = njt_lua_ffi_balancer_set_more_tries(r, count, errmsg)
    if rc == FFI_OK then
        if errmsg[0] == nil then
            return true
        end
        return true, ffi_str(errmsg[0])  -- return the warning
    end

    return nil, ffi_str(errmsg[0])
end


function _M.get_last_failure()
    local r = get_request()
    if not r then
        error("no request found")
    end

    local state = njt_lua_ffi_balancer_get_last_failure(r, int_out, errmsg)

    if state == 0 then
        return nil
    end

    if state == FFI_ERROR then
        return nil, nil, ffi_str(errmsg[0])
    end

    return peer_state_names[state] or "unknown", int_out[0]
end


function _M.set_timeouts(connect_timeout, send_timeout, read_timeout)
    local r = get_request()
    if not r then
        error("no request found")
    end

    if not connect_timeout then
        connect_timeout = 0
    elseif type(connect_timeout) ~= "number" or connect_timeout <= 0 then
        error("bad connect timeout", 2)
    else
        connect_timeout = connect_timeout * 1000
    end

    if not send_timeout then
        send_timeout = 0
    elseif type(send_timeout) ~= "number" or send_timeout <= 0 then
        error("bad send timeout", 2)
    else
        send_timeout = send_timeout * 1000
    end

    if not read_timeout then
        read_timeout = 0
    elseif type(read_timeout) ~= "number" or read_timeout <= 0 then
        error("bad read timeout", 2)
    else
        read_timeout = read_timeout * 1000
    end

    local rc

    rc = njt_lua_ffi_balancer_set_timeouts(r, connect_timeout,
                                           send_timeout, read_timeout,
                                           errmsg)

    if rc == FFI_OK then
        return true
    end

    return false, ffi_str(errmsg[0])
end


if subsystem == 'http' then
    function _M.recreate_request()
        local r = get_request()
        if not r then
            error("no request found")
        end

        local rc = C.njt_http_lua_ffi_balancer_recreate_request(r, errmsg)
        if rc == FFI_OK then
            return true
        end

        if errmsg[0] ~= nil then
            return nil, ffi_str(errmsg[0])
        end

        return nil, "failed to recreate the upstream request"
    end
end


return _M
```

http subsystem
--------------

```nginx
http {
    upstream backend {
        server 0.0.0.1;   # just an invalid address as a place holder

        balancer_by_lua_block {
            local balancer = require "njt.balancer"

            -- well, usually we calculate the peer's host and port
            -- according to some balancing policies instead of using
            -- hard-coded values like below
            local host = "127.0.0.2"
            local port = 8080

            local ok, err = balancer.set_current_peer(host, port)
            if not ok then
                njt.log(njt.ERR, "failed to set the current peer: ", err)
                return njt.exit(500)
            end
        }

        keepalive 10;  # connection pool
    }

    server {
        # this is the real entry point
        listen 80;

        location / {
            # make use of the upstream named "backend" defined above:
            proxy_pass http://backend/fake;
        }
    }

    server {
        # this server is just for mocking up a backend peer here...
        listen 127.0.0.2:8080;

        location = /fake {
            echo "this is the fake backend peer...";
        }
    }
}
```

stream subsystem
----------------

```nginx
stream {
    upstream backend {
        server 0.0.0.1:1234;   # just an invalid address as a place holder

        balancer_by_lua_block {
            local balancer = require "njt.balancer"

            -- well, usually we calculate the peer's host and port
            -- according to some balancing policies instead of using
            -- hard-coded values like below
            local host = "127.0.0.2"
            local port = 8080

            local ok, err = balancer.set_current_peer(host, port)
            if not ok then
                njt.log(njt.ERR, "failed to set the current peer: ", err)
                return njt.exit(njt.ERROR)
            end
        }
    }

    server {
        # this is the real entry point
        listen 10000;

        # make use of the upstream named "backend" defined above:
        proxy_pass backend;
    }

    server {
        # this server is just for mocking up a backend peer here...
        listen 127.0.0.2:8080;

        echo "this is the fake backend peer...";
    }
}
```

### 完整的 Lua 模块代码 (zset_upstream.lua)

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
local UPSTREAM_HEARTBEAT_KEY = "upstream:heartbeat"    -- HASH: server -> last_heartbeat

-- 默认配置
local DEFAULT_CONFIG = {
    heartbeat_ttl = 30,        -- 心跳过期时间（秒）
    cleanup_interval = 10,     -- 清理间隔（秒）
    max_idle_time = 60         -- 最大空闲时间（秒）
}

function _M.new()
    local self = {
        redis_conn = nil,
        upstream_cache = nil,
        cache_ttl = 3, -- 缓存3秒
        config = DEFAULT_CONFIG
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

-- 更新心跳时间
function _M:update_heartbeat(server_addr)
    if not self:connect_redis() then
        return false, "Failed to connect to Redis"
    end
    
    local now = ngx.now()
    local ok, err = self.redis_conn:hset(UPSTREAM_HEARTBEAT_KEY, server_addr, now)
    self:close_redis()
    
    if not ok then
        return false, "Failed to update heartbeat: " .. (err or "unknown error")
    end
    
    ngx.log(ngx.DEBUG, "Updated heartbeat for ", server_addr, " at ", now)
    return true
end

-- 检查服务器是否活跃
function _M:is_server_active(server_addr)
    if not self:connect_redis() then
        return false
    end
    
    local last_heartbeat, err = self.redis_conn:hget(UPSTREAM_HEARTBEAT_KEY, server_addr)
    self:close_redis()
    
    if not last_heartbeat or last_heartbeat == ngx.null then
        return false
    end
    
    local heartbeat_time = tonumber(last_heartbeat)
    local now = ngx.now()
    local time_since_heartbeat = now - heartbeat_time
    
    return time_since_heartbeat <= self.config.heartbeat_ttl
end

-- 清理过期服务器
function _M:cleanup_expired_servers()
    if not self:connect_redis() then
        return false, "Failed to connect to Redis"
    end
    
    local now = ngx.now()
    local expired_servers = {}
    
    -- 获取所有服务器的心跳时间
    local all_heartbeats, err = self.redis_conn:hgetall(UPSTREAM_HEARTBEAT_KEY)
    if not all_heartbeats then
        self:close_redis()
        return false, "Failed to get heartbeats: " .. (err or "unknown error")
    end
    
    -- 找出过期的服务器
    for i = 1, #all_heartbeats, 2 do
        local server_addr = all_heartbeats[i]
        local last_heartbeat = tonumber(all_heartbeats[i + 1])
        
        if last_heartbeat then
            local time_since_heartbeat = now - last_heartbeat
            if time_since_heartbeat > self.config.heartbeat_ttl then
                table.insert(expired_servers, server_addr)
                ngx.log(ngx.WARN, "Server ", server_addr, " expired, last heartbeat: ", 
                        time_since_heartbeat, " seconds ago")
            end
        end
    end
    
    -- 禁用过期的服务器
    local disabled_count = 0
    for _, server_addr in ipairs(expired_servers) do
        -- 获取当前元数据
        local meta_json, err = self.redis_conn:hget(UPSTREAM_METADATA_KEY, server_addr)
        if meta_json and meta_json ~= ngx.null then
            local meta = cjson.decode(meta_json)
            if meta and meta.enabled then
                -- 禁用服务器
                meta.enabled = false
                meta.disabled_at = now
                meta.disabled_reason = "heartbeat_timeout"
                
                local hset_ok, hset_err = self.redis_conn:hset(UPSTREAM_METADATA_KEY, server_addr, cjson.encode(meta))
                if hset_ok then
                    disabled_count = disabled_count + 1
                    ngx.log(ngx.INFO, "Disabled expired server: ", server_addr)
                else
                    ngx.log(ngx.ERR, "Failed to disable expired server ", server_addr, ": ", hset_err)
                end
            end
        end
    end
    
    self:close_redis()
    
    -- 清除缓存
    self.upstream_cache = nil
    
    ngx.log(ngx.INFO, "Cleanup completed: ", disabled_count, " servers disabled due to heartbeat timeout")
    return true, { expired_count = #expired_servers, disabled_count = disabled_count }
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
                -- 检查心跳是否活跃
                local is_active = self:is_server_active(server_addr)
                if is_active then
                    local host, port = self:parse_server_addr(server_addr)
                    if host and port then
                        table.insert(enabled_servers, {
                            addr = server_addr,
                            host = host,
                            port = port,
                            weight = meta.weight or 1,
                            max_conns = meta.max_conns or 1000,
                            last_heartbeat = meta.last_heartbeat
                        })
                    end
                else
                    ngx.log(ngx.WARN, "Server ", server_addr, " is enabled but heartbeat expired")
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
    local now = ngx.now()
    local meta = {
        host = actual_host,
        port = port,
        weight = weight,
        max_conns = max_conns,
        enabled = true,
        created_at = now,
        last_heartbeat = now
    }
    
    local hset_ok, hset_err = self.redis_conn:hset(UPSTREAM_METADATA_KEY, server_addr, cjson.encode(meta))
    if not hset_ok then
        -- 回滚：从 ZSET 中移除
        self.redis_conn:zrem(UPSTREAM_ZSET_KEY, server_addr)
        self:close_redis()
        return false, "Failed to add server metadata: " .. (hset_err or "unknown error")
    end
    
    -- 设置初始心跳
    local heartbeat_ok, heartbeat_err = self.redis_conn:hset(UPSTREAM_HEARTBEAT_KEY, server_addr, now)
    if not heartbeat_ok then
        ngx.log(ngx.WARN, "Failed to set initial heartbeat for ", server_addr, ": ", heartbeat_err)
    end
    
    self:close_redis()
    
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
    
    -- 从心跳 HASH 移除
    local heartbeat_del_ok, heartbeat_del_err = self.redis_conn:hdel(UPSTREAM_HEARTBEAT_KEY, server_addr)
    
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

-- 更新服务器心跳
function _M:refresh_heartbeat(host, port)
    local server_addr, actual_host = self:build_server_addr(host, port)
    if not server_addr then
        return false, actual_host -- actual_host contains error message
    end
    
    if not self:connect_redis() then
        return false, "Failed to connect to Redis"
    end
    
    -- 检查服务器是否存在
    local exists, err = self.redis_conn:zscore(UPSTREAM_ZSET_KEY, server_addr)
    if not exists or exists == ngx.null then
        self:close_redis()
        return false, "Server not found: " .. server_addr
    end
    
    -- 更新心跳时间
    local now = ngx.now()
    local heartbeat_ok, heartbeat_err = self.redis_conn:hset(UPSTREAM_HEARTBEAT_KEY, server_addr, now)
    
    -- 同时更新元数据中的最后心跳时间
    local meta_json, err = self.redis_conn:hget(UPSTREAM_METADATA_KEY, server_addr)
    if meta_json and meta_json ~= ngx.null then
        local meta = cjson.decode(meta_json)
        if meta then
            meta.last_heartbeat = now
            self.redis_conn:hset(UPSTREAM_METADATA_KEY, server_addr, cjson.encode(meta))
        end
    end
    
    self:close_redis()
    
    if not heartbeat_ok then
        return false, "Failed to refresh heartbeat: " .. (heartbeat_err or "unknown error")
    end
    
    -- 清除缓存
    self.upstream_cache = nil
    
    ngx.log(ngx.DEBUG, "Refreshed heartbeat for ", server_addr, " at ", now)
    return true, { server_addr = server_addr, heartbeat_time = now }
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
    
    -- 获取所有服务器的心跳
    local all_heartbeats, err = self.redis_conn:hgetall(UPSTREAM_HEARTBEAT_KEY)
    
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
    
    -- 构建心跳查找表
    local heartbeat_lookup = {}
    if all_heartbeats then
        for i = 1, #all_heartbeats, 2 do
            local server_addr = all_heartbeats[i]
            local heartbeat = tonumber(all_heartbeats[i + 1])
            heartbeat_lookup[server_addr] = heartbeat
        end
    end
    
    -- 构建结果
    local result = {}
    local now = ngx.now()
    
    for i = 1, #servers_with_scores, 2 do
        local server_addr = servers_with_scores[i]
        local connections = tonumber(servers_with_scores[i + 1]) or 0
        local meta = meta_lookup[server_addr]
        local last_heartbeat = heartbeat_lookup[server_addr]
        
        if meta then
            local host, port = self:parse_server_addr(server_addr)
            local time_since_heartbeat = last_heartbeat and (now - last_heartbeat) or nil
            local is_active = last_heartbeat and ((now - last_heartbeat) <= self.config.heartbeat_ttl) or false
            
            table.insert(result, {
                addr = server_addr,
                host = host,
                port = port,
                connections = connections,
                weight = meta.weight,
                max_conns = meta.max_conns,
                enabled = meta.enabled,
                is_active = is_active,
                last_heartbeat = last_heartbeat,
                time_since_heartbeat = time_since_heartbeat,
                created_at = meta.created_at,
                updated_at = meta.updated_at,
                disabled_at = meta.disabled_at,
                disabled_reason = meta.disabled_reason
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

-- 配置设置
function _M:set_config(new_config)
    for key, value in pairs(new_config) do
        if self.config[key] ~= nil then
            self.config[key] = value
        end
    end
    return true
end

return _M
```

### 完整的 Nginx 配置

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
    lua_shared_dict timer_locks 1m;
    
    # 初始化 Redis 连接池和定时器
    init_worker_by_lua_block {
        require "resty.redis"
        
        -- 导入模块
        local zset_upstream = require "zset_upstream"
        
        -- 清理过期服务器的定时器
        local function cleanup_expired_servers(premature)
            if premature then
                return
            end
            
            local lock_key = "cleanup_lock"
            local locks = ngx.shared.timer_locks
            
            -- 使用共享内存锁避免多个worker同时执行清理
            local ok, err = locks:add(lock_key, true, 5) -- 5秒锁
            if not ok then
                if err == "exists" then
                    -- 其他worker正在执行清理
                    ngx.log(ngx.DEBUG, "Cleanup already running in another worker")
                else
                    ngx.log(ngx.ERR, "Failed to acquire cleanup lock: ", err)
                end
                return
            end
            
            -- 执行清理
            local balancer = zset_upstream.new()
            local ok, result = balancer:cleanup_expired_servers()
            if not ok then
                ngx.log(ngx.ERR, "Cleanup failed: ", result)
            else
                ngx.log(ngx.INFO, "Cleanup completed: ", result.disabled_count, " servers disabled")
            end
            
            -- 释放锁
            locks:delete(lock_key)
        end
        
        -- 每10秒执行一次清理
        local function schedule_cleanup()
            local ok, err = ngx.timer.at(10, cleanup_expired_servers)
            if not ok then
                ngx.log(ngx.ERR, "Failed to create cleanup timer: ", err)
            end
        end
        
        -- 立即启动清理定时器
        schedule_cleanup()
        
        -- 设置定时器，每10秒执行一次
        local heartbeat_timer
        heartbeat_timer = ngx.timer.every(10, function(premature)
            if premature then
                return
            end
            cleanup_expired_servers(premature)
        end)
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

        # 根路径 - 显示管理界面
        location = / {
            content_by_lua_block {
                ngx.say([[
                <!DOCTYPE html>
                <html>
                <head>
                    <title>Dynamic Upstream Manager</title>
                    <meta charset="utf-8">
                    <style>
                        body { font-family: Arial, sans-serif; margin: 40px; line-height: 1.6; }
                        h1 { color: #333; border-bottom: 1px solid #ddd; padding-bottom: 10px; }
                        ul { list-style-type: none; padding: 0; }
                        li { margin: 10px 0; padding: 10px; background: #f5f5f5; border-radius: 4px; }
                        a { text-decoration: none; color: #0066cc; }
                        a:hover { text-decoration: underline; }
                        .endpoint { font-weight: bold; color: #333; }
                        .description { color: #666; margin-left: 10px; }
                        .info { background: #e7f3ff; padding: 15px; border-radius: 4px; margin: 20px 0; }
                    </style>
                </head>
                <body>
                    <h1>Dynamic Upstream Manager with Heartbeat</h1>
                    <p>Available endpoints:</p>
                    <ul>
                        <li><span class="endpoint"><a href="/upstream/status">GET /upstream/status</a></span> <span class="description">- 查看服务器状态</span></li>
                        <li><span class="endpoint">POST /upstream/add</span> <span class="description">- 添加服务器</span></li>
                        <li><span class="endpoint">POST /upstream/remove</span> <span class="description">- 移除服务器</span></li>
                        <li><span class="endpoint">POST /upstream/toggle</span> <span class="description">- 启用/禁用服务器</span></li>
                        <li><span class="endpoint">POST /upstream/refresh</span> <span class="description">- 刷新心跳</span></li>
                        <li><span class="endpoint">POST /upstream/reset</span> <span class="description">- 重置连接数</span></li>
                        <li><span class="endpoint"><a href="/upstream/health">GET /upstream/health</a></span> <span class="description">- 健康检查</span></li>
                        <li><span class="endpoint"><a href="/upstream/cleanup">GET /upstream/cleanup</a></span> <span class="description">- 手动清理</span></li>
                    </ul>
                    
                    <div class="info">
                        <p><strong>心跳机制说明：</strong></p>
                        <ul>
                            <li>服务器需要定期调用 <span class="endpoint">/upstream/refresh</span> 接口刷新心跳</li>
                            <li>默认心跳过期时间：30秒</li>
                            <li>自动清理间隔：10秒</li>
                            <li>过期服务器会被自动禁用</li>
                        </ul>
                    </div>
                    
                    <p><strong>使用示例：</strong></p>
                    <pre>
# 添加服务器（使用客户端IP）
curl -X POST http://localhost/upstream/add -d "port=8080"

# 刷新心跳
curl -X POST http://localhost/upstream/refresh -d "host=192.168.1.10" -d "port=8080"

# 查看状态
curl http://localhost/upstream/status | jq .
                    </pre>
                </body>
                </html>
                ]])
            }
        }

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
        
        # 管理接口：刷新心跳
        location /upstream/refresh {
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
                
                local ok, result = balancer:refresh_heartbeat(host, port)
                if ok then
                    ngx.say('{"code": 200, "message": "Heartbeat refreshed successfully", "data": {')
                    ngx.say('  "addr": "' .. result.server_addr .. '",')
                    ngx.say('  "heartbeat_time": ' .. result.heartbeat_time)
                    ngx.say('}}')
                else
                    ngx.status = 404
                    ngx.say('{"code": 404, "message": "' .. (result or "Failed to refresh heartbeat") .. '"}')
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
                
                local active_servers = 0
                local enabled_servers = 0
                local total_servers = #servers
                
                for _, server in ipairs(servers) do
                    if server.enabled then
                        enabled_servers = enabled_servers + 1
                        if server.is_active then
                            active_servers = active_servers + 1
                        end
                    end
                end
                
                local status = (active_servers > 0) and "healthy" or "unhealthy"
                
                ngx.say(cjson.encode({
                    code = 200,
                    message = "Upstream health status",
                    status = status,
                    data = {
                        total_servers = total_servers,
                        enabled_servers = enabled_servers,
                        active_servers = active_servers,
                        expired_servers = enabled_servers - active_servers
                    }
                }))
            }
        }
        
        # 清理接口（手动触发）
        location /upstream/cleanup {
            content_by_lua_block {
                local zset_upstream = require "zset_upstream"
                local cjson = require "cjson.safe"
                local balancer = zset_upstream.new()
                
                local ok, result = balancer:cleanup_expired_servers()
                if ok then
                    ngx.say(cjson.encode({
                        code = 200,
                        message = "Cleanup completed successfully",
                        data = result
                    }))
                else
                    ngx.status = 500
                    ngx.say(cjson.encode({
                        code = 500,
                        message = "Cleanup failed: " .. (result or "unknown error")
                    }))
                end
            }
        }
    }
}
```

### 初始化脚本

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
red:del("upstream:heartbeat")

-- 初始服务器列表
local servers = {
    { host = "192.168.1.10", port = 8080, weight = 1, max_conns = 1000 },
    { host = "192.168.1.11", port = 8080, weight = 2, max_conns = 2000 },
    { host = "192.168.1.12", port = 8080, weight = 1, max_conns = 1500 }
}

-- 添加到 ZSET 和 HASH
local now = os.time()
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
        created_at = now,
        last_heartbeat = now
    }
    
    local ok, err = red:hset("upstream:servers:meta", server_addr, cjson.encode(meta))
    if not ok then
        print("Failed to add server metadata: ", server_addr, " - ", err)
    else
        print("Added server metadata: ", server_addr)
    end
    
    -- 设置初始心跳
    local ok, err = red:hset("upstream:heartbeat", server_addr, now)
    if not ok then
        print("Failed to set initial heartbeat: ", server_addr, " - ", err)
    else
        print("Set initial heartbeat: ", server_addr)
    end
end

-- 验证数据
local servers_count = red:zcard("upstream:servers:zset")
local meta_count = red:hlen("upstream:servers:meta")
local heartbeat_count = red:hlen("upstream:heartbeat")

print("\nInitialization completed:")
print("ZSET servers count: " .. (servers_count or 0))
print("HASH metadata count: " .. (meta_count or 0))
print("Heartbeat count: " .. (heartbeat_count or 0))

-- 显示当前服务器状态
print("\nCurrent server status:")
local servers, err = red:zrange("upstream:servers:zset", 0, -1, "WITHSCORES")
if servers then
    for i = 1, #servers, 2 do
        local server_addr = servers[i]
        local connections = servers[i + 1]
        local meta_json = red:hget("upstream:servers:meta", server_addr)
        local heartbeat = red:hget("upstream:heartbeat", server_addr)
        
        if meta_json and meta_json ~= ngx.null then
            local meta = cjson.decode(meta_json)
            if meta then
                local status = meta.enabled and "✓" or "✗"
                local time_since_heartbeat = heartbeat and (now - tonumber(heartbeat)) or "N/A"
                print(string.format("  %s %s (conn: %s, weight: %d, max: %d, heartbeat: %ds ago)", 
                    status, server_addr, connections, meta.weight, meta.max_conns, time_since_heartbeat))
            end
        end
    end
end

red:close()
```

### 部署说明

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
# 查看服务器状态（包含心跳信息）
curl http://localhost/upstream/status | jq .

# 手动刷新心跳
curl -X POST http://localhost/upstream/refresh -d "host=192.168.1.10" -d "port=8080"

# 手动触发清理
curl http://localhost/upstream/cleanup

# 健康检查
curl http://localhost/upstream/health | jq .
```

### 心跳示例

```bash
#!/bin/bash
# heartbeat_client.sh

# 配置
BASE_URL="http://your-openresty-host"
HOST=""  # 为空则使用客户端IP
PORT="8080"
INTERVAL=20  # 心跳间隔（秒）

echo "Starting heartbeat client for server: ${HOST:-auto}:$PORT"
echo "Heartbeat interval: $INTERVAL seconds"

while true; do
    TIMESTAMP=$(date '+%Y-%m-%d %H:%M:%S')
    
    # 发送心跳请求
    RESPONSE=$(curl -s -X POST "$BASE_URL/upstream/refresh" \
        -d "host=$HOST" \
        -d "port=$PORT")
    
    # 解析响应
    CODE=$(echo "$RESPONSE" | grep -o '"code":[0-9]*' | cut -d: -f2)
    MESSAGE=$(echo "$RESPONSE" | grep -o '"message":"[^"]*"' | cut -d: -f2- | tr -d '"')
    
    if [ "$CODE" = "200" ]; then
        echo "[$TIMESTAMP] ✓ Heartbeat successful: $MESSAGE"
    else
        echo "[$TIMESTAMP] ✗ Heartbeat failed: $MESSAGE"
        
        # 如果服务器不存在，尝试重新注册
        if echo "$MESSAGE" | grep -q "not found"; then
            echo "[$TIMESTAMP] Attempting to register server..."
            REGISTER_RESPONSE=$(curl -s -X POST "$BASE_URL/upstream/add" \
                -d "host=$HOST" \
                -d "port=$PORT" \
                -d "weight=1")
            
            REGISTER_CODE=$(echo "$REGISTER_RESPONSE" | grep -o '"code":[0-9]*' | cut -d: -f2)
            if [ "$REGISTER_CODE" = "200" ]; then
                echo "[$TIMESTAMP] ✓ Server registered successfully"
            else
                echo "[$TIMESTAMP] ✗ Failed to register server"
            fi
        fi
    fi
    
    sleep $INTERVAL
done
```

----------------------------------------------

# OpenResty 动态 Upstream 管理系统

基于 OpenResty 和 Redis 实现的动态 Upstream 管理系统，支持最少连接负载均衡、服务器自动发现、心跳健康检查和动态配置管理。

## 特性

- 🚀 **动态 Upstream 管理** - 支持动态添加、移除、启用/禁用后端服务器
- ⚖️ **最少连接负载均衡** - 基于 Redis ZSET 实现的高效负载均衡算法
- ❤️ **心跳健康检查** - 客户端定时刷新心跳，自动禁用过期服务器
- 🔄 **自动故障转移** - 过期服务器自动禁用，确保流量只路由到健康节点
- 📊 **实时监控** - 完整的服务器状态和连接数监控
- 🔧 **RESTful API** - 简单易用的 HTTP API 接口
- 🎯 **客户端自动注册** - 支持使用客户端 IP 自动注册服务器

## 架构概述

```
客户端请求 → OpenResty → 最少连接负载均衡 → 后端服务器
                    ↓
                  Redis (存储服务器列表、连接数、心跳数据)
```

## 快速开始

### 环境要求

- OpenResty 1.19+
- Redis 5.0+
- Lua 5.1+

### 安装部署

1. **克隆项目**
```bash
git clone <repository-url>
cd openresty-dynamic-upstream
```

2. **配置 Redis**
```bash
# 启动 Redis 服务
redis-server
```

3. **初始化数据**
```bash
# 执行初始化脚本
/usr/local/openresty/bin/resty scripts/init_redis_zset.lua
```

4. **启动 OpenResty**
```bash
# 启动服务
/usr/local/openresty/bin/openresty -p /usr/local/openresty/nginx -c conf/nginx.conf

# 重新加载配置
/usr/local/openresty/bin/openresty -p /usr/local/openresty/nginx -c conf/nginx.conf -s reload

# 停止服务
/usr/local/openresty/bin/openresty -p /usr/local/openresty/nginx -c conf/nginx.conf -s stop
```

## API 使用说明

### 基础信息

- **基础 URL**: `http://your-openresty-host`
- **数据格式**: JSON
- **认证**: 当前版本无需认证（生产环境建议添加）

### 1. 代理请求

所有非管理接口的请求都会被代理到后端服务器，自动使用最少连接负载均衡算法。

**请求示例**:
```bash
curl -X GET http://localhost/api/users
curl -X POST http://localhost/api/orders -d '{"item": "test"}'
```

### 2. 获取服务器状态

获取所有后端服务器的详细状态信息。

**端点**: `GET /upstream/status`

**响应示例**:
```json
{
  "code": 200,
  "message": "Success",
  "data": [
    {
      "addr": "192.168.1.10:8080",
      "host": "192.168.1.10",
      "port": 8080,
      "connections": 15,
      "weight": 1,
      "max_conns": 1000,
      "enabled": true,
      "is_active": true,
      "last_heartbeat": 1633046800,
      "time_since_heartbeat": 5,
      "created_at": 1633046400,
      "updated_at": 1633046500
    }
  ]
}
```

**字段说明**:
- `addr`: 服务器完整地址
- `host`: 服务器主机地址
- `port`: 服务器端口
- `connections`: 当前连接数
- `weight`: 权重值
- `max_conns`: 最大连接数限制
- `enabled`: 是否启用
- `is_active`: 心跳是否活跃
- `last_heartbeat`: 最后心跳时间戳
- `time_since_heartbeat`: 距离最后心跳的秒数

### 3. 添加服务器

向负载均衡池中添加新的后端服务器。

**端点**: `POST /upstream/add`

**请求参数**:
| 参数名 | 类型 | 必需 | 默认值 | 描述 |
|--------|------|------|--------|------|
| `host` | string | 否 | 客户端IP | 服务器主机地址，为空时自动使用请求来源 IP |
| `port` | integer | 是 | - | 服务器端口 (1-65535) |
| `weight` | integer | 否 | 1 | 服务器权重，数值越大分配的请求越多 |
| `max_conns` | integer | 否 | 1000 | 最大连接数限制 |

**请求示例**:
```bash
# 明确指定 host 和 port
curl -X POST http://localhost/upstream/add \
  -d "host=192.168.1.14" \
  -d "port=8080" \
  -d "weight=2" \
  -d "max_conns=2000"

# 自动使用客户端 IP (host 为空)
curl -X POST http://localhost/upstream/add \
  -d "port=8080" \
  -d "weight=1"
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

### 4. 移除服务器

从负载均衡池中移除指定的后端服务器。

**端点**: `POST /upstream/remove`

**请求参数**:
| 参数名 | 类型 | 必需 | 描述 |
|--------|------|------|------|
| `host` | string | 是 | 服务器主机地址 |
| `port` | integer | 是 | 服务器端口 |

**请求示例**:
```bash
curl -X POST http://localhost/upstream/remove \
  -d "host=192.168.1.14" \
  -d "port=8080"
```

**成功响应**:
```json
{
  "code": 200,
  "message": "Server removed successfully",
  "data": {
    "addr": "192.168.1.14:8080"
  }
}
```

### 5. 启用/禁用服务器

启用或禁用指定的后端服务器。

**端点**: `POST /upstream/toggle`

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
  "message": "Server disabled successfully",
  "data": {
    "addr": "192.168.1.11:8080",
    "enabled": false
  }
}
```

### 6. 刷新心跳

客户端定时调用此接口来刷新服务器的心跳时间。

**端点**: `POST /upstream/refresh`

**请求参数**:
| 参数名 | 类型 | 必需 | 描述 |
|--------|------|------|------|
| `host` | string | 是 | 服务器主机地址 |
| `port` | integer | 是 | 服务器端口 |

**请求示例**:
```bash
curl -X POST http://localhost/upstream/refresh \
  -d "host=192.168.1.10" \
  -d "port=8080"
```

**成功响应**:
```json
{
  "code": 200,
  "message": "Heartbeat refreshed successfully",
  "data": {
    "addr": "192.168.1.10:8080",
    "heartbeat_time": 1633046800
  }
}
```

### 7. 重置连接数

重置所有服务器的连接数统计（用于调试和测试）。

**端点**: `POST /upstream/reset`

**请求示例**:
```bash
curl -X POST http://localhost/upstream/reset
```

**成功响应**:
```json
{
  "code": 200,
  "message": "All connection counts reset to zero",
  "data": {
    "servers_reset": 5
  }
}
```

### 8. 健康检查

获取系统的健康状态。

**端点**: `GET /upstream/health`

**响应示例**:
```json
{
  "code": 200,
  "message": "Upstream health status",
  "status": "healthy",
  "data": {
    "total_servers": 5,
    "enabled_servers": 3,
    "active_servers": 2,
    "expired_servers": 1
  }
}
```

### 9. 手动清理

手动触发过期服务器的清理操作。

**端点**: `GET /upstream/cleanup`

**响应示例**:
```json
{
  "code": 200,
  "message": "Cleanup completed successfully",
  "data": {
    "expired_count": 2,
    "disabled_count": 2
  }
}
```

## 心跳机制

### 工作原理

1. **客户端注册** - 客户端通过 `/upstream/add` 接口注册服务器
2. **定时心跳** - 客户端需要定期调用 `/upstream/refresh` 接口刷新心跳
3. **自动清理** - 系统每10秒自动检查并禁用过期服务器
4. **故障转移** - 过期服务器自动禁用，流量路由到健康节点

### 心跳配置

- **心跳过期时间**: 30秒（服务器30秒内未刷新心跳会被禁用）
- **自动清理间隔**: 10秒（系统每10秒检查一次过期服务器）
- **推荐心跳间隔**: 15-20秒（客户端刷新间隔）

### 客户端心跳脚本

```bash
# 启动心跳客户端
./scripts/heartbeat_client.sh

# 或手动指定参数
HOST="192.168.1.100" PORT="8080" INTERVAL=20 ./scripts/heartbeat_client.sh
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

# 4. 启动心跳（客户端需要定期执行）
curl -X POST http://localhost/upstream/refresh \
  -d "host=192.168.1.20" \
  -d "port=8080"

# 5. 验证服务器状态
curl http://localhost/upstream/status | jq .

# 6. 临时禁用服务器进行维护
curl -X POST http://localhost/upstream/toggle \
  -d "host=192.168.1.10" \
  -d "port=8080" \
  -d "action=disable"

# 7. 维护完成后重新启用
curl -X POST http://localhost/upstream/toggle \
  -d "host=192.168.1.10" \
  -d "port=8080" \
  -d "action=enable"

# 8. 移除不再使用的服务器
curl -X POST http://localhost/upstream/remove \
  -d "host=192.168.1.20" \
  -d "port=8080"
```

### 批量操作脚本

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

refresh_heartbeat() {
    local host=$1
    local port=$2
    echo "Refreshing heartbeat: $host:$port"
    curl -s -X POST "$BASE_URL/upstream/refresh" \
        -d "host=$host" \
        -d "port=$port"
    echo
}

# 批量添加服务器
add_server "192.168.1.101" 8080 1 1000
add_server "192.168.1.102" 8080 2 2000
add_server "192.168.1.103" 8080 1 1500

# 批量刷新心跳
refresh_heartbeat "192.168.1.101" 8080
refresh_heartbeat "192.168.1.102" 8080
refresh_heartbeat "192.168.1.103" 8080

# 查看最终状态
echo "Final server status:"
curl -s "$BASE_URL/upstream/status" | jq .
```

## 配置说明

### Redis 配置

默认 Redis 配置（可在 Lua 模块中修改）:
```lua
local REDIS_CONFIG = {
    host = "127.0.0.1",
    port = 6379,
    timeout = 1000,
    pool_size = 100
}
```

### 心跳配置

默认心跳配置:
```lua
local DEFAULT_CONFIG = {
    heartbeat_ttl = 30,        -- 心跳过期时间（秒）
    cleanup_interval = 10,     -- 清理间隔（秒）
    max_idle_time = 60         -- 最大空闲时间（秒）
}
```

## 故障排除

### 常见问题

1. **服务器显示为不活跃**
   - 检查客户端是否定期调用 `/upstream/refresh` 接口
   - 确认心跳间隔小于30秒

2. **无法添加服务器**
   - 检查端口号是否在 1-65535 范围内
   - 确认服务器地址格式正确

3. **负载均衡不工作**
   - 检查 Redis 连接是否正常
   - 确认有启用的活跃服务器

4. **连接数异常**
   - 使用 `/upstream/reset` 接口重置连接数统计
   - 检查是否有请求未正确释放连接

### 日志查看

```bash
# 查看 OpenResty 错误日志
tail -f /usr/local/openresty/nginx/logs/error.log

# 查看访问日志
tail -f /usr/local/openresty/nginx/logs/access.log
```

## 性能优化建议

1. **Redis 优化**
   - 使用 Redis 集群提高可用性
   - 配置适当的 Redis 内存策略
   - 启用 Redis 持久化

2. **OpenResty 优化**
   - 调整 worker_processes 数量
   - 优化 Lua 代码缓存设置
   - 配置适当的连接超时时间

3. **客户端优化**
   - 使用连接池减少连接建立开销
   - 批量操作减少 API 调用次数
   - 实现重试机制处理网络波动

## 生产环境部署

### 安全建议

1. **添加认证**
   - 为管理接口添加 HTTP Basic 认证
   - 使用 JWT Token 进行身份验证
   - 配置 IP 白名单限制访问

2. **网络安全**
   - 使用 HTTPS 加密通信
   - 配置防火墙规则
   - 启用请求速率限制

3. **监控告警**
   - 集成 Prometheus 监控
   - 配置关键指标告警
   - 设置日志审计

### 高可用部署

1. **多节点部署**
   - 部署多个 OpenResty 实例
   - 使用负载均衡器分发请求
   - 配置共享 Redis 集群

2. **数据持久化**
   - 配置 Redis AOF 持久化
   - 定期备份 Redis 数据
   - 实现配置版本管理


