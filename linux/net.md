# 解决 Linux 下 TIME_WAIT 和 CLOSE_WAIT 过多的问题

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