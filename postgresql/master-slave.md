# postgres主从流复制

为了减少输入密码的操作，可以建立文件`.pgpass`

```txt
10.1.11.73:5678:postgres:replica:123456
```

```shell
chmod 0400 .pgpass 
```

## postgres的主配置

主是10.45.241.141这台机器

首先需要配置一个账号进行主从同步。

修改`pg_hba.conf`，增加replica用户，进行同步。

```shell
host    replication     replica        10.45.243.27/32         md5
```

127.0.0.1/32和192.168.1.1/24中的32与24，用32表示该IP被固定，用24表示前3位固定，后面一位可以由自己设。

给replica用户设置密码，登录和备份权限。

```shell
sudo -u postgres psql
CREATE ROLE replica login replication encrypted password 'replica'
```

修改`postgresql.conf`，注意设置下下面几个地方：
```conf
wal_level = hot_standby		# 这个是设置主为wal的主机

max_wal_senders = 10		# 这个设置了可以最多有几个流复制连接，差不多有几个从，就设置几个
wal_keep_segments = 20		# 设置流复制保留的最多的xlog数目
wal_sender_timeout = 60s	# 设置流复制主机发送数据的超时时间

max_connections = 100		# 这个设置要注意下，从库的max_connections必须要大于主库的
```

### 重启主服务

```
service postgres restart
sudo ufw allow from 10.45.243.27
```

### 拷贝文件到从服务器

```shell
scp -r /var/lib/postgresql/9.4/main postgres@10.45.243.27:/var/lib/postgresql/9.4/
```

也可以在从库执行
```shell
pg_basebackup -D /var/lib/postgresql/9.4/main -F p -x -l basebackup -P -v -h 10.45.241.141 -p5432 -U postgres
```
- -F, --format=p|t       output format (plain (default), tar)
- -x, --xlog             include required WAL files in backup (fetch mode)
- -l, --label=LABEL      set backup label
- -P, --progress         show progress information
- -v, --verbose          output verbose messages
- -W, --password         force password prompt (should happen automatically)

查看复制状态

```shell
select * from pg_stat_replication;
```

## postgres的从配置

从是`10.45.243.27`这台机器

编写 recovery.conf

```shell
cp /usr/share/postgresql/9.4/recovery.conf.sample /var/lib/postgresql/9.4/main/recovery.conf
```

```conf
# 这个说明这台机器为从库
standby_mode = on
# 这个说明这台机器对应主库的信息
primary_conninfo = 'host=10.45.241.141 port=5432 user=postgres password=mobilehunkxiao'
# 这个说明这个流复制同步到最新的数据
recovery_target_timeline = 'latest'
```

`postgresql.conf`

```conf
max_connections = 500			#一般查多于写的应用从库的最大连接数要比较大

hot_standby = on			#说明这台机器不仅仅是用于数据归档，也用于数据查询
max_standby_streaming_delay = 30s	# 数据流备份的最大延迟时间
wal_receiver_status_interval = 5s	# 多久向主报告一次从的状态，当然从每次数据复制都会向主报告状态，这里只是设置最长的间隔时间
hot_standby_feedback = on		# 如果有错误的数据复制，是否向主进行反馈
```

重启从服务器

```shell
sudo ufw allow from 10.45.241.141
sudo service postgresql restart
```

# 在Ubuntu 16.04上为PostgreSQL 9.6设置主从复制

https://www.howtoforge.com/tutorial/how-to-set-up-master-slave-replication-for-postgresql-96-on-ubuntu-1604/

## 先决条件

1. MASTER
– 允许读取和写入
– IP：10.0.15.10

2. SLAVE
– 只读根特权
– IP：10.0.15.11

## 添加源

将postgreSQL 9.6存储库添加到sources.list.d目录（https://www.postgresql.org/download/linux/ubuntu/）。

```shell
echo "deb http://apt.postgresql.org/pub/repos/apt/ xenial-pgdg main" | tee /etc/apt/sources.list.d/postgresql.list
```

并将PostgreSQL签名密钥导入系统。
```shell
wget --quiet -O - https://www.postgresql.org/media/keys/ACCC4CF8.asc | sudo apt-key add -
sudo apt-get update 
```

## 安装

```shell
apt-get install -y postgresql-9.6 postgresql-contrib-9.6
# 自启动
systemctl enable postgresql
```

## 修改密码

```shell
sudo -upostgres psql
```

