# Splash 使用教程

Splash 是一个 JavaScript 渲染服务，带有 HTTP API，专门用于处理使用 JavaScript 的动态网页。它基于 Twisted 和 QT 开发，常用于网络爬虫中处理 JavaScript 渲染的页面。

## 1. 安装与启动

### Docker 安装（推荐）
```bash
# 拉取 Splash 镜像
docker pull scrapinghub/splash

# 运行 Splash 容器
docker run -p 8050:8050 scrapinghub/splash
```

### 本地安装
```bash
# 使用 pip 安装
pip install scrapy-splash

# 或者从源码安装
git clone https://github.com/scrapinghub/splash
cd splash
pip install -r requirements.txt
python setup.py install
```

## 2. 基本使用

### 通过 HTTP API 使用
启动后访问 `http://localhost:8050` 可以看到 Splash 的 Web 界面。

#### 渲染页面 HTML
```bash
curl 'http://localhost:8050/render.html?url=http://example.com&timeout=10&wait=0.5'
```

#### 获取页面截图
```bash
curl 'http://localhost:8050/render.png?url=http://example.com&width=1024&height=768'
```

### 参数说明
- `url`: 要渲染的网页 URL
- `wait`: 等待时间（秒），用于等待 JavaScript 执行
- `timeout`: 超时时间（秒）
- `images`: 是否加载图片（0/1）
- `js`: 是否执行 JavaScript（0/1）
- `proxy`: 使用代理服务器

## 3. Lua 脚本高级用法

Splash 支持通过 Lua 脚本进行更复杂的操作。

### 基本 Lua 脚本示例
```lua
function main(splash)
    -- 访问页面
    splash:go("http://example.com")
    
    -- 等待页面加载
    splash:wait(2)
    
    -- 获取页面标题
    local title = splash:evaljs("document.title")
    
    -- 返回结果
    return {
        title = title,
        html = splash:html(),
        url = splash:url()
    }
end
```

### 执行 Lua 脚本
```bash
curl -X POST \
  http://localhost:8050/execute \
  -H 'Content-Type: application/json' \
  -d '{
    "lua_source": "function main(splash) splash:go(\"http://example.com\") return splash:html() end"
  }'
```

## 4. 在 Python 中使用 Splash

### 使用 requests 库
```python
import requests
import json

def render_with_splash(url):
    splash_url = 'http://localhost:8050/render.html'
    params = {
        'url': url,
        'wait': 2,
        'timeout': 30
    }
    
    response = requests.get(splash_url, params=params)
    return response.text

# 使用示例
html = render_with_splash('https://example.com')
print(html)
```

### 使用 Scrapy 集成
首先安装 scrapy-splash：
```bash
pip install scrapy-splash
```

在 Scrapy 项目的 settings.py 中添加：
```python
# 启用 Splash 中间件
SPIDER_MIDDLEWARES = {
    'scrapy_splash.SplashDeduplicateArgsMiddleware': 100,
}

DOWNLOADER_MIDDLEWARES = {
    'scrapy_splash.SplashCookiesMiddleware': 723,
    'scrapy_splash.SplashMiddleware': 725,
    'scrapy.downloadermiddlewares.httpcompression.HttpCompressionMiddleware': 810,
}

# Splash 服务器地址
SPLASH_URL = 'http://localhost:8050'

# 去重过滤器
DUPEFILTER_CLASS = 'scrapy_splash.SplashAwareDupeFilter'
```

在 Spider 中使用：
```python
import scrapy
from scrapy_splash import SplashRequest

class MySpider(scrapy.Spider):
    name = 'my_spider'
    
    def start_requests(self):
        url = 'http://example.com'
        yield SplashRequest(
            url, 
            self.parse,
            args={'wait': 2}
        )
    
    def parse(self, response):
        # 处理渲染后的页面
        yield {
            'title': response.css('title::text').get(),
            'url': response.url
        }
```

## 5. 高级 Lua 脚本示例

