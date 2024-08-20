
## 创建虚拟 Wifi
来创建一个名为"wifi123"，密码为"123456"的虚拟WiFi网络‌。（services.msc 中确保服务启动 WLAN AutoConfig）
```shell
netsh wlan set hostednetwork mode=allow ssid=wifi123 key=12345678
netsh wlan start hostednetwork
```


