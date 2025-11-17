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

这个教程涵盖了 Splash 的基本使用和高级功能，可以帮助你开始使用 Splash 来处理 JavaScript 渲染的网页。