### 处理动态加载内容
```lua
function main(splash)
    splash:go("https://example.com")
    
    -- 等待特定元素出现
    splash:wait_for_selector("#dynamic-content")
    
    -- 执行 JavaScript 点击操作
    splash:runjs("document.querySelector('#load-more').click()")
    splash:wait(1)
    
    -- 获取所有内容
    return splash:html()
end
```

### 处理无限滚动
```lua
function main(splash)
    splash:go("https://example.com/infinite-scroll")
    
    -- 滚动多次以加载所有内容
    for i = 1, 5 do
        splash:runjs("window.scrollTo(0, document.body.scrollHeight);")
        splash:wait(1)
    end
    
    return splash:html()
end
```

### 处理登录表单
```lua
function main(splash)
    splash:go("https://example.com/login")
    
    -- 填写表单
    splash:send_text("input[name='username']", "my_username")
    splash:send_text("input[name='password']", "my_password")
    
    -- 提交表单
    splash:runjs("document.querySelector('form').submit()")
    splash:wait(2)
    
    return splash:html()
end
```

## 6. 性能优化技巧

1. **设置合适的等待时间**：根据页面复杂度调整 `wait` 参数
2. **禁用不必要的资源**：如图片、CSS 等
```lua
function main(splash)
    splash:on_request(function(request)
        if request.url:find('%.css') or request.url:find('%.png') then
            request:abort()
        end
    end)
    splash:go("http://example.com")
    return splash:html()
end
```

3. **使用缓存**：对于相同页面避免重复渲染
4. **合理设置超时时间**：避免长时间等待

## 7. 常见问题解决

### 内存泄漏
定期重启 Splash 服务：
```bash
# 使用 --max-timeout 参数限制运行时间
docker run -p 8050:8050 scrapinghub/splash --max-timeout 3600
```

### 处理证书错误
```lua
function main(splash)
    splash:on_request(function(request)
        request:set_header('Accept-Language', 'en-US,en;q=0.9')
    end)
    splash:go("https://example.com")
    return splash:html()
end
```

## 1. 基础渲染端点

### 1.1 render.html - 获取渲染后的 HTML
```bash
# 基本用法
curl "http://localhost:8050/render.html?url=https://example.com"

# 带等待时间
curl "http://localhost:8050/render.html?url=https://example.com&wait=2"

# 完整参数示例
curl "http://localhost:8050/render.html?\
url=https://example.com&\
wait=2&\
timeout=30&\
images=1&\
js=1&\
proxy=proxy.example.com:8080"
```

### 1.2 render.png - 获取页面截图
```bash
# 基本截图
curl "http://localhost:8050/render.png?url=https://example.com" -o screenshot.png

# 自定义尺寸和质量的截图
curl "http://localhost:8050/render.png?\
url=https://example.com&\
width=1024&\
height=768&\
render_all=1&\
wait=2" -o fullpage.png
```

### 1.3 render.jpeg - 获取 JPEG 格式截图
```bash
curl "http://localhost:8050/render.jpeg?\
url=https://example.com&\
width=800&\
height=600&\
quality=85" -o screenshot.jpg
```

### 1.4 render.har - 获取页面 HAR 数据
```bash
curl "http://localhost:8050/render.har?\
url=https://example.com&\
wait=1" -o page.har
```

### 1.5 render.json - 获取多种格式数据
```bash
curl "http://localhost:8050/render.json?\
url=https://example.com&\
html=1&\
png=1&\
har=1&\
wait=1"
```

## 2. 代理设置详细说明

### 2.1 HTTP/HTTPS 代理
```bash
# 基本代理设置
curl "http://localhost:8050/render.html?\
url=https://httpbin.org/ip&\
proxy=http://proxy.example.com:8080"

# 需要认证的代理
curl "http://localhost:8050/render.html?\
url=https://httpbin.org/ip&\
proxy=http://username:password@proxy.example.com:8080"
```

### 2.2 SOCKS 代理
```bash
# SOCKS5 代理
curl "http://localhost:8050/render.html?\
url=https://httpbin.org/ip&\
proxy=socks5://127.0.0.1:1080"

# 带认证的 SOCKS5 代理
curl "http://localhost:8050/render.html?\
url=https://httpbin.org/ip&\
proxy=socks5://user:pass@127.0.0.1:1080"
```

