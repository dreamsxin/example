# jabberd2

JabberD XMPP Server

https://github.com/jabberd2/jabberd2.git


## 编译安装

```shell
yum install libcppunit-dev libidn11-dev zlib1g-dev libexpat1-dev libssl-dev libldap2-dev libmysqlclient-dev libgsasl7-dev
autoreconf -fi
# CFLAGS="-I/usr/local/src/udns-0.4" LDFLAGS="-L/usr/local/src/udns-0.4"  
./configure --enable-postgresql --enable-mysql --enable-websocket --enable-ssl
make
make install
```

## 配置

默认端口 `5347`
管理监听IP和端口地址 `router.xml`
```xml
  <!-- Local network configuration -->
  <local>
    <!-- IP address to bind to (default: 0.0.0.0) -->
    <ip>0.0.0.0</ip>

    <!-- Port to bind to (default: 5347) -->
    <port>5347</port>
```

管理员用户、信息存储 `sm.xml`
```xml
<!-- Session manager configuration -->
<sm>
  <!-- Our ID on the network (default: sm) -->
  <id>ip/domain</id>
```

注册存储 `c2s.xml`
```xml
<!-- c2s configuration -->
<c2s>
  <!-- Our ID on the network (default: c2s) -->
  <id>ip/domain</id>

  <id register-enable='mu'>ip/domain</id>
```