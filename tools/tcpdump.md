# tcpdump

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