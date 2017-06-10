# 创建客户化的PostgreSQL镜像

运行一个操作系统为 ubuntu 的容器，并且将宿主机的两个本地目录 mount 到了容器内部。

```shell
docker pull ubuntu
mkdir -p /backup/postgresql    
mkdir -p /storage/docker/postgresql


## 新建镜像并运行 build_pg_img
docker run -it --name build_pg_img \
-v /storage/docker/postgresql:/var/lib/postgresql -v /backup/postgresql:/backup \
ubuntu \
/bin/bash
```

## 连接容器

```shell
docker exec –it build_pg_img /bin/bash
```

### 安装软件

```shell
apt-get update    
apt-get install vim    
apt-get install postgresql-9.5
```

### 切换到postgres用户 

```shell
su - postgres
```

### 编辑 postgres 用户的 profile

`.bash_profile`：
```shell
#!/bin/bash    
PGHOME=/usr/lib/postgresql/9.5    
export PGHOME    
export PGPORT=5432    
export PGDATA=/var/lib/postgresql/data    
PATH=$PGHOME/bin:$PATH:$HOME/bin    
export PATH    
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PGHOME/lib    
export LD_LIBRARY_PATH
```

### 初始化PG数据库目录 

```shell
cd /var/lib/postgresql/
# 我们不使用安装时的 PG 库
mv 9.5 9.5_bak
mkdir data    
initdb -D /var/lib/postgresql/data     
cd /var/lib/postgresql/data    
mkdir pg_log pg_archlog
```
### 配置`pg_hba.conf`

```conf  
host all          all       192.168.0.0/16    md5    
host replication  repuser   192.168.0.0/16    md5
```

### 启动数据库

```shell
pg_ctl start -D /var/lib/postgresql/data
```

### 创建扩展

```shell
create extension pg_stat_statements;
```

## 停止数据库，退出容器

```shell
pg_ctl stop -D /var/lib/postgresql/data -m fast    
exit    
exit
```

## 查看运行状态

```shell
docker ps -a |grep build_pg_img
```

## 创建镜像

```shell
docker commit build_pg_img pg9.5_template
```

## 编辑 Dockerfile

默认用户 postgresql，启动 Postgresql 服务。

```Dockerfile
FROM  pg9.5_template    
USER postgres    
ENV PGHOME /usr/lib/postgresql/9.5    
ENV PGPORT 5432    
ENV PGDATA /var/lib/postgresql/data    
ENV PATH $PGHOME/bin:$PATH    
ENV LD_LIBRARY_PATH=$PGHOME/lib:$LD_LIBRARY_PATH    
EXPOSE 5432    
CMD ["/usr/lib/postgresql/9.5/bin/postgres", "-D", "/var/lib/postgresql/data"]
```

## 查看镜像列表

```shell
docker images
```

## 创建新镜像

```shell
docker build -t eg_postgresql .
```

## 基于新镜像eg_postgresql来启动新的pg容器:

```shell
docker run 
--name pg_pri \
-v /storage/docker/postgresql:/var/lib/postgresql \
-v /backup/postgresql:/backup \
-p 5436:5432 \
-d eg_postgresql
```

## 查看容器状态

```shell
docker ps  |grep pg_pri
```

## 进入容器

```shell
docker exec -it pg_pri /bin/bash 
```

## 使用 root 用户进入容器

```shell
docker exec -u root -it pg_pri /bin/bash
```

然后就可以连接数据库

```shell
psql
```

## 将镜像打包

```shell
docker save -o eg_postgresql.tar eg_postgresql
```

## 传输到从库所在的机器上

```shell
scp eg_postgresql.tar core@192.168.11.2:/tmp/.
```

## 在从库机器（host2）上load image

```shell
docker load -i eg_postgresql.tar
```

## 搭建主从流复制

### 主服务器

* 进入主库容器
```shell
docker exec -it pg_pri /bin/bash
```

*主库创建流复制用户
```shell
CREATE USER repuser replication LOGIN CONNECTION LIMIT 3 ENCRYPTED PASSWORD 'repuser';
```

* 主库打开备份

```shell
select pg_start_backup('Replition work');
```

* 退出容器，备份操作数据文件拷贝到从服务器
```shell
cd /storage/docker/postgresql
tar cvf data.tar data
scp data.tar core@192.168.11.2:/tmp/.
```

* 从服务器上操作

```shell
mkdir -p /backup/postgresql
mkdir -p /storage/docker/postgresql
cp /tmp/data.tar /storage/docker/postgresql/.
tar xvf data.tar
cd /storage/docker/postgresql/data
# 删除 pid 文件    
rm -f $PGDATA/postmaster.pid
```

* 编辑从库 `recovery.conf`

```shell
cd /storage/docker/postgresql/data
vi recovery.conf
```

```conf
standby_mode = 'on'
primary_conninfo = 'host=192.168.17.61 port=5436 user=repuser password=repuser keepalives_idle=60'
trigger_file = '/var/lib/postgresql/primary_down'
```

* 启动从库容器

```shell
docker run 
--name pg_slv \
-v /storage/docker/postgresql:/var/lib/postgresql \
-v /backup/postgresql:/backup \
-p 5436:5432 \
-d eg_postgresql
```

* 进入从库容器

```shell
docker exec -it pg_slv /bin/bash
```

* 查看从库状态

```shell
ps -ef|grep post
```