执行 sql：
```sql
\password postgres
```

## 查看连接信息

执行 sql：
```sql
\conninfo
```

## 配置 UFW 防火墙

假如没有安装 UFW 防火墙可以忽略这一步。

```shell
ufw allow ssh
ufw allow postgresql
```

在MASTER和SLAVE服务器上运行上诉步骤。 

## 配置 MASTER 服务器

主服务器的IP地址为`10.0.15.10`，并且postgres服务将以该IP为默认端口运行。 主服务器将具有对数据库的READ和WRITE权限，并对从服务器执行流复制。

编辑 `/etc/postgresql/9.6/main/postgresql.conf`文件，修改`listen_addresses`：

```conf
listen_addresses = 'localhost,10.0.15.10'
```

取消注释`wal_level`行，并将值更改为`hot_standby`。
```conf
wal_level = hot_standby
```

对于同步级别，我们将使用本地同步。 取消注释和更改值行如下。
```conf
synchronous_commit = local
```

启用归档模式并将`archive_command`选项更改为命令`cp %p /var/lib/postgresql/9.6/main/archive/%f`。
```conf
archive_mode = on
archive_command = 'cp %p /var/lib/postgresql/9.6/main/archive/%f'
```

对于“复制”设置，在本教程中，我们仅使用2个服务器，主服务器和从服务器，取消注释`wal_sender`行，并将值更改为`2`，`wal_keep_segments`值为`10`。
```conf
max_wal_senders = 2
wal_keep_segments = 10
```

指定同步复制的 Standby 的名称，取消注释`synchronous_standby_names`行并将值更改为名称`pgslave001`。
```conf
synchronous_standby_names = 'pgslave001,pgslave002'
```
 注：此处设置的'pgslave001,pgslave002'就是在 Standby 数据库中配置连接参数“application_name”。

保存文件并退出编辑器。

在`postgresql.conf`文件中，归档模式已启用，因此我们需要为归档创建一个新目录。 更改权限并将所有者更改为`postgres`用户。

```shell
mkdir -p /var/lib/postgresql/9.6/main/archive/
chmod 700 /var/lib/postgresql/9.6/main/archive/
chown -R postgres:postgres /var/lib/postgresql/9.6/main/archive/
```

### 身份验证配置

`pg_hba.conf`：
```conf
# Localhost
host    replication     replica          127.0.0.1/32            md5

# PostgreSQL Master IP address
host    replication     replica          10.0.15.10/32            md5

# PostgreSQL SLave IP address
host    replication     replica          10.0.15.11/32            md5
```

### 创建用户

```sql
CREATE USER replica REPLICATION LOGIN ENCRYPTED PASSWORD 'aqwe123@';
```

现在，在下面查看具有`\du`查询的新用户，您将看到具有复制权限的副本用户。 

### 显示用户

```sql
\du
```

## 配置 SLAVE 服务器

停止服务

```shell
systemctl stop postgresql
```

编辑 `/etc/postgresql/9.6/main/postgresql.conf`文件：

```conf
listen_addresses = 'localhost,10.0.15.11'

wal_level = hot_standby

synchronous_commit = local

max_wal_senders = 2
wal_keep_segments = 10

hot_standby = on
```

## 从 MASTER 复制数据到 SLAVE

登录 SLAVE 服务器：
```shell
su - postgres
cd /var/lib/postgresql/9.6/
mkdir main/
chmod 700 main/
pg_basebackup -h 10.0.15.10 -U replica -D /var/lib/postgresql/9.6/main -P --xlog
```

创建`/var/lib/postgresql/9.6/main/recovery.conf`

```conf
standby_mode = 'on'
primary_conninfo = 'host=10.0.15.10 port=5432 user=replica password=aqwe123@ application_name=pgslave001'
# restore_command = 'cp /var/lib/postgresql/9.6/main/archive/%f %p'
recovery_target_timeline = 'latest'
trigger_file = '/tmp/postgresql.trigger.5432'
```

最后重启：
```shell
systemctl start postgresql
netstat -plntu
```

## 查看状态

登录 MASTER 服务器
```shell
sudo -upostgres psql -c "select application_name, state, sync_priority, sync_state from pg_stat_replication;"
sudo -upostgres psql -x -c "select * from pg_stat_replication;"
```

## 安装 pgbouncer

## 问题查找

```shell
systemctl status postgresql.service
```