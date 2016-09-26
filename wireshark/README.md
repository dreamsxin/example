# Wireshark 使用

支持remote packet capture protocol协议远程抓包，只要在远程主机上安装相应的rpcapd服务。

## 安装 rpcapd
```shell
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