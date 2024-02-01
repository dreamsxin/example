#
```shell
modinfo ip_tables
```
iptables 实际上只是位于用户空间的一个面向系统管理员的Linux防火墙的管理工具而已，而真正实现防火墙功能的是 `netfilter`，它是 Linux 内核中实现包过滤的内核模块，iptables 对应在内核中的模块应该是 ip_tables，我们查看系统内核中 ip_tables 的信息的时候可以看到 `ip_tables.ko` 这个模块是在 netfilter 这个目录下的。
实际上除了iptables还有如nftables、firewalld等防火墙工具都是在用户空间（用户层）对相应的内核空间中对应的netfilter相关的模块进行操作的工具。

## iptables的发展

iptables 的前身叫 ipfirewall （内核1.x时代）,这是一个作者从freeBSD上移植过来的，能够工作在内核当中的，对数据包进行检测的一款简易访问控制工具。但是 `ipfirewall` 工作功能极其有限(它需要将所有的规则都放进内核当中，这样规则才能够运行起来，而放进内核，这个做法一般是极其困难的)。当内核发展到2.x系列的时候，软件更名为ipchains，它可以定义多条规则，将他们串起来，共同发挥作用，而现在，它叫做 iptables，可以将规则组成一个列表，实现绝对详细的访问控制功能。

他们都是工作在用户空间中，定义规则的工具，本身并不算是防火墙。它们定义的规则，可以让在内核空间当中的 `netfilter` 来读取，并且实现让防火墙工作。而放入内核的地方必须要是特定的位置，必须是tcp/ip的协议栈经过的地方。而这个tcp/ip协议栈必须经过的地方，可以实现读取规则的地方就叫做 netfilter.(网络过滤器)

### iptables的常见情况
下面我们利用上面的流程图来对几种常见的情况解析：关键点在于发往iptables主机的数据包的目的地址是否是iptables主机本机。如果是，那我们就可以理解为常见的开启了iptables防火墙的网站服务器主机；如果不是，那就是走ip_forward进行转发，比如我们常见的NAT路由器的NAT服务和策略路由等。如下图为开启了ip_forward功能的openwrt路由器。

## 四表五链

iptables 一共在内核空间中选择了5个位置，

- 1.内核空间中：从一个网络接口进来，到另一个网络接口去的
- 2.数据包从内核流入用户空间的
- 3.数据包从用户空间流出的
- 4.进入/离开本机的外网接口
- 5.进入/离开本机的内网接口
- 
### 四表

iptables 的四个表 iptable_filter，iptable_mangle，iptable_nat，iptable_raw，默认表是filter（没有指定表的时候就是filter表）。

- filter 表：用来对数据包进行过滤，具体的规则要求决定如何处理一个数据包。
对应的内核模块为：iptable_filter，其表内包括三个链：input、forward、output;

- nat 表：nat 全称：network address translation 网络地址转换，主要用来修改数据包的 IP 地址、端口号信息。
对应的内核模块为：iptable_nat，其表内包括三个链：prerouting、postrouting、output;

- mangle 表：主要用来修改数据包的服务类型，生存周期，为数据包设置标记，实现流量整形、策略路由等。
对应的内核模块为：iptable_mangle，其表内包括五个链：prerouting、postrouting、input、output、forward;

- raw 表：主要用来决定是否对数据包进行状态跟踪。
对应的内核模块为：iptable_raw，其表内包括两个链：output、prerouting;

raw表只使用在PREROUTING链和OUTPUT链上,因为优先级最高，从而可以对收到的数据包在系统进行ip_conntrack（连接跟踪）前进行处理。一但用户使用了raw表,在某个链上，raw表处理完后，将跳过NAT表和ip_conntrack处理，即不再做地址转换和数据包的链接跟踪处理了。RAW表可以应用在那些不需要做nat的情况下，以提高性能。

### 五链

iptables的五个链PREROUTING，INPUT，FORWARD，OUTPUT，POSTROUTING。

input 链：当收到访问防火墙本机地址的数据包时，将应用此链中的规则；
output 链：当防火墙本机向外发送数据包时，将应用此链中的规则；
forward 链：当收到需要通过防火中转发给其他地址的数据包时，将应用此链中的规则，注意如果需要实现forward转发需要开启Linux内核中的ip_forward功能；
prerouting 链：在对数据包做路由选择之前，将应用此链中的规则；
postrouting 链：在对数据包做路由选择之后，将应用此链中的规则；

