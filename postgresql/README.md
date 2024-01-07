中文手册 https://github.com/postgres-cn/pgdoc-cn

# 生态圈

 Data processing engine base on Postgresql 

1. Postgresql		-- master RDBMS
2. CitusDB			-- OLAP
3. PipelineDB		-- Streaming big data analysis 
4. Postgresql-XL	-- Scale out OLTP 
5. rubyrep、bucardo

# 安装 Postgresql

```shell
# Create the file repository configuration:
sudo sh -c 'echo "deb http://apt.postgresql.org/pub/repos/apt $(lsb_release -cs)-pgdg main" > /etc/apt/sources.list.d/pgdg.list'

# Import the repository signing key:
wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | sudo apt-key add -

# Update the package lists:
sudo apt-get update

# Install the latest version of PostgreSQL.
# If you want a specific version, use 'postgresql-12' or similar instead of 'postgresql':
sudo apt-get -y install postgresql

# Ubuntu 12
sudo add-apt-repository ppa:pitti/postgresql
sudo apt-get update
sudo apt-get install postgresql
```

## Ubuntu

官网 https://www.postgresql.org/download/linux/ubuntu/

编辑 `/etc/apt/sources.list` 追加一行

14.04
```conf
deb http://apt.postgresql.org/pub/repos/apt/ trusty-pgdg main
```
16.04
```conf
deb http://apt.postgresql.org/pub/repos/apt/ xenial-pgdg main
```

执行
```shell
wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | sudo apt-key add -
sudo apt-get update
apt-get install postgresql-9.5
```

## psql 使用

- 查看所有库：\l
- 查看所有表：\dt
- 查看某个表的结构： \d 表名
- 查看搜索路径下所有函数：\df

# 设置默认密码
```shell
psql -U postgres
# or
sudo -u postgres psql
\password postgres
```

# 远程连接修改pg_hba.conf
```conf
local   all         postgres                          trust
# TYPE  DATABASE    USER        CIDR-ADDRESS          METHOD

# "local" is for Unix domain socket connections only
local   all         all                               ident
# IPv4 local connections:
host    all         all         127.0.0.1/32          md5
host    all         all         0.0.0.0/0             md5
# IPv6 local connections:
host    all         all         ::1/128               md5
```

# 监听地址修改postgresql.conf
```conf
listen_addresses = 'localhost,192.168.1.100'
```

## 时区
修改配置
```conf
log_timezone = 'Asia/Shanghai'
timezone = 'Asia/Shanghai'
```
```psql
SHOW timezone;
```
客户端连接时带上
```go
dsn := fmt.Sprintf("user=%s password=%s host=%s port=%d dbname=%s sslmode=disable TimeZone=Asia/Shanghai", UserName, Password, Host, Port, DBName)
```
# 使用 pg_dump

## 明文 导出指定表数据
```shell
sudo -u postgres pg_dump --format plain --data-only --inserts --column-inserts --verbose --file "db.backup" --table "public.adlogs" --table "public.ads" --table "public.apps" --table "public.countries" --table "public.phone_apps" --table "public.phones" --table "public.projects" --table "public.settings" --table "public.source_rules" --table "public.sources" --table "public.users" "ads"
```

## 查看配置

```sql
show shared_buffers;
show all;
```

## 执行 sql 文件

```shell
psql -d ads -f db.backup
```

也可直接在 sql 脚本开头加：`psql ads (-U postgres)<<!`，然后执行运行该脚本。

也可以在psql交互界面中可以直接用：
```shell
\i db.backup
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
#系统视图查看可用扩展
SELECT * FROM pg_available_extensions;
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

PGPASSWORD="test" && /usr/lib/postgresql/9.5/bin/pg_dump --username "postgres" --format plain --data-only --inserts --column-inserts --verbose --file "db.backup" -h192.168.1.1 -p5434 "test"
```

# 恢复 pg_xlog

