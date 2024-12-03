# -*- coding:utf-8 -*-
import time
from selenium import webdriver
from selenium.webdriver.chrome.service import Service
import requests


def driver_setting_yundeng(browserid):
    # time.sleep(2)

    headers = {
        'Content-Type': 'application/json'
    }
    url = f"http://localhost:xxxxx/api/v2/browser/start?account_id={browserid}"
    response = requests.get(url, headers=headers)
    json_data = response.json()
    print('获取信息', json_data)

    options = webdriver.ChromeOptions()
    options.add_argument('--no-sandbox')
    options.add_experimental_option("debuggerAddress", json_data['data']['ws']['selenium'])
    # 指定chromedriver的路径
    chromedriver_path = json_data['data']['webdriver']
    service = Service(executable_path=chromedriver_path)

    driver = webdriver.Chrome(options=options, service=service)
    return driver

def driver_stop(browserid):
    headers = {
        'Content-Type': 'application/json'
    }
    url = f"http://localhost:xxxx/api/v2/browser/stop?account_id={browserid}"
    response = requests.get(url, headers=headers)
    json_data = response.json()
    print('关闭浏览器', json_data)

