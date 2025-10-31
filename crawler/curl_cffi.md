curl_cffi 完整配置指南：impersonate、TLS、HTTP/2 指纹和代理设置

下面是一个完整的 curl_cffi 使用指南，涵盖 impersonate 参数、TLS/HTTP/2 指纹模拟以及代理设置。

1. 完整的 curl_cffi 客户端实现

```python
# complete_curl_cffi_client.py
from curl_cffi import requests
import random
import time
import threading
from urllib.parse import urlparse
from fake_useragent import UserAgent
from user_agents import parse
import json

class CompleteCurlCffiClient:
    def __init__(self, 
                 proxy_url=None,
                 default_language="zh",
                 default_platform="windows", 
                 default_browser="chrome",
                 verify_ssl=False,
                 timeout=30,
                 max_retries=3):
        """
        完整的 curl_cffi 客户端
        
        参数:
            proxy_url: 代理地址 (例如: "http://127.0.0.1:8080")
            default_language: 默认语言 (zh, en, mixed)
            default_platform: 默认平台 (windows, macos, linux, mobile, tablet)
            default_browser: 默认浏览器 (chrome, firefox, safari, edge, random)
            verify_ssl: 是否验证 SSL 证书
            timeout: 请求超时时间
            max_retries: 最大重试次数
        """
        self.proxy_url = proxy_url
        self.default_language = default_language
        self.default_platform = default_platform
        self.default_browser = default_browser
        self.verify_ssl = verify_ssl
        self.timeout = timeout
        self.max_retries = max_retries
        
        self.session_storage = {}
        self.lock = threading.Lock()
        
        # 初始化 UA 生成器
        try:
            self.ua_generator = UserAgent()
            print("✓ fake-useragent 初始化成功")
        except Exception as e:
            print(f"⚠️ fake-useragent 初始化失败: {e}")
            self.ua_generator = None
        
        # 浏览器 impersonate 配置映射
        self.browser_impersonate_map = {
            'chrome': [
                "chrome110", "chrome107", "chrome104", "chrome101", 
                "chrome100", "chrome99", "chrome95", "chrome90"
            ],
            'firefox': [
                "firefox110", "firefox108", "firefox105", "firefox102",
                "firefox100", "firefox98", "firefox95"
            ],
            'safari': [
                "safari15_5", "safari15_3", "safari15_1", "safari14_1",
                "safari13_1", "safari12_1"
            ],
            'edge': [
                "edge99", "edge101", "edge95", "edge90"
            ]
        }
        
        # 语言配置
        self.language_configs = {
            'zh': {
                'name': '中文',
                'accept_language': self._generate_accept_language(['zh-CN', 'zh', 'en-US', 'en'], [1.0, 0.8, 0.6, 0.4]),
            },
            'en': {
                'name': '英文', 
                'accept_language': self._generate_accept_language(['en-US', 'en', 'zh-CN', 'zh'], [1.0, 0.9, 0.3, 0.2]),
            },
            'mixed': {
                'name': '混合语言',
                'accept_language': self._generate_accept_language(['zh-CN', 'en-US', 'en', 'zh-TW', 'zh'], [1.0, 0.7, 0.6, 0.5, 0.4]),
            }
        }
        
        # 平台配置
        self.platform_configs = {
            'windows': {
                'name': 'Windows',
                'sec_platform': '"Windows"'
            },
            'macos': {
                'name': 'macOS', 
                'sec_platform': '"macOS"'
            },
            'linux': {
                'name': 'Linux',
                'sec_platform': '"Linux"'
            },
            'mobile': {
                'name': '移动设备',
                'sec_platform': '"Android"'
            },
            'tablet': {
                'name': '平板设备', 
                'sec_platform': '"iPad"'
            }
        }
        
        print(f"curl_cffi 客户端初始化完成")
        if proxy_url:
            print(f"✓ 代理设置: {proxy_url}")
        print(f"✓ 默认语言: {self.language_configs[default_language]['name']}")
        print(f"✓ 默认平台: {self.platform_configs[default_platform]['name']}")
        print(f"✓ 默认浏览器: {default_browser}")
        print(f"✓ SSL 验证: {'开启' if verify_ssl else '关闭'}")
    
    def _generate_accept_language(self, languages, weights):
        """生成 Accept-Language 头部值"""
        language_parts = []
        for i, lang in enumerate(languages):
            if i < len(weights):
                q = weights[i]
                language_parts.append(f"{lang};q={q:.1f}")
            else:
                language_parts.append(lang)
        
        if len(language_parts) > 1:
            secondary_langs = language_parts[1:]
            random.shuffle(secondary_langs)
            language_parts = [language_parts[0]] + secondary_langs
        
        return ', '.join(language_parts)
    
    def get_impersonate_target(self, browser_type, platform):
        """根据浏览器类型和平台获取 impersonate 目标"""
        if browser_type == "random":
            # 随机选择浏览器类型
            browser_type = random.choice(list(self.browser_impersonate_map.keys()))
        
        if browser_type in self.browser_impersonate_map:
            targets = self.browser_impersonate_map[browser_type]
            return random.choice(targets)
        else:
            # 默认使用 Chrome
            return random.choice(self.browser_impersonate_map['chrome'])
    
    def get_random_user_agent(self, platform=None, browser_type=None):
        """根据平台和浏览器类型获取随机 User-Agent"""
        try:
            if self.ua_generator:
                if platform == "windows":
                    ua = self.ua_generator.windows
                elif platform == "macos":
                    ua = self.ua_generator.mac
                elif platform == "linux":
                    ua = self.ua_generator.linux
                elif platform in ["mobile", "tablet"]:
                    ua = self.ua_generator.mobile
                else:
                    ua = self.ua_generator.random
                
                # 确保 UA 匹配浏览器类型
                if browser_type and browser_type != "random":
                    max_attempts = 10
                    for _ in range(max_attempts):
                        if browser_type in ua.lower():
                            return ua
                        ua = self.ua_generator.random
                
                return ua
            else:
                return self._get_fallback_ua(platform, browser_type)
        except Exception as e:
            print(f"获取 User-Agent 失败: {e}")
            return self._get_fallback_ua(platform, browser_type)
    
    def _get_fallback_ua(self, platform, browser_type):
        """备用的 UA 生成"""
        base_ua = {
            'windows': {
                'chrome': "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36",
                'firefox': "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/119.0",
                'edge': "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36 Edg/119.0.0.0",
            },
            'macos': {
                'chrome': "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36",
                'firefox': "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:109.0) Gecko/20100101 Firefox/119.0",
                'safari': "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.0 Safari/605.1.15",
            },
            'linux': {
                'chrome': "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36",
                'firefox': "Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/119.0"
            },
            'mobile': {
                'chrome': "Mozilla/5.0 (Linux; Android 10; SM-G973F) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Mobile Safari/537.36",
                'safari': "Mozilla/5.0 (iPhone; CPU iPhone OS 16_6 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/16.6 Mobile/15E148 Safari/604.1"
            }
        }
        
        if platform not in base_ua:
            platform = 'windows'
        
        if browser_type and browser_type in base_ua[platform]:
            return base_ua[platform][browser_type]
        else:
            return list(base_ua[platform].values())[0]
    
    def parse_user_agent(self, user_agent_string):
        """解析 User-Agent 字符串"""
        try:
            ua = parse(user_agent_string)
            return {
                'browser_family': ua.browser.family.lower(),
                'browser_version': ua.browser.version_string,
                'os_family': ua.os.family.lower(),
                'os_version': ua.os.version_string,
                'is_mobile': ua.is_mobile,
                'is_tablet': ua.is_tablet,
                'is_pc': ua.is_pc,
                'device_family': ua.device.family.lower()
            }
        except Exception as e:
            ua_lower = user_agent_string.lower()
            return {
                'browser_family': 'chrome' if 'chrome' in ua_lower else 'firefox' if 'firefox' in ua_lower else 'safari' if 'safari' in ua_lower and 'chrome' not in ua_lower else 'edge' if 'edg' in ua_lower else 'chrome',
                'browser_version': '119.0',
                'os_family': 'windows' if 'windows' in ua_lower else 'mac' if 'mac' in ua_lower else 'linux' if 'linux' in ua_lower else 'windows',
                'os_version': '10',
                'is_mobile': any(x in ua_lower for x in ['mobile', 'android', 'iphone']),
                'is_tablet': any(x in ua_lower for x in ['tablet', 'ipad']),
                'is_pc': not any(x in ua_lower for x in ['mobile', 'android', 'iphone', 'tablet', 'ipad']),
                'device_family': 'other'
            }
    
    def generate_accept_header(self, ua_info):
        """生成 Accept 头部"""
        browser_family = ua_info['browser_family'].lower()
        device_type = 'mobile' if ua_info['is_mobile'] else 'desktop'
        
        accept_configs = {
            'chrome': {
                'desktop': "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7",
                'mobile': "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7"
            },
            'firefox': {
                'desktop': "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8",
                'mobile': "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8"
            },
            'safari': {
                'desktop': "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
                'mobile': "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"
            },
            'edge': {
                'desktop': "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7",
                'mobile': "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7"
            }
        }
        
        if browser_family in accept_configs:
            return accept_configs[browser_family][device_type]
        else:
            return accept_configs['chrome'][device_type]
    
    def generate_accept_encoding_header(self, ua_info):
        """生成 Accept-Encoding 头部"""
        browser_family = ua_info['browser_family'].lower()
        
        if browser_family in ['chrome', 'firefox', 'edge']:
            encodings = ['gzip', 'deflate', 'br', 'zstd']
        elif browser_family == 'safari':
            encodings = ['gzip', 'deflate', 'br']
        else:
            encodings = ['gzip', 'deflate', 'br']
        
        if ua_info['is_mobile'] and 'zstd' in encodings:
            encodings = [e for e in encodings if e != 'zstd']
        
        if random.random() > 0.3:
            base = encodings[0]
            others = encodings[1:]
            random.shuffle(others)
            encodings = [base] + others
        else:
            random.shuffle(encodings)
        
        return ', '.join(encodings)
    
    def get_session(self, session_id=None, language=None, platform=None, browser=None):
        """获取或创建会话"""
        if session_id is None:
            session_id = threading.current_thread().ident
            
        language = language or self.default_language
        platform = platform or self.default_platform
        browser = browser or self.default_browser
        
        with self.lock:
            if session_id not in self.session_storage:
                # 获取 impersonate 目标
                impersonate_target = self.get_impersonate_target(browser, platform)
                
                # 生成 User-Agent
                user_agent = self.get_random_user_agent(platform, browser)
                
                # 解析 UA 信息
                ua_info = self.parse_user_agent(user_agent)
                
                # 生成其他头部
                accept = self.generate_accept_header(ua_info)
                accept_language = self.language_configs[language]['accept_language']
                accept_encoding = self.generate_accept_encoding_header(ua_info)
                
                # 创建 curl_cffi Session
                session = requests.Session()
                
                self.session_storage[session_id] = {
                    'session': session,
                    'impersonate_target': impersonate_target,
                    'user_agent': user_agent,
                    'language': language,
                    'platform': platform,
                    'browser': browser,
                    'ua_info': ua_info,
                    'headers': {
                        'User-Agent': user_agent,
                        'Accept': accept,
                        'Accept-Language': accept_language,
                        'Accept-Encoding': accept_encoding
                    },
                    'last_request_time': 0,
                    'request_count': 0,
                    'domain_cookies': {}
                }
                
                print(f"创建会话 {session_id}: "
                      f"{self.platform_configs[platform]['name']} - "
                      f"{browser} ({impersonate_target}) - "
                      f"{self.language_configs[language]['name']}")
                
            return self.session_storage[session_id]
    
    def generate_sec_headers(self, session_data):
        """生成安全相关头部"""
        ua_info = session_data['ua_info']
        platform = session_data['platform']
        
        try:
            major_version = ua_info['browser_version'].split('.')[0]
        except:
            major_version = "119"
        
        platform_header = self.platform_configs[platform]['sec_platform']
        browser_family = ua_info['browser_family']
        
        if browser_family == "chrome":
            return {
                "Sec-Ch-Ua": f'"Google Chrome";v="{major_version}", "Chromium";v="{major_version}", "Not?A_Brand";v="24"',
                "Sec-Ch-Ua-Mobile": "?0",
                "Sec-Ch-Ua-Platform": platform_header,
            }
        elif browser_family == "firefox":
            return {
                "Sec-Ch-Ua": f'"Not.A/Brand";v="8", "Chromium";v="{major_version}", "Google Chrome";v="{major_version}"',
                "Sec-Ch-Ua-Mobile": "?0", 
                "Sec-Ch-Ua-Platform": platform_header,
            }
        elif browser_family == "safari":
            return {
                "Sec-Ch-Ua": f'"Google Chrome";v="{major_version}", "Chromium";v="{major_version}", "Not?A_Brand";v="24"',
                "Sec-Ch-Ua-Mobile": "?0",
                "Sec-Ch-Ua-Platform": platform_header,
            }
        elif browser_family == "edge":
            return {
                "Sec-Ch-Ua": f'"Microsoft Edge";v="{major_version}", "Chromium";v="{major_version}", "Not?A_Brand";v="24"',
                "Sec-Ch-Ua-Mobile": "?0",
                "Sec-Ch-Ua-Platform": platform_header,
            }
        else:
            return {
                "Sec-Ch-Ua": f'"Google Chrome";v="{major_version}", "Chromium";v="{major_version}", "Not?A_Brand";v="24"',
                "Sec-Ch-Ua-Mobile": "?0",
                "Sec-Ch-Ua-Platform": platform_header,
            }
    
    def make_request(self, url, method="GET", headers=None, data=None, 
                    delay=True, session_id=None, language=None, 
                    platform=None, browser=None, retry_count=0):
        """发送请求"""
        session_data = self.get_session(session_id, language, platform, browser)
        session = session_data['session']
        impersonate_target = session_data['impersonate_target']
        
        # 人类化延迟
        if delay and session_data['request_count'] > 0:
            current_time = time.time()
            time_since_last = current_time - session_data['last_request_time']
            min_delay = random.uniform(1, 3)
            if time_since_last < min_delay:
                sleep_time = min_delay - time_since_last + random.uniform(0.1, 0.5)
                time.sleep(sleep_time)
        
        # 准备请求头部
        request_headers = session_data['headers'].copy()
        
        # 添加安全头部
        sec_headers = self.generate_sec_headers(session_data)
        request_headers.update(sec_headers)
        
        # 添加 Fetch 头部
        fetch_headers = {
            "Sec-Fetch-Dest": "document",
            "Sec-Fetch-Mode": "navigate",
            "Sec-Fetch-Site": "none",
            "Sec-Fetch-User": "?1",
        }
        request_headers.update(fetch_headers)
        
        # 添加其他头部
        request_headers["Upgrade-Insecure-Requests"] = "1"
        if random.random() > 0.5:
            request_headers["Dnt"] = "1"
        request_headers["Cache-Control"] = "max-age=0"
        
        # 添加自定义头部
        if headers:
            request_headers.update(headers)
        
        # 准备代理设置
        proxies = None
        if self.proxy_url:
            proxies = {
                'http': self.proxy_url,
                'https': self.proxy_url
            }
        
        # 准备请求参数
        request_kwargs = {
            'method': method,
            'url': url,
            'impersonate': impersonate_target,  # 关键：设置 TLS/HTTP2 指纹模拟
            'headers': request_headers,
            'timeout': self.timeout,
            'verify': self.verify_ssl
        }
        
        # 添加代理设置
        if proxies:
            request_kwargs['proxies'] = proxies
        
        if data:
            if method.upper() in ['POST', 'PUT', 'PATCH']:
                if isinstance(data, (dict, list)):
                    request_kwargs['json'] = data
                else:
                    request_kwargs['data'] = data
        
        try:
            # 发送请求
            start_time = time.time()
            response = session.request(**request_kwargs)
            response_time = time.time() - start_time
            
            # 更新会话状态
            with self.lock:
                session_data['last_request_time'] = time.time()
                session_data['request_count'] += 1
                
                # 存储域名cookies
                domain = urlparse(url).netloc
                if hasattr(response, 'cookies') and response.cookies:
                    session_data['domain_cookies'][domain] = dict(response.cookies)
            
            # 记录请求信息
            self.log_request(url, session_data, response.status_code, response_time)
            
            return response
            
        except Exception as e:
            print(f"请求失败 ({retry_count + 1}/{self.max_retries + 1}): {e}")
            
            # 重试逻辑
            if retry_count < self.max_retries:
                wait_time = (2 ** retry_count) + random.uniform(0.1, 0.5)
                print(f"等待 {wait_time:.2f} 秒后重试...")
                time.sleep(wait_time)
                return self.make_request(
                    url, method, headers, data, delay, 
                    session_id, language, platform, browser, retry_count + 1
                )
            else:
                print(f"请求失败，已达到最大重试次数: {url}")
                return None
    
    def log_request(self, url, session_data, status_code, response_time):
        """记录请求信息"""
        domain = urlparse(url).netloc
        platform_name = self.platform_configs[session_data['platform']]['name']
        browser_name = session_data['browser']
        impersonate_target = session_data['impersonate_target']
        language_name = self.language_configs[session_data['language']]['name']
        
        status_color = "🟢" if status_code == 200 else "🟡" if status_code < 400 else "🔴"
        
        print(f"{status_color} [{platform_name} - {browser_name}({impersonate_target}) - {language_name}] "
              f"{domain} -> {status_code} ({response_time:.2f}s)")
    
    def get_session_info(self, session_id=None):
        """获取会话信息"""
        session_data = self.get_session(session_id)
        return {
            'session_id': session_id or threading.current_thread().ident,
            'platform': self.platform_configs[session_data['platform']]['name'],
            'browser': session_data['browser'],
            'impersonate_target': session_data['impersonate_target'],
            'language': self.language_configs[session_data['language']]['name'],
            'user_agent': session_data['user_agent'],
            'accept': session_data['headers']['Accept'],
            'accept_language': session_data['headers']['Accept-Language'],
            'accept_encoding': session_data['headers']['Accept-Encoding'],
            'request_count': session_data['request_count'],
            'proxy': self.proxy_url
        }
    
    def get_available_impersonate_targets(self):
        """获取可用的 impersonate 目标"""
        return self.browser_impersonate_map
    
    def close_session(self, session_id=None):
        """关闭会话"""
        if session_id is None:
            session_id = threading.current_thread().ident
            
        with self.lock:
            if session_id in self.session_storage:
                # curl_cffi 的 Session 不需要显式关闭，但我们可以清理资源
                del self.session_storage[session_id]
                print(f"会话 {session_id} 已关闭")
    
    def close_all_sessions(self):
        """关闭所有会话"""
        with self.lock:
            session_count = len(self.session_storage)
            self.session_storage.clear()
            print(f"所有会话已关闭 (共 {session_count} 个)")
```