之所以叫做链就是因为在访问该链的时候会按照每个链对应的表依次进行查询匹配执行的操作，如 PREROUTING 链对应的就是(raw->mangle->nat)，每个表按照优先级顺序进行连接，每个表中还可能有多个规则，因此最后看起来就像链一样，因此称为链。而 iptables 的表中存储的就是对应的规则和需要执行的操作，这里以路由器为例查看其中 iptables 的 filter 表：
```shell
iptables -t filter -nvL
```
注意每一个链对应的表都是不完全一样的，表和链之间是多对多的对应关系。但是不管一个链对应多少个表，它的表都是按照下面的优先顺序来进行查找匹配的。

表的处理优先级：`raw>mangle>nat>filter`。


```shell
m state --state NEW,ESTABLISHED -j ACCEPT

iptables -R INPUT 1 -d 172.16.100.1 -p udp --dport 53 -j ACCEPT
```

练习题2：

假如我们允许自己ping别人，但是别人ping自己ping不通如何实现呢？

分析：对于ping这个协议，进来的为8（ping），出去的为0(响应).我们为了达到目的，需要8出去,允许0进来

在出去的端口上：iptables -A OUTPUT -p icmp --icmp-type 8 -j ACCEPT

在进来的端口上：iptables -A INPUT -p icmp --icmp-type 0 -j ACCEPT

小扩展：对于127.0.0.1比较特殊，我们需要明确定义它

```shell
iptables -A INPUT -s 127.0.0.1 -d 127.0.0.1 -j ACCEPT
iptables -A OUTPUT -s 127.0.0.1 -d 127.0.0.1 -j ACCEPT
```

8、SNAT和DNAT的实现
由于我们现在IP地址十分紧俏，已经分配完了，这就导致我们必须要进行地址转换，来节约我们仅剩的一点IP资源。那么通过iptables如何实现NAT的地址转换呢？

8.1、SNAT基于原地址的转换

基于原地址的转换一般用在我们的许多内网用户通过一个外网的口上网的时候，这时我们将我们内网的地址转换为一个外网的IP，我们就可以实现连接其他外网IP的功能。

所以我们在iptables中就要定义到底如何转换：

定义的样式：

比如我们现在要将所有192.168.10.0网段的IP在经过的时候全都转换成172.16.100.1这个假设出来的外网地址：

`iptables -t nat -A POSTROUTING -s 192.168.10.0/24 -j SNAT --to-source 172.16.100.1`

这样，只要是来自本地网络的试图通过网卡访问网络的，都会被统统转换成172.16.100.1这个IP.

那么，如果172.16.100.1不是固定的怎么办？

我们都知道当我们使用联通或者电信上网的时候，一般它都会在每次你开机的时候随机生成一个外网的IP，意思就是外网地址是动态变换的。这时我们就要将外网地址换成 MASQUERADE(动态伪装):它可以实现自动寻找到外网地址，而自动将其改为正确的外网地址。所以，我们就需要这样设置：

iptables -t nat -A POSTROUTING -s 192.168.10.0/24 -j MASQUERADE

这里要注意：地址伪装并不适用于所有的地方。

8.2、DNAT目标地址转换

对于目标地址转换，数据流向是从外向内的，外面的是客户端，里面的是服务器端通过目标地址转换，我们可以让外面的ip通过我们对外的外网ip来访问我们服务器不同的服务器，而我们的服务却放在内网服务器的不同的服务器上。

如何做目标地址转换呢？：

`iptables -t nat -A PREROUTING -d 192.168.10.18 -p tcp --dport 80 -j DNAT --todestination 172.16.100.2`

目标地址转换要做在到达网卡之前进行转换,所以要做在PREROUTING这个位置上

9、控制规则的存放以及开启
注意：你所定义的所有内容，当你重启的时候都会失效，要想我们能够生效，需要使用一个命令将它保存起来

9.1、service iptables save 命令

它会保存在/etc/sysconfig/iptables这个文件中

9.2、iptables-save 命令

`iptables-save > /etc/sysconfig/iptables`

9.3、iptables-restore 命令

开机的时候，它会自动加载/etc/sysconfig/iptabels

如果开机不能加载或者没有加载，而你想让一个自己写的配置文件（假设为iptables.2）手动生效的话：

`iptables-restore < /etc/sysconfig/iptables.2`，则完成了将iptables中定义的规则手动生效

