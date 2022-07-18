# YugabyteDB

支持 PostgreSQL 扩展。

YugabyteDB is a high-performance, cloud-native distributed SQL database that aims to support all PostgreSQL features.

https://github.com/yugabyte/yugabyte-db

## 下载

```shell
wget https://downloads.yugabyte.com/releases/2.15.0.1/yugabyte-2.15.0.1-b4-linux-x86_64.tar.gz
tar xvfz yugabyte-2.15.0.1-b4-linux-x86_64.tar.gz
```

## 配置

`/etc/sysctl.conf`

```conf
kern.maxfiles=1048576
kern.maxproc=2500
kern.maxprocperuid=2500
kern.maxfilesperproc=1048576
```

`/etc/security/limits.conf`
```conf
* - nofile 65536
* soft noproc 65536
* hard noproc 65536
* soft nofile 65536
* hard nofile 65536
```

```shell
cd yugabyte-2.15.0.1/
./bin/post_install.sh
```

## 单节点

* 启动
```shell
./bin/yugabyted start --master_webserver_port=9999
```

* 状态

```shell
./bin/yugabyted status
```

## 多节点

