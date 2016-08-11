# postgres主从流复制

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

停止主
```
service postgres stop
```

拷贝文件到从服务器

```shell
scp -r /var/lib/postgresql/9.4/main postgres@10.45.243.27:/var/lib/postgresql/9.4/
sudo ufw allow from 10.45.243.27
```

查看复制状态

```shell
select * from pg_stat_replication;
```

## postgres的从配置

从是10.45.243.27这台机器

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