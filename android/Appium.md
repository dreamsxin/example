
- https://github.com/appium/appium

## 环境变量

- path
E:\data\Android\platform-tools

- JAVA_HOME
https://adoptium.net/en-GB/download/

- ANDROID_HOME nor ANDROID_SDK_ROOT


## 安装

```shell
npm install -g appium
appium driver install uiautomator2
```
运行 appinum 服务
```shell
appium
```

## Appium Inspector

https://github.com/appium/appium-inspector

## 启动模拟器

```shell
 .\emulator.exe -avd Medium_Phone_API_35
```
**开发者模式**
打开 Setting -> About emulated device
连续点击 Build numnber 

## 编写测试

```shell
npm init
npm i --save-dev webdriverio
```

```shell
# 查看已连接设备名
adb devices
# 查看设备版本
adb shell getprop ro.build.version.release
```
```json
{
  "platformName": "Android",
  "platformVersion": "15",
  "appium:automationName": "UiAutomator2",
  "appium:deviceName": "Android",
  "appium:appPackage": "com.android.settings",
  "appium:appActivity": ".Settings"
}
```

在 Appium Inspector 点击启动 Start Session 即可看到效果。

**获取当前应用信息**
只需要去掉appPackage、appActivity即可。
```json
{
  "platformName": "Android",
  "platformVersion": "15",
  "deviceName": "Android",
  "automationName": "uiautomator2"
}
```

## 获取当前应用包名

> 可以直接获取 apk 包名：aapt dump badging *.apk

```python
from appium import webdriver
 
# 设置Desired Capabilities
desired_caps = {
    'platformName': 'Android',
    'deviceName': 'your_device_name',
    'appPackage': 'com.example.app',  # 替换为你的应用包名
    'appActivity': '.MainActivity'  # 替换为你的应用Activity
}
 
# 初始化WebDriver
driver = webdriver.Remote('http://localhost:4723/wd/hub', desired_caps)
 
# 获取当前应用的包名和Activity
package_name = driver.current_package
activity = driver.current_activity
 
print("Package Name:", package_name)
print("Activity:", activity)
 
# 其他测试代码...
 
# 关闭Appium会话
driver.quit()
```

## 遍历

```python
from appium import webdriver
from appium.webdriver.common.mobileby import MobileBy
from appium.webdriver.common.touch_action import TouchAction
from appium.webdriver.common.multi_action import MultiAction

# 设置Appium连接参数：
desired_caps = {
    "platformName": "Android",
    "platformVersion": "8.0.0",
    "deviceName": "Android Emulator",
    "appPackage": "com.example.app",
    "appActivity": "com.example.app.MainActivity"
}

# 创建Appium驱动对象：
driver = webdriver.Remote('http://localhost:4723/wd/hub', desired_caps)

# 获取当前页面所有控件
elements = driver.find_elements(MobileBy.XPATH, "//*")

# 遍历控件
for element in elements:
    # 进行相应的操作，如获取文本、点击等
    print(element.text)
    element.click()

# 关闭Appium驱动：
driver.quit()
```
