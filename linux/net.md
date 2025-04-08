## 查看网络设备状态

```shell
ip link show
ip addr show
# 启用网卡
sudo ip link set enp7s0 up
sudo ip link set enp7s0 down
```

-‌ MULTICAST‌：表示这个网卡可以发送多播包。多播是一种网络通信方式，允许一个发送方将数据同时发送给多个接收方，而不必为每个接收方单独发送数据包‌。
‌-‌ UP‌：表示网卡处于启动状态，但不一定有物理连接‌。
-‌ ‌LOWER_UP‌：表示L1层（物理层）是启动的，即插着网线，有物理连接‌。
-‌ ‌LOOPBACK‌：表示这是一个环回接口，通常用于本机通信，地址通常为127.0.0.1‌。
-‌ ‌RUNNING‌：表示网卡不仅启动了，而且正在运行中，通常用于指示网络接口的物理和逻辑连接都已就绪‌。

```shell
# 查看启用的网卡
ifconfig
# 查看所有网卡
ifconfig -a
# 启用网卡
sudo ifconfig enp7s0 up
```

### 配置 IP

```shell
sudo ip addr add 192.168.1.100/24 dev eth0
sudo ifconfig eth0 192.168.1.100 netmask 255.255.255.0 up
```

**DHCP**
```shell
sudo systemctl enable dhcpcd
sudo systemctl start dhcpcd
# or
sudo systemctl enable dhclient
sudo systemctl start dhclient
```
`/etc/network/interfaces`
```conf
auto eth0
iface eth0 inet dhcp
#or
ADDRESS=192.168.0.10
NETMASK=255.255.255.0

DEVICE=eth0 #物理设备名
IPADDR=192.168.1.10 #IP地址
NETMASK=255.255.255.0 #掩码值
NETWORK=192.168.1.0 #网络地址(可不要)
BROADCAST=192.168.1.255 #广播地址（可不要）
GATEWAY=192.168.1.1 #网关地址
ONBOOT=yes # [yes|no]（引导时是否激活设备）
USERCTL=no #[yes|no]（非root用户是否可以控制该设备）
BOOTPROTO=static #[none|static|bootp|dhcp]（引导时不使用协议|静态分配|BOOTP协议|DHCP协议）
```

## 查看系统中都在监听哪些端口

```shell
netstat -ntl
```

## 解决 Linux 下 TIME_WAIT 和 CLOSE_WAIT 过多的问题

使用以下命令查看 TIME_WAIT 和 CLOSE_WAIT 链接状态。

```shell
netstat -n | awk '/^tcp/ {++S[$NF]} END {for(a in S) print a, S[a]}'
```

* TIME_WAIT

主动关闭连接的一方保持的状态，对于服务器来说它本身就是“客户端”，在完成一个爬取任务之后，它就会发起主动关闭连接，从而进入TIME_WAIT的状态，然后在保持这个状态2MSL（max segment lifetime）时间之后，彻底关闭回收资源。这个是 TCP/IP 的设计者规定的，主要出于以下两个方面的考虑：

1. 防止上一次连接中的包，迷路后重新出现，影响新连接（经过2MSL，上一次连接中所有的重复包都会消失）
2. 可靠的关闭TCP连接。在主动关闭方发送的最后一个 ACK(FIN) ，有可能丢失，这时被动方会重新发 FIN，如果这时主动方处于 CLOSED 状态，就会响应 RST 而不是 ACK；所以主动方要处于 TIME_WAIT 状态，而不能是 CLOSED。另外这么设计 TIME_WAIT 会定时的回收资源，并不会占用很大资源的，除非短时间内接受大量请求或者受到攻击。

解决方案可以通过修改/etc/sysctl.conf文件，服务器能够快速回收和重用 TIME_WAIT 的资源：

```text
net.ipv4.tcp_syncookies = 1  #表示开启SYN Cookies。当出现SYN等待队列溢出时，启用cookies来处理，可防范少量SYN攻击，默认为0，表示关闭         
net.ipv4.tcp_tw_reuse = 1    #表示开启重用。允许将TIME-WAIT sockets重新用于新的TCP连接，默认为0，表示关闭  
net.ipv4.tcp_tw_recycle = 1  #表示开启TCP连接中TIME-WAIT sockets的快速回收，默认为0，表示关闭    
net.ipv4.tcp_fin_timeout=30  #表示如果套接字由本端要求关闭，这个参数决定了它保持在FIN-WAIT-2状态的时间
```

执行`/sbin/sysctl -p`让参数生效

* CLOSE_WAIT

对于已经建立的连接，网络双方要进行四次握手才能成功断开连接，如果缺少了其中某个步骤，将会使连接处于假死状态，连接本身占用的资源不会被释放。网络服务器程序要同时管理大量连接，所以很有必要保证无用连接完全断开，否则大量僵死的连接会浪费许多服务器资源。

导致 CLOSE_WAIT 大量存在的原因：就是在对方关闭连接之后服务器程序自己没有进一步发出 ACK 信号。换句话说，就是在对方连接关闭之后，程序里没有检测到，或者程序压根就忘记了这个时候需要关闭连接，于是这个资源就一直被程序占着；服务器对于程序抢占的资源没有主动回收的权利，除非终止程序运行。

解决方案：代码需要判断 socket ，一旦读到 0，断开连接，read 返回负，检查一下 errno，如果不是 AGAIN，就断开连接。
所以解决 CLOSE_WAIT 大量存在的方法还是从自身的代码出发。

## ping 丢包

```shell
ping -c 100 -i 1 -s 1024 -f ip
```

* mtr 路由逐跳 ping

```shell
mtr -c 10 -i 1 -n -r ip
```

    mtr报告各列含义
```text
    Loss%       表示在每一跳的丢包率
    Snt         每个中间设备收到的发送的报的数目（上图为400个包），MTR会同            时对所有中间节点发送ICMP包进行测试。
    Last        最后一个数据包往返时间（ms）
    Avg         数据包往返平均时间（ms）
    Best        数据包往返最小时间（ms）
    Wrst        数据包往返最大时间（ms）
    StDev       标准偏差。如果标准偏差越高，说明数据包在这个节点上的延时越  
```

接下来接着说相关参数：

- -s 用来指定ping数据包的大小
- -n no-dns不对IP地址做域名解析
- -a 来设置发送数据包的IP地址 这个对一个主机由多个IP地址是有用的
- -i 使用这个参数来设置ICMP返回之间的要求默认是1秒
- -4 IPv4
- -6 IPv6
