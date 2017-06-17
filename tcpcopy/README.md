TCPCopy是如何工作的

基于 Server 的请求回放领域，一般分为离线回放和在线实时复制两种。

在线实时复制：

1）基于应用层的请求复制 ，
2）基于底层数据包的请求复制。
 
如果从应用层面进行复制，比如基于服务器的请求复制，实现起来相对简单，但也存在着若干缺点：

1）请求复制从应用层出发，穿透整个协议栈，这样就容易挤占应用的资源，比如宝贵的连接资源 
2）测试跟实际应用耦合在一起，容易影响在线系统
3）也因此很难支撑压力大的请求复制
4）很难控制网络延迟

而基于底层数据包的请求复制，可以做到无需穿透整个协议栈，路程最短的，可以从数据链路层抓请求包，从数据链路层发包，路程一般的，可以在IP层抓请求包，从IP层发出去，不管怎么走，只要不走TCP，对在线的影响就会小得多。这也就是 TCPCopy 的基本思路。
 
从传统架构的 rawsocket + iptable + netlink，到新架构的 pacp + route，它经历了三次架构调整，现如今的 TCPCopy 分为三个角色：

- Online Server(OS)：上面要部署 TCPCopy，从数据链路层(pcap 接口)抓请求数据包，发包是从IP层发出去；
- Test Server(TS)：最新的架构调整把 intercept 的工作从 TS 中 offload 出来。TS 设置路由信息，把 被测应用 的需要被捕获的响应数据包信息路由到 AS；
- Assistant Server(AS)：这是一台独立的辅助服务器，原则上一定要用同网段的一台闲置服务器来充当辅助服务器。AS 在数据链路层截获到响应包，从中抽取出有用的信息，再返回给相应的 OS 上的 tcpcopy 进程。

## tcpcopy 有两种工作模式：
1）实时拷贝数据包；
2）通过使用 tcpdump 等抓包生成的文件进行离线（offline）请求重放。

## Online Server 上的抓包：

tcpcopy 的新架构在 OS 上抓请求数据包默认采用 raw socket input 接口抓包。王斌则推荐采用 pcap 抓包，安装命令如下：
```shell
./configure --enable-advanced --enable-pcap
make
make install
```

这样就可以在内核态进行过滤，否则只能在用户态进行包的过滤，而且在 intercept 端或者 tcpcopy 端设置 filter（通过 -F 参数，类似 tcpdump 的 filter），达到起多个实例来共同完成抓包的工作，这样可扩展性就更强，适合于超级高并发的场合。

为了便于理解 pcap 抓包，下面简单描述一下 libpcap 的工作原理。

一个包的捕捉分为三个主要部分：

- 面向底层包捕获，
- 面向中间层的数据包过滤，
- 面向应用层的用户接口。

这与 Linux 操作系统对数据包的处理流程是相同的（网卡->网卡驱动->数据链路层->IP层->传输层->应用程序）。包捕获机制是在数据链路层增加一个旁路处理（并不干扰系统自身的网络协议栈的处理），对发送和接收的数据包通过Linux内核做过滤和缓冲处理，最后直接传递给上层应用程序。

## Online Server 上的发包：

新架构和传统架构一样，OS 默认使用 raw socket output 接口发包，此时发包命令如下： 
./tcpcopy -x 80-测试机IP:测试机应用端口 -s 服务器IP -i eth0
其中 -i 参数指定 pcap 从哪个网卡抓取请求包。
此外，新架构还支持通过 pcap_inject（编译时候增加--enable-dlinject）来发包。
 
Test Server 上的响应包路由：
需要在 Test Server 上添加静态路由，确保被测试应用程序的响应包路由到辅助测试服务器，而不是回包给 Online Server。
 
Assistant Server 上的捕获响应包：
辅助服务器要确保没有开启路由模式 cat /proc/sys/net/ipv4/ip_forward，为0表示没有开启。
辅助服务器上的 intercept 进程通过 pcap 抓取测试机应用程序的响应包，将头部抽取后发送给 Online Server 上的 tcpcopy 进程，从而完成一次请求的复制。

Online Server 上的操作：
下载并安装 tcpcopy 客户端；
```shell
git clone http://github.com/session-replay-tools/tcpcopy
./configure
make && make install
```

安装完成后的各结构目录：
```text
Configuration summary
 
  tcpcopy path prefix: "/usr/local/tcpcopy"
  tcpcopy binary file: "/usr/local/tcpcopy/sbin/tcpcopy"
  tcpcopy configuration prefix: "/usr/local/tcpcopy/conf"
  tcpcopy configuration file: "/usr/local/tcpcopy/conf/plugin.conf"
  tcpcopy pid file: "/usr/local/tcpcopy/logs/tcpcopy.pid"
  tcpcopy error log file: "/usr/local/tcpcopy/logs/error_tcpcopy.log"
```

运行 tcpcopy 客户端，有几种可选方式：
```shell
./tcpcopy -x 80-172.16.***.52:80 -s 172.16.***.53 -d       #全流量复制
./tcpcopy -x 80-172.16.***.52:80 -s 172.16.***.53 -r 20 -d  #复制20%的流量
./tcpcopy -x 80-172.16.***.52:80 -s 172.16.***.53 -n 2 -d    #放大2倍流量
```

3.2 Test Server 上的操作：
添加静态路由：
```shell
route add -net 0.0.0.0/0 gw 172.16.***.53
```
 
3.3 Assistant Server 上的操作：
下载并安装 intercept 服务端；
```shell
git clone http://github.com/session-replay-tools/intercept
./configure
make && make install
```

安装完成后的各结构目录：
```text
Configuration summary
  intercept path prefix: "/usr/local/intercept"
  intercept binary file: "/usr/local/intercept/sbin/intercept"
  intercept configuration prefix: "/usr/local"
  intercept configuration file: "/usr/local/intercept/"
  intercept pid file: "/usr/local/intercept/logs/intercept.pid"
  intercept error log file: "/usr/local/intercept/logs/error_intercept.log"
```
运行 intercept 服务端；
```shell
./intercept -i eth0 -F 'tcp and src port 80' -d
```