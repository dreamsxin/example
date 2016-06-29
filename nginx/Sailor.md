# 安装 Sailor

## luarocks 版本升级
```shell
sudo apt-get install luarocks
luarocks --version
sudo luarocks install luarocks
# or
sudo apt-get remove luarocks --purge
wget http://keplerproject.github.io/luarocks/releases/luarocks-2.3.0.tar.gz
tar zxvf luarocks-2.3.0.tar.gz
./configure
```

```shell
sudo luarocks install sailor
sudo luarocks install luafilesystem
sudo luarocks install luapgsql PQ_INCDIR=/usr/include/postgresql
```

# 配置 nginx

## HTTP 段加入
```conf
lua_package_path '/usr/local/share/lua/5.1/?.lua;/var/www/html/lua/hello_word/?.lua;';
lua_package_cpath '/usr/local/lib/lua/5.1/?.so;';
```

## SERVER 段
```conf
location ~ ^/hello_word/(.+)\.lua {
    root /var/www/html/lua;
    lua_need_request_body on;
    lua_code_cache off;
    content_by_lua_file /var/www/html/lua/hello_word/$1.lua;
    index index.lua index.lp;
}
location ~* ^/hello_word/.+\.(?:css|eot|js|json|png|svg|ttf|woff)$ {
    root /var/www/html/lua;
}
```

# 创建项目

```shell
sailor create 'Hello Word' /var/www/html/lua
```

## 目录结构

默认的Sailor应用程序将具有以下目录树结构：

- /conf - configuration files, open and edit them.
- /controllers - controllers you will make!
- /themes - default layout files.
- /models - models you will make!
- /pub - publicly accessible files (js libraries, for example)
- /runtime - temporary files generated during runtime.
- /views - this is where your lua pages in .lp will go

# 控制器 Controllers

```lua
local main = {} --a table 

--Your actions are functions indexed by this table. They receive the 'page' object.
function main.index(page)
    --Let's send a message to our view
    page:render("index", { msg = "Hello"})
end

function main.hello(page)
    page:write( "Hello world!")
end

return main --don't forget to return your controller at the end of the file.
```

访问地址：`http://localhost/app/index.lua?r=main/hello`

使用 `page:write`

```lua
local main = {}

function main.index(page)
    page:write("Hello")
end
return main
```

使用 `page:inspect` 进行调试，需要设置 `conf.debug.inspect = true`

```lua
local main = {}

function main.index(page)
    page:inspect(page.POST)
end
return main
```

# 视图 views

`/views/main/index.lp`
```lp
<?lua 
  page:print(msg) 
?> <!--If this came from the controller we just created in the last section of our tutorial, this will print 'Hello'--> 
<%= msg %> <!--This will do the same thing -->
<br/>
<div id="app"></div>
<?lua@client

local app = window.document:getElementById('app')
print(app.textContent)
app.textContent = 'lets go'

window:alert('This code was written in Lua')

?>
```

# 模型 models

```lua
local val = require "valua"
local post = {}
-- Attributes and their validation rules
post.attributes = { 
-- { <attribute> = <validation function, valua required> or "safe" if no validation required for this attribute} 
  { id = "safe" }, -- No validation rules
  { author = val:new().not_empty().len(6,20) }, -- Cannot be empty and must have between 6 and 20 characters
  { title = val:new().not_empty().len(6,100) }, -- Cannot be empty and must have between 6 and 100 characters
  { body = val:new().not_empty() } -- Cannot be empty
}

post.db = {
  key = 'id', -- the primary key
  table = 'post' -- make sure this field contains the same name as your SQL table!
}

post.relations = {
  comments = {relation = "HAS_MANY", model = "comment", attribute = "post_id"}, -- details below
}

function post.test() return "test" end -- A public method

return post
```

# 保存数据

```lua
local post = sailor.model("post"):new()
local saved
if next(page.POST) then
	post:get_post(page.POST)
	saved = post:save()
end
```

# 配置 access


```lua

```

```lua
local M = {}

local access = require "sailor.access"

function M.index(page)
	local access = require "sailor.access"
	if access.is_guest() then
		return 404
	end
end

-- 使用 model 登录
function M.login(page)
	access.settings({model = 'user'})
	if next(page.POST) then
		local login, err = access.login(page.POST.username, page.POST.password)
		if not login then
			user.errors.password = err
		end
	end
	page:render('login',{user = user})
end

-- 外部判断登录
function M.adminLogin(page)
	local u
	if page.POST.username then
		local user = sailor.model('user')
		u = Model:find_by_attributes{ username = page.POST.username }
	}

	if u ~= nil and page.POST.password then
		if access.hash(page.POST.username, page.POST.password) == u.password then
			page:inspect("is equal")
			access.grant({id=1,username='admin'})
			page:redirect('main/index')
		end
	end
	page:render('admin')
end

function M.logout(page)
	access.logout()
	page:redirect('user/login')
end
```
