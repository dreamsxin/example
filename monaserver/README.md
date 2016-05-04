# 安装


```shell
git clone http://luajit.org/git/luajit-2.0.git
cd luajit && make -j4 && sudo make install && cd ..
```

```shell
git clone https://github.com/MonaSolutions/MonaServer.git
cd MonaServer && make -j4
```

# 配置 `MonaServer.ini` 可以省略
```ini
;MonaServer.ini
socketBufferSize = 114688
[RTMFP]
port = 1985
keepAlivePeer = 10
keepAliveServer = 15
[logs]
name=log
directory=logs
```

# 运行
假如推送的地址是 `rtmp://localhost/live` 需要建立目录 `www/live`
```shell
sudo ./MonaServer
sudo ./MonaServer --daemon [--pidfile=/var/run/MonaServer.pid]
```