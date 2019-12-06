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

## 目录结构

- `mio`、`sx`、`util`
数据处理模块
- `c2s`、`router`、`sm、`s2s`
功能模块，每个模块的目录中都包括了 `main.c`，都是可以独立运行的程序。
- `storage`
包含了持久化数据访问的实现，
`authreg_` 前缀文件主要是 `c2s` 模块使用。
`storage_` 前缀文件主要是 `sm` 模块使用。

```txt
jabber2/

    docs/       - Documentation
        code/   - Doxygen generated
        dev/    - Hacker docs

    mio/        - Managed Input/Ouput (FD event processor)
    sx/         - Streams for XML (Jabber connection / stream library)
    util/       - Utilities (config, logging, NADs, hashtables, etc)

    c2s/        - Client-to-server
    router/     - XML router
    sm/         - Session manager
    s2s/        - Server-to-server
```

## 模块的 main.c 代码

* c2s：
客户端连接的入口
```c
    c2s->mio = mio_new(c2s->io_max_fds);
    if(c2s->mio == NULL) {
        log_write(c2s->log, LOG_ERR, "failed to create MIO, aborting");
        exit(1);
    }
    // ...
     _c2s_router_connect(c2s);
    // ...
    while(!c2s_shutdown) {
        mio_run(c2s->mio, mio_timeout);
        // ...
    }
```
通过 `_c2s_router_connect` 连接到 `router` IO 事件处理服务器，`c2s_router_mio_callback` 作为 IO 事件的处理入口。
在 IO 事件 `action_ACCEPT` 时，使用 `sx_new` 创建 sx 流处理器，`_c2s_client_sx_callback` 作为 流事件的处理入口。
当触发 `event_PACKET` 流事件时，也就是 `router` 返回成功通知时，调用 `mio_listen` 创建 IO 事件处理服务器，监听 5222 端口。

* `router`：
在组件之间进行消息转发
```c

    r->mio = mio_new(r->io_max_fds);

    r->fd = mio_listen(r->mio, r->local_port, r->local_ip, router_mio_callback, (void *) r);
    if(r->fd == NULL) {
        log_write(r->log, LOG_ERR, "[%s, port=%d] unable to listen (%s)", r->local_ip, r->local_port, MIO_STRERROR(MIO_ERROR));
        exit(1);
    }
    // ...

    while(!router_shutdown)
    {
        mio_run(r->mio, 5);
        // ...
    }
```
使用 `mio_listen` 创建 IO 事件处理服务器，使用 `router_mio_callback` 作为 IO 事件的处理入口。
在 IO 事件 `action_ACCEPT` 时，使用 `sx_new` 创建 sx 流处理器，`_router_sx_callback` 作为 流事件的处理入口。

* `sm`：
main packet dispatcher
```c

    sm->mio = mio_new(MIO_MAXFD);
    // ...
    _sm_router_connect(sm);
    // ...
    while(!sm_shutdown) {
        mio_run(sm->mio, 5);
        // ...
    }
```
通过 `_sm_router_connect` 连接到 `router` IO 事件处理服务器，`sm_mio_callback` 作为 IO 事件的处理入口。
`sm_sx_callback` 处理 `event_PACKET` 流事件时，调用 `dispatcher`。

* `s2s`：
```c

    s2s->mio = mio_new(s2s->io_max_fds);

    if((s2s->udns_fd = dns_init(NULL, 1)) < 0) {
        log_write(s2s->log, LOG_ERR, "unable to initialize dns library, aborting");
        exit(1);
    }
    s2s->udns_mio_fd = mio_register(s2s->mio, s2s->udns_fd, _mio_resolver_callback, (void *) s2s);
    // ...

    while(!s2s_shutdown) {
        mio_run(s2s->mio, dns_timeouts(0, 5, time(NULL)));
        // ...
    }
```

## MIO

MIO 提供了统一的同步 IO 事件通知机制，支持 `select`、`poll`、`epoll`等接口。

- mio_listen
创建服务端的socket

- mio_connect
创建客户端的socket

- mio_app
邦定socket的处理函数

- mio_write
监视socket的写事件

- mio_read
监视socket的读事件

## 流程

客户端连接 `c2s` 服务器 `5222` 端口，此后会进入 `_c2s_client_mio_callback` 处理，创建新 `sx` 流事件处理器，使用 `_c2s_client_sx_callback` 当回调函数。