10、总结
Iptables是一个非常重要的工具，它是每一个防火墙上几乎必备的设置，也是我们在做大型网络的时候，为了很多原因而必须要设置的。学好Iptables,可以让我们对整个网络的结构有一个比较深刻的了解，同时，我们还能够将内核空间中数据的走向以及linux的安全给掌握的非常透彻。我们在学习的时候，尽量能结合着各种各样的项目，实验来完成，这样对你加深iptables的配置，以及各种技巧有非常大的帮助。


## 命令

iptables配置

在了解清楚iptables的工作原理和每个表以及链的作用之后，我们就可以根据其特点进行针对性的配置。

iptables 的基本语法命令格式

`iptables [-t 表名上面的4表] 管理选项 [链名五链] [匹配条件] [-j 控制类型]`
- 表名、链名：指定iptables命令所操作的表和链，未指定表名时将默认使用filter表；
- 管理选项：表示iptables规则的操作方式，比如：插入、增加、删除、查看等；
- 匹配条件：指定要处理的数据包的特征，不符合指定条件的数据包不处理；
- 控制类型：指数据包的处理方式，比如：允许accept、拒绝reject、丢弃drop、日志LOG等；

```txt
iptables 命令的常用管理选项
-A:在指定链的末尾添加一条新的规则
-D:删除指定链中的某一条规则，可删除指定序号或具体内容
-I:在指定链中插入一条新规则，未指定序号时默认作为第一条规则
-R:修改、替换指定链中的某一条规则，可指定规则序号或具体内容
-L:列出指定链中所有的规则，未指定链名，则列出表中的所有链
-F:清空指定链中所有的规则，未指定链名，则清空表中的所有链
-P:设置指定链的默认策略
-n:使用数字形式显示输出结果
-v:查看规则列表时显示详细的信息
-h:查看命令帮助信息
--line-numbers:查看规则列表时，同时显示规则在链中的顺序号
```
在添加规则之前我们先开启iptables的独立log功能，对于centos6，我们可以执行以下操作：
```shell
echo "kern.*     /var/log/iptables.log" >> /etc/rsyslog.conf
# 记录所有级别的日志到指定目录中
service rsyslog restart
# 重启rsyslog服务使配置生效
```
接着我们开始添加一条比较有针对性的规则：
```shell
iptables -A INPUT -j LOG --log-prefix "*** INPUT ***" --log-level debug
iptables -t filter -A INPUT -p tcp -s 192.168.100.100 --dport 80 -j REJECT
# 查看规则
iptables -nL INPUT
```
上述两条命令第一条增加了一条记录日志的规则，对于INPUT链中的所有操作都记录到日志中，添加日志前缀*** INPUT ***并设定日志级别为 debug
第二条是在INPUT链的filter表中插入一条规则，限定对192.168.100.100这个IP使用tcp协议访问本机的目的端口80端口的时候拒绝掉数据包。

```shell
# 清空规则
iptables --flush
iptables -nL INPUT
```
## 多网卡流量流入流出
### DNAT/SNAT技术实现
方案思路是创建两个虚拟网卡或网桥br-eip1（172.19.0.100）和br-eip2（172.19.0.101）。go应用分别监听网卡br-eip1和网卡br-eip2。通过DNAT技术将来eth0的流量导向br-eip1，将来自eth1的流程导向br-eip2。同过SNAT技术将从br-eip1流出的外部流量导向eth0，br-eip2流出的流量导向eth1

创建虚拟网卡
```shell
apt-get install bridge-utils // 安装brctl

sudo brctl addbr br-eip1 // 添加网桥
sudo ip link set br-eip1 up // 激活网桥
sudo ifconfig  br-eip1 172.19.0.100 // 指定br-eip1网桥的ip

sudo brctl addbr br-eip2 // 添加网桥
sudo ip link set br-eip2 up // 激活网桥
sudo ifconfig  br-eip2 172.19.0.101 // 指定br-eip2网桥的ip

```

配置dnat
```shell
#流入
sudo iptables -t nat -I PREROUTING -d 172.31.0.8/32 ! -i br-eip1 -p tcp -m tcp --dport 8090 -j DNAT --to-destination 172.19.0.100:8090
sudo iptables -t nat -I PREROUTING -d 172.31.0.14/32 ! -i br-eip2 -p tcp -m tcp --dport 8090 -j DNAT --to-destination 172.19.0.101:8090
#流出
sudo iptables -t nat -I POSTROUTING -p all -s 172.19.0.100 -j SNAT --to-source 172.31.0.8
sudo iptables -t nat -I POSTROUTING -p all -s 172.19.0.101 -j SNAT --to-source 172.31.0.14
```

