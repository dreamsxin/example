如果是一个 *.LUA 的文件， 里面用到了自己写的库， 或者第三方写的库， 但是你不想把它放到 lua 的安装目录里， 则在代码里面可以指定require搜索的路径。

```lua
package.path = '/usr/local/share/lua/5.1/?.lua;/home/resty/?.lua;'    --搜索lua模块
package.cpath = '/usr/local/lib/lua/5.1/?.so;'        --搜索so模块
```

如果是在 nginx.conf 文件中引用了第三方的库，则需要在 http 段中添加下面的代码

```conf
lua_package_path '/usr/local/share/lua/5.1/?.lua;/home/resty/?.lua;';
lua_package_cpath '/usr/local/lib/lua/5.1/?.so;';
```

## request对象的属性和方法

-- 属性
- method          = ngx.var.request_method    -- http://wiki.nginx.org/HttpCoreModule#.24request_method
- schema          = ngx.var.schema            -- http://wiki.nginx.org/HttpCoreModule#.24scheme
- host            = ngx.var.host              -- http://wiki.nginx.org/HttpCoreModule#.24host
- hostname        = ngx.var.hostname          -- http://wiki.nginx.org/HttpCoreModule#.24hostname
- uri             = ngx.var.request_uri       -- http://wiki.nginx.org/HttpCoreModule#.24request_uri
- path            = ngx.var.uri               -- http://wiki.nginx.org/HttpCoreModule#.24uri
- filename        = ngx.var.request_filename  -- http://wiki.nginx.org/HttpCoreModule#.24request_filename
- query_string    = ngx.var.query_string      -- http://wiki.nginx.org/HttpCoreModule#.24query_string
- user_agent      = ngx.var.http_user_agent   -- http://wiki.nginx.org/HttpCoreModule#.24http_HEADER
- remote_addr     = ngx.var.remote_addr       -- http://wiki.nginx.org/HttpCoreModule#.24remote_addr
- remote_port     = ngx.var.remote_port       -- http://wiki.nginx.org/HttpCoreModule#.24remote_port
- remote_user     = ngx.var.remote_user       -- http://wiki.nginx.org/HttpCoreModule#.24remote_user
- remote_passwd   = ngx.var.remote_passwd     -- http://wiki.nginx.org/HttpCoreModule#.24remote_passwd
- content_type    = ngx.var.content_type      -- http://wiki.nginx.org/HttpCoreModule#.24content_type
- content_length  = ngx.var.content_length    -- http://wiki.nginx.org/HttpCoreModule#.24content_length
- headers         = ngx.req.get_headers()     -- http://wiki.nginx.org/HttpLuaModule#ngx.req.get_headers
- uri_args        = ngx.req.get_uri_args()    -- http://wiki.nginx.org/HttpLuaModule#ngx.req.get_uri_args
- post_args       = ngx.req.get_post_args()   -- http://wiki.nginx.org/HttpLuaModule#ngx.req.get_post_args
- socket          = ngx.req.socket            -- http://wiki.nginx.org/HttpLuaModule#ngx.req.socket

-- 方法
- request:read_body()                         -- http://wiki.nginx.org/HttpLuaModule#ngx.req.read_body
- request:get_uri_arg(name, default)
- request:get_post_arg(name, default)
- request:get_arg(name, default)

- request:get_cookie(key, decrypt)
- request:rewrite(uri, jump)                  -- http://wiki.nginx.org/HttpLuaModule#ngx.req.set_uri
- request:set_uri_args(args)                  -- http://wiki.nginx.org/HttpLuaModule#ngx.req.set_uri_args

## response对象的属性和方法

-- 属性
- headers         = ngx.header                -- http://wiki.nginx.org/HttpLuaModule#ngx.header.HEADER

-- 方法
- response:set_cookie(key, value, encrypt, duration, path)
- response:write(content)
- response:writeln(content)
- response:ltp(template,data)
- response:redirect(url, status)              -- http://wiki.nginx.org/HttpLuaModule#ngx.redirect

- response:finish()                           -- http://wiki.nginx.org/HttpLuaModule#ngx.eof
- response:is_finished()
- response:defer(func, ...)                   -- 在response返回后执行

# Simple monitoring for NGINX with Lua

```conf
#we need for shared memory for several nginx's workers
lua_shared_dict requests_counter 20m;
#count request on complete (log phase)
log_by_lua_file '/path/to/lua/counter.lua';

server {
    listen       127.0.0.1:8890;

    server_name ams.local;

    #simple output of monitor data
    location /monitor {
        content_by_lua '
            local keys = ngx.shared.requests_counter:get_keys(10000)
            for _, ip in pairs(keys) do
                ngx.say(ip .. ";" .. ngx.shared.requests_counter:get(ip))
            end
        ';
    }
}
```

