# tsocks 使用

## 安装
```shell
sudo apt install tsocks
```

## 配置

修改 `/etc/tsocks.conf` 默认服务器地址，SOCKETS v4 就把 server_type 改为 4

```conf
# Default server
# For connections that aren't to the local subnets or to 150.0.0.0/255.255.0.0
# the server at 192.168.0.1 should be used (again, hostnames could be used
# too, see note above)

server = 127.0.0.1
# Server type defaults to 4 so we need to specify it as 5 for this one
server_type = 5
# The port defaults to 1080 but I've stated it here for clarity 
server_port = 1080
```

```shell
tsocks cvs -z3 -d :pserver:anonymous@cvs.schmorp.de/schmorpforge co libeio
```