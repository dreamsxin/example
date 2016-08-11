# 配置rsync

`/etc/rsyncd.conf` `/etc/default/rsync`

```conf
# /etc/rsyncd.conf
# Minimal configuration for rsync daemon
# Next line required for init script
pid file = /var/run/rsyncd.pid
#告诉进程写到 /var/run/rsyncd.pid 文件中
log file = /var/log/rsyncd.log
#日志文件
use chroot = yes
uid = root
gid = root
read only = false
write only = false

[disk]
# 可以用别名，可以创建多个单元
path = /tmp/mnt/USB-disk-a1/
#服务器待备份数据存放的路径
ignore errors
read only = false
list = true
#hosts allow =192.168.168135
#hosts deny = 0.0.0.0/32
#auth users = backup
#secrets file = /etc/backserver.pas
#................
# Simple example to configure server
#[openwrt-etc]
#path = /etc
#comment = OpenWrt Configuration Files
#exclude = /init.d ~ ~ 
```

四：启动rsync服务
```shell
/etc/init.d/rsyncd restart/start/stop
```