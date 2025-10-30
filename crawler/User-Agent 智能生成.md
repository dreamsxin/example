# 配合 UA 随机生成 Accept 头部的完整方案

随机生成的 User-Agent 智能生成匹配的 Accept、Accept-Language 和 Accept-Encoding 头部。

## 1. 完整的头部生成器类

```python
# headers_generator.py
import random
from user_agents import parse

class HeadersGenerator:
    def __init__(self):
        # 语言配置
        self.language_configs = {
            'zh': {  # 中文用户
                'primary': 'zh-CN',
                'secondary': ['zh', 'zh-TW', 'en-US', 'en'],
                'weights': [1.0, 0.8, 0.6, 0.4]
            },
            'en': {  # 英文用户
                'primary': 'en-US',
                'secondary': ['en', 'zh-CN', 'zh'],
                'weights': [1.0, 0.9, 0.3, 0.2]
            },
            'mixed': {  # 混合语言用户
                'primary': 'zh-CN',
                'secondary': ['en-US', 'en', 'zh-TW', 'zh'],
                'weights': [1.0, 0.7, 0.6, 0.5, 0.4]
            }
        }
        
        # 编码配置
        self.encoding_configs = {
            'modern': ['gzip', 'deflate', 'br', 'zstd'],  # 现代浏览器
            'standard': ['gzip', 'deflate', 'br'],        # 标准浏览器
            'basic': ['gzip', 'deflate']                  # 基础浏览器
        }
        
        # 浏览器特定的 Accept 配置
        self.accept_configs = {
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
            # 简单回退解析
            ua_lower = user_agent_string.lower()
            return {
                'browser_family': 'chrome' if 'chrome' in ua_lower else 'firefox' if 'firefox' in ua_lower else 'safari' if 'safari' in ua_lower else 'edge' if 'edg' in ua_lower else 'chrome',
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
        browser_family = ua_info['browser_family']
        device_type = 'mobile' if ua_info['is_mobile'] else 'desktop'
        
        # 获取基础 Accept
        if browser_family in self.accept_configs:
            base_accept = self.accept_configs[browser_family][device_type]
        else:
            base_accept = self.accept_configs['chrome'][device_type]
        
        # 根据浏览器版本添加特性
        if browser_family == 'chrome':
            # Chrome 特定调整
            chrome_version = self._parse_version(ua_info.get('browser_version', '119.0'))
            if chrome_version and chrome_version >= 94:
                # Chrome 94+ 支持 avif
                if 'image/avif' not in base_accept:
                    base_accept = base_accept.replace('image/webp', 'image/avif,image/webp')
        
        elif browser_family == 'firefox':
            # Firefox 特定调整
            firefox_version = self._parse_version(ua_info.get('browser_version', '109.0'))
            if firefox_version and firefox_version >= 93:
                # Firefox 93+ 支持 avif
                if 'image/avif' not in base_accept:
                    base_accept = base_accept.replace('image/webp', 'image/avif,image/webp')
        
        elif browser_family == 'safari':
            # Safari 特定调整
            safari_version = self._parse_version(ua_info.get('browser_version', '16.0'))
            if safari_version and safari_version >= 16:
                # Safari 16+ 支持 webp
                if 'image/webp' not in base_accept:
                    base_accept = base_accept.replace('*/*;q=0.8', 'image/webp,*/*;q=0.8')
        
        return base_accept
    
    def generate_accept_language_header(self, ua_info, language_profile='zh'):
        """生成 Accept-Language 头部"""
        if language_profile not in self.language_configs:
            language_profile = 'zh'
        
        config = self.language_configs[language_profile]
        
        # 构建语言字符串
        languages = [config['primary']]
        languages.extend(config['secondary'])
        
        # 添加质量参数
        language_header_parts = []
        for i, lang in enumerate(languages):
            if i < len(config['weights']):
                q = config['weights'][i]
                language_header_parts.append(f"{lang};q={q:.1f}")
            else:
                language_header_parts.append(lang)
        
        # 随机排序次要语言（保持主要语言在第一位）
        if len(language_header_parts) > 1:
            secondary_langs = language_header_parts[1:]
            random.shuffle(secondary_langs)
            language_header_parts = [language_header_parts[0]] + secondary_langs
        
        return ', '.join(language_header_parts)
    
    def generate_accept_encoding_header(self, ua_info):
        """生成 Accept-Encoding 头部"""
        browser_family = ua_info['browser_family']
        browser_version = self._parse_version(ua_info.get('browser_version', '119.0'))
        
        # 基础编码
        if browser_family == 'chrome' and browser_version and browser_version >= 80:
            encodings = self.encoding_configs['modern']  # Chrome 80+ 支持 zstd
        elif browser_family == 'firefox' and browser_version and browser_version >= 87:
            encodings = self.encoding_configs['modern']  # Firefox 87+ 支持 zstd
        elif browser_family == 'safari' and browser_version and browser_version >= 14:
            encodings = self.encoding_configs['standard']  # Safari 14+ 支持 br
        elif browser_family == 'edge' and browser_version and browser_version >= 85:
            encodings = self.encoding_configs['modern']  # Edge 85+ 支持 zstd
        else:
            encodings = self.encoding_configs['standard']
        
        # 移动设备可能不支持某些编码
        if ua_info['is_mobile'] and 'zstd' in encodings:
            # 移动设备移除 zstd（某些移动浏览器支持不完全）
            encodings = [e for e in encodings if e != 'zstd']
        
        # 随机排序（保持 gzip 在前的概率较高）
        if random.random() > 0.3:  # 70% 的概率保持 gzip 在前
            base = encodings[0]
            others = encodings[1:]
            random.shuffle(others)
            encodings = [base] + others
        else:
            random.shuffle(encodings)
        
        return ', '.join(encodings)
    
    def generate_all_headers(self, user_agent_string, language_profile='zh'):
        """为给定的 User-Agent 生成所有相关头部"""
        ua_info = self.parse_user_agent(user_agent_string)
        
        return {
            'User-Agent': user_agent_string,
            'Accept': self.generate_accept_header(ua_info),
            'Accept-Language': self.generate_accept_language_header(ua_info, language_profile),
            'Accept-Encoding': self.generate_accept_encoding_header(ua_info)
        }
    
    def _parse_version(self, version_string):
        """解析版本字符串，返回主版本号"""
        try:
            if version_string:
                major_version = version_string.split('.')[0]
                return int(major_version)
        except:
            pass
        return None

# 使用示例
if __name__ == "__main__":
    generator = HeadersGenerator()
    
    test_user_agents = [
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/119.0",
        "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.0 Safari/605.1.15",
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36 Edg/119.0.0.0",
        "Mozilla/5.0 (iPhone; CPU iPhone OS 16_6 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/16.6 Mobile/15E148 Safari/604.1",
        "Mozilla/5.0 (Linux; Android 10; SM-G973F) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Mobile Safari/537.36"
    ]
    
    for ua in test_user_agents:
        print(f"\n{'='*60}")
        print(f"User-Agent: {ua}")
        headers = generator.generate_all_headers(ua)
        for key, value in headers.items():
            print(f"{key}: {value}")
```