2. 高级使用示例

```python
# advanced_usage_examples.py
from complete_curl_cffi_client import CompleteCurlCffiClient
import time
import random
from concurrent.futures import ThreadPoolExecutor, as_completed

def demo_basic_usage():
    """基础使用示例"""
    print("=== curl_cffi 基础使用示例 ===")
    
    # 创建不代理的客户端
    client = CompleteCurlCffiClient(
        proxy_url=None,  # 不使用代理
        default_language="zh",
        default_platform="windows",
        default_browser="chrome",
        verify_ssl=False
    )
    
    # 获取会话信息
    info = client.get_session_info()
    print(f"会话配置: {info['platform']} - {info['browser']}({info['impersonate_target']}) - {info['language']}")
    
    # 发送请求
    response = client.make_request("https://httpbin.org/headers")
    if response and response.status_code == 200:
        print("请求成功!")
        headers = response.json()['headers']
        print(f"User-Agent: {headers.get('User-Agent', 'N/A')}")
    
    client.close_all_sessions()
    print()

def demo_with_proxy():
    """使用代理示例"""
    print("=== 使用代理示例 ===")
    
    # 创建使用 mitmproxy 的客户端
    client = CompleteCurlCffiClient(
        proxy_url="http://127.0.0.1:8080",  # mitmproxy 代理
        default_language="en",
        default_platform="macos", 
        default_browser="safari",
        verify_ssl=False  # 使用代理时需要关闭 SSL 验证
    )
    
    info = client.get_session_info()
    print(f"代理配置: {info['proxy']}")
    print(f"会话配置: {info['platform']} - {info['browser']}({info['impersonate_target']}) - {info['language']}")
    
    response = client.make_request("https://httpbin.org/headers")
    if response and response.status_code == 200:
        print("通过代理请求成功!")
    
    client.close_all_sessions()
    print()

def demo_different_browsers():
    """不同浏览器指纹示例"""
    print("=== 不同浏览器指纹示例 ===")
    
    browsers = ['chrome', 'firefox', 'safari', 'edge']
    
    for browser in browsers:
        client = CompleteCurlCffiClient(
            default_browser=browser,
            verify_ssl=False
        )
        
        info = client.get_session_info()
        print(f"{browser.upper()}: impersonate={info['impersonate_target']}, UA={info['user_agent'][:50]}...")
        
        response = client.make_request("https://httpbin.org/user-agent")
        if response and response.status_code == 200:
            returned_ua = response.json().get('user-agent', 'N/A')
            print(f"  验证: {returned_ua[:50]}...")
        
        client.close_all_sessions()
        time.sleep(1)
    
    print()

def demo_platform_specific():
    """平台特定配置示例"""
    print("=== 平台特定配置示例 ===")
    
    platforms = ['windows', 'macos', 'linux', 'mobile']
    
    for platform in platforms:
        client = CompleteCurlCffiClient(
            default_platform=platform,
            default_browser="chrome",
            verify_ssl=False
        )
        
        info = client.get_session_info()
        print(f"{platform.upper()}: {info['user_agent'][:60]}...")
        
        client.close_all_sessions()
        time.sleep(0.5)
    
    print()

def demo_parallel_requests():
    """并行请求示例"""
    print("=== 并行请求示例 ===")
    
    def worker(worker_id, client):
        """工作线程函数"""
        # 每个工作线程使用不同的配置
        languages = ["zh", "en", "mixed"]
        platforms = ["windows", "macos", "linux"]
        browsers = ["chrome", "firefox", "safari", "edge"]
        
        language = random.choice(languages)
        platform = random.choice(platforms)
        browser = random.choice(browsers)
        
        session_info = client.get_session_info(worker_id)
        print(f"工作线程 {worker_id}: {session_info['platform']} - {session_info['browser']}({session_info['impersonate_target']}) - {session_info['language']}")
        
        response = client.make_request(
            "https://httpbin.org/headers",
            session_id=worker_id,
            language=language,
            platform=platform,
            browser=browser
        )
        
        if response and response.status_code == 200:
            return f"工作线程 {worker_id} 成功"
        else:
            return f"工作线程 {worker_id} 失败"
    
    # 创建客户端（不使用代理）
    client = CompleteCurlCffiClient(verify_ssl=False)
    
    # 并行执行
    with ThreadPoolExecutor(max_workers=3) as executor:
        futures = [
            executor.submit(worker, i, client)
            for i in range(6)
        ]
        
        for future in as_completed(futures):
            result = future.result()
            print(result)
    
    client.close_all_sessions()
    print()

def demo_fingerprint_verification():
    """指纹验证示例"""
    print("=== 指纹验证示例 ===")
    
    # 测试指纹验证网站
    test_urls = [
        "https://tls.browserleaks.com/json",
        "https://httpbin.org/headers",
        "https://ja3er.com/json"
    ]
    
    client = CompleteCurlCffiClient(
        default_browser="chrome",
        verify_ssl=False
    )
    
    for url in test_urls:
        print(f"测试 {url}:")
        response = client.make_request(url)
        if response and response.status_code == 200:
            try:
                data = response.json()
                if "ja3" in data:
                    print(f"  JA3 指纹: {data['ja3']}")
                if "akamai" in data:
                    print(f"  HTTP/2 指纹: {data['akamai']}")
                if "headers" in data:
                    ua = data['headers'].get('User-Agent', 'N/A')
                    print(f"  User-Agent: {ua[:80]}...")
            except:
                print(f"  响应: {response.text[:100]}...")
        else:
            print(f"  请求失败: {response.status_code if response else '无响应'}")
        
        print()
        time.sleep(2)
    
    client.close_all_sessions()

if __name__ == "__main__":
    demo_basic_usage()
    demo_with_proxy()
    demo_different_browsers()
    demo_platform_specific()
    demo_parallel_requests()
    demo_fingerprint_verification()
```

