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
