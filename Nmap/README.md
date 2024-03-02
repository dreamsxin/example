# 
nmap 是一个免费开放的网络扫描和嗅探的工具包，也叫网络映射器。
- https://github.com/nmap/nmap
- https://github.com/nmap/npcap
- https://github.com/nmap/ncrack

## 发包和收包
用的是 https://github.com/google/gopacket，基于libpcap，支持多个系统，并且可以发原始数据包。

### 数据链路层
要自己组装协议，数据链路层，网络层，传输层，都要自己组装。

对于数据链路层，需要知道自己的网卡Mac以及目标网卡Mac，对于外网，目标网卡就是路由器的Mac，对于内网，目标网卡就是arp探测到的mac。

先调用系统的 nslookup 去请求一个地址，然后监听返回包，返回包中会包含数据链路层，直接拿来用即可。

这次我采用一个古老又复杂的方法，是 `https://github.com/v-byte-cpu/sx` 这个项目提供的灵感，但它也没有很好实现。

- 调用系统路由表，确定目标IP该使用的网卡，此时就能获得本地网卡的mac地址和网关IP。
- 调用系统arp缓存，查找目标ip是否有，如果有，就能获得目标地址的网卡mac（无论是内网主机的mac还是网卡的mac，缓存中都有）
- 如果系统arp缓存没有找到，则使用arp协议，像内网段广播获得网关的mac，如果目标IP是同一个网段，则广播查找目标IP的Mac。

## 使用

nmap 有个参数可以显示发送的包 `--packet-trace`，再将 `-vv` 详细输出打开，事先找好Open和Close的TCP端口，用以下命令：
```shell
nmap -p80,20 --packet-trace -vv -O 123.123.123.123
```
