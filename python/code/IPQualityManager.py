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
                    print(f"代理 {proxy} 测试 {url} 失败, 状态码: {response.status_code}")
                    return False
            
            # 检查 IP 类型
            ip_info = requests.get(
                "https://ipinfo.io/json",
                proxies={"http": proxy, "https": proxy},
                timeout=10
            ).json()
            print(f"ip_info: {ip_info}")
            print("=== IP信息检测结果 ===")
            print(f"IP地址: {ip_info.get('ip', '未知')}")
            print(f"主机名: {ip_info.get('hostname', '未知')}")
            print(f"城市: {ip_info.get('city', '未知')}")
            print(f"地区: {ip_info.get('region', '未知')}")
            print(f"国家: {ip_info.get('country', '未知')}")
            print(f"地理位置: {ip_info.get('loc', '未知')}")
            print(f"组织: {ip_info.get('org', '未知')}")
            print(f"时区: {ip_info.get('timezone', '未知')}")
            
            print("\n=== 网络类型分析 ===")
            
            # 分析组织信息来判断网络类型
            org = ip_info.get('org', '').lower()
            hostname = ip_info.get('hostname', '').lower()
            
            # 检测住宅网络特征
            residential_keywords = ['residential', 'broadband', 'dsl', 'cable', 'fiber', 'home', 'consumer']
            is_residential = any(keyword in org for keyword in residential_keywords)
            
            # 检测移动网络特征
            mobile_keywords = ['mobile', 'cellular', 'wireless', '4g', '5g', 'lte']
            is_mobile = any(keyword in org for keyword in mobile_keywords)
            
            # 检测ISP特征
            isp_keywords = ['isp', 'internet', 'telecom', 'communications', 'network']
            is_isp = any(keyword in org for keyword in isp_keywords) or 'as' in org
            
            print(f"住宅网络可能性: {'是' if is_residential else '否'}")
            print(f"移动网络可能性: {'是' if is_mobile else '否'}")
            print(f"ISP网络: {'是' if is_isp else '否'}")
            
            # 基于域名分析
            if any(domain in hostname for domain in ['.mobile.', '.cell.', '.wireless.']):
                print("域名提示: 可能为移动网络")
                is_mobile = True
            
            print(f"\n原始组织信息: {ip_info.get('org', '无')}")
            
            # 综合判断
            print(f"\n=== 综合判断 ===")
            if is_mobile:
                print("主要特征: 移动网络")
            elif is_residential:
                print("主要特征: 住宅网络")
            elif is_isp:
                print("主要特征: ISP网络")
            else:
                print("主要特征: 无法确定，可能是数据中心或企业网络")

            # 优选住宅 IP
            if any(tag in str(ip_info).lower() for tag in ['residential', 'isp', 'mobile']):
                return True
                
        except Exception as e:
            print(f"代理 {proxy} 测试失败: {e}")
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

# 从文件读取代理
with open('proxies.txt', 'r') as f:
    proxy_list = [line.strip() for line in f if line.strip()]

proxy_manager = IPQualityManager(proxy_list)
# 更换代理
new_proxy = proxy_manager.get_best_proxy()
print(f"更换代理: {new_proxy}")
print(f"优秀代理数: {len(proxy_manager.verified_proxies)}")
