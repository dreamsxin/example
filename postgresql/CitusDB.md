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

