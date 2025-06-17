## 启动本地

```shell
pip install -U selenium # -U表示升级所有的依赖包到最新版本
```
```py
#codding=utf-8
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service

options = webdriver.ChromeOptions()
options.binary_location = r'C:\Chrome.exe'

service = Service(service_args=['--disable-build-check'], executable_path=r'C:\TEST\chromedriver.exe')

driver = webdriver.Chrome(service=service, options=options)
driver.get('http://www.baidu.com')
```

## 连接本地已启动

```python
from selenium import webdriver
from selenium.webdriver.chrome.service import Service


options = webdriver.ChromeOptions()
#options.add_argument('--no-sandbox')
options.add_experimental_option("debuggerAddress", "127.0.0.1:56499")
# 指定chromedriver的路径
webdriver_path = "D:\\TEST\\chromedriver.exe"
service = Service(
   service_args=['--disable-build-check'],
   executable_path=webdriver_path)
service.service_args = [
    '--connect-timeout=30',    # 连接超时
    '--read-timeout=60',       # 读取超时
    '--startup-timeout=90'     # 启动超时:ml-citation{ref="2,4" data="citationList"}
]
print('webdriver_path', webdriver_path)

driver = webdriver.Chrome(options=options, service=service)

# 直接打开页面
# 目标url
target_url = 'https://www.baidu.com/'
driver.get(url=target_url)
#time.sleep(random.randint(5, 10))
```

## 链接远程服务，需要 Selenium Server
```python
from selenium import webdriver

# 配置远程调试地址
chrome_options = webdriver.ChromeOptions()
chrome_options.debugger_address = "127.0.0.1:56499"

# 创建远程连接
service_url = 'http://<hub-ip>:4444/wd/hub'
driver = webdriver.Remote(command_executor=service_url, options=chrome_options)
# 进行浏览器操作
driver.get('https://www.baidu.com')
# ...
# 关闭连接
driver.quit()
```