## NAT工作原理
接下来介绍一些NAT(Network Address Translation，网络地址转换)的基本知识，众所周知，IPv4的公网IP地址已经枯竭，但是需要接入互联网的设备还在不断增加，这其中NAT就发挥了很大的作用（此处不讨论IPv6）。NAT服务器提供了一组私有的IP地址池（10.0.0.0/8、172.16.0.0/12、192.168.0.0/16），使得连接该NAT服务器的设备能够获得一个私有的IP地址（也称局域网IP/内网IP），当设备需要连接互联网的时候，NAT服务器将该设备的私有IP转换成可以在互联网上路由的公网IP（全球唯一）。NAT的实现方式有很多种，这里我们主要介绍三种：静态NAT、动态NAT和网络地址端口转换（NAPT）。

### BNAT
静态NAT：LVS的官方文档中也称为(N-to-N mapping)
，前面的N指的是局域网中需要联网的设备数量，后面的N指的是该NAT服务器所拥有的公网IP的数量。既然数量相等，那么就可以实现静态转换，即一个设备对应一个公网IP，这时候的NAT服务器只需要维护一张静态的NAT映射转换表。

内网IP	外网IP
192.168.1.55	219.152.168.222
192.168.1.59	219.152.168.223
192.168.1.155	219.152.168.224
动态NAT：LVS的官方文档中也称为(M-to-N mapping)
，注意这时候的M>N，也就是说局域网中需要联网的设备数量多于NAT服务器拥有的公网IP数量，这时候就需要由NAT服务器来实现动态的转换，这样每个内网设备访问公网的时候使用的公网IP就不一定是同一个IP。

在一些家用路由器中，DMZ是指一部所有端口都暴露在外部网络的内部网络主机，除此以外的端口都被转发。严格来说这不是真正的DMZ，因为该主机仍能访问内部网络，并非独立于内部网络之外的。但真正的DMZ是不允许访问内部网络的，DMZ和内部网络是分开的。这种 DMZ主机并没有真正DMZ所拥有的子网划分的安全优势，其常常以一种简单的方法将所有端口转发到另外的防火墙或NAT设备上。

### NAPT
以上的这两种都属于基本网络地址转换（Basic NAT），这种转换在技术上比较简单，仅支持地址转换，不支持端口映射，这也就带来了另一个问题就是资源的浪费。我们知道一个IP实际上可以对应多个端口，而我们访问应用实际上是通过IP地址+端口号的形式来访问的，即客户端访问的时候发送请求到服务器端应用程序监听的端口即可实现访问。那么NAPT就是在这基础上的扩展延申，它在IP地址的基础上加上了端口号，支持了端口映射的功能。

NAPT：NAPT实际上还可以分为源地址转换（SNAT）和目的地址转换（DNAT）两种。注意这个源地址和目的地址是针对NAT服务器而言

首先我们这里有一个客户端，上面运行着一个浏览器，假设它使用的是5566端口，它需要访问14.25.23.47这个Web服务器的HTTPS服务的443端口，它在访问的时候需要经过局域网出口的这个路由器网关（同时也是NAT服务器），路由器对它进行一个NAPT的源地址转换（SNAT），这个时候客户端的请求经过NAT服务器之后变成了222.17.23.45:7788这个IP端口对Web服务器的443端口进行访问。注意在这个过程中，目标服务器（Web服务器）的IP和端口是一直没有改变的。

接下来在Web服务器接收到请求之后，需要返回数据给发送请求的设备，注意这时候web服务器返回数据的指向IP应该是刚刚NAT服务器发送请求的227.17.23.45:7788这个IP端口，这时候路由器网关再进行一次NAPT的目标地址转换（DNAT），目标的IP端口就是最开始发送请求的192.168.1.77:5566这个端口。

实际上对于大多数人来说日常接触到最多的就是路由器做的SNAT和DNAT操作，它们一般成对出现用于解决公网IP资源不足的问题，需要注意的是NAT是可以进行嵌套操作的，即NAT下面的网络设备还可以继续做NAT，只要做NAT的网段不和上层的NAT的网段相同即可。