```shell
sudo -u postgres /usr/lib/postgresql/9.4/bin/pg_resetxlog  .
```

# 查看数据库占用空间

```sql
select pg_database.datname, pg_database_size(pg_database.datname) AS size from pg_database;
```

# 查看表占用空间

```sql
# 所有表
select relname, pg_size_pretty(pg_relation_size(relid)) from pg_stat_user_tables where schemaname='public' order by pg_relation_size(relid) desc;
# 指定表
select pg_size_pretty(pg_table_size('phone_signs'));
```

## 其他

```sql
# 增加一列
ALTER TABLE table_name ADD column_name datatype;
 
# 删除一列
ALTER TABLE table_name DROP  column_name;

# 更改列的数据类型
ALTER TABLE table_name ALTER  column_name TYPE datatype;

# 表的重命名
ALTER TABLE table_name RENAME TO new_name;

# 更改列的名字
ALTER TABLE table_name RENAME column_name to new_column_name;
 
# 字段的not null设置
ALTER TABLE table_name ALTER column_name {SET|DROP} NOT NULL;

# 给列添加default
ALTER TABLE table_name ALTER column_name SET DEFAULT expression;

# 删除约束
ALTER TABLE table_name DROP CONSTRAINT projects_code_unique;

# 创建约束
ALTER TABLE table_name ADD CONSTRAINT projects_code_unique UNIQUE(channel_code,project_code);
```

## 慢查询

```sql
CREATE EXTENSION pg_stat_statements;
SELECT query, calls, total_time, (total_time/calls) as average ,rows, 100.0 * shared_blks_hit /nullif(shared_blks_hit + shared_blks_read, 0) AS hit_percent FROM  pg_stat_statements ORDER BY average DESC LIMIT 10;
```

## 正在执行 SQL 列表

```sql
SELECT 
    procpid, 
    start, 
    now() - start AS lap, 
    current_query 
FROM 
    (SELECT 
        backendid, 
        pg_stat_get_backend_pid(S.backendid) AS procpid, 
        pg_stat_get_backend_activity_start(S.backendid) AS start, 
       pg_stat_get_backend_activity(S.backendid) AS current_query 
    FROM 
        (SELECT pg_stat_get_backend_idset() AS backendid) AS S 
    ) AS S 
WHERE current_query <> '<IDLE>' 
ORDER BY lap DESC;
```

- procpid：进程id
- start：进程开始时间
- lap：经过时间
- current_query：执行中的sql

停止正在执行的sql
- SELECT pg_cancel_backend(进程id);
- kill -9 进程id;

## 优化

```sql
EXPLAIN ANALYZE SELECT * FROM "logs" WHERE DATE("phone_signs"."created") BETWEEN '2017-11-10' AND '2017-11-11'
```

### 配置

监听IPv4的所有IP.
- listen_addresses = '0.0.0.0'

最大允许1000个连接.
- max_connections = 1000

为超级用户保留3个可用连接.
- superuser_reserved_connections = 3

默认的unix socket文件放在/tmp, 修改为$PGDATA, 以确保安全.
- unix_socket_directory = '/pgdata/digoal/1921/data02/pg_root'

默认的访问权限是0777, 修改为0700更安全.
- unix_socket_permissions = 0700

Linux下面默认是2小时.tcp的keepalives包发送间隔以及重试次数, 如果客户端没有响应, 将主动释放对应的SOCKET.
- tcp_keepalives_idle = 60
- tcp_keepalives_interval = 10
- tcp_keepalives_count = 6

大的shared_buffers需要大的checkpoint_segments,同时需要申请更多的System V共享内存资源.
这个值不需要设的太大, 因为PostgreSQL还依赖操作系统的cache来提高读性能, 另外, 写操作频繁的数据库这个设太大反而会增加checkpoint压力.
- shared_buffers = 512MB

