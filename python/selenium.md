```shell
pip install -U selenium # -U表示升级所有的依赖包到最新版本
```
```py
#codding=utf-8
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service

options = webdriver.ChromeOptions()
options.binary_location = r'C:\Users\Administrator\FbBrowser\2.0.1.2\YunLogin.exe'


service = Service(service_args=['--disable-build-check'], executable_path=r'C:\Users\Administrator\FbBrowser\2.0.1.2\chromedriver.exe')

driver = webdriver.Chrome(service=service, options=options)
driver.get('http://www.baidu.com')
```
