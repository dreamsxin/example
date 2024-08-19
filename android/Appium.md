
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
# 查看已连接设备
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
