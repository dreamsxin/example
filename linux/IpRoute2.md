# IpRoute2 简明教程

前言
最近想学习一下 iproute2 的用法，Google 搜索后发现了这篇排名靠前的文章。本文是自己基于对原文理解的意译，如有问题，欢迎大家指出。

 

## 介绍
在管理 Linux 服务器和排查网络问题时，熟悉一些基本的网络工具是很有帮助的。这些工具中的一部分最初是为了查看网络状态而编写的，但其中也包含了一些可以用来管理和配置网络连接的底层工具。

一直以来，这些互不相关的网络工具被打包在一起发布，它们被统称为 net-tools。虽然 net-tools 工具包提供了相当完善的功能，但是包内工具的开发却是相互独立的，它们的使用方式也各不相同。

由于 net-tools 包内工具的使用方式大相径庭，并且包内的部分工具也已经停止维护，因此使用名为 iproute2 的新工具包来替换原来 net-tools。iproute2 包内的各个工具被统一开发维护，并且他们之间的使用方式也非常相似。另外，这些工具也可以很好地互相配合一起使用。

在本篇教程中，我们将会讨论如何使用 iproute2 来对你的网络进行配置、维护和信息查看。我们将会使用 Ubuntu 12.04 VPS 来进行演示，不过大多数其它现代 Linux 发行版都提供了相同的功能。

虽然查询指令可以在非特权模式（非 root）下执行，不过在修改网络配置时，大多数情况下还是需要 root 权限的。

 

## 如何查看网络接口、地址、路由
iproute2 工具包最基础的功能就是管理本机的网络接口。

一般来说，本机的网络接口通常会被命名成像是 eth0，eth1，lo 这样的名称。在过去，常常使用 net-tools 包提供的 ifconfig 命令来配置网络接口。而使用 iproute2 时，通常可以使用子命令 ip addr 和 ip link 来完成相同的功能。

使用 ifconfig 时，你可以通过输入不带参数的如下命令来查看本机当前所有网络接口的状态：
```shell
ifconfig
eth0      Link encap:Ethernet  HWaddr 54:be:f7:08:c2:1b  
          inet addr:192.168.56.126  Bcast:192.168.56.255  Mask:255.255.255.0
          inet6 addr: fe80::56be:f7ff:fe08:c21b/64 Scope:Link
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:114273 errors:0 dropped:0 overruns:0 frame:0
          TX packets:58866 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000 
          RX bytes:73490903 (73.4 MB)  TX bytes:14294252 (14.2 MB)
          Interrupt:20 Memory:f7f00000-f7f20000 

lo        Link encap:Local Loopback  
          inet addr:127.0.0.1  Mask:255.0.0.0
          inet6 addr: ::1/128 Scope:Host
          UP LOOPBACK RUNNING  MTU:65536  Metric:1
          RX packets:3942 errors:0 dropped:0 overruns:0 frame:0
          TX packets:3942 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:0 
          RX bytes:668121 (668.1 KB)  TX bytes:668121 (668.1 KB)
```
如果想要查看某个特定网络接口的信息，你可以将网络接口名称作为参数传给 ifconfig：
```shell
ifconfig eth0
eth0      Link encap:Ethernet  HWaddr 54:be:f7:08:c2:1b  
          inet addr:192.168.56.126  Bcast:192.168.56.255  Mask:255.255.255.0
          inet6 addr: fe80::56be:f7ff:fe08:c21b/64 Scope:Link
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:114273 errors:0 dropped:0 overruns:0 frame:0
          TX packets:58866 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000 
          RX bytes:73490903 (73.4 MB)  TX bytes:14294252 (14.2 MB)
          Interrupt:20 Memory:f7f00000-f7f20000
```
使用 iproute2 包提供的子命令，我们可以实现相同的功能。

如果想要查看每个网络接口被配置的地址，可以输入不带参数的 ip addr 命令：
```shell
ip addr
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN 
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host 
       valid_lft forever preferred_lft forever
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP qlen 1000
    link/ether 54:be:f7:08:c2:1b brd ff:ff:ff:ff:ff:ff
    inet 192.168.56.126/24 brd 192.168.56.255 scope global eth0
       valid_lft forever preferred_lft forever
    inet6 fe80::56be:f7ff:fe08:c21b/64 scope link 
       valid_lft forever preferred_lft forever
```
如果想要查看某个特定网络接口的地址信息，你可以使用如下格式的命令：
```shell
ip addr show eth0
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP qlen 1000
    link/ether 54:be:f7:08:c2:1b brd ff:ff:ff:ff:ff:ff
    inet 192.168.56.126/24 brd 192.168.56.255 scope global eth0
       valid_lft forever preferred_lft forever
    inet6 fe80::56be:f7ff:fe08:c21b/64 scope link 
       valid_lft forever preferred_lft forever
```
事实上，ip addr 只是 ip addr show 命令的别名。