这个值越大, VACUUM, CREATE INDEX的操作越快, 当然大到一定程度瓶颈就不在内存了, 可能是CPU例如创建索引.
这个值是一个操作的内存使用上限, 而不是一次性分配出去的. 并且需要注意如果开启了autovacuum, 最大可能有autovacuum_max_workers*maintenance_work_mem的内存被系统消耗掉.
- maintenance_work_mem = 512MB

一般设置为比系统限制的略少,ulimit -a : stack size              (kbytes, -s) 10240
- max_stack_depth = 8MB

手动执行vacuum操作时, 默认是没有停顿执行到底的, 为了防止VACUUM操作消耗太多数据库服务器硬件资源, 这个值是指vacuum在消耗多少资源后停顿多少时间,以便其他的操作可以使用更多的硬件资源.
- vacuum_cost_delay = 10ms

默认bgwriter进程执行一次后会停顿200ms再被唤醒执行下一次操作, 当数据库的写操作很频繁的时候, 200ms可能太长, 导致其他进程需要花费过多的时间来进行bgwriter的操作.
- bgwriter_delay = 10ms

如果需要做数据库WAL日志备份的话至少需要设置成archive级别, 如果需要做hot_standby那么需要设置成hot_standby, 由于这个值修改需要重启数据库, 所以先设置成hot_standby比较好. 当然hot_standby意味着WAL记录得更详细, 如果没有打算做hot_standby设置得越低性能越好.
- wal_level = hot_standby

wal buffers默认是-1 根据shared_buffers的设置自动调整shared_buffers*3% .最大限制是XLOG的segment_size.
- wal_buffers = 16384kB

多少个xlog file产生后开始checkpoint操作, 这个值越大, 允许shared_buffer中的被频繁访问的脏数据存储得更久. 一定程度上可以提高数据库性能. 但是太大的话会导致在数据库发生checkpoint的时候需要处理更多的脏数据带来长时间的IO开销. 太小的话会导致产生更多的WAL文件(因为full page writes=on,CHECKPOINT后的第一次块的改变要写全块, checkpoint越频繁, 越多的数据更新要写全块导致产生更多WAL).
- checkpoint_segments = 64

这个和checkpoint_segments的效果是一样的, 只是触发的条件是时间条件.
- checkpoint_timeout = 5min

归档参数的修改也需要重启数据库, 所以就先打开吧.
- archive_mode = on

这个是归档调用的命令, 我这里用date代替, 所以归档的时候调用的是输出时间而不是拷贝wal文件.
- archive_command = '/bin/date'

如果要做hot standby这个必须大于0, 并且修改之后要重启数据库所以先设置为32.
- max_wal_senders = 32

这是个standby 数据库参数, 为了方便角色切换, 我一般是所有的数据库都把他设置为on 的.
- hot_standby = on

这个参数是说数据库中随机的PAGE访问的开销占seq_page_cost的多少倍 , seq_page_cost默认是1. 其他的开销都是seq_page_cost的倍数. 这些都用于基于成本的执行计划选择.
- random_page_cost = 2.0

和上一个参数一样, 用于基于成本的执行计划选择. 不是说会用多少cache, 它只是个度量值. 表示系统有多少内存可以作为操作系统的cache. 越大的话, 数据库越倾向使用index这种适合random访问的执行计划.
- effective_cache_size = 12000MB

下面是日志输出的配置.
```conf
log_destination = 'csvlog'
logging_collector = on
log_directory = '/var/applog/pg_log/digoal/1921'
log_truncate_on_rotation = on
log_rotation_age = 1d
log_rotation_size = 10MB
```

这个参数调整的是记录执行时间超过1秒的SQL到日志中, 一般用于跟踪哪些SQL执行时间长.
- log_min_duration_statement = 1000ms

记录每一次checkpoint到日志中.
- log_checkpoints = on

记录锁等待超过1秒的操作, 一般用于排查业务逻辑上的问题.
- log_lock_waits = on
- deadlock_timeout = 1s

