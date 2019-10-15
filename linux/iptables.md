
```shell
#关闭所有的80端口
iptables -I INPUT -p tcp --dport 80 -j DROP

#开启ip段192.168.1.0/24端的80口
iptables -I INPUT -s 192.168.1.0/24 -p tcp --dport 80 -j ACCEPT

#开启ip段211.123.16.123/24端ip段的80口
iptables -I INPUT -s 211.123.16.123/24 -p tcp --dport 80 -j ACCEPT
```