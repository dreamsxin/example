```shell
# codding=utf-8
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service

from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from warnings import filterwarnings

filterwarnings("ignore")  # 如果我们得到任何不必要的错误，为了让我们看到
from time import sleep

options = Options()
options.add_argument('--headless')
options.add_argument('--no-sandbox')  # fix:DevToolsActivePort file doesn't exist
options.add_argument('--disable-gpu')  # fix:DevToolsActivePort file doesn't exist
options.add_argument('--disable-dev-shm-usage')  # fix:DevToolsActivePort file doesn't exist
options.add_argument('--remote-debugging-port=9222')  # fix:DevToolsActivePort file doesn'
options.binary_location = r'C:\Users\Administrator\FbBrowser\2.0.1.2\YunLogin.exe'


service = Service(executable_path=r'C:\Users\Administrator\FbBrowser\2.0.1.2\chromedriver.exe')

dr = webdriver.Chrome(service=service, options=options)
# driver.get('http://www.baidu.com')

print("title of current page is %s\n" % (dr.title))
print("url of current page is %s\n" % (dr.current_url))

btnNewCreateBrowser = dr.find_element(By.XPATH, '//*[@id="app"]/div[1]/div[1]/div[1]/button')
name = btnNewCreateBrowser.text

print(name)

btnNewCreateBrowser.click()

```