## 2. 集成到 mitmproxy 插件

更新 `browser_fingerprint.py`：

```python
# browser_fingerprint.py - 集成智能头部生成
from mitmproxy import http, ctx
import random
import time
from fake_useragent import UserAgent
from headers_generator import HeadersGenerator

class BrowserFingerprintSimulator:
    def __init__(self):
        self.client_profiles = {}
        self.headers_generator = HeadersGenerator()
        
        # 初始化 fake-useragent
        try:
            self.ua_generator = UserAgent(
                browsers=['chrome', 'firefox', 'safari', 'edge'],
                os=['windows', 'macos', 'linux'],
                min_percentage=1.0
            )
            ctx.log.info("fake-useragent 初始化成功")
        except Exception as e:
            ctx.log.warn(f"fake-useragent 初始化失败: {e}")
            self.ua_generator = None
        
        # 语言配置
        self.language_profiles = ['zh', 'en', 'mixed']
    
    def get_random_user_agent(self):
        """获取随机 User-Agent"""
        try:
            if self.ua_generator:
                return self.ua_generator.random
            else:
                return "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36"
        except Exception as e:
            ctx.log.warn(f"获取 User-Agent 失败: {e}")
            return "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36"
    
    def get_client_profile(self, client_id):
        """获取或创建客户端配置档案"""
        if client_id not in self.client_profiles:
            user_agent_string = self.get_random_user_agent()
            language_profile = random.choice(self.language_profiles)
            
            # 生成完整的头部
            headers = self.headers_generator.generate_all_headers(
                user_agent_string, 
                language_profile
            )
            
            ua_info = self.headers_generator.parse_user_agent(user_agent_string)
            
            self.client_profiles[client_id] = {
                'user_agent': user_agent_string,
                'language_profile': language_profile,
                'generated_headers': headers,
                'ua_info': ua_info,
                'created_at': time.time(),
                'request_count': 0
            }
            
            ctx.log.info(f"为新客户端 {client_id} 创建配置: {ua_info['browser_family']} on {ua_info['os_family']} (语言: {language_profile})")
            
        return self.client_profiles[client_id]
    
    def get_sec_headers(self, ua_info):
        """生成安全相关头部"""
        browser_family = ua_info['browser_family']
        platform = ua_info['os_family']
        browser_version = ua_info.get('browser_version', '119.0')
        
        # 简化版本号
        try:
            major_version = browser_version.split('.')[0]
        except:
            major_version = "119"
        
        platform_header = self._format_platform_header(platform)
        
        if browser_family == "chrome":
            return {
                "sec-ch-ua": f'"Google Chrome";v="{major_version}", "Chromium";v="{major_version}", "Not?A_Brand";v="24"',
                "sec-ch-ua-mobile": "?0",
                "sec-ch-ua-platform": platform_header,
            }
        elif browser_family == "firefox":
            return {
                "sec-ch-ua": f'"Not.A/Brand";v="8", "Chromium";v="{major_version}", "Google Chrome";v="{major_version}"',
                "sec-ch-ua-mobile": "?0", 
                "sec-ch-ua-platform": platform_header,
            }
        elif browser_family == "safari":
            return {
                "sec-ch-ua": f'"Google Chrome";v="{major_version}", "Chromium";v="{major_version}", "Not?A_Brand";v="24"',
                "sec-ch-ua-mobile": "?0",
                "sec-ch-ua-platform": platform_header,
            }
        elif browser_family == "edge":
            return {
                "sec-ch-ua": f'"Microsoft Edge";v="{major_version}", "Chromium";v="{major_version}", "Not?A_Brand";v="24"',
                "sec-ch-ua-mobile": "?0",
                "sec-ch-ua-platform": platform_header,
            }
        else:
            return {
                "sec-ch-ua": f'"Google Chrome";v="{major_version}", "Chromium";v="{major_version}", "Not?A_Brand";v="24"',
                "sec-ch-ua-mobile": "?0",
                "sec-ch-ua-platform": platform_header,
            }
    
    def _format_platform_header(self, platform):
        """格式化平台头部值"""
        platform_map = {
            'windows': '"Windows"',
            'mac': '"macOS"',
            'mac os x': '"macOS"',
            'linux': '"Linux"',
            'ios': '"iOS"',
            'android': '"Android"'
        }
        return platform_map.get(platform, '"Windows"')
    
    def get_fetch_headers(self, request_method, ua_info):
        """生成 Fetch 相关头部"""
        if request_method.upper() == "GET":
            return {
                "sec-fetch-dest": "document",
                "sec-fetch-mode": "navigate",
                "sec-fetch-site": "none",
                "sec-fetch-user": "?1",
            }
        else:  # POST and others
            return {
                "sec-fetch-dest": "empty",
                "sec-fetch-mode": "cors", 
                "sec-fetch-site": "same-origin",
            }
    
    def request(self, flow: http.HTTPFlow):
        """修改请求以模拟浏览器指纹"""
        # 获取客户端ID
        client_id = flow.client_conn.address[0] if flow.client_conn else "default"
        
        # 获取客户端配置
        profile = self.get_client_profile(client_id)
        ua_info = profile['ua_info']
        generated_headers = profile['generated_headers']
        
        # 移除可疑的代理头
        suspicious_headers = [
            'x-forwarded-for', 'x-forwarded-proto', 'via', 
            'x-real-ip', 'forwarded'
        ]
        for header in suspicious_headers:
            flow.request.headers.pop(header, None)
        
        headers = flow.request.headers
        
        # 1. 设置基础头部（Accept, Accept-Language, Accept-Encoding, User-Agent）
        for header_name in ['User-Agent', 'Accept', 'Accept-Language', 'Accept-Encoding']:
            if header_name in generated_headers:
                headers[header_name] = generated_headers[header_name]
        
        # 2. 设置安全头
        sec_headers = self.get_sec_headers(ua_info)
        for key, value in sec_headers.items():
            headers[key] = value
        
        # 3. 设置 Fetch 头
        fetch_headers = self.get_fetch_headers(flow.request.method, ua_info)
        for key, value in fetch_headers.items():
            headers[key] = value
        
        # 4. 设置其他通用头
        headers["upgrade-insecure-requests"] = "1"
        if random.random() > 0.5:  # 50%的概率设置 DNT
            headers["dnt"] = "1"
        headers["cache-control"] = "max-age=0"
        
        # 更新请求计数
        profile['request_count'] += 1
        
        # 记录调试信息
        if profile['request_count'] <= 3:  # 只记录前3个请求
            ctx.log.info(f"客户端 {client_id} 请求 {profile['request_count']}: {ua_info['browser_family']} - 语言: {profile['language_profile']}")
    
    def response(self, flow: http.HTTPFlow):
        """处理响应"""
        pass

addons = [BrowserFingerprintSimulator()]
```

