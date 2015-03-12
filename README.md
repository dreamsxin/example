example
=======

各种代码实例


双网卡NAT
```shell
sudo iptables -F
sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE #打开NAT
sudo iptables -t nat -A POSTROUTING -s 192.168.1.0/24 -o eth1 -j SNAT --to-source xxx.xxx.xxx.xxx
sudo iptables -t nat -A POSTROUTING -s 192.168.1.0/24 -d ! 192.168.1.0/24 -j SNAT --to-source xxx.xxx.xxx.xxx
sudo iptables -A FORWARD -s 192.168.1.0/24 -p tcp -m tcp --tcp-flags SYN,RST SYN -j TCPMSS --set-mss 1200
#sudo ifconfig eth1 mtu 1500
#sudo ifconfig eth0 mtu 1500
```