### 2.3 代理白名单/黑名单
```lua
# 通过 Lua 脚本控制代理使用
curl -X POST http://localhost:8050/execute \
-H 'Content-Type: application/json' \
-d '{
  "lua_source": "function main(splash)\n  splash:on_request(function(request)\n    -- 只对特定域名使用代理\n    if string.find(request.url, \"target-site.com\") then\n      request:set_proxy{\"http://proxy.example.com:8080\"}\n    end\n  end)\n  splash:go(\"https://httpbin.org/ip\")\n  return splash:html()\nend"
}'
```

## 3. 请求头设置

### 3.1 自定义请求头
```bash
# 通过 headers 参数
curl -G "http://localhost:8050/render.html" \
--data-urlencode "url=https://httpbin.org/headers" \
--data-urlencode "headers={\"User-Agent\": \"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36\", \"Accept-Language\": \"en-US,en;q=0.9\"}"
```

### 3.2 使用 Lua 脚本设置请求头
```lua
curl -X POST http://localhost:8050/execute \
-H 'Content-Type: application/json' \
-d '{
  "lua_source": "function main(splash)\n  splash:set_user_agent(\"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36\")\n  splash:set_custom_headers({\n    [\"X-Custom-Header\"] = \"CustomValue\",\n    [\"Accept\"] = \"text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\"\n  })\n  splash:go(\"https://httpbin.org/headers\")\n  return splash:html()\nend"
}'
```

## 4. 认证处理

### 4.1 HTTP 基本认证
```bash
# 方法1: 直接在 URL 中包含认证信息
curl "http://localhost:8050/render.html?\
url=http://username:password@example.com/protected"

# 方法2: 通过 Lua 脚本
curl -X POST http://localhost:8050/execute \
-H 'Content-Type: application/json' \
-d '{
  "lua_source": "function main(splash)\n  splash:set_http_user_agent(\"Mozilla/5.0\")\n  splash:set_http_raw_headers({\n    \"Authorization\" = \"Basic \" .. splash:base64_encode(\"username:password\")\n  })\n  splash:go(\"http://example.com/protected\")\n  return splash:html()\nend"
}'
```

### 4.2 Cookie 认证
```lua
curl -X POST http://localhost:8050/execute \
-H 'Content-Type: application/json' \
-d '{
  "lua_source": "function main(splash)\n  -- 先登录获取 Cookie\n  splash:go(\"https://example.com/login\")\n  splash:wait(1)\n  \n  -- 填写登录表单\n  splash:send_text(\"#username\", \"my_username\")\n  splash:send_text(\"#password\", \"my_password\")\n  splash:wait(0.5)\n  \n  -- 提交表单\n  splash:runjs(\"document.querySelector(\"#login-form\").submit()\")\n  splash:wait(2)\n  \n  -- 访问需要认证的页面\n  splash:go(\"https://example.com/dashboard\")\n  return splash:html()\nend"
}'
```

## 5. 高级过滤和资源控制

### 5.1 资源过滤
```lua
curl -X POST http://localhost:8050/execute \
-H 'Content-Type: application/json' \
-d '{
  "lua_source": "function main(splash)\n  -- 阻止图片和 CSS 加载\n  splash:on_request(function(request)\n    if request.url:find(\".css$\") or request.url:find(\".png$\") or request.url:find(\".jpg$\") then\n      request:abort()\n    end\n    -- 阻止特定域名\n    if request.url:find(\"ads.example.com\") then\n      request:abort()\n    end\n  end)\n  \n  splash:go(\"https://example.com\")\n  splash:wait(2)\n  return splash:html()\nend"
}'
```

### 5.2 请求重写
```lua
curl -X POST http://localhost:8050/execute \
-H 'Content-Type: application/json' \
-d '{
  "lua_source": "function main(splash)\n  splash:on_request(function(request)\n    -- 修改请求头\n    request:set_header(\"User-Agent\", \"Custom Bot\")\n    \n    -- 重写 URL\n    if request.url:find(\"old-domain.com\") then\n      request:set_url(string.gsub(request.url, \"old-domain.com\", \"new-domain.com\"))\n    end\n  end)\n  \n  splash:go(\"https://example.com\")\n  return splash:html()\nend"
}'
```