## 3. 集成到 curl_cffi 客户端

更新 `curl_cffi_client_enhanced.py`：

```python
# curl_cffi_client_enhanced.py - 集成智能头部生成
from curl_cffi import requests
import random
import time
import threading
from urllib.parse import urlparse
from fake_useragent import UserAgent
from headers_generator import HeadersGenerator

class EnhancedBrowserClient:
    def __init__(self, proxy_url="http://127.0.0.1:8080"):
        self.proxy_url = proxy_url
        self.session_storage = {}
        self.lock = threading.Lock()
        self.headers_generator = HeadersGenerator()
        
        # 初始化 UA 生成器
        try:
            self.ua = UserAgent(
                browsers=['chrome', 'firefox', 'safari', 'edge'],
                os=['windows', 'macos', 'linux'],
                min_percentage=1.0
            )
            print("✓ fake-useragent 初始化成功")
        except Exception as e:
            print(f"⚠️ fake-useragent 初始化失败: {e}")
            self.ua = None
        
        # 浏览器模拟配置
        self.browser_profiles = self._generate_browser_profiles()
        
        # 语言配置
        self.language_profiles = ['zh', 'en', 'mixed']
    
    def _generate_browser_profiles(self):
        """生成浏览器配置档案"""
        profiles = []
        
        # Chrome 配置
        chrome_configs = [
            {"impersonate": "chrome110", "name": "Chrome 110"},
            {"impersonate": "chrome107", "name": "Chrome 107"},
            {"impersonate": "chrome104", "name": "Chrome 104"},
            {"impersonate": "chrome101", "name": "Chrome 101"},
        ]
        
        # Safari 配置
        safari_configs = [
            {"impersonate": "safari15_5", "name": "Safari 15.5"},
            {"impersonate": "safari15_3", "name": "Safari 15.3"},
        ]
        
        profiles.extend(chrome_configs * 3)
        profiles.extend(safari_configs)
        
        return profiles
    
    def get_random_user_agent(self):
        """获取随机 User-Agent"""
        try:
            if self.ua:
                return self.ua.random
            else:
                return "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36"
        except Exception as e:
            print(f"获取 User-Agent 失败: {e}")
            return "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36"
    
    def get_session(self, session_id=None):
        """获取或创建会话"""
        if session_id is None:
            session_id = threading.current_thread().ident
            
        with self.lock:
            if session_id not in self.session_storage:
                # 为每个会话选择浏览器配置、User-Agent 和语言配置
                browser_profile = random.choice(self.browser_profiles)
                user_agent = self.get_random_user_agent()
                language_profile = random.choice(self.language_profiles)
                
                # 生成完整的头部
                headers = self.headers_generator.generate_all_headers(user_agent, language_profile)
                ua_info = self.headers_generator.parse_user_agent(user_agent)
                
                self.session_storage[session_id] = {
                    'session': requests.Session(),
                    'browser_profile': browser_profile,
                    'user_agent': user_agent,
                    'language_profile': language_profile,
                    'generated_headers': headers,
                    'ua_info': ua_info,
                    'last_request_time': 0,
                    'request_count': 0,
                    'domain_cookies': {}
                }
                
                print(f"创建新会话 {session_id}: {browser_profile['name']} - {ua_info['browser_family']} - 语言: {language_profile}")
                
            return self.session_storage[session_id]
    
    def generate_browser_headers(self, session_data, url, extra_headers=None):
        """生成完整的浏览器头部"""
        # 使用预生成的头部作为基础
        headers = session_data['generated_headers'].copy()
        
        # 添加安全头部
        ua_info = session_data['ua_info']
        browser_family = ua_info['browser_family'].lower()
        
        if 'chrome' in browser_family:
            headers.update({
                'Sec-Ch-Ua': '"Google Chrome";v="119", "Chromium";v="119", "Not?A_Brand";v="24"',
                'Sec-Ch-Ua-Mobile': '?0',
                'Sec-Ch-Ua-Platform': '"Windows"',
            })
        elif 'firefox' in browser_family:
            headers.update({
                'Sec-Ch-Ua': '"Not.A/Brand";v="8", "Chromium";v="119", "Google Chrome";v="119"',
                'Sec-Ch-Ua-Mobile': '?0',
                'Sec-Ch-Ua-Platform': '"Windows"',
            })
        
        # 添加 Fetch 头部
        headers.update({
            'Sec-Fetch-Dest': 'document',
            'Sec-Fetch-Mode': 'navigate',
            'Sec-Fetch-Site': 'none',
            'Sec-Fetch-User': '?1',
        })
        
        # 随机添加 DNT
        if random.random() > 0.5:
            headers['Dnt'] = '1'
        
        headers['Cache-Control'] = 'max-age=0'
        headers['Upgrade-Insecure-Requests'] = '1'
        
        # 添加自定义头部
        if extra_headers:
            headers.update(extra_headers)
        
        return headers
    
    def make_request(self, url, method="GET", headers=None, data=None, delay=True, session_id=None):
        """发送请求"""
        session_data = self.get_session(session_id)
        session = session_data['session']
        browser_profile = session_data['browser_profile']
        
        # 人类化延迟
        if delay and session_data['request_count'] > 0:
            current_time = time.time()
            time_since_last = current_time - session_data['last_request_time']
            min_delay = random.uniform(1, 3)
            if time_since_last < min_delay:
                sleep_time = min_delay - time_since_last + random.uniform(0.1, 0.5)
                time.sleep(sleep_time)
        
        # 生成浏览器头部
        browser_headers = self.generate_browser_headers(session_data, url, headers)
        
        # 准备请求参数
        request_kwargs = {
            'method': method,
            'url': url,
            'impersonate': browser_profile['impersonate'],
            'headers': browser_headers,
            'proxies': {
                'http': self.proxy_url,
                'https': self.proxy_url
            },
            'timeout': 30,
            'verify': False
        }
        
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
            self.log_request(url, browser_profile['name'], session_data, response.status_code, response_time)
            
            return response
            
        except Exception as e:
            print(f"请求失败: {e}")
            return None
    
    def log_request(self, url, browser_profile, session_data, status_code, response_time):
        """记录请求信息"""
        domain = urlparse(url).netloc
        ua_info = session_data['ua_info']
        browser_name = ua_info['browser_family']
        language_profile = session_data['language_profile']
        
        print(f"[{browser_profile}] {domain} -> {status_code} ({response_time:.2f}s) - {browser_name} - 语言: {language_profile}")
    
    def get_session_info(self, session_id=None):
        """获取会话信息"""
        session_data = self.get_session(session_id)
        return {
            'session_id': session_id or threading.current_thread().ident,
            'browser_profile': session_data['browser_profile']['name'],
            'user_agent': session_data['user_agent'],
            'language_profile': session_data['language_profile'],
            'request_count': session_data['request_count'],
            'accept_header': session_data['generated_headers']['Accept'],
            'accept_language_header': session_data['generated_headers']['Accept-Language'],
            'accept_encoding_header': session_data['generated_headers']['Accept-Encoding']
        }

# 测试函数
def test_headers_generation():
    """测试头部生成"""
    client = EnhancedBrowserClient()
    
    print("测试头部生成...")
    
    for i in range(5):
        session_info = client.get_session_info(i)
        print(f"\n--- 会话 {i} ---")
        print(f"User-Agent: {session_info['user_agent']}")
        print(f"Accept: {session_info['accept_header']}")
        print(f"Accept-Language: {session_info['accept_language_header']}")
        print(f"Accept-Encoding: {session_info['accept_encoding_header']}")
        print(f"语言配置: {session_info['language_profile']}")

if __name__ == "__main__":
    test_headers_generation()
```