如果你只关心网络接口本身，而不在意它们被配置的地址，那么你可以使用 ip link 子命令：
```shell
ip link
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN 
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP qlen 1000
    link/ether 04:01:13:8a:a2:01 brd ff:ff:ff:ff:ff:ff
```
如果想要查看某个特定网络接口的信息，那么你可以添加关键字 show 和对应网络接口的名字：
```shell
ip link show eth0
```
如果想要获得网络接口如何通信的统计数据，那么你可以给 link 子命令传入 -s 选项：
```shell
ip -s link show eth0
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP qlen 1000
    link/ether 04:01:13:8a:a2:01 brd ff:ff:ff:ff:ff:ff
    RX: bytes  packets  errors  dropped overrun mcast   
    853144     14672    0       0       0       0      
    TX: bytes  packets  errors  dropped carrier collsns 
    91257      537      0       0       0       0
```
那么我们如何找到我们的路由表呢？路由表包含了到达其他网络地址的路径信息，我们可以通过输入以下内容来打印当前的路由表：
```shell
ip route show
default via 107.170.58.1 dev eth0  metric 100
107.170.58.0/24 dev eth0  proto kernel  scope link  src 107.170.58.162
```
这条命令的输出内容告诉我们，通往更大互联网的默认路由是通过 eth0 接口到达地址为 107.170.58.1 的网关。通过 eth0 接口可以访问地址为 107.170.58.1 的网关，并且 eth0 接口自身的 ip 地址是 107.170.58.162。

 
## 如何配置网络接口和地址
既然你已经熟悉了如何获取网络接口信息和与它们关联的地址信息，那么下一步就应该是了解如何修改网络接口的状态了。

首先需要了解如何配置接口本身。你仍然可以使用 ip link 子命令，不过这次你需要传入 set 而不是 show 来修改配置。

例如，我们可以通过使用以下命令来打开或关闭网络接口：
```shell
ip link set eth1 up
ip link set eth1 down
```
注意：当心不要意外关闭了你正连接的 VPS 的网络接口！

你也可以使用 ip link 子命令来配置网络接口的属性。比如，如果你想打开或关闭网络接口的多播标志，你可以输入：
```shell
ip link set eth1 multicast on
ip link set eth1 multicast off
```
你也可以输入如下的命令来修改 MTU 和分组队列的长度：
```shell
ip link set eth1 mtu 1500
ip link set eth1 txqueuelen 1000
```
如果你正配置的网络接口是关闭（down）状态，那么你可以修改网络接口的名称和与之关联的 arp 标志：
```shell
ip link set eth1 name eth10
ip link set eth1 arp on
```
如果要修改网络接口的地址，那么我们可以再次使用 ip addr 子命令。

通过输入以下命令我们可以为网络接口添加一个地址：
```shell
ip addr add IPADDRESS/NETPREFIX brd + dev INTERFACE
```
该命令的 brd + 部分会自动设置广播地址。每个网络接口都可以添加多个地址而不会出现任何问题。

我们也可以执行相反的操作来删除网络地址。要删除指定网络接口的某个地址，可以使用如下命令：
```shell
ip addr del IPADDRESS/NETPREFIX dev INTERFACE
```
你也可以省略输入地址，这样与该网络接口关联的第一个被列出的地址将被删除。

通过使用 ip route [add | change | replace | delete ] 语法，你也可以删除路由规则。但我们不会在这里介绍，因为大多数人不会定期对此进行调整。

## IpRoute2 的其他功能
IPRoute2 还具有一些其他功能，不过我们将不会在本教程中进行深入讨论。相反，我们将讨论这些是什么，以及在什么情况下你可能会发现它们有用。

IP 路由规则（rule）的概念很难谈论，因为它非常依赖于具体的情况。基本上，你可以基于多个字段（包括目标地址，源地址，路由协议，数据包大小等）来决定如何路由流量。

我们使用 ip rule 子命令使用此功能。基本查询命令遵循和其他子命令一样的语法：
```shell
ip rule show
0:  from all lookup local 
32766:  from all lookup main 
32767:  from all lookup default
```
这三条路由规则是内核配置的默认规则。第一行可以匹配任何流量，用于路由高优先级的流量。第二行是处理常规路由的主要规则。最后一行是空规则，如果上面的规则与数据包不匹配，则使用该行进行后处理（post-processing）。

由 IPRoute2 配置的路由规则被存储在路由策略数据库（routing policy database）中。通过与规则集合进行匹配来选择该数据库中的路由策略。我们可以使用适当的操作来添加或删除规则。但是，你不应该在不知道自己在做什么的情况下执行这些操作。

可以通过 man 查看 ip rule 的手册来进行进一步的学习。
```shell
man ip         # search for "ip rule"
```
我们将简要讨论的另一件事是通过这些工具来处理 arp 信息。处理这些信息的子命令被称作 ip neigh。
```shell
ip neigh
107.170.58.1 dev eth0 lladdr 00:00:5e:00:01:68 DELAY
```
默认情况下，这里至少应列出你的网关。ARP（Address Resolution Protocol）是地址解析协议的缩写，它可以通过 IP 地址来获取 MAC 地址。

基本上，每当需要转发 IP 分组时，你的主机就会在本地网络上广播 ARP 请求，向网络中的其他主机询问谁拥有这个 IP 地址。拥有这个 IP 地址的主机将把自己的 MAC 地址回复给询问者，然后询问者就知道该把 IP 分组发给谁了。询问者会把 IP 地址到 MAC 地址的映射缓存到本地（一般持续 15 分钟），这样后续的分组在需要转发时就不需要再次通过 ARP 请求询问了。

## 结语
现在，你应该对如何使用 iproute2 包中的工具有了一个很好的了解了。

尽管许多指南和教程仍然引用旧的 net-tools 工具包，部分原因是有经验的系统管理员过去经常使用旧的工具包，但本指南中讨论的命令将在未来几年内逐步替代 net-tools 工具。

重要的是，现在你必须熟悉这些命令，然后才能在已切换到这些命令的系统上解决网络问题（Arch Linux 早在 2011 年就已经完全切换到了新工具）。新工具用法更加一致，你可以指望在所有命令中都可以使用类似的用法。你使用这些命令的次数越多，它们对你而言就越自然。

By Justin Ellingwood.
