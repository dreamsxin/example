# ethstatus

实时监控网卡带宽占用

```shell
sudo apt-get install ethstatus
# sudo ethstatus -i ppp0
sudo ethstatus -i eth0
```

```text
┌──────────────────────────────.EthStatus v0.4a.───────────────────────────────┐
│                                                                              │
│                                                                              │
│                                                                              │
│                                                                              │
│                                                                              │
│                                                                              │
│                                                                              │
│                                                                              │
│                                                                              │
│                                                                           ## │
│                                                                              │
│         ###                                                                  │
│        ON/OFF                      RX  TX                                    │
│                           384.00 B/s   6 Packets/s                           │
│                                                                              │
│                                                                              │
│     IP Address:          192.168.1.6                                         │
│     Interface name:      wlan0         Graphic Using Speed:    9.77 MB/s     │
│     Top Speed:           988.00 B/s    Top Packets/s:          14            │
│     Received Packets:    27137         Transmited Packets:     28802         │
│     Received:            19.04 MB      Transmitted:            5.08 MB       │
│     Errors on Receiving: 0             Errors on Transmission: 0             │
└──────────────────────────────────────────────────────────────────────────────┘
```

相关解释：

- RX:接受
- TX:发送
- Top Speed：最高速度
- Received:接收总流量
- Transmitted:发送总流量
- Interface name:当前监听网卡