3. 配置管理器

```python
# client_config_manager.py
import json
from complete_curl_cffi_client import CompleteCurlCffiClient

class ClientConfigManager:
    """客户端配置管理器"""
    
    def __init__(self, config_file="client_configs.json"):
        self.config_file = config_file
        self.configs = self.load_configs()
    
    def load_configs(self):
        """加载配置"""
        default_configs = {
            'default_chinese': {
                'name': '默认中文用户',
                'proxy_url': None,
                'language': 'zh',
                'platform': 'windows',
                'browser': 'chrome',
                'verify_ssl': False,
                'description': '典型的中文Windows Chrome用户'
            },
            'english_mac_user': {
                'name': '英文Mac用户',
                'proxy_url': None, 
                'language': 'en',
                'platform': 'macos',
                'browser': 'safari',
                'verify_ssl': False,
                'description': '典型的英文macOS Safari用户'
            },
            'proxy_chinese': {
                'name': '代理中文用户',
                'proxy_url': 'http://127.0.0.1:8080',
                'language': 'zh',
                'platform': 'windows',
                'browser': 'chrome', 
                'verify_ssl': False,
                'description': '使用代理的中文Windows用户'
            },
            'mobile_user': {
                'name': '移动用户',
                'proxy_url': None,
                'language': 'zh', 
                'platform': 'mobile',
                'browser': 'chrome',
                'verify_ssl': False,
                'description': '中文移动用户'
            },
            'random_user': {
                'name': '随机用户',
                'proxy_url': None,
                'language': 'random',
                'platform': 'random',
                'browser': 'random',
                'verify_ssl': False,
                'description': '随机配置的用户'
            }
        }
        
        try:
            with open(self.config_file, 'r', encoding='utf-8') as f:
                return json.load(f)
        except:
            return default_configs
    
    def save_configs(self):
        """保存配置到文件"""
        with open(self.config_file, 'w', encoding='utf-8') as f:
            json.dump(self.configs, f, indent=2, ensure_ascii=False)
        print(f"配置已保存到 {self.config_file}")
    
    def create_client(self, config_name, **overrides):
        """根据配置名称创建客户端"""
        if config_name not in self.configs:
            raise ValueError(f"未知的配置: {config_name}")
        
        config = self.configs[config_name].copy()
        config.update(overrides)  # 应用覆盖参数
        
        client = CompleteCurlCffiClient(
            proxy_url=config['proxy_url'],
            default_language=config['language'],
            default_platform=config['platform'],
            default_browser=config['browser'],
            verify_ssl=config['verify_ssl']
        )
        
        return client, config
    
    def list_configs(self):
        """列出所有可用配置"""
        print("可用客户端配置:")
        print("=" * 60)
        for config_name, config in self.configs.items():
            print(f"{config_name}:")
            print(f"  名称: {config['name']}")
            print(f"  描述: {config['description']}")
            print(f"  代理: {config['proxy_url'] or '无'}")
            print(f"  语言: {config['language']}")
            print(f"  平台: {config['platform']}")
            print(f"  浏览器: {config['browser']}")
            print(f"  SSL验证: {'开启' if config['verify_ssl'] else '关闭'}")
            print()
    
    def add_config(self, config_name, config_data):
        """添加新配置"""
        self.configs[config_name] = config_data
        self.save_configs()
        print(f"配置 '{config_name}' 已添加")

# 使用示例
if __name__ == "__main__":
    manager = ClientConfigManager()
    manager.list_configs()
    
    # 测试不同配置
    print("测试不同配置:")
    print("=" * 40)
    
    test_configs = ['default_chinese', 'english_mac_user', 'mobile_user']
    
    for config_name in test_configs:
        print(f"\n测试配置: {config_name}")
        client, config = manager.create_client(config_name)
        
        info = client.get_session_info()
        print(f"客户端信息: {info['platform']} - {info['browser']}({info['impersonate_target']}) - {info['language']}")
        
        # 测试请求
        response = client.make_request("https://httpbin.org/user-agent")
        if response and response.status_code == 200:
            print("请求成功!")
        
        client.close_all_sessions()
        time.sleep(1)
```

