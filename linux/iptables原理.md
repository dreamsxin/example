iptables的发展

       iptables的前身叫ipfirewall （内核1.x时代）,这是一个作者从freeBSD上移植过来的，能够工作在内核当中的，对数据包进行检测的一款简易访问控制工具。但是ipfirewall工作功能极其有限(它需要将所有的规则都放进内核当中，这样规则才能够运行起来，而放进内核，这个做法一般是极其困难的)。当内核发展到2.x系列的时候，软件更名为ipchains，它可以定义多条规则，将他们串起来，共同发挥作用，而现在，它叫做iptables，可以将规则组成一个列表，实现绝对详细的访问控制功能。

       他们都是工作在用户空间中，定义规则的工具，本身并不算是防火墙。它们定义的规则，可以让在内核空间当中的netfilter来读取，并且实现让防火墙工作。而放入内核的地方必须要是特定的位置，必须是tcp/ip的协议栈经过的地方。而这个tcp/ip协议栈必须经过的地方，可以实现读取规则的地方就叫做 netfilter.(网络过滤器)

       作者一共在内核空间中选择了5个位置，

       1.内核空间中：从一个网络接口进来，到另一个网络接口去的

        2.数据包从内核流入用户空间的

        3.数据包从用户空间流出的

        4.进入/离开本机的外网接口

        5.进入/离开本机的内网接口
m state --state NEW,ESTABLISHED -j ACCEPT

     iptables -R INPUT 1 -d 172.16.100.1 -p udp --dport 53 -j ACCEPT

练习题2：

假如我们允许自己ping别人，但是别人ping自己ping不通如何实现呢？

分析：对于ping这个协议，进来的为8（ping），出去的为0(响应).我们为了达到目的，需要8出去,允许0进来

在出去的端口上：iptables -A OUTPUT -p icmp --icmp-type 8 -j ACCEPT

在进来的端口上：iptables -A INPUT -p icmp --icmp-type 0 -j ACCEPT

小扩展：对于127.0.0.1比较特殊，我们需要明确定义它

iptables -A INPUT -s 127.0.0.1 -d 127.0.0.1 -j ACCEPT

iptables -A OUTPUT -s 127.0.0.1 -d 127.0.0.1 -j ACCEPT

8、SNAT和DNAT的实现
      由于我们现在IP地址十分紧俏，已经分配完了，这就导致我们必须要进行地址转换，来节约我们仅剩的一点IP资源。那么通过iptables如何实现NAT的地址转换呢？

8.1、SNAT基于原地址的转换

       基于原地址的转换一般用在我们的许多内网用户通过一个外网的口上网的时候，这时我们将我们内网的地址转换为一个外网的IP，我们就可以实现连接其他外网IP的功能。

所以我们在iptables中就要定义到底如何转换：

定义的样式：

       比如我们现在要将所有192.168.10.0网段的IP在经过的时候全都转换成172.16.100.1这个假设出来的外网地址：

iptables -t nat -A POSTROUTING -s 192.168.10.0/24 -j SNAT --to-source 172.16.100.1

这样，只要是来自本地网络的试图通过网卡访问网络的，都会被统统转换成172.16.100.1这个IP.

那么，如果172.16.100.1不是固定的怎么办？

       我们都知道当我们使用联通或者电信上网的时候，一般它都会在每次你开机的时候随机生成一个外网的IP，意思就是外网地址是动态变换的。这时我们就要将外网地址换成 MASQUERADE(动态伪装):它可以实现自动寻找到外网地址，而自动将其改为正确的外网地址。所以，我们就需要这样设置：

         iptables -t nat -A POSTROUTING -s 192.168.10.0/24 -j MASQUERADE

         这里要注意：地址伪装并不适用于所有的地方。

8.2、DNAT目标地址转换

对于目标地址转换，数据流向是从外向内的，外面的是客户端，里面的是服务器端通过目标地址转换，我们可以让外面的ip通过我们对外的外网ip来访问我们服务器不同的服务器，而我们的服务却放在内网服务器的不同的服务器上。

    如何做目标地址转换呢？：

    iptables -t nat -A PREROUTING -d 192.168.10.18 -p tcp --dport 80 -j DNAT --todestination 172.16.100.2

    目标地址转换要做在到达网卡之前进行转换,所以要做在PREROUTING这个位置上

9、控制规则的存放以及开启
    注意：你所定义的所有内容，当你重启的时候都会失效，要想我们能够生效，需要使用一个命令将它保存起来

    9.1、service iptables save 命令

             它会保存在/etc/sysconfig/iptables这个文件中

    9.2、iptables-save 命令

             iptables-save > /etc/sysconfig/iptables

    9.3、iptables-restore 命令

             开机的时候，它会自动加载/etc/sysconfig/iptabels

             如果开机不能加载或者没有加载，而你想让一个自己写的配置文件（假设为iptables.2）手动生效的话：

              iptables-restore < /etc/sysconfig/iptables.2，则完成了将iptables中定义的规则手动生效

10、总结
         Iptables是一个非常重要的工具，它是每一个防火墙上几乎必备的设置，也是我们在做大型网络的时候，为了很多原因而必须要设置的。学好Iptables,可以让我们对整个网络的结构有一个比较深刻的了解，同时，我们还能够将内核空间中数据的走向以及linux的安全给掌握的非常透彻。我们在学习的时候，尽量能结合着各种各样的项目，实验来完成，这样对你加深iptables的配置，以及各种技巧有非常大的帮助。


## 命令

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





