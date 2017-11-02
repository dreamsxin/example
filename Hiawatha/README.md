# Hiawatha

目前最安全，最先进的网页服务器。

https://www.hiawatha-webserver.org/howto

## 安装

PPA：
```shell
sudo add-apt-repository ppa:octavhendra/hiawatha
```

### 源码编译安装

```shell
git clone https://github.com/hsleisink/hiawatha.git
cd hiawatha
mkdir build
cd build
cmake ..
make
make install/strip
```

`cmake` 有下列选项可用，大写代表默认值：
- -DENABLE_CACHE=ON|off		Enable cache support in Hiawatha.	
- -DENABLE_DEBUG=on|OFF		Enable debug information (for development only).	
- -DENABLE_IPV6=ON|off		Enable IPv6 support in Hiawatha.	
- -DENABLE_MONITOR=on|OFF	Enable support for the Hiawatha Monitor.	
- -DENABLE_RPROXY=ON|off	Enable reverse proxy support in Hiawatha.
- -DENABLE_TLS=ON|off		Enable TLS (mbed TLS) support in Hiawatha.	
- -DENABLE_TOMAHAWK=on|OFF	Enable Tomahawk in Hiawatha.	
- -DENABLE_TOOLKIT=ON|off	Enable the URL toolkit in Hiawatha.	
- -DENABLE_XSLT=ON|off		Enable XSLT support in Hiawatha.	

还有一些设置路径的选项：
- -DCMAKE_INSTALL_PREFIX=<path>		The prefix for all other CMAKE_INSTALL directories.	
- -DCMAKE_INSTALL_BINDIR=<path>		Location of the ssi-cgi binary.	
- -DCMAKE_INSTALL_SBINDIR=<path>	Location of the other Hiawatha binaries.	
- -DCMAKE_INSTALL_SYSCONFDIR=<path>	The configuration files will be installed in /hiawatha.	
- -DCMAKE_INSTALL_LIBDIR=<path>		The mbed TLS shared library will be installed in /hiawatha.	
- -DCMAKE_INSTALL_MANDIR=<path>		Manual pages will be installed in /man1.	
- -DCONFIG_DIR=<path>				Location of the Hiawatha configuration files.	
- -DLOG_DIR=<path>					Log directory used in the default hiawatha.conf.	
- -DPID_DIR=<path>					Location of the Hiawatha and php-fcgi PID files.	
- -DWEBROOT_DIR=<path>				Webroot directory used in the default hiawatha.conf.	
- -DWORK_DIR=<path>					Path of directory where Hiawatha can write temporary files.

### 生成 Debian 软件包进行安装

```shell
./extra/make_debian_package
dpkg -i hiawatha_<version>_<architecture>.deb
# 卸载
dpkg --purge hiawatha
```

### 验证配置文件 `hiawatha.conf` 是否正确

```shell
hiawatha -k
```

## 启动与停止

* 源码编译安装的启动和停止方式
```shell
# 启动
hiawatha
# 停止
kill -15 `cat /var/run/hiawatha.pid`
```
* Debian 软件包安装的启动和停止
```shell
/etc/init.d/hiawatha start
/etc/init.d/hiawatha stop
```

## 配置

### 设置监听端口和地址

```text
Binding {
	Interface = 192.168.0.1
    Port = 80
}
```

### 设置请求超时时间和数据大小

`TimeForRequest` 可以设置单个请求超时时间，以及限制单个连接下所有请求的超时时间。
例如：`TimeForRequest = 5, 30`，就是限制第一个请求超时时间5秒，以及剩下请求的超时时间为30秒。也就是说该连接30秒后会关闭。
```text
Binding {
    Port = 80
    TimeForRequest = 10
    MaxRequestSize = 512
}
```

### 设置 TLS

```text
Binding {
    Port = 443
    TLScertFile = /etc/hiawatha/tls/certificate.pem
}
```

### 强制用户使用 HTTPS

```text
VirtualHost {
    ...
    RequireTLS = yes
}
```

## 服务器名字指示 SNI（Server Name Indication）

我们可以在同一个 IP 下给不同的域名设置不同的证书。
```text
Binding {
    Port = 443
    TLScertFile = certificate.pem
}

VirtualHost {
    Hostname = www.website.org
    ...
    TLScertFile = website.pem
}
```

`VirtualHost` 中的 `TLScertFile` 设置将会覆盖 `Binding` 中的设置。

### 定义站点

* 默认站点

```text
Hostname = <IP address of server>
WebsiteRoot = /var/www/dummy
```

* 定义默认首页

```text
StartFile = start.html
```

创建 `/var/www/dummy/index.html` 文件，然后重新启动即可访问。

* 虚拟主机（Virtual hosts）

```text
VirtualHost {
    Hostname = www.mywebsite.com
    WebsiteRoot = /var/www/website1/wwwroot

    AccessLogfile = /var/www/website1/log/access.log
    ErrorLogfile = /var/www/website1/log/error.log
}
```

* 定义默认首页

```text
StartFile = start.html
```

## 使用 CGI 和 FastCGI