4. 完整的使用流程

安装依赖

```bash
pip install curl-cffi fake-useragent user-agents
```

基础使用

```python
from complete_curl_cffi_client import CompleteCurlCffiClient

# 创建客户端（不使用代理）
client = CompleteCurlCffiClient(
    proxy_url=None,
    default_language="zh", 
    default_platform="windows",
    default_browser="chrome",
    verify_ssl=False
)

# 发送请求
response = client.make_request("https://httpbin.org/headers")
if response:
    print(response.json())

# 关闭会话
client.close_all_sessions()
```

使用 mitmproxy 代理

```python
# 使用 mitmproxy 代理
client = CompleteCurlCffiClient(
    proxy_url="http://127.0.0.1:8080",  # mitmproxy 地址
    verify_ssl=False  # 必须关闭 SSL 验证
)

response = client.make_request("https://httpbin.org/headers")
```

自定义会话配置

```python
# 按请求自定义配置
response = client.make_request(
    "https://example.com",
    language="en",
    platform="macos", 
    browser="safari"
)
```

关键特性说明

1. impersonate 参数

· 自动根据浏览器类型选择对应的 impersonate 目标
· 支持 Chrome、Firefox、Safari、Edge 等多种浏览器
· 模拟真实的 TLS 和 HTTP/2 指纹

