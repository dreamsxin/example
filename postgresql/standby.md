# standby节点逻辑复制验证

## 搭建实验环境

主机	ip
- pg16node1	192.168.100.51
- pg16node2	192.168.100.52
- pg16node3	192.168.100.53

## 创建用户和所需目录
```shell
groupadd postgres
useradd -g postgres postgres
echo '123456' | passwd --stdin postgres
mkdir -p /data/postgres/
chown -R postgres:postgres /data
```

## 设置环境变量，方便后期使用
```shell
vim ~/.bash_profile

export PATH=$PATH:/data/postgres/bin
export PGDATA=/data/data

source ~/.bash_profile
``

## node1

### 创建实验表并插入实验数据

```shell
postgres=# create table no_filter(id int, primary key (id));
CREATE TABLE
postgres=# insert into no_filter values(1),(2),(3);
INSERT 0 3
postgres=# create table has_filter(id int,filter_tag int, primary key (id));
CREATE TABLE
postgres=# insert into has_filter values(1,1),(2,0);
INSERT 0 2
```

###  创建流复制用户和发布订阅用户并授权
```shell
postgres=# create user repuser with replication login encrypted password '1234@1234';
CREATE ROLE
postgres=# create user pubuser with replication login encrypted password '1234@4321';
CREATE ROLE
-- repuser 是搭建流复制时使用，不用赋权
postgres=# grant select on table no_filter, has_filter to pubuser;
GRANT
```

### 创建发布
```shell
postgres=# create publication no_filter_pub for table no_filter;
CREATE PUBLICATION
postgres=# create publication has_filter_pub for table has_filter where (filter_tag = 1);
CREATE PUBLICATION
```

### 修改数据库配置参数
```shell
# 编辑配置文件
vim /data/data/postgresql.conf

# 打开异机登录
listen_addresses = '*'
# 配置端口
port = 5432  
# 设置日志界别
wal_level = logical  
# 开启归档
archive_mode = on 
# 实验环境不配置具体归档命令
archive_command = '/bin/date'
# 设置日志发送进程最大数量
max_wal_senders = 10 
# 设置复制槽最大数量
max_replication_slots = 10
# 开启备机只读
hot_standby = on 
```
### 配置访问权限
```shell
vim /data/data/pg_hba.conf
# 文件末尾添加如下内容
host    replication     repuser         192.168.100.51/32       md5
host    replication     repuser         192.168.100.52/32       md5

# 配置修改后需要重启数据库
pg_ctl restart -D /data/data
```

## node2

```shell
搭建流复制集群备节点
以下步骤在pg16node2上使用postgres用户操作

# 停止备节点
pg_ctl stop
# 删除数据目录
rm -rf /data/data/*
# 做一个基准备份
[postgres@pg16node2 ~]$ pg_basebackup -D /data/data -Fp -Xs -v -P -h 192.168.100.51 -p 5432 -U repuser
Password:
pg_basebackup: initiating base backup, waiting for checkpoint to complete
pg_basebackup: checkpoint completed
pg_basebackup: write-ahead log start point: 0/D000028 on timeline 1
pg_basebackup: starting background WAL receiver
pg_basebackup: created temporary replication slot "pg_basebackup_34692"
26989/26989 kB (100%), 1/1 tablespace
pg_basebackup: write-ahead log end point: 0/D000100
pg_basebackup: waiting for background process to finish streaming ...
pg_basebackup: syncing data to disk ...
pg_basebackup: renaming backup_manifest.tmp to backup_manifest
pg_basebackup: base backup completed

# 修改配置文件
vim /data/data/postgresql.conf

# 配置恢复目标
recovery_target_timeline = 'latest' 
# 配置主节点信息
primary_conninfo = 'host=192.168.100.51 port=5432 user=repuser password=1234@1234'

# 添加第三节点的访问权限
vim /data/data/pg_hba.conf
# 文件末尾添加如下内容
host    all             pubuser         192.168.100.53/32       md5

# 创建standby标记文件
touch /data/data/standby.signal

# 保存后启动数据库
pg_ctl start
登录后复制
-- 使用 psql 登录数据库，确认数据库处于standby的恢复状态
postgres=# select pg_is_in_recovery();
 pg_is_in_recovery
-------------------
 t
(1 row)
```

## node3 创建订阅

验证standby节点逻辑复制
以下步骤在pg16node3节点执行
```shell
-- 使用 psql 登录数据，创建要复制的表
postgres=# create table no_filter(id int, primary key (id));
CREATE TABLE
postgres=# create table has_filter(id int,filter_tag int, primary key (id));
CREATE TABLE

-- 创建订阅
-- 创建不过滤订阅
create subscription no_filter_sub connection 'dbname=postgres host=192.168.100.52 port=5432 user=pubuser password=1234@4321' publication no_filter_pub;
-- 以上命令在主库空闲时会卡住，在pg16node1上执行如下命令可以使命令继续
postgres=# select pg_log_standby_snapshot();
 pg_log_standby_snapshot
-------------------------
 0/E0069B8
(1 row)

-- 创建过滤订阅
create subscription has_filter_sub connection 'dbname=postgres host=192.168.100.52 port=5432 user=pubuser password=1234@4321' publication has_filter_pub;
-- 还是需要在主库上执行 select pg_log_standby_snapshot(); 才能使命令继续
```

## node2

验证standby节点创建逻辑复制槽成功
以下命令在standby节点pg16node2执行

```shell
postgres=# select * from pg_replication_slots ;
-[ RECORD 1 ]-------+---------------
slot_name           | no_filter_sub
plugin              | pgoutput
slot_type           | logical
datoid              | 5
database            | postgres
temporary           | f
active              | t
active_pid          | 31338
xmin                |
catalog_xmin        | 1646
restart_lsn         | 0/12000720
confirmed_flush_lsn | 0/12000758
wal_status          | reserved
safe_wal_size       |
two_phase           | f
conflicting         | f
-[ RECORD 2 ]-------+---------------
slot_name           | has_filter_sub
plugin              | pgoutput
slot_type           | logical
datoid              | 5
database            | postgres
temporary           | f
active              | t
active_pid          | 31343
xmin                |
catalog_xmin        | 1646
restart_lsn         | 0/12000720
confirmed_flush_lsn | 0/12000758
wal_status          | reserved
safe_wal_size       |
two_phase           | f
conflicting         | f
```

## node3

验证逻辑复制情况
以下步骤在pg16node3节点执行
```shell
-- 查询无过滤表数据
postgres=# select * from no_filter;
 id
----
  1
  2
  3
(3 rows)
-- 没有过滤条件的表已经同步成功
-- 在主库执行 insert into no_filter values(4); 插入一条数据，再次查询
postgres=#  select * from no_filter;
 id
----
  1
  2
  3
  4
(4 rows)
-- 新增数据也同步成功
-- 查询有过滤表数据
postgres=# select * from has_filter ;
 id | filter_tag
----+------------
(0 rows)
-- 此时没有数据，在主库执行 insert into has_filter values(3,1); 插入一条数据
--再次查询
postgres=# select * from has_filter ;
 id | filter_tag
----+------------
  1 |          1
  3 |          1
(2 rows)
-- 符合条件的数据已同步
```
