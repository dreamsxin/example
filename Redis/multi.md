# Redis 多实例

* 注意配置文件的权限。

## 安装

```shell
sudo apt-get install -y redis-server
```

## 拷贝文件


- /etc/init.d/redis-server  redis的可执行程序
- /etc/redis/redis.conf  redis的配置文件
- /usr/bin/redis-server  redis的自启动文件

* 配置文件
```shell
cp /etc/redis/redis.conf /etc/redis/redis6380.conf
chown redis:redis /etc/redis/redis6380.conf
```
修改端口
```conf
pidfile /var/run/redis/redis-server6380.pid
port 6380
logfile /var/log/redis/redis-server6380.log
dbfilename dump6380.rdb
bind 0.0.0.0    # 允许其他服务器访问
```

* 执行文件
```shell
sudo cp /etc/init.d/redis-server /etc/init.d/redis-server6380
```
```conf
DAEMON=/usr/bin/redis-server
DAEMON_ARGS=/etc/redis/redis_6380.conf
NAME=redis-server
DESC=redis-server6380

RUNDIR=/var/run/redis
PIDFILE=$RUNDIR/redis-server6380.pid
```

* 启动
```shell
sudo service redis-server6380 (start|stop|restart)
```

* 开机启动
```shell
sudo update-rc.d redis-server6380 defaults 20    # 开机启动
```

* 取消开启启动
```
sudo update-rc.d -f redis-server6380 remove
```