```lua
local requests = ngx.shared.requests_limit;
local function monitor()
    local key = ngx.var.server_name;     local status = ngx.var.status;
    local list = ngx.shared.requests_counter;
    local banned_statuses = { ['444'] = true, ['408'] = true, ['449'] = true, ['400'] = true };

    local keys = {
        ["a"] = key .. ";all", 
        ["be"] = key .. ";backend", 
        ["bn"] = key .. ";banned",
        ["ajax"] = key .. ";ajax", 
        ["bs"] = key .. ";bytes"
    };

    for _, _key in pairs(keys) do
        local temp_val = list:get(keys[_key]);

        if temp_val == nil then
            list:add(_key, 0);
        end

        --anyone know what will be when "temp_val" reach self limit? Its will be equal to zero?
        if temp_val and temp_val >= 70368744177664 then --math.pow(2, 46) value
            list:set(_key, 0);
        end
    end

    list:incr(keys.a, 1);
  
    if ngx.var.upstream_status ~= nil then
        list:incr(keys.be, 1);
    end

    if banned_statuses[status] then
        list:incr(keys.bn, 1);
    end

    if ngx.header['X-Requested-With'] ~= nil then
        list:incr(keys.ajax, 1);
    end


    if ngx.var.body_bytes_sent ~= nil then
        list:incr(keys.bs, ngx.var.body_bytes_sent);
    end
end

monitor();
```

# 根据 url 显示日期

`http://localhost:8080/1/days/ago`

```conf
location ~ ^/([0-9]+)/days/ago {
   content_by_lua '
      local count = ngx.shared.count
      local now = os.date("*t")
      local point = tonumber(ngx.var[1])
      local specific_day = os.date
        "%Y-%m-%d",
        os.time({
           day=tonumber(now.day-point)
           month=now.month,
           year=now.year
          }
       ngx.say(specific_day)
   ';
}
```

# 统计访问人数

```html
<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8"/>
    <script src="//ajax.googleapis.com/ajax/libs/jquery/2.0.0/jquery.min.js"></script>
    <script type="text/javascript">
      $(document).ready(function() {
        $.get("/count/up", function(data) {
          $("#today").html(data);
        });
        $.get("/1/days/ago", function(data) {
          $("#one-days-ago").html(data);
        });
      });
    </script>
  </head>
  <body>
    <div>
      <p>今日访问人数:<span id="today"></span></p>
      <p>昨日访问人数:<span id="one-days-ago"></span></p>
    </div>
  </body>
</html>
```

```conf
worker_processes  1;

error_log  logs/error.log;
pid        logs/nginx.pid;

events {
    worker_connections  1024;
}

http {

    lua_package_path "/usr/local/nginx-1.4.0/lua-lib/lua-resty-string/lib/?.lua;;";

    include       mime.types;
    default_type  text/html;

    access_log  logs/access.log;

    sendfile        on;
    keepalive_timeout  65;

    gzip  on;

    lua_shared_dict count 5m;
    lua_shared_dict check_of_user_access 5m;

    server {
        listen       8080;
        server_name  localhost;

        access_log  logs/host.access.log;

        location / {
            root   html;
            try_files $uri $uri/ /index.html;
        }

        location /count/up {
          content_by_lua '
            local count = ngx.shared.count
            local check_of_user_access = ngx.shared.check_of_user_access

            local date = os.date("%Y-%m-%d")

            is_exists_val, flags = count:get(date)
            if is_exists_val == nil then
              count:set(date, 0)
            end

            local user_access_key = (
                ngx.var.remote_addr .. "+" ..
                ngx.req.get_headers()["User-Agent"] .. "+" ..
                date
            )

            is_access_val, flags = check_of_user_access:get(user_access_key)
            if is_access_val == nil then
              check_of_user_access:set(user_access_key, 1)
              new_incr_val, err = count:incr(date, 1)
            end

            latest_count, flags = count:get(date)
            ngx.say(latest_count)
          ';
        }

        location ~ ^/([0-9]+)/days/ago {
          content_by_lua '
            local count = ngx.shared.count
            local now = os.date("*t")
            local point = tonumber(ngx.var[1])
            local specific_day = os.date(
              "%Y-%m-%d",
              os.time({
                day=tonumber(now.day-point),
                month=now.month,
                year=now.year
              }))

            record, flag = count:get(specific_day)
            if record == nil then
              record = 0
            end
            ngx.say(record)
          ';
        }
    }
}
```

# 验证
```conf
		access_by_lua '
			local args = ngx.req.get_uri_args()
			if (args.token == nil) then
				ngx.exit(ngx.HTTP_FORBIDDEN)
			end
			local token = ngx.md5("push" .. args.id)
			if (args.token ~= token) then
				ngx.exit(ngx.HTTP_FORBIDDEN)
			end
		';
```

# 数据收集

```conf
location /1.gif {
	#伪装成gif文件
	default_type image/gif;

	#本身关闭access_log，通过subrequest记录log
	access_log off;
	access_by_lua "
		-- 用户跟踪cookie名为__utrace
		local uid = ngx.var.cookie___utrace
		if not uid then
			-- 如果没有则生成一个跟踪cookie，算法为md5(时间戳+IP+客户端信息)
			uid = ngx.md5(ngx.now() .. ngx.var.remote_addr .. ngx.var.http_user_agent)
		end
		ngx.header['Set-Cookie'] = {'__utrace=' .. uid .. '; path=/'}
		if ngx.var.arg_domain then
			-- 通过subrequest到/i-log记录日志，将参数和用户跟踪cookie带过去
			ngx.location.capture('/i-log?' .. ngx.var.args .. '&utrace=' .. uid)
		end
	";

	#此请求不缓存
	add_header Expires "Fri, 01 Jan 1980 00:00:00 GMT";
	add_header Pragma "no-cache";
	add_header Cache-Control "no-cache, max-age=0, must-revalidate";

	#返回一个1×1的空gif图片
	empty_gif;
}
```