2. 代理支持

· 支持 HTTP/HTTPS 代理
· 自动配置代理设置
· 与 mitmproxy 完美配合

3. 指纹模拟

· 完整的浏览器指纹模拟
· 自动生成匹配的 User-Agent、Accept、Accept-Language 等头部
· 支持安全头部（Sec-*）和 Fetch 头部

4. 会话管理

· 线程安全的会话管理
· 每个会话保持一致的指纹配置
· 自动维护 cookies 和会话状态

5. 错误处理和重试

· 自动重试机制
· 指数退避策略
· 详细的错误日志

这个完整的 curl_cffi 客户端提供了企业级的浏览器指纹模拟能力，能够有效绕过大多数反爬系统的检测。

# 为什么 curl_cffi 仍无法绕过部分高级反爬系统

尽管 curl_cffi 提供了优秀的 TLS 和 HTTP/2 指纹模拟能力，但仍然有一些网站无法绕过。以下是主要原因和相应的解决方案：

1. JavaScript 挑战和浏览器环境检测

问题分析

许多高级反爬系统（如 Cloudflare 5秒盾、Datadome、Akamai）会：

· 执行 JavaScript 挑战计算
· 检测浏览器 API 支持情况
· 验证 DOM 环境和 WebGL 渲染
· 检查字体列表和屏幕分辨率

