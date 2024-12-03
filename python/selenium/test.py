import requests
import time
from selenium import webdriver
from selenium.webdriver.chrome.service import Service

def start_browser(api_url, account_id):
    params = {"account_id": account_id}
    response = requests.get(api_url, params=params)
    if response.status_code == 200 and response.json().get("code") == 0:
        data = response.json().get("data")
        return data
    else:
        print("浏览器启动失败：", response.text)
        return None

# API的基础URL
base_url = "http://localhost:xxxxxx" #请替换为实际的API基础URL
endpoint = "/v2/browser/start"
api_url = f"{base_url}{endpoint}"
account_id = "" #替换为实际的账号ID

# 启动浏览器并获取连接信息
browser_data = start_browser(api_url, account_id)

print(browser_data)
if browser_data:

    options = webdriver.ChromeOptions()
    options.add_argument('--no-sandbox')
    options.add_experimental_option("debuggerAddress", browser_data['ws']['selenium'])

    selenium_url = f"http://{browser_data['ws']['selenium']}/wd/hub"
    webdriver_path = browser_data['webdriver']
    service = Service(executable_path=webdriver_path)
    driver = webdriver.Chrome(service=service, options=options)

    time.sleep(5) # Let the user actually see something!

    driver.close()
    driver.quit()
