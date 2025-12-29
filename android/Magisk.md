Magisk 
- https://github.com/topjohnwu/Magisk
- Systemless Root
- Zygisk-Assistant
https://github.com/snake-4/Zygisk-Assistant

- https://github.com/hang666/JustTrustMePro
## DenyList不生效
```bash
# 1. 确保Zygisk已启用
magisk --zygisk enable

# 2. 检查DenyList是否正确配置
magisk --denylist status

# 3. 清除目标应用数据
pm clear <package_name>

# 4. 强制停止应用后重新打开
am force-stop <package_name>

# 5. 重启设备
reboot

```
## Magisk伪装后无法找到
```bash
# 方法1：通过ADB查找
adb shell pm list packages | grep -v "com.topjohnwu.magisk"

# 方法2：查看最近安装的应用
adb shell pm list packages -i | sort -k 2 -t '=' -r | head -n 10

# 方法3：使用Activity启动
adb shell am start -n <伪装后的包名>/.ui.MainActivity

```
