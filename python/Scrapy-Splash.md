# Scrapy-Splash 详细使用教程

## 1. 环境安装与配置

### 1.1 安装依赖
```bash
# 安装 scrapy-splash
pip install scrapy-splash

# 或者从源码安装
pip install git+https://github.com/scrapy-plugins/scrapy-splash
```

### 1.2 启动 Splash 服务
```bash
# 使用 Docker 启动 Splash
docker run -p 8050:8050 scrapinghub/splash

# 验证 Splash 是否正常运行
curl http://localhost:8050
```

### 1.3 Scrapy 项目配置

在 `settings.py` 中添加以下配置：

```python
# Splash 服务器地址
SPLASH_URL = 'http://localhost:8050'

# 启用 Splash 下载器中间件
DOWNLOADER_MIDDLEWARES = {
    'scrapy_splash.SplashCookiesMiddleware': 723,
    'scrapy_splash.SplashMiddleware': 725,
    'scrapy.downloadermiddlewares.httpcompression.HttpCompressionMiddleware': 810,
}

# 启用 Splash 爬虫中间件
SPIDER_MIDDLEWARES = {
    'scrapy_splash.SplashDeduplicateArgsMiddleware': 100,
}

# 使用 Splash 感知的去重过滤器
DUPEFILTER_CLASS = 'scrapy_splash.SplashAwareDupeFilter'

# 使用 Splash 感知的 HTTP 缓存
HTTPCACHE_STORAGE = 'scrapy_splash.SplashAwareFSCacheStorage'

# 可选：设置并发请求数
CONCURRENT_REQUESTS = 8

# 可选：设置下载延迟
DOWNLOAD_DELAY = 1
```

## 2. 基础用法

### 2.1 使用 SplashRequest

```python
import scrapy
from scrapy_splash import SplashRequest
import json

class BasicSplashSpider(scrapy.Spider):
    name = 'basic_splash'
    
    def start_requests(self):
        urls = [
            'https://example.com',
            'https://httpbin.org/headers'
        ]
        
        for url in urls:
            yield SplashRequest(
                url=url,
                callback=self.parse,
                # 等待页面渲染完成
                args={'wait': 2},
                # 缓存相同参数的请求
                dont_filter=False,
                # 设置超时
                meta={'splash_timeout': 30}
            )
    
    def parse(self, response):
        self.logger.info(f'Processing: {response.url}')
        
        yield {
            'url': response.url,
            #'title': response.css('title::text').get(),
            'content_length': len(response.body)
        }
```

### 2.2 使用 Lua 脚本

```python
class LuaScriptSpider(scrapy.Spider):
    name = 'lua_script'
    
    def start_requests(self):
        script = """
        function main(splash)
            splash:go(splash.args.url)
            splash:wait(2)
            
            -- 滚动页面
            splash:runjs("window.scrollTo(0, document.body.scrollHeight)")
            splash:wait(1)
            
            -- 获取页面信息
            return {
                html = splash:html(),
                url = splash:url(),
                title = splash:evaljs("document.title")
            }
        end
        """
        
        yield SplashRequest(
            url='https://example.com',
            callback=self.parse,
            endpoint='execute',
            args={
                'lua_source': script,
                'timeout': 60
            }
        )
    
    def parse(self, response):
        # 响应数据在 response.data 中
        data = response.data
        
        yield {
            'url': data['url'],
            'title': data['title'],
            'html': data['html'][:500] + '...' if len(data['html']) > 500 else data['html']
        }
```

## 3. 高级功能

### 3.1 处理 JavaScript 动态内容

```python
class DynamicContentSpider(scrapy.Spider):
    name = 'dynamic_content'
    
    def start_requests(self):
        script = """
        function main(splash)
            splash:go(splash.args.url)
            splash:wait(2)
            
            -- 等待特定元素加载
            splash:wait_for_selector(".dynamic-content")
            
            -- 点击加载更多按钮（如果存在）
            local load_more = splash:select("#load-more")
            if load_more then
                load_more:click()
                splash:wait(2)
            end
            
            -- 处理无限滚动
            for i=1,3 do
                splash:runjs("window.scrollTo(0, document.body.scrollHeight)")
                splash:wait(1)
            end
            
            return splash:html()
        end
        """
        
        yield SplashRequest(
            url='https://example.com/infinite-scroll',
            callback=self.parse,
            endpoint='execute',
            args={
                'lua_source': script,
                'wait': 5,
                'timeout': 90
            }
        )
    
    def parse(self, response):
        # 现在 response 包含所有动态加载的内容
        items = response.css('.item')
        
        for item in items:
            yield {
                'title': item.css('h2::text').get(),
                'content': item.css('.content::text').get()
            }
```

### 3.2 表单提交和登录