解决方案

```python
# 使用 Playwright 处理 JS 挑战
from playwright.sync_api import sync_playwright
import time

def bypass_js_challenge(url, proxy=None):
    with sync_playwright() as p:
        # 启动真实浏览器
        browser = p.chromium.launch(
            headless=False,  # 可视模式更容易绕过检测
            proxy=proxy
        )
        
        # 创建上下文，模拟真实用户
        context = browser.new_context(
            viewport={"width": 1920, "height": 1080},
            user_agent="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36",
            locale="zh-CN",
            timezone_id="Asia/Shanghai"
        )
        
        page = context.new_page()
        
        # 添加额外的指纹混淆
        page.add_init_script("""
            Object.defineProperty(navigator, 'webdriver', {
                get: () => undefined,
            });
            Object.defineProperty(navigator, 'plugins', {
                get: () => [1, 2, 3, 4, 5],
            });
        """)
        
        try:
            page.goto(url, wait_until="networkidle", timeout=30000)
            
            # 等待可能的 JS 挑战
            time.sleep(5)
            
            # 执行页面内容
            content = page.content()
            return content
            
        except Exception as e:
            print(f"JS挑战绕过失败: {e}")
            return None
        finally:
            browser.close()
```

2. 行为分析和机器学习检测

问题分析

高级系统会分析用户行为模式：

· 鼠标移动轨迹
· 点击模式和滚动行为
· 请求时间间隔分布
· 会话持续时间

解决方案

```python
# 高级行为模拟
import random
import time
from selenium.webdriver.common.action_chains import ActionChains

class BehavioralSimulator:
    def __init__(self, driver):
        self.driver = driver
        self.actions = ActionChains(driver)
    
    def human_like_mouse_movement(self, element):
        """模拟人类鼠标移动"""
        # 获取元素位置
        location = element.location
        size = element.size
        
        # 生成随机移动路径
        moves = random.randint(3, 8)
        for i in range(moves):
            # 随机偏移
            offset_x = random.randint(-50, 50)
            offset_y = random.randint(-50, 50)
            
            self.actions.move_by_offset(offset_x, offset_y)
            time.sleep(random.uniform(0.05, 0.2))
        
        # 最终移动到元素
        self.actions.move_to_element(element)
        self.actions.perform()
    
    def random_delay(self, min_delay=1, max_delay=3):
        """随机延迟"""
        time.sleep(random.uniform(min_delay, max_delay))
    
    def human_typing(self, element, text):
        """模拟人类输入"""
        for char in text:
            element.send_keys(char)
            time.sleep(random.uniform(0.1, 0.3))
    
    def random_scroll(self):
        """随机滚动"""
        scroll_amount = random.randint(100, 500)
        if random.random() > 0.5:
            self.driver.execute_script(f"window.scrollBy(0, {scroll_amount});")
        else:
            self.driver.execute_script(f"window.scrollBy(0, -{scroll_amount});")
        time.sleep(random.uniform(0.5, 2))
```

3. 高级 TLS 指纹检测

问题分析

某些系统会检测：

· TLS 密码套件顺序
· TLS 扩展列表和顺序
· ALPN 协议协商
· 证书透明度信息

解决方案

