# Wireshark 安装

抓取本地回环，需要安装 `npcap loopback adapter`
##

```shell
sudo add-apt-repository ppa:n-muench/programs-ppa
sudo apt-get update
sudo apt-get install wireshark
sudo apt-get install libcap2-bin
```

安装后打开wireshark 提示权限不足消息：

`Couldn’t run /usr/bin/dumpcap in child process: Permission denied`

解决方案：
```shell
sudo apt-get install libcap2-bin
#添加一个组，名字为 wireshark ..我执行时，提示已经存在相同名字的组了
sudo groupadd wireshark  
#把自己当前的用户名添加到 wireshark组
sudo usermod -a -G wireshark YOUR-USER-NAME
```

newgrp wireshark
```shell
#修改组别
sudo chgrp wireshark /usr/bin/dumpcap
#添加执行权限
sudo chmod 754 /usr/bin/dumpcap
```

最后一步：Grant Capabilities
```shell
#下面两句执行其中一句就可以了，我执行的是第一句
sudo setcap cap_net_raw,cap_net_admin=eip /usr/bin/dumpcap
sudo setcap 'CAP_NET_RAW+eip CAP_NET_ADMIN+eip' /usr/bin/dumpcap
```

## 编译

```shell
sudo apt-get install libpcap-dev libqt4-dev && sudo ldconfig  
./autogen.sh &&./configure && make && sudo make install  
# or
 ./configure
$ make
$ sudo make install
```

支持remote packet capture protocol协议远程抓包，只要在远程主机上安装相应的rpcapd服务。

## 安装 rpcapd
```shell
git clone https://github.com/frgtn/rpcapd-linux
cd ./rpcapd-linux/libpcap
./configure
make
cd ..
make
sudo ./rpcapd -4 -n -p 8888
# or
apt-get install bison flex
wget http://www.winpcap.org/install/bin/WpcapSrc_4_1_2.zip
unzip WpcapSrc_4_1_2.zip
cd winpcap/wpcap/libpcap
chmod +x configure runlex.sh
CFLAGS=-static ./configure
make
cd rpcapd
make
```

## 运行 rpcapd

```shell
./rpcapd -n

# 以dameon模式在后台运行
./rpcapd -n -d 
```

## Wireshark 连接远程抓包服务

在 InterFace 中选择 Remote，在弹出的框中输入IP地址点击确定。

## 过滤 http 请求

```shell
http and http.request.uri contains "/api/v2"
```

```shell
ip.dst == xxx.xxx.xxx.xxx
tcp.stream eq 125
```

## 抓包 https

设置环境变量 `SSLKEYLOGFILE` 值 `D:\xxxx.log`。

在wireshark首选项中，选择TLS协议，编辑Pre-Master-Secret文件路径：

匹配域名
```shell
tls.handshake.extensions_server_name == "www.baidu.com"
tls.handshake.extensions_server_name contains "baidu"
```

匹配端口
```shell
tls && (tcp.port == 443 || tcp.port == 8443)
```
