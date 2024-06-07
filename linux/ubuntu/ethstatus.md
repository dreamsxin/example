# 监控当前系统的网速带宽流量

- iftop 通过进程号跟踪网络连接
- nethogs 快速告知哪些进程在占用带宽
- vnstat 实时记录网络使用情况

## iftop

令人称赞的 iftop 可以监听您指定的网络接口，并以 top 的样式呈现。
这是一个不错的小工具，可以用于找出网络拥塞，测速和维持网络流量总量。

```shell
sudo iftop
sudo iftop -i wlan0
```

单独监测一台主机。需要该主机的 IP 地址和子网掩码。
使用 dig 命令找到 IP 地址：

```shell
dig A myleftstudio.com
```

使用 ipcalc 得到子网掩码：
```shell
ipcalc -b x.x.x.x
```

现在，将 IP 地址和子网掩码提供给 iftop：
```shell
sudo iftop -F x.x.x.x/x -i wlan0
```

## ipcalc

```shell
# 输入一个 IP 地址和子网掩码
$ ipcalc 192.0.2.0/24
Address:   192.0.2.0       11000000.00000000.00000010.00000000
Netmask:   255.255.255.0 = 24
野蛮模式：/24
Hostmask:  0.0.0.255      00000000.00000000.00000000.11111111
Broadcast: 192.0.2.255    11000000.00000000.00000010.11111111
Network:   192.0.2.0/24

```
## nethogs

Nethogs 指定要监听的接口，它会给你显示大量的应用程序及其进程号，所以如果你想的话，你可以借此杀死任一进程。

```shell
sudo nethogs wlan0
```

## vnstat

vnstat 是最容易使用的网络数据收集工具，默认情况下它会显示所有的网络接口累计的数据。使用 -i 选项来选择某个接口。也可以像这样合并多个接口的数据：

```shell
vnstat -i wlan0+eth0+eth1
```

## ethstatus

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

## nload

nload 是一个控制台应用程序，可实时监控网络流量和带宽使用情况。
