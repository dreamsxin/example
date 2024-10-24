# 
aria2 is a lightweight multi-protocol & multi-source command-line download utility. It supports HTTP/HTTPS, FTP, SFTP, BitTorrent and Metalink. aria2 can be manipulated via built-in JSON-RPC and XML-RPC interfaces.

## 安装
```shell
git clone https://github.com/aria2/aria2.git
cd aria2
sudo apt-get install pkg-config libxml2-dev libcppunit-dev autoconf automake autotools-dev autopoint libtool
autoreconf -i
```

## AC_CONFIG_MACRO_DIRS([build-aux/m4]) conflicts with ACLOCAL.AMFLAGS=-I build-aux/m4
该错误常发生在 wsl，因为文件存储在 windows 上导致的格式错误。
```shell
find . -name \*.m4|xargs dos2unix
find . -name \*.ac|xargs dos2unix
find . -name \*.am|xargs dos2unix
```
