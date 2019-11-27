# firewalld（firewall-cmd）实现 NAT 转发 软路由

* 查看火墙状态
```shell
firewall-cmd --list-all
```
查看被允许的端口/服务
```shell
firewall-cmd --list-all --zone=public
firewall-cmd --list-services --zone=public
firewall-cmd --list-ports--zone=public
```

添加/删除被允许的服务
```shell
firewall-cmd --add-service=ssh --zone=public --permanent
firewall-cmd --remove-service=ssh --zone=public --permanent
```

添加/删除被允许的端口
```shell
firewall-cmd --add-port=22/tcp --zone=public --permanent
firewall-cmd --remove-port=22/tcp --zone=public --permanent
```

可利用的服务列表
```shell
firewall-cmd --get-services
```

服务重启
```shell
firewall-cmd --reload
```

把主机的ip这个源添加到trusted域里
```shell
firewall-cmd --add-source=192.168.1.2 --zone=trusted

firewall-cmd --remove-interface=eth1 --zone=public
firewall-cmd --add-interface=eth1 --zone=trusted
firewall-cmd --list-all --zone=trusted
```

```shell
# 开启 NAT 转发
firewall-cmd --permanent --zone=public --add-masquerade

# 开放 DNS 使用的 53 端口，UDP
# 必须，否则其他机器无法进行域名解析
firewall-cmd --zone=public --add-port=80/tcp --permanent

# 检查是否允许 NAT 转发
firewall-cmd --query-masquerade
# 禁止防火墙 NAT 转发
firewall-cmd --remove-masquerade
```

```shell
# 将80端口的流量转发至8080
firewall-cmd --add-forward-port=port=80:proto=tcp:toport=8080

# 将80端口的流量转发至192.168.0.1
firewall-cmd --add-forward-port=proto=80:proto=tcp:toaddr=192.168.0.1

# 将80端口的流量转发至192.168.0.1的8080端口
firewall-cmd --add-forward-port=proto=80:proto=tcp:toaddr=192.168.0.1:toport=8080
```