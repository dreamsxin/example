# tcpdump

https://github.com/dreamsxin/example/blob/master/network/tcp.md
```shell
tcpdump -i eth0   #捕获指定接口(网卡)的数据包 可通过 netstat -i查看网卡设备
tcpdump -i eth0 -c 3   #捕获指定个数的数据包（3个数据包）
tcpdump -A -i eth0    #用ASCII码格式输出捕获的数据包
tcpdump -D    #显示可用的系统接口
tcpdump -XX -i eth0   #用十六进制和ASCII码格式显示捕获的数据包
tcpdump -w tempDump.pcap -i eth0   #把捕获的数据包写入到一个.pcap后缀的文件中
tcpdump -r tempDump.pcap    #读取捕获数据包文件的内容
tcpdump -n -i eth0    # 单个 n 表示不解析域名，直接显示 IP
tcpdump -i eth0 tcp    #捕获TCP类型的数据包
tcpdump -i eth0 port 22   #捕获指定端口（这里是22）的数据包
tcpdump -i eth0 src 源ip地址   #捕获请求源是 192.169.12.101 的数据包
tcpdump -i eth0 dst 目标ip地址   #捕获指定目的IP的数据包
tcpdump -i eth6 dst host 目标ip地址 and port 8800 -w data.pcap  #抓取指定网卡，指定IP和端口的数据包 并写入到data.pcap文件中
tcpdump host 192.168.12.101 and 192.168.1.201 -w out &  #后台抓取两主机之间的数据
tcpdump -nn 单个 n 表示不解析域名，直接显示 IP；两个 n 表示不解析域名和端口。这样不仅方便查看 IP 和端口号，而且在抓取大量数据时非常高效，因为域名解析会降低抓取速度
```
1. 监听指定的网络接口
```shell
sudo tcpdump -i eth0
```

2.监听指定的IP地址和端口号
```shell
sudo tcpdump -i eth0 host 192.168.1.100 and port 80
```

3.监听指定的协议
```shell
sudo tcpdump -i eth0 icmp
```

4.将捕获的数据包保存到文件
```shell
sudo tcpdump -i eth0 -w packets.pcap
```

5.读取保存的数据包文件并进行解析
```shell
sudo tcpdump -r packets.pcap
```

6.显示数据包的ASCII数据
```shell
sudo tcpdump -nn
```
```shell
sudo tcpdump -A
```

7.显示数据包的十六进制数据
```shell
sudo tcpdump -X
```

8.显示数据包的摘要信息
```shell
sudo tcpdump -nn
```
## 借助 tcpdump 统计 http 请求

1、捕捉10秒的数据包。
```shell
tcpdump -i eth0 tcp[20:2]=0x4745 or tcp[20:2]=0x504f -w /tmp/tcp.cap -s 512 2>&1 &
sleep 10
kill `ps aux | grep tcpdump | grep -v grep | awk '{print $2}'`
```
此命令表示监控网卡eth0，捕捉tcp，且21-22字节字符为GE或者PO，表示匹配GET或者POST请求的数据包，并写到/tmp/tcp.cap文件。

2、这时候我们得到最新10秒的二进制数据包文件，我们下一步就是通过strings命令来找出GET/POST的url以及Host。
```shell
strings /tmp/tcp.cap | grep -E "GET /|POST /|Host:" | grep --no-group-separator -B 1 "Host:" | grep --no-group-separator -A 1 -E "GET /|POST /" | awk '{url=$2;getline;host=$2;printf ("%s\n",host""url)}' > url.txt
```
此命令是本文的关键，通过strings显示二进制文件tcp.cap所有可打印字符，然后通过grep和awk过滤出http请求，并把拼接得到的url(包括域名+uri)写进一个文件url.txt。

3、这时我们拿到了近10秒钟所有的访问url，接下来的统计就容易得出，比如:
统计QPS：
```shell
(( qps=$(wc -l /tmp/url.txt | cut -d' ' -f 1) / 10 ))
```
排除静态文件统计前10访问url:
```shell
grep -v -i -E "\.(gif|png|jpg|jpeg|ico|js|swf|css)" /tmp/url.txt | sort | uniq -c | sort -nr | head -n 10
```
