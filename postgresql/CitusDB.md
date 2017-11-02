# CitusDB

CitusDB 是一个基于最新 PostgreSQL 构建的分布式数据库。

https://github.com/citusdata/citus

## 单机器集群安装

https://docs.citusdata.com/en/v7.0/installation/single_machine_deb.html

```shell
# Add Citus repository for package manager
curl https://install.citusdata.com/community/deb.sh | sudo bash

# install the server and initialize db
sudo apt-get -y install postgresql-9.6-citus-7.0
```

### 初始化节点

```shell
# this user has access to sockets in /var/run/postgresql
sudo su - postgres

# include path to postgres binaries
export PATH=$PATH:/usr/lib/postgresql/9.6/bin

cd ~
mkdir -p citus/coordinator citus/worker1 citus/worker2

# create three normal postgres instances
initdb -D citus/coordinator
initdb -D citus/worker1
initdb -D citus/worker2
```

### 配置

```conf
echo "shared_preload_libraries = 'citus'" >> citus/coordinator/postgresql.conf
echo "shared_preload_libraries = 'citus'" >> citus/worker1/postgresql.conf
echo "shared_preload_libraries = 'citus'" >> citus/worker2/postgresql.conf
```

### 启动协调者和事务参与者（Start the coordinator and workers）

```shell
pg_ctl -D citus/coordinator -o "-p 9700" -l coordinator_logfile start
pg_ctl -D citus/worker1 -o "-p 9701" -l worker1_logfile start
pg_ctl -D citus/worker2 -o "-p 9702" -l worker2_logfile start
```

### 创建扩展

```shell
psql -p 9700 -c "CREATE EXTENSION citus;"
psql -p 9701 -c "CREATE EXTENSION citus;"
psql -p 9702 -c "CREATE EXTENSION citus;"
```

### 在 coordinator 中增加节点信息

```shell
psql -p 9700 -c "SELECT * from master_add_node('localhost', 9701);"
psql -p 9700 -c "SELECT * from master_add_node('localhost', 9702);"
```
验证安装
```shell
psql -p 9700 -c "select * from master_get_active_worker_nodes();"
```

## 创建表以及测试 citus 的功能

在citus中有两类表：

- Distributed Tables：即分片表。表的内容通过hash分在各个worker节点中
- Reference Tables：即广播表，在每个分片中都复制一份。

创建测试用户和数据库：

```shell
psql -p 9700 -c "CREATE USER citusr SUPERUSER;CREATE DATABASE test OWNER citusr;"
psql -p 9701 -c "CREATE USER citusr SUPERUSER;CREATE DATABASE test OWNER citusr;"
psql -p 9702 -c "CREATE USER citusr SUPERUSER;CREATE DATABASE test OWNER citusr;"
```

* 创建分表

使用我们之前建的用户`citusr`连接 `master`：
```shell
psql -U citusr -p 9700 -d test
```
创建两个分片表：
```sql
create table t01(id int, id2 int, t text);
create table t02(id int, id2 int, t text);
```

然后把这两张表定义为分片表：
```sql
select create_distributed_table('t01', 'id2');
select create_distributed_table('t02', 'id2', colocate_with=>'t01');
```

* 创建广播表：

```sql
create table t03(id int, id2 int, t text);
create table t04(id int, id2 int, t text);

select create_reference_table('t03');
select create_reference_table('t04');
```