## About Greenplum Database

Greenplum Database is an MPP SQL Database based on PostgreSQL. 
Its used in production in hundreds of large corporations and government agencies around the world and including the open source has over thousands of deployments globally. Greenplum Database scales to multi-petabyte data sizes with ease and allows a cluster of powerful servers to work together to provide a single SQL interface to the data. In addition to using SQL for analyzing structured data, Greenplum provides modules and extensions on top of the PostgreSQL abstractions for in database machine learning and AI, Geospatial analytics, Text Search (with Apache Solr) and Text Analytics with Python and Java, and the ability to create user-defined functions with Python, R, Java, Perl, C or C++.


See http://greenplum.org/install-greenplum-oss-on-ubuntu/

## 安装

```shell
sudo apt install software-properties-common
sudo add-apt-repository ppa:greenplum/db
sudo apt update
sudo apt install greenplum-db-6
```

```txt
$ source /opt/greenplum-db-6.9.1/greenplum_path.sh
$ which gpssh
/opt/green
# 拷贝配置文件
cp $GPHOME/docs/cli_help/gpconfigs/gpinitsystem_singlenode .
```
 ## 配置

* 主机列表

- MACHINE_LIST_FILE=./hostlist_singlenode
- declare -a DATA_DIRECTORY=(/home/gpadmin/primary /home/gpadmin/primary)
- MASTER_HOSTNAME=ubuntu
- MASTER_DIRECTORY=/home/gpadmin/master

* 运行 Going system

```shell
gpssh-exkeys -h localhost
gpssh-exkeys -f hostlist_singlenode
gpinitsystem -c gpinitsystem_singlenode
```

