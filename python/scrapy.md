# scrapy

- https://github.com/scrapy/scrapy

## 安装

```shell
pip install scrapy
pip install scrapy-user-agents
```

## 新建项目

```shell
scrapy startproject google_search
```

### 新建爬虫
```shell
scrapy genspider google_search www.google.com
```

此时项目目录结构如下：
```txt
google_scraper/
├── google_scraper/
│   ├── __init__.py
│   ├── items.py
│   ├── middlewares.py
│   ├── pipelines.py
│   ├── settings.py
│   └── spiders/
│       ├── __init__.py
│       └── google_search.py
└── scrapy.cfg
```

### 1. 定义 Item
编辑 items.py 文件：
```py
import scrapy

class GoogleSearchItem(scrapy.Item):
    title = scrapy.Field()
    link = scrapy.Field()
    snippet = scrapy.Field()
    position = scrapy.Field()
    query = scrapy.Field()
    search_time = scrapy.Field()
```

### 2. 配置设置
编辑 settings.py 文件，添加以下配置：
```py
BOT_NAME = 'google_scraper'

SPIDER_MODULES = ['google_scraper.spiders']
NEWSPIDER_MODULE = 'google_scraper.spiders'

# 遵守 robots.txt 规则 (注意: Google 不允许爬虫，所以在生产环境中请谨慎使用)
ROBOTSTXT_OBEY = False

# 配置默认请求头
DEFAULT_REQUEST_HEADERS = {
    'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
    'Accept-Language': 'en',
    'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36',
}

# 启用下载中间件
DOWNLOADER_MIDDLEWARES = {
    'google_scraper.middlewares.GoogleScraperDownloaderMiddleware': 543,
    'scrapy.downloadermiddlewares.useragent.UserAgentMiddleware': None,
    'scrapy_user_agents.middlewares.RandomUserAgentMiddleware': 400,
}

# 配置延迟下载，避免请求过于频繁
DOWNLOAD_DELAY = 5
RANDOMIZE_DOWNLOAD_DELAY = True

# 配置并发请求数
CONCURRENT_REQUESTS = 1

# 启用 Cookie
COOKIES_ENABLED = True

# 配置重试次数
RETRY_ENABLED = True
RETRY_TIMES = 3
RETRY_HTTP_CODES = [500, 502, 503, 504, 403, 404, 408]

# 配置输出
FEED_EXPORT_ENCODING = 'utf-8'
```

### 3. 修改中间件
编辑 middlewares.py 文件，添加以下代码：

## 例子

- https://github.com/hanxweb/Scrapy-SearchEngines
- https://github.com/naqushab/SearchEngineScrapy
