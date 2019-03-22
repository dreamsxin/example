# rsync + inotify-tools

## 主服务器

Ubuntu 已默认安装 `rsync`，但在没有配置文件，可以copy示例文件到`/etc`下

### 安装

```shell
sudo apt-get install inotify-tools
```

### 建立密码认证文件

`/etc/rsyncd.pass`，备份服务器需要设置用户名和密码，主服务器只要密码

```pass
password
```

修改rsyncd.pass权限

```shell
chmod 600 /etc/rsyncd.pass
```

### 创建rsync复制脚本

此项功能主要是将主服务器的/tmp目录里的内容，如果修改了（无论是添加、修改、删除文件）能够通过inotify监控到，并通过rsync实时的同步给备份服务器的/tmp目录里。

`rsync.sh`

```shell
#!/bin/bash
# 备份服务器ip
host=192.168.1.108
# 主服务器监控目录
src=/tmp/
# 认证的模块名，需要与client一致
des=web
# user是备份服务器建立密码文件里的认证用户
user=www-data
/usr/local/inotify/bin/inotifywait -mrq --timefmt '%d/%m/%y %H:%M' --format '%T %w%f%e' -e modify,delete,create,attrib $src | while read files
do
/usr/bin/rsync -vzrtopg --delete --progress --password-file=/usr/local/rsync/rsync.passwd $src $user@$host::$des
echo "${files} was rsynced" >>/tmp/rsync.log 2>&1
done
```

在备份服务器启动rsync服务后启动
```shell
chmod 764 rsync.sh
sh /tmp/rsync.sh &
```

### 开启启动

将启动命令加入`/etc/rc.local `

## 备份服务器


### 建立密码认证文件

`/etc/rsyncd.pass`

```pass
www-data:password
```

修改rsyncd.pass权限

```shell
chmod 600 /etc/rsyncd.pass
```

### 建立配置文件

```shell
cp /usr/share/doc/rsync/examples/rsyncd.conf /etc
vi /etc/rsyncd.conf
```
默认配置
```conf
# sample rsyncd.conf configuration file

# GLOBAL OPTIONS

#motd file=/etc/motd
#log file=/var/log/rsyncd
# for pid file, do not use /var/run/rsync.pid if
# you are going to run rsync out of the init.d script.
# pid file=/var/run/rsyncd.pid
#syslog facility=daemon
#socket options=

# MODULE OPTIONS

[www]

        comment = wifi files
        path = /var/www/html/files
        use chroot = yes
#       max connections=10
        lock file = /var/lock/rsyncd
# the default for read only is yes...
        read only = yes
        list = yes
        uid = nobody
        gid = nogroup
#       exclude =
#       exclude from =
#       include =
#       include from =
#       auth users =
#       secrets file = /etc/rsyncd.secrets
        strict modes = yes
#       hosts allow =
#       hosts deny =
        ignore errors = no
        ignore nonreadable = yes
        transfer logging = no
#       log format = %t: host %h (%a) %o %f (%l bytes). Total %b bytes.
        timeout = 600
        refuse options = checksum dry-run
        dont compress = *.gz *.tgz *.zip *.z *.rpm *.deb *.iso *.bz2 *.tbz
```

```conf
uid = root
gid = root
use chroot = no
max connections = 10
strict modes = yes
pid file = /var/run/rsyncd.pid
lock file = /var/run/rsync.lock
log file = /var/log/rsyncd.log
[web]
path = /tmp/
comment = web file
ignore errors
read only = no
write only = no
hosts allow = 192.168.1.101
hosts deny = *
list = false
uid = root
gid = root
auth users = www-data
secrets file = /usr/local/rsync/rsync.passwd
```

现在就可以启动rsync了

```shell
rsync --daemon
```

### 客户端测试

```shell
rsync -vzrtopg --delete --password-file=/etc/rsync.pwd  www@192.168.1.108::www ./twofiles/
```

同步多台服务器
```shell
#!/bin/bash
src=/var/www/
des1=web1
des2=web2
host1=192.168.1.101
host2=192.168.1.102
user1=web
user2=web

/usr/local/bin/inotifywait -mrq --timefmt '%d/%m/%y %H:%M' --format '%T %w %f' -e modify,delete,create,attrib $src | while read file DATE TIME DIR;

do	/usr/bin/rsync -vzrtopg --delete --progress $src $user1@$host1::$des1 --password-file=/etc/web.passwd
	/usr/bin/rsync -vzrtopg --delete --progress $src $user2@$host2::$des2 --password-file=/etc/web.passwd
	echo "${files} was rsynced" &gt;&gt; /var/log/rsync.log 2&gt;&1
done
```