```python
class LoginSpider(scrapy.Spider):
    name = 'login_spider'
    
    def start_requests(self):
        login_script = """
        function main(splash)
            splash:go(splash.args.url)
            splash:wait(2)
            
            -- 填写登录表单
            splash:send_text("#username", splash.args.username)
            splash:send_text("#password", splash.args.password)
            splash:wait(0.5)
            
            -- 提交表单
            splash:runjs("document.querySelector('#login-form').submit()")
            splash:wait(3)
            
            -- 检查是否登录成功
            local error_msg = splash:select(".error-message")
            if error_msg then
                return {error = "Login failed"}
            end
            
            -- 跳转到目标页面
            splash:go(splash.args.target_url)
            splash:wait(2)
            
            return splash:html()
        end
        """
        
        yield SplashRequest(
            url='https://example.com/login',
            callback=self.parse_after_login,
            endpoint='execute',
            args={
                'lua_source': login_script,
                'username': 'your_username',
                'password': 'your_password',
                'target_url': 'https://example.com/dashboard',
                'timeout': 60
            }
        )
    
    def parse_after_login(self, response):
        if 'error' in response.data:
            self.logger.error(f'Login failed: {response.data["error"]}')
            return
        
        # 处理登录后的页面
        yield {
            'dashboard_title': response.css('h1::text').get(),
            'content': response.css('.dashboard-content').get()
        }
```

### 3.3 资源过滤和性能优化

```python
class OptimizedSpider(scrapy.Spider):
    name = 'optimized_spider'
    
    def start_requests(self):
        script = """
        function main(splash)
            -- 设置资源过滤
            splash:on_request(function(request)
                -- 阻止图片加载
                if request.url:find('%.(jpg|png|gif)$') then
                    request:abort()
                end
                
                -- 阻止广告和跟踪脚本
                if request.url:find('ads%.') or request.url:find('tracking%.') then
                    request:abort()
                end
            end)
            
            splash:go(splash.args.url)
            splash:wait(splash.args.wait_time or 2)
            
            return {
                html = splash:html(),
                url = splash:url(),
                stats = splash:history()
            }
        end
        """
        
        yield SplashRequest(
            url='https://example.com',
            callback=self.parse,
            endpoint='execute',
            args={
                'lua_source': script,
                'wait_time': 3,
                'images': 0,  # 禁用图片加载
                'resource_timeout': 30
            },
            meta={
                'splash_timeout': 60,
                'splash_resource_timeout': 30
            }
        )
    
    def parse(self, response):
        data = response.data
        
        yield {
            'url': data['url'],
            'page_size': len(data['html']),
            'requests_made': len(data['stats']),
            'title': response.css('title::text').get()
        }
```

## 4. 代理和请求头设置

### 4.1 使用代理

```python
class ProxySpider(scrapy.Spider):
    name = 'proxy_spider'
    
    def start_requests(self):
        script = """
        function main(splash)
            -- 设置代理
            if splash.args.proxy then
                splash:on_request(function(request)
                    request:set_proxy(splash.args.proxy)
                end)
            end
            
            -- 设置自定义请求头
            splash:set_custom_headers({
                ["User-Agent"] = splash.args.user_agent,
                ["Accept-Language"] = "en-US,en;q=0.9"
            })
            
            splash:go(splash.args.url)
            splash:wait(2)
            
            return splash:html()
        end
        """
        
        yield SplashRequest(
            url='https://httpbin.org/ip',
            callback=self.parse,
            endpoint='execute',
            args={
                'lua_source': script,
                'proxy': 'http://proxy.example.com:8080',
                'user_agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36'
            }
        )
    
    def parse(self, response):
        yield {
            'ip_info': response.text
        }
```

### 4.2 轮换 User-Agent 和代理

```python
from scrapy_splash import SplashRequest

class RotatingSpider(scrapy.Spider):
    name = 'rotating_spider'
    
    # 用户代理列表
    USER_AGENTS = [
        'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36',
        'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15',
        'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36'
    ]
    
    # 代理列表
    PROXIES = [
        'http://proxy1.example.com:8080',
        'http://proxy2.example.com:8080',
        'http://proxy3.example.com:8080'
    ]
    
    def start_requests(self):
        urls = ['https://httpbin.org/headers'] * 6
        
        for i, url in enumerate(urls):
            user_agent = self.USER_AGENTS[i % len(self.USER_AGENTS)]
            proxy = self.PROXIES[i % len(self.PROXIES)] if self.PROXIES else None
            
            yield SplashRequest(
                url=url,
                callback=self.parse,
                args={
                    'wait': 1,
                    'lua_source': """
                    function main(splash)
                        splash:set_user_agent(splash.args.ua)
                        if splash.args.proxy then
                            splash:set_proxy(splash.args.proxy)
                        end
                        splash:go(splash.args.url)
                        splash:wait(1)
                        return splash:html()
                    end
                    """
                },
                endpoint='execute',
                meta={
                    'user_agent': user_agent,
                    'proxy': proxy,
                    'splash_args': {
                        'ua': user_agent,
                        'proxy': proxy
                    }
                }
            )
    
    def parse(self, response):
        yield {
            'user_agent': response.meta['user_agent'],
            'proxy': response.meta['proxy'],
            'response': response.text[:200] + '...'
        }
```

## 5. 错误处理和重试

### 5.1 自定义错误处理

