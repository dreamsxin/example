# 安装 Postgresql
```shell
# Ubuntu 12
sudo add-apt-repository ppa:pitti/postgresql
sudo apt-get update
sudo apt-get install postgresql
# Ubuntu 14 到 Postgresql 官网
```

# 设置默认密码
```shell
psql -U postgres
sudo -u postgres psql 
\passwd
```

# 远程连接修改pg_hba.conf
```conf
host    all             all             0.0.0.0/0               md5
```

# 备份与恢复

## 注意目录权限
```shell
chown postgres:postgres . -R
```
## main/pg_xlog/archive_status/  目录必须存在

## 配置 wal 日志 /etc/postgresql/9.4/main/postgresql.conf
```conf
wal_level = archive
archive_mode = on
archive_command = 'test ! -f /mnt/server/archivedir/%f && cp /var/lib/postgresql/9.4/main/%p /mnt/server/archivedir/%f'         # command to use to archive a logfile segment
```

## 重启数据库

## 創建基礎備份
清空 /mnt/server/archivedir/ 目录
```shell
postgres=# select pg_start_backup('bakup_base');
```

## 備份整個data目錄 (tar方式)
```shell
cd /var/lib/postgresql/9.4/
tar cvf main.tar main
```

## 停止備份
```shell
postgres=# select pg_stop_backup();
```

## 切換日誌
```shell
postgres=# select pg_switch_xlog();
```

## 写入测试数据

## 停止数据库
```shell
sudo service postgresql stop
```

## 使用基础备份目录替换当前目录
```shell
cd /var/lib/postgresql/9.4/
rm main -R
tar xvf main.tar 
```

## 编写 recovery.conf
```shell
cp /usr/share/postgresql/9.4/recovery.conf.sample /var/lib/postgresql/9.4/main/recovery.conf
```
在时间点恢复使用 select now() 获取的时间格式
```conf
restore_command = 'cp /mnt/server/archivedir/%f %p'
recovery_target_time = '2016-01-18 10:20:08.367751+08'
```

## 启动数据库完成恢复


# pgagent 的配置使用

## 安装
```shell
sudo apt-get install pgagent
```

## 在库中创建扩展
```shell
CREATE EXTENSION pgagent;
#sudo -u postgres psql -U postgres -d ads < /usr/share/pgadmin3/pgagent.sql
```

## 启动任务
```shell
sudo -u postgres pgagent -l2  hostaddr=localhost dbname=ads user=postgres
```
## 配置开机启动
创建脚本
```shell
#!/bin/bash
set -e
 
#
# Starts / stops the pgagent daemon
#
# /etc/init.d/pgagent
 
### BEGIN INIT INFO
# Provides:         pgagent
# Required-Start:   $local_fs $remote_fs $network $time postgresql
# Required-Stop:    $local_fs $remote_fs $network $time
# Should-Start:     $syslog
# Should-Stop:      $syslog
# Default-Start:    2 3 4 5
# Default-Stop:     0 1 6
# Short-Description:    pgagent Postgresql Job Service
### END INIT INFO
 
# For SELinux we need to use 'runuser' not 'su'
if [ -x /sbin/runuser ]
then
    SU=runuser
else
    SU=su
fi
 
DBNAME=${DBNAME-postgres}
DBUSER=${DBUSER-postgres}
DBHOST=${DBHOST-localhost}
DBPORT=${DBPORT-5432}
LOGFILE=${LOGFILE-/var/log/pgagent.log}
pidfile="/var/run/pgagent.pid"
 
RETVAL=0
NAME="pgagent"
PROG="/usr/bin/pgagent"
 
# Override defaults from /etc/default/pgagent file,if file is present:
[ -f /etc/default/pgagent ] && . /etc/default/pgagent
 
echo_success() {
    echo "Success."
}
echo_failure() {
    echo "Failure."
}
 
start() {
    # Make sure that pgagent is not already running:
    if [ -e "${pidfile}" ]
    then
        echo "${NAME} is already running"
        exit 0
    fi
 
    echo "Starting ${NAME} service... "
 
    if [ ! -e "${LOGFILE}" ]; then
        touch ${LOGFILE}
        chown root:pgagent ${LOGFILE}
        chmod g+rw ${LOGFILE} 
    fi
 
    $SU - pgagent -c "$PROG -s $LOGFILE hostaddr=$DBHOST dbname=$DBNAME user=$DBUSER"
    RETVAL=$?
    if [ $RETVAL -eq 0 ]
    then
        echo_success
        touch $pidfile
        echo `pidof pgagent` > $pidfile
    else
        echo_failure
        return -1
    fi
}
 
stop(){
    echo $"Stopping ${NAME} service... "
 
    if [ ! -e "$pidfile" ]; then
        echo "${NAME} is not running."
        exit 0
    else
        pid=`cat $pidfile`
        kill $pid || true
        rm $pidfile
        echo_success
        return 0
    fi
}
 
status() {
    if [ ! -f "${pidfile}" ]; then
        echo "${NAME} is not running."
    else
        echo "${NAME} is running."
    fi
}
 
#
case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    reload|restart)
        stop
        start
        ;;
    status)
        status
        ;;
    *)
        echo $"Usage: $0 {start|stop|restart|reload|status}"
esac
 
exit $?
```

使用该脚本后，如需要更改pgagent运行时的选项，在/etc/default/pgagent文件添加选项，覆盖默认值即可。

在各个运行级对应的启动脚本目录下创建符号连接
```shell
update-rc.d pgagent defaults
```

# 安装 jsobx 扩展
```shell
git clone https://github.com/erthalion/jsonbx.git
cd jsonbx
make -j4 && sudo make install
```

# 连接池 PgBouncer
```shell
sudo apt-get install pgbouncer
```

## 修改配置 `/etc/pgbouncer/pgbouncer.ini`
```ini
[databases]
ads = host=127.0.0.1 port=5432 dbname=ads

auth_type = md5
auth_file = /etc/pgbouncer/userlist.txt
```

`/etc/pgbouncer/userlist.txt`
```ini
"postgres" "123456"
```

## 测试
```shell
psql -h 127.0.0.1 -p 6432 -U postgres ads
```

## pg_dump 备份

```shell
/usr/bin/pg_dump --host 127.0.0.1 --port 5432 --username "postgres" --no-password  --format plain --data-only --inserts --column-inserts --verbose --file "/var/www/db.backup" --table "public.users" --table "public.blogs" --table "public.apps" --table "public.countries" --table "public.phone_apps" --table "public.phones" --table "public.projects"  "ads"

/usr/lib/postgresql/9.5/bin/pg_dump --host 127.0.0.1 --port 5432 --username "postgres" --no-password  --format plain --data-only --inserts --column-inserts --verbose --file "/var/www/db.backup" "blog"
```