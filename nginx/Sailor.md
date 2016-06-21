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

# 控制器 Controllers

```lua
local main = {} --a table 

--Your actions are functions indexed by this table. They receive the 'page' object.
function main.index(page)
    --Let's send a message to our view
    page:render("index", { msg = "Hello"})
end

return main --don't forget to return your controller at the end of the file.
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