```python
from scrapy_splash import SplashRequest
from scrapy.downloadermiddlewares.retry import RetryMiddleware
from scrapy.utils.response import response_status_message

class RobustSplashSpider(scrapy.Spider):
    name = 'robust_splash'
    
    def start_requests(self):
        script = """
        function main(splash)
            local ok, reason = splash:go(splash.args.url)
            if not ok then
                return {error = reason}
            end
            
            splash:wait(2)
            
            -- 检查页面是否正常加载
            local title = splash:evaljs("document.title")
            if not title or title:find("Error") then
                return {error = "Page load error", title = title}
            end
            
            return {
                html = splash:html(),
                title = title,
                url = splash:url()
            }
        end
        """
        
        yield SplashRequest(
            url='https://example.com',
            callback=self.parse,
            endpoint='execute',
            args={
                'lua_source': script,
                'timeout': 60
            },
            errback=self.errback,
            meta={
                'max_retry_times': 3,
                'splash_timeout': 60
            }
        )
    
    def parse(self, response):
        # 检查 Lua 脚本是否返回错误
        if 'error' in response.data:
            self.logger.error(f"Splash error: {response.data['error']}")
            return
        
        yield {
            'url': response.data['url'],
            'title': response.data['title'],
            'content': response.data['html'][:500] + '...'
        }
    
    def errback(self, failure):
        # 处理请求失败
        self.logger.error(f"Request failed: {failure}")
        
        # 可以在这里实现重试逻辑
        if failure.check(scrapy.exceptions.TimeoutError):
            self.logger.error("Timeout error")
        elif failure.check(scrapy.exceptions.HttpError):
            self.logger.error("HTTP error")

# 自定义重试中间件
class CustomRetryMiddleware(RetryMiddleware):
    def process_response(self, request, response, spider):
        # 检查 Splash 响应中的错误
        if hasattr(response, 'data') and 'error' in response.data:
            reason = f"Splash error: {response.data['error']}"
            return self._retry(request, reason, spider) or response
        
        return super().process_response(request, response, spider)
```

## 6. 数据提取和管道处理

### 6.1 结合 Item Loader

```python
from scrapy.loader import ItemLoader
from myproject.items import MyItem

class ItemSpider(scrapy.Spider):
    name = 'item_spider'
    
    def start_requests(self):
        yield SplashRequest(
            url='https://example.com/products',
            callback=self.parse_products,
            args={'wait': 2}
        )
    
    def parse_products(self, response):
        products = response.css('.product-item')
        
        for product in products:
            loader = ItemLoader(item=MyItem(), selector=product)
            
            loader.add_css('name', 'h2::text')
            loader.add_css('price', '.price::text')
            loader.add_css('description', '.description::text')
            loader.add_value('url', response.url)
            
            yield loader.load_item()

# items.py
import scrapy

class MyItem(scrapy.Item):
    name = scrapy.Field()
    price = scrapy.Field()
    description = scrapy.Field()
    url = scrapy.Field()
```

### 6.2 分页处理

```python
class PaginationSpider(scrapy.Spider):
    name = 'pagination_spider'
    
    def start_requests(self):
        yield SplashRequest(
            url='https://example.com/products?page=1',
            callback=self.parse_page,
            args={'wait': 2}
        )
    
    def parse_page(self, response):
        # 提取当前页面的产品
        products = response.css('.product')
        for product in products:
            yield {
                'name': product.css('h3::text').get(),
                'price': product.css('.price::text').get()
            }
        
        # 查找下一页链接
        next_page = response.css('.next-page::attr(href)').get()
        if next_page:
            next_url = response.urljoin(next_page)
            yield SplashRequest(
                url=next_url,
                callback=self.parse_page,
                args={'wait': 2}
            )
```

## 7. 性能监控和调试

### 7.1 添加统计信息

```python
class MonitoringSpider(scrapy.Spider):
    name = 'monitoring_spider'
    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.stats = {
            'successful_requests': 0,
            'failed_requests': 0,
            'total_response_time': 0
        }
    
    def start_requests(self):
        urls = ['https://example.com'] * 10
        
        for url in urls:
            yield SplashRequest(
                url=url,
                callback=self.parse,
                args={'wait': 1},
                meta={'start_time': time.time()}
            )
    
    def parse(self, response):
        end_time = time.time()
        start_time = response.meta['start_time']
        response_time = end_time - start_time
        
        self.stats['successful_requests'] += 1
        self.stats['total_response_time'] += response_time
        
        yield {
            'url': response.url,
            'response_time': response_time,
            'status': 'success'
        }
    
    def closed(self, reason):
        avg_response_time = (self.stats['total_response_time'] / 
                           self.stats['successful_requests'] if self.stats['successful_requests'] > 0 else 0)
        
        self.logger.info(f"Spider finished: {reason}")
        self.logger.info(f"Successful requests: {self.stats['successful_requests']}")
        self.logger.info(f"Failed requests: {self.stats['failed_requests']}")
        self.logger.info(f"Average response time: {avg_response_time:.2f}s")
```
