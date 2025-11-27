```lua
-- lua-balancer\upstream.lua
local upstream = {}

local cjson = require "cjson"
local redis = require "resty.redis"
local ngx_shared = ngx.shared.upstream_servers

-- 配置常量
local REDIS_HOST = "127.0.0.1"
local REDIS_PORT = 6379
local REDIS_DB = 0
local REDIS_KEY = "upstream:servers"
local HEALTH_CHECK_INTERVAL = 10  -- 健康检查间隔（秒）
local SERVER_TIMEOUT = 60        -- 服务器超时时间（秒）

-- 获取Redis连接
local function get_redis()
    local red = redis:new()
    red:set_timeout(1000) -- 1秒超时
    
    local ok, err = red:connect(REDIS_HOST, REDIS_PORT)
    if not ok then
        ngx.log(ngx.ERR, "failed to connect to redis: ", err)
        return nil, err
    end
    
    local ok, err = red:select(REDIS_DB)
    if not ok then
        ngx.log(ngx.ERR, "failed to select redis db: ", err)
        return nil, err
    end
    
    return red
end

-- 从Redis加载服务器列表
local function load_servers_from_redis()
    local red, err = get_redis()
    if not red then
        return nil, err
    end
    
    local servers_json, err = red:get(REDIS_KEY)
    if not servers_json then
        return nil, err
    end
    
    if servers_json == ngx.null then
        return {}
    end
    
    local servers, err = cjson.decode(servers_json)
    if not servers then
        return nil, err
    end
    
    -- 关闭Redis连接
    local ok, err = red:set_keepalive(10000, 100)
    if not ok then
        ngx.log(ngx.ERR, "failed to set keepalive: ", err)
    end
    
    return servers
end

-- 保存服务器列表到Redis
local function save_servers_to_redis(servers)
    local red, err = get_redis()
    if not red then
        return false, err
    end
    
    local servers_json, err = cjson.encode(servers)
    if not servers_json then
        return false, err
    end
    
    local ok, err = red:set(REDIS_KEY, servers_json)
    if not ok then
        return false, err
    end
    
    -- 关闭Redis连接
    local ok, err = red:set_keepalive(10000, 100)
    if not ok then
        ngx.log(ngx.ERR, "failed to set keepalive: ", err)
    end
    
    return true
end

-- 健康检查函数
local function health_check_server(server)
    local http = require "resty.http"
    local httpc = http:new()
    
    local url = "http://" .. server.host .. ":" .. server.port .. "/health"
    local res, err = httpc:request_uri(url, {
        method = "GET",
        timeout = 2000, -- 2秒超时
        headers = {
            ["User-Agent"] = "OpenResty Health Check",
        }
    })
    
    if not res then
        return false
    end
    
    return res.status == 200
end

-- 初始化清理定时器
function upstream.init_cleanup_timer()
    local function cleanup_timer()
        local servers = ngx_shared:get("servers")
        if not servers then
            return
        end
        
        local servers_list = cjson.decode(servers)
        local current_time = ngx.time()
        local updated = false
        
        -- 检查每个服务器的健康状态和超时
        for i = #servers_list, 1, -1 do
            local server = servers_list[i]
            
            -- 检查超时
            if current_time - server.last_update > SERVER_TIMEOUT then
                table.remove(servers_list, i)
                updated = true
                ngx.log(ngx.INFO, "removed timeout server: ", server.host, ":", server.port)
            else
                -- 定期健康检查
                if current_time % HEALTH_CHECK_INTERVAL == 0 then
                    local is_healthy = health_check_server(server)
                    if not is_healthy then
                        server.healthy = false
                        updated = true
                        ngx.log(ngx.WARN, "server unhealthy: ", server.host, ":", server.port)
                    elseif not server.healthy then
                        server.healthy = true
                        updated = true
                        ngx.log(ngx.INFO, "server recovered: ", server.host, ":", server.port)
                    end
                end
            end
        end
        
        -- 如果有更新，保存到共享内存
        if updated then
            local servers_json = cjson.encode(servers_list)
            ngx_shared:set("servers", servers_json)
            
            -- 同步到Redis
            save_servers_to_redis(servers_list)
        end
    end
    
    -- 注册定时器
    local ok, err = ngx.timer.every(1, cleanup_timer)
    if not ok then
        ngx.log(ngx.ERR, "failed to create timer: ", err)
        return
    end
    
    ngx.log(ngx.INFO, "cleanup timer initialized")
end

-- 最少连接数负载均衡
function upstream.least_conn_balancer()
    local servers_json = ngx_shared:get("servers")
    if not servers_json then
        ngx.log(ngx.ERR, "no servers available")
        return ngx.exit(502)
    end
    
    local servers = cjson.decode(servers_json)
    local healthy_servers = {}
    
    -- 筛选健康的服务器
    for _, server in ipairs(servers) do
        if server.healthy ~= false then
            table.insert(healthy_servers, server)
        end
    end
    
    if #healthy_servers == 0 then
        ngx.log(ngx.ERR, "no healthy servers available")
        return ngx.exit(502)
    end
    
    -- 选择连接数最少的服务器
    local selected_server = nil
    local min_connections = math.huge
    
    for _, server in ipairs(healthy_servers) do
        local conn_key = "conn:" .. server.host .. ":" .. server.port
        local conn_count = ngx_shared:get(conn_key) or 0
        
        if conn_count < min_connections then
            min_connections = conn_count
            selected_server = server
        end
    end
    
    if not selected_server then
        selected_server = healthy_servers[1] -- 回退到第一个服务器
    end
    
    -- 增加连接数
    local conn_key = "conn:" .. selected_server.host .. ":" .. selected_server.port
    local conn_count = ngx_shared:get(conn_key) or 0
    ngx_shared:set(conn_key, conn_count + 1)
    
    -- 保存选择的服务器信息
    ngx.ctx.selected_server = selected_server
    
    -- 设置后端服务器
    local ok, err = ngx.balancer.set_current_peer(selected_server.host, selected_server.port)
    if not ok then
        ngx.log(ngx.ERR, "failed to set current peer: ", err)
        return ngx.exit(502)
    end
end

-- 减少连接数
function upstream.decrease_conn_count(server)
    if not server then
        return
    end
    
    local conn_key = "conn:" .. server.host .. ":" .. server.port
    local conn_count = ngx_shared:get(conn_key) or 0
    
    if conn_count > 0 then
        ngx_shared:set(conn_key, conn_count - 1)
    end
end

-- 添加服务器
function upstream.add_server()
    ngx.req.read_body()
    local body = ngx.req.get_body_data()
    
    if not body then
        ngx.status = 400
        ngx.say(cjson.encode({error = "no request body"}))
        return
    end
    
    local server_info, err = cjson.decode(body)
    if not server_info then
        ngx.status = 400
        ngx.say(cjson.encode({error = "invalid json: " .. err}))
        return
    end
    
    -- 验证必需字段
    if not server_info.host or not server_info.port then
        ngx.status = 400
        ngx.say(cjson.encode({error = "missing required fields: host and port"}))
        return
    end
    
    -- 获取当前服务器列表
    local servers_json = ngx_shared:get("servers")
    local servers = {}
    if servers_json then
        servers = cjson.decode(servers_json)
    end
    
    -- 检查是否已存在
    for _, existing_server in ipairs(servers) do
        if existing_server.host == server_info.host and existing_server.port == server_info.port then
            ngx.status = 409
            ngx.say(cjson.encode({error = "server already exists"}))
            return
        end
    end
    
    -- 添加新服务器
    local new_server = {
        host = server_info.host,
        port = tonumber(server_info.port),
        weight = tonumber(server_info.weight) or 1,
        healthy = true,
        last_update = ngx.time(),
        conn_count = 0
    }
    
    table.insert(servers, new_server)
    
    -- 保存到共享内存
    local servers_json = cjson.encode(servers)
    ngx_shared:set("servers", servers_json)
    
    -- 同步到Redis
    local ok, err = save_servers_to_redis(servers)
    if not ok then
        ngx.log(ngx.WARN, "failed to save to redis: ", err)
    end
    
    ngx.say(cjson.encode({
        success = true,
        message = "server added successfully",
        server = new_server
    }))
end

-- 移除服务器
function upstream.remove_server()
    ngx.req.read_body()
    local body = ngx.req.get_body_data()
    
    if not body then
        ngx.status = 400
        ngx.say(cjson.encode({error = "no request body"}))
        return
    end
    
    local server_info, err = cjson.decode(body)
    if not server_info then
        ngx.status = 400
        ngx.say(cjson.encode({error = "invalid json: " .. err}))
        return
    end
    
    -- 验证必需字段
    if not server_info.host or not server_info.port then
        ngx.status = 400
        ngx.say(cjson.encode({error = "missing required fields: host and port"}))
        return
    end
    
    -- 获取当前服务器列表
    local servers_json = ngx_shared:get("servers")
    if not servers_json then
        ngx.status = 404
        ngx.say(cjson.encode({error = "no servers found"}))
        return
    end
    
    local servers = cjson.decode(servers_json)
    local removed = false
    
    -- 查找并移除服务器
    for i = #servers, 1, -1 do
        local server = servers[i]
        if server.host == server_info.host and server.port == tonumber(server_info.port) then
            table.remove(servers, i)
            removed = true
            
            -- 清理连接数
            local conn_key = "conn:" .. server.host .. ":" .. server.port
            ngx_shared:delete(conn_key)
        end
    end
    
    if not removed then
        ngx.status = 404
        ngx.say(cjson.encode({error = "server not found"}))
        return
    end
    
    -- 保存到共享内存
    local servers_json = cjson.encode(servers)
    ngx_shared:set("servers", servers_json)
    
    -- 同步到Redis
    local ok, err = save_servers_to_redis(servers)
    if not ok then
        ngx.log(ngx.WARN, "failed to save to redis: ", err)
    end
    
    ngx.say(cjson.encode({
        success = true,
        message = "server removed successfully"
    }))
end

-- 获取状态
function upstream.get_status()
    local servers_json = ngx_shared:get("servers")
    local servers = {}
    
    if servers_json then
        servers = cjson.decode(servers_json)
        
        -- 添加当前连接数信息
        for _, server in ipairs(servers) do
            local conn_key = "conn:" .. server.host .. ":" .. server.port
            server.current_connections = ngx_shared:get(conn_key) or 0
        end
    end
    
    local status = {
        servers = servers,
        total_servers = #servers,
        healthy_servers = 0,
        timestamp = ngx.time()
    }
    
    -- 统计健康服务器数量
    for _, server in ipairs(servers) do
        if server.healthy ~= false then
            status.healthy_servers = status.healthy_servers + 1
        end
    end
    
    ngx.say(cjson.encode(status))
end

return upstream

```