记录DDL语句, 一般用于跟踪数据库中的危险操作.
- log_statement = 'ddl'

这个原本是1024表示跟踪的SQL在1024的地方截断, 超过1024将无法显示全SQL. 修改为2048会消耗更多的内存(基本可以忽略), 不过可以显示更长的SQL. 
- track_activity_query_size = 2048

默认autovacuum就是打开的, log_autovacuum_min_duration = 0记录所有的autovacuum操作.
- autovacuum = on
- log_autovacuum_min_duration = 0

这个模块用于记录数据库中的最近的1000条SQL以及这些SQL的统计信息, 如执行了多少次, 总共耗时是多少. 一般用于发现业务上最频繁调用的SQL是什么, 有针对性的进行SQL优化.
```conf
shared_preload_libraries = 'pg_stat_statements'
custom_variable_classes = 'pg_stat_statements'
pg_stat_statements.max = 1000
pg_stat_statements.track = all
```

这些参数的详细解释如有疑问请参考PostgreSQL官方文档.

仔细查看两个计划的预估成本和实际运行时间，实际运行时间与估计成本的比率是非常不同的。 造成这种差异的主要原因是顺序扫描成本估算，PostgreSQL估计顺序扫描比4000+索引扫描更好，但实际上索引扫描速度要快50倍。
这主要与'random_page_cost'和'seq_page_cost'配置选项相关。对于'random_page_cost'，'seq_page_cost'分别默认的PostgreSQL值为4和1，这些值是针对硬盘设置的，随机存取磁盘比顺序存取要贵。然而，这些成本对于使用固态驱动器的gp2 EBS卷部署是不准确的。对于这种部署方式，随机和顺序访问几乎是一样的。
Amplitude将“random_page_cost”更改为1并重试了查询。这一次，PostgreSQL使用了一个嵌套循环，查询速度提高了50倍。改变之后，我们也注意到PostgreSQL的最大响应时间显著下降。

### 优化器选择并行计算的相关参数

PostgreSQL会通过这些参数来决定是否使用并行，以及该启用几个work process。

- max_worker_processes (integer)
很显然，这个参数决定了整个数据库集群允许启动多少个work process，注意如果有standby，standby的参数必须大于等于主库的参数值。

如果设置为0，表示不允许并行。

- max_parallel_workers_per_gather (integer)
这个参数决定了每个Gather node最多允许启用多少个work process。

同时需要注意，在OLTP业务系统中，不要设置太大，因为每个worker都会消耗同等的work_mem等资源，争抢会比较厉害。

建议在OLAP中使用并行，并且做好任务调度，减轻冲突。

- parallel_setup_cost (floating point)
表示启动woker process的启动成本，因为启动worker进程需要建立共享内存等操作，属于附带的额外成本。

- parallel_tuple_cost (floating point)
woker进程处理完后的tuple要传输给上层node，即进程间的row交换成本，按node评估的输出rows来乘。

- min_parallel_relation_size (integer)
表的大小，也作为是否启用并行计算的条件，如果小于它，不启用并行计算。

- force_parallel_mode (enum)
强制开启并行，可以作为测试的目的，也可以作为hint来使用。

- parallel_workers (integer)
以上都是数据库的参数，parallel_workers是表级参数，可以在建表时设置，也可以后期设置。

```sql
# 设置表级并行度
alter table test set (parallel_workers=0);
# 关闭表的并行
alter table test set (parallel_workers=0);
# 重置参数，那么在create_plain_partial_paths中会通过表的pages计算出一个合理的并行度
alter table test reset (parallel_workers);
```

## 单用户模式

如果忘记用户密码或者误删用户之后，可以进入单用户模式进行修复。

```shell
sudo -upostgres postgres --single -D /usr/xxx/pgsql/data
```

```shell
postgres --single -D $HG_HOME/data
PostgreSQL stand-alone backend 9.4.7
backend> ALTER role postgres WITH SUPPERUSER;
```
