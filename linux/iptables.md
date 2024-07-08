
```shell
#关闭所有的80端口
iptables -I INPUT -p tcp --dport 80 -j DROP

#开启ip段192.168.1.0/24端的80口
iptables -I INPUT -s 192.168.1.0/24 -p tcp --dport 80 -j ACCEPT

#开启ip段211.123.16.123/24端ip段的80口
iptables -I INPUT -s 211.123.16.123/24 -p tcp --dport 80 -j ACCEPT

#允许特定ip
iptables -I INPUT -s 192.168.1.123 -ptcp --dport 3306 -j ACCEPT
```

## NAT 模式

```shell
## Director 和 Real Servers的定义如下：
##    D : 192.168.0.100
##    RS: 192.168.0.10 192.168.0.11
## 在Director容器中配置IP路由转发规则，分别是192.168.0.10、192.168.0.11，监听端口为80
sudo iptables -t nat -A PREROUTING -p tcp -i eth0 --dport 80 -m state --state NEW -m statistic --mode nth --every 4 --packet 0 -j DNAT --to-destination 192.168.0.10:80
sudo iptables -t nat -A PREROUTING -p tcp -i eth0 --dport 80 -m state --state NEW -m statistic --mode nth --every 4 --packet 1 -j DNAT --to-destination 192.168.0.11:80

## 设置数据包流出时，改写IP包的目标地址
sudo iptables -t nat -A POSTROUTING -j MASQUERADE

## 更新iptables中的规则
sudo iptables-save

## 配置完成后，可以使用如下的命令来查看路由转发规则，下面只截取了新添加的规则信息
[mlp@rerver3 ~]$ /usr/bin/sudo iptables --list-rules -t nat
-A PREROUTING -i eth0 -p tcp -m tcp --dport 80 -m state --state NEW -m statistic --mode nth --every 4 --packet 0 -j DNAT --to-destination 192.168.0.10:80
-A PREROUTING -i eth0 -p tcp -m tcp --dport 80 -m state --state NEW -m statistic --mode nth --every 4 --packet 1 -j DNAT --to-destination 192.168.0.11:80
-A POSTROUTING -j MASQUERADE
```

## state 模块

```shell
-m state --state <状态>
```
有数种状态，状态有：

- ▪ INVALID：无效的封包，例如数据破损的封包状态
- ▪ ESTABLISHED：已经联机成功的联机状态；
- ▪ NEW：想要新建立联机的封包状态；
- ▪ RELATED：这个最常用！表示这个封包是与我们主机发送出去的封包有关， 可能是响应封包或者是联机成功之后的传送封包！这个状态很常被设定，因为设定了他之后，只要未来由本机发送出去的封包，即使我们没有设定封包的 INPUT 规则，该有关的封包还是可以进入我们主机， 可以简化相当多的设定规则。

### 实验1：

允许 ICMP 封包与允许已建立的联机通过 filter 表中 INPU T链为 DROP，OUTPUT 链为 ACCEPT，此时本机ping其他主机不通，在INPUT链中添加规则：

`iptables -AINPUT -m state --state ESTABLISHED,RELATED -j ACCEPT`

本机可以ping其他主机，但是其他主机无法ping本机

## statistic 模块

`iptables -m statistic -h`
```text
# iptables -m statistic -h

statistic match options:
 --mode mode                    Match mode (random, nth)
 random mode:
[!] --probability p              Probability
 nth mode:
[!] --every n                    Match every nth packet
 --packet p                      Initial counter value (0 <= p <= n-1, default 0)
```

### nth 模式
如下策略配置，使用 nth 模式，每三个报文中丢弃第二个报文（报文索引由0开始）:
```shell
# iptables -A INPUT -s 192.168.1.105 -p icmp -m statistic --mode nth --every 3 --packet 1 -j DROP
#
# iptables -L -v -n  
Chain INPUT (policy ACCEPT 0 packets, 0 bytes)
 pkts bytes target     prot opt in     out     source               destination         
    0     0 DROP       icmp --  *      *       192.168.1.105        0.0.0.0/0            statistic mode nth every 3 packet 1
```
如下策略，对于源自192.168.1.105的ICMP报文，按照50%的概率执行丢弃。
```shell
# iptables -A INPUT -s 192.168.1.105 -p icmp -m statistic --mode random --probability 0.5 -j DROP
#
# iptables -L -v -n
Chain INPUT (policy ACCEPT 0 packets, 0 bytes)
 pkts bytes target     prot opt in     out     source               destination         
    0     0 DROP       icmp --  *      *       192.168.1.105        0.0.0.0/0            statistic mode random probability 0.50000000000
```
