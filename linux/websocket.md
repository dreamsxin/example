
```shell
# 建立WebSocket连接并保持
websocat --ping-interval 10 ws://example.com:端口/路径
```

```shell
wscat -c ws://example.com:端口/路径
# 手动交互测试，输入消息观察响应
```

模拟网络波动

```shell
# 添加延迟和丢包（需root权限）
sudo tc qdisc add dev eth0 root netem delay 100ms loss 20%

# 测试WebSocket连接稳定性
websocat --ping-interval 5 ws://example.com

# 恢复网络设置
sudo tc qdisc del dev eth0 root
```