## 4. 验证脚本

创建验证脚本检查生成的头部：

```python
# verify_headers.py
from curl_cffi_client_enhanced import EnhancedBrowserClient
import requests

def verify_headers_locally():
    """本地验证生成的头部"""
    client = EnhancedBrowserClient()
    
    print("本地头部验证:")
    print("=" * 60)
    
    for i in range(3):
        session_info = client.get_session_info(i)
        print(f"\n会话 {i}:")
        print(f"User-Agent: {session_info['user_agent']}")
        print(f"Accept: {session_info['accept_header']}")
        print(f"Accept-Language: {session_info['accept_language_header']}")
        print(f"Accept-Encoding: {session_info['accept_encoding_header']}")
        print(f"语言配置: {session_info['language_profile']}")

def verify_headers_through_proxy():
    """通过代理验证头部"""
    client = EnhancedBrowserClient()
    
    print("\n通过代理验证头部:")
    print("=" * 60)
    
    test_url = "https://httpbin.org/headers"
    
    for i in range(2):
        response = client.make_request(test_url, session_id=i)
        if response and response.status_code == 200:
            headers = response.json()['headers']
            print(f"\n会话 {i} 的实际请求头:")
            print(f"User-Agent: {headers.get('User-Agent', 'N/A')}")
            print(f"Accept: {headers.get('Accept', 'N/A')}")
            print(f"Accept-Language: {headers.get('Accept-Language', 'N/A')}")
            print(f"Accept-Encoding: {headers.get('Accept-Encoding', 'N/A')}")
            
            # 检查头部是否一致
            session_info = client.get_session_info(i)
            expected_ua = session_info['user_agent']
            actual_ua = headers.get('User-Agent', '')
            
            if expected_ua == actual_ua:
                print("✓ User-Agent 匹配")
            else:
                print("✗ User-Agent 不匹配")

if __name__ == "__main__":
    verify_headers_locally()
    verify_headers_through_proxy()
```

## 5. 使用说明

### 安装依赖
```bash
pip install fake-useragent user-agents curl-cffi
```

### 启动流程
1. 启动 mitmproxy：
   ```bash
   mitmdump -s browser_fingerprint.py --listen-port 8080
   ```

2. 运行测试：
   ```bash
   python verify_headers.py
   ```

3. 使用客户端：
   ```python
   from curl_cffi_client_enhanced import EnhancedBrowserClient
   
   client = EnhancedBrowserClient()
   response = client.make_request("https://httpbin.org/headers")
   ```

## 关键特性

1. **智能匹配**：根据 UA 自动生成匹配的 Accept、Accept-Language、Accept-Encoding 头部
2. **语言配置**：支持中文、英文和混合语言配置
3. **版本感知**：根据浏览器版本调整支持的格式（如 avif、webp）
4. **设备适配**：为移动设备和桌面设备生成不同的头部
5. **随机化**：在合理范围内随机化头部顺序和内容
6. **会话一致性**：同一会话保持一致的头部配置
