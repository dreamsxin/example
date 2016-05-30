# 安装pgAgent

```shell
apt-get install pgagent
```

# 安装pgAgent数据库

pgAgent需要一些数据库表和其他对象的支持,因此需要先安装pgAgent数据库。pgagen包中已经包含了创建pgAgent数据库的脚本，有两个文件，分别是:
`/usr/share/pgadmin3/pgagent.sql`
`/usr/share/pgadmin3/pgagent_upgrade.sql`

以数据库管理员身份连接到系统数据库postgres,分别执行这两个脚本,会创建一个新的schema pgagent。pgagent的详细参数见man pgagent。

pgagent需要一个标准的postgresql连接字符串连接到数据库，比如:

```shell
pgagent hostaddr=localhost dbname=postgres user=postgres
```

因为安全原因，不能直接将密码写入连接字符串，因为那样任何人使用ps命令就可以看到密码。所以使用~/.pgpass文件为数据库用户提供密码。

创建pgagent用户和.pgpass文件

pgAgent将以pgagent用户的身份运行，因此首先创建pgagent用户

```shell
adduser --shell=/bin/bash pgagent
```

然后在pgagent用户的主目录下新建.pgpass文件

```ini
#hostname:port:database:username:password
*:*:*:postgres:postgres
```
将.pgpass的访问权限设置为0600

```shell
chown pgagent:pgagent .pgpass
chmod 0600 .pgpass
```

有一点儿应该注意，用户pgagent连接数据库时的用户必须支持使用MD5密码认证才行,具体详见pg_hba.conf配置。

## 开机启动脚本

/usr/bin/pgagent需要作为守护程序运行，它会周期性的查询postgresql数据库，然后执行用户设定的jobs。
pgagent包并没有带init脚本，因此写了下面的init脚本,使pgagent可以作为Daemon程序自动运行。代码如下:

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
    echo "$PROG -s $LOGFILE hostaddr=$DBHOST port=$DBPORT dbname=$DBNAME user=$DBUSER"
    $SU - pgagent -c "$PROG -s $LOGFILE hostaddr=$DBHOST port=$DBPORT dbname=$DBNAME user=$DBUSER"
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

使用这个脚本需要新建pgagent用户来运行daemon程序，并且要在pgagent的用户主目录下添加.pgpass文件。

如需要更改pgagent运行时的选项，在/etc/default/pgagent文件添加选项，覆盖默认值即可。

在各个运行级对应的启动脚本目录下创建符号连接

```shell
sudo chmod a+x /etc/init.d/pgagent
sudo update-rc.d pgagent defaults
```