```python
# 使用更底层的 TLS 配置
import ssl
import socket
from urllib3.util.ssl_ import create_urllib3_context

class AdvancedTLSConfig:
    @staticmethod
    def create_chrome_like_context():
        """创建 Chrome 类似的 TLS 上下文"""
        context = create_urllib3_context()
        
        # 设置 Chrome 类似的密码套件
        context.set_ciphers(
            'TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384:'
            'TLS_CHACHA20_POLY1305_SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:'
            'ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:'
            'ECDHE-RSA-AES256-GCM-SHA384'
        )
        
        # 设置 TLS 扩展
        context.set_alpn_protocols(['h2', 'http/1.1'])
        
        return context

# 或者使用 specialized-tls 库
try:
    from specialized_tls import ClientHello
    import asyncio
    
    async def advanced_tls_handshake():
        client_hello = ClientHello.imitate_chrome()
        # 自定义 TLS 握手过程
        # ...
except ImportError:
    print("specialized-tls 未安装，使用标准 TLS")
```

4. IP 信誉和质量问题

问题分析

· 数据中心 IP 被标记
· 代理 IP 被列入黑名单
· IP 地理位置异常

解决方案

```python
# IP 质量检测和轮换
import requests
from concurrent.futures import ThreadPoolExecutor

class IPQualityManager:
    def __init__(self, proxy_list):
        self.proxy_list = proxy_list
        self.verified_proxies = []
    
    def check_ip_quality(self, proxy):
        """检查 IP 质量"""
        test_urls = [
            "https://httpbin.org/ip",
            "https://api.ipify.org?format=json",
            "https://ipinfo.io/json"
        ]
        
        try:
            for url in test_urls:
                response = requests.get(
                    url, 
                    proxies={"http": proxy, "https": proxy},
                    timeout=10
                )
                if response.status_code != 200:
                    return False
            
            # 检查 IP 类型
            ip_info = requests.get(
                "https://ipinfo.io/json",
                proxies={"http": proxy, "https": proxy},
                timeout=10
            ).json()
            
            # 优选住宅 IP
            if any(tag in str(ip_info).lower() for tag in ['residential', 'isp', 'mobile']):
                return True
                
        except:
            return False
        
        return False
    
    def verify_proxies(self):
        """验证代理列表"""
        with ThreadPoolExecutor(max_workers=10) as executor:
            results = executor.map(self.check_ip_quality, self.proxy_list)
            
        self.verified_proxies = [
            proxy for proxy, is_valid in zip(self.proxy_list, results) 
            if is_valid
        ]
        
        print(f"验证通过 {len(self.verified_proxies)}/{len(self.proxy_list)} 个代理")
        return self.verified_proxies
    
    def get_best_proxy(self):
        """获取最佳代理"""
        if not self.verified_proxies:
            self.verify_proxies()
        
        return random.choice(self.verified_proxies) if self.verified_proxies else None
```

5. 高级 HTTP/2 指纹检测

问题分析

检测：

· SETTINGS 帧的精确顺序和值
· WINDOW_UPDATE 帧的时机
· HEADERS 帧的压缩算法使用
· 流优先级和依赖关系

解决方案

```python
# 使用专门的低级 HTTP/2 库
import h2.connection
import h2.config

class AdvancedHTTP2Client:
    def __init__(self):
        config = h2.config.H2Configuration(
            client_side=True,
            header_encoding='utf-8'
        )
        self.conn = h2.connection.H2Connection(config=config)
    
    def simulate_chrome_h2(self):
        """模拟 Chrome 的 HTTP/2 行为"""
        # Chrome 特定的 SETTINGS
        chrome_settings = {
            h2.settings.SettingCodes.HEADER_TABLE_SIZE: 65536,
            h2.settings.SettingCodes.MAX_CONCURRENT_STREAMS: 1000,
            h2.settings.SettingCodes.INITIAL_WINDOW_SIZE: 6291456,
            h2.settings.SettingCodes.MAX_FRAME_SIZE: 16384,
            h2.settings.SettingCodes.MAX_HEADER_LIST_SIZE: 262144,
        }
        
        self.conn.local_settings.update(chrome_settings)
        self.conn.remote_settings.update(chrome_settings)
        
        return self.conn
```

6. Canvas 和 WebGL 指纹

问题分析

网站通过 Canvas 绘图检测硬件和浏览器差异。

解决方案

```python
# 使用 Playwright 修改 Canvas 指纹
from playwright.sync_api import sync_playwright

def modify_canvas_fingerprint(page):
    """修改 Canvas 指纹"""
    page.add_init_script("""
        // 覆盖 Canvas 方法
        const originalGetImageData = CanvasRenderingContext2D.prototype.getImageData;
        CanvasRenderingContext2D.prototype.getImageData = function(...args) {
            const result = originalGetImageData.apply(this, args);
            // 添加微小随机噪声
            for (let i = 0; i < result.data.length; i += 4) {
                result.data[i] = result.data[i] + Math.random() * 0.01;
            }
            return result;
        };
        
        // 覆盖 WebGL 方法
        const originalGetParameter = WebGLRenderingContext.prototype.getParameter;
        WebGLRenderingContext.prototype.getParameter = function(parameter) {
            const result = originalGetParameter.call(this, parameter);
            
            // 修改特定参数
            if (parameter === this.VERSION) {
                return "WebGL 1.0 (OpenGL ES 2.0 Chromium)";
            }
            if (parameter === this.SHADING_LANGUAGE_VERSION) {
                return "WebGL GLSL ES 1.0 (OpenGL ES GLSL ES 1.0 Chromium)";
            }
            
            return result;
        };
    """)
```

7. 综合解决方案：混合方法