## 6. 执行 JavaScript

### 6.1 页面交互
```lua
curl -X POST http://localhost:8050/execute \
-H 'Content-Type: application/json' \
-d '{
  "lua_source": "function main(splash)\n  splash:go(\"https://example.com\")\n  splash:wait(1)\n  \n  -- 点击按钮\n  splash:runjs(\"document.querySelector(\"#load-more\").click()\")\n  splash:wait(2)\n  \n  -- 滚动页面\n  splash:runjs(\"window.scrollTo(0, document.body.scrollHeight)\")\n  splash:wait(1)\n  \n  -- 获取动态数据\n  local data = splash:evaljs(\"window.someData || null\")\n  \n  return {\n    html = splash:html(),\n    dynamic_data = data,\n    url = splash:url()\n  }\nend"
}'
```

## 7. 错误处理和调试

### 7.1 错误处理
```lua
curl -X POST http://localhost:8050/execute \
-H 'Content-Type: application/json' \
-d '{
  "lua_source": "function main(splash)\n  local ok, reason = splash:go(\"https://example.com\")\n  if not ok then\n    return {error = reason, success = false}\n  end\n  \n  -- 检查元素是否存在\n  local element_exists = splash:select(\"#main-content\")\n  if not element_exists then\n    return {error = \"Element not found\", success = false}\n  end\n  \n  splash:wait(2)\n  return {\n    success = true,\n    html = splash:html(),\n    title = splash:evaljs(\"document.title\")\n  }\nend"
}'
```

### 7.2 调试信息
```lua
curl -X POST http://localhost:8050/execute \
-H 'Content-Type: application/json' \
-d '{
  "lua_source": "function main(splash)\n  splash:go(\"https://example.com\")\n  \n  -- 获取控制台日志\n  local console_logs = {}\n  splash:on_console_message(function(message)\n    table.insert(console_logs, message)\n  end)\n  \n  splash:wait(2)\n  \n  return {\n    html = splash:html(),\n    console_logs = console_logs,\n    requested_urls = splash:history(),\n    last_url = splash:url()\n  }\nend"
}'
```

## 8. 性能优化参数

### 8.1 缓存控制
```bash
# 禁用缓存
curl "http://localhost:8050/render.html?\
url=https://example.com&\
wait=1&\
cache_profile=no_cache"

# 使用缓存
curl "http://localhost:8050/render.html?\
url=https://example.com&\
cache_profile=per_domain"
```

### 8.2 内存和性能限制
```bash
# 限制资源使用
curl -X POST http://localhost:8050/execute \
-H 'Content-Type: application/json' \
-d '{
  "timeout": 60,
  "lua_source": "function main(splash)\n  splash:go(\"https://example.com\")\n  splash:wait(2)\n  return splash:html()\nend",
  "filters": "nostore"
}'
```

## 9. 批量处理

### 9.1 使用 Python 批量处理
```python
import requests
import json

def batch_render(urls):
    splash_url = "http://localhost:8050/execute"
    results = []
    
    for url in urls:
        lua_script = f"""
        function main(splash)
            splash:go("{url}")
            splash:wait(2)
            return {{
                url = splash:url(),
                title = splash:evaljs("document.title"),
                html = splash:html()
            }}
        end
        """
        
        response = requests.post(splash_url, json={
            "lua_source": lua_script,
            "timeout": 30
        })
        
        if response.status_code == 200:
            results.append(response.json())
        else:
            results.append({"error": f"Failed to render {url}"})
    
    return results

# 使用示例
urls = ["https://example.com", "https://example.org", "https://example.net"]
results = batch_render(urls)
print(json.dumps(results, indent=2))
```

## 10. 监控和健康检查

### 10.1 健康检查端点
```bash
# 检查 Splash 状态
curl "http://localhost:8050/_ping"

# 获取统计信息
curl "http://localhost:8050/_debug"

# 获取活跃任务数
curl "http://localhost:8050/_active_tasks"
```

