## 部署架构

- VIP：192.168.226.150
- Server1：192.168.226.149	负载均衡服务器 director
- Server2：192.168.226.145	真实服务器 real server
- Server3：192.168.226.148	真实服务器 real server

所有发送到 VIP 的请求包都会由 director 按照设置的算法分发到不同的 real server 进行处理。

## director 配置

```shell
#由于内核已集成ipvs模块，只需要安装ipvsadm即可
apt-get install ipvsadm

# 在director上建立一个虚拟的ip地址进行消息广播
ifconfig eth0:0 192.168.226.150 netmask 255.255.255.0 broadcast 192.168.226.150

#将虚拟地址添加到路由表
route add -host 192.168.226.150 dev eth0:0

echo "1">/proc/sys/net/ipv4/ip_forward 

#使用ipvsadm安装LVS服务
ipvsadm -A -t 192.168.226.150:80 -s rr 

#增加真实服务器
ipvsadm -a -t 192.168.226.150:80 -r 192.168.226.145 -g
ipvsadm -a -t 192.168.226.150:80 -r 192.168.226.148 -g
```

## real server 配置

```shell
ifconfig lo:0 192.168.226.150 netmask 255.255.255.255 broadcast 192.168.226.150 up

route add -host 192.168.226.150 dev lo:0

echo "1">/proc/sys/net/ipv4/ip_forward
```