```python
# 综合绕过方案
class AdvancedBypassSystem:
    def __init__(self):
        self.proxy_manager = IPQualityManager([])
        self.current_strategy = "curl_cffi"
    
    def try_bypass(self, url, max_retries=3):
        """尝试多种绕过方法"""
        strategies = [
            self._curl_cffi_approach,
            self._playwright_approach,
            self._selenium_approach,
            self._manual_approach
        ]
        
        for attempt in range(max_retries):
            for strategy in strategies:
                print(f"尝试策略: {strategy.__name__}, 第 {attempt + 1} 次尝试")
                
                try:
                    result = strategy(url)
                    if result:
                        print(f"成功使用 {strategy.__name__}")
                        return result
                except Exception as e:
                    print(f"策略 {strategy.__name__} 失败: {e}")
                    continue
            
            # 更换代理
            new_proxy = self.proxy_manager.get_best_proxy()
            print(f"更换代理: {new_proxy}")
            
            time.sleep(2 ** attempt)  # 指数退避
        
        return None
    
    def _curl_cffi_approach(self, url):
        """curl_cffi 方法"""
        from curl_cffi import requests
        
        response = requests.get(
            url,
            impersonate="chrome110",
            timeout=30
        )
        
        if response.status_code == 200:
            # 检查是否被挑战
            if "challenge" in response.text.lower() or "cloudflare" in response.text.lower():
                raise Exception("触发挑战页面")
            return response.text
        
        raise Exception(f"HTTP {response.status_code}")
    
    def _playwright_approach(self, url):
        """Playwright 方法"""
        with sync_playwright() as p:
            browser = p.chromium.launch(headless=True)
            context = browser.new_context()
            page = context.new_page()
            
            # 设置高级指纹混淆
            self._setup_advanced_fingerprinting(page)
            
            page.goto(url, wait_until="networkidle")
            
            # 处理可能的挑战
            if self._handle_challenges(page):
                content = page.content()
                browser.close()
                return content
            
            browser.close()
            raise Exception("Playwright 方法失败")
    
    def _selenium_approach(self, url):
        """Selenium 方法"""
        from selenium import webdriver
        from selenium.webdriver.chrome.options import Options
        
        options = Options()
        options.add_argument("--disable-blink-features=AutomationControlled")
        options.add_experimental_option("excludeSwitches", ["enable-automation"])
        options.add_experimental_option('useAutomationExtension', False)
        
        driver = webdriver.Chrome(options=options)
        driver.execute_script("Object.defineProperty(navigator, 'webdriver', {get: () => undefined})")
        
        try:
            driver.get(url)
            # 添加行为模拟
            behavioral_sim = BehavioralSimulator(driver)
            behavioral_sim.random_delay(2, 5)
            behavioral_sim.random_scroll()
            
            return driver.page_source
        finally:
            driver.quit()
    
    def _manual_approach(self, url):
        """手动分析方法"""
        # 分析页面，手动提取数据
        # 使用多个来源组合信息
        # 调用官方 API（如果存在）
        pass
    
    def _setup_advanced_fingerprinting(self, page):
        """设置高级指纹混淆"""
        page.add_init_script("""
            // 修改各种指纹属性
            Object.defineProperty(navigator, 'platform', {
                get: () => 'Win32'
            });
            
            Object.defineProperty(navigator, 'hardwareConcurrency', {
                get: () => 8
            });
            
            Object.defineProperty(navigator, 'deviceMemory', {
                get: () => 8
            });
            
            // 修改插件列表
            Object.defineProperty(navigator, 'plugins', {
                get: () => [
                    {name: 'Chrome PDF Plugin'}, 
                    {name: 'Chrome PDF Viewer'},
                    {name: 'Native Client'}
                ]
            });
        """)
    
    def _handle_challenges(self, page):
        """处理挑战页面"""
        # 检测挑战类型并相应处理
        challenge_selectors = [
            '#challenge-form',
            '.challenge-form',
            '[data-ray]',
            '.cf-browser-verification'
        ]
        
        for selector in challenge_selectors:
            if page.query_selector(selector):
                print("检测到挑战页面，尝试自动处理...")
                # 等待挑战完成或手动处理
                page.wait_for_timeout(10000)
                return True
        
        return False
```

8. 针对特定反爬系统的专门方案

Cloudflare 5秒盾

```python
def bypass_cloudflare(url):
    """专门绕过 Cloudflare"""
    from cloudscraper import create_scraper
    
    scraper = create_scraper(
        browser={
            'browser': 'chrome',
            'platform': 'windows',
            'mobile': False
        }
    )
    
    try:
        response = scraper.get(url, timeout=30)
        return response.text
    except Exception as e:
        print(f"Cloudflare 绕过失败: {e}")
        return None
```

Datadome 保护

```python
def bypass_datadome(url):
    """专门绕过 Datadome"""
    # Datadome 需要更复杂的 JS 执行环境
    from selenium import webdriver
    from selenium.webdriver.chrome.options import Options
    
    options = Options()
    options.add_argument("--disable-blink-features=AutomationControlled")
    
    driver = webdriver.Chrome(options=options)
    
    try:
        driver.get(url)
        
        # 等待可能的反爬处理
        import time
        time.sleep(10)
        
        # 检查是否被拦截
        if "access denied" in driver.page_source.lower():
            print("被 Datadome 拦截")
            return None
        
        return driver.page_source
    finally:
        driver.quit()
```

总结

无法绕过的原因主要包括：

1. JavaScript 执行要求 - 需要真实浏览器环境
2. 行为分析 - 需要模拟人类交互模式
3. 高级指纹检测 - 需要更底层的协议控制
4. IP 质量 - 需要高质量的住宅代理
5. 机器学习模型 - 需要不断更新的对抗策略

解决方案：

· 组合使用多种工具（curl_cffi + Playwright + Selenium）
· 使用专业反爬服务（如 ScrapingBee、ScraperAPI）
· 人工分析目标网站的具体防护机制
· 持续更新指纹和策略

对于最严格的防护，通常需要结合多种技术手段，并且可能需要人工干预或使用商业反爬服务。