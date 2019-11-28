# 设置代理

## 安装代理

```shel
sudo apt-get install tinyproxy -y
```
配置文件在 `/etc/tinyproxy.conf`

## 全局代理

在“首选项”->“网络代理”那里，多了个“System-wide”按钮，在这里设置后，`apt-get`确实可以使用代理了。

所设置修改了两个文件，
一个是`/etc/environment`，这个是系统的环境变量，里面定义了`http_proxy`等代理环境变量。
另一个是“/etc/apt/apt.conf”，这个就是apt的配置，内容如下

```conf
Acquire::http::proxy "http://127.0.0.1:8000/";
Acquire::ftp::proxy "ftp://127.0.0.1:8000/";
Acquire::https::proxy "https://127.0.0.1:8000/";
```

## apt-get 代理

apt-get 可以用`-c`选项来指定使用配置文件
```shell
sudo apt-get -c /etc/apt/apt.conf update
```

apt-get 还有有一个`-o`选项，直接设置变量：
```shell
sudo apt-get -o Acquire::http::proxy="http://127.0.0.1:8000/" update
sudo apt-get -o Acquire::socks::proxy="socks5://127.0.0.1:1080" update;
```

## 代理上网工具 tsocks/proxychains

Linux 有一个能够强迫任何软件通过 SOCKS 代理上网的工具，其名就是 tsocks。

```shell
sudo apt-get install tsocks
sudo apt-get install proxychains
```

`/etc/tsocks.conf`
```conf
local = 192.168.1.0/255.255.255.0 # local表示本地的网络，也就是不使用socks代理的网络
server = 127.0.0.1                # SOCKS 服务器的 IP
server_type = 5                   # SOCKS 服务版本
server_port = 9999                # SOCKS 服务使用的端口
```
使用范例
```shell
tsocks apt-get update
```

`/etc/proxychains.conf`