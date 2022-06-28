# CockroachDB

- https://www.cockroachlabs.com/docs/stable/secure-a-cluster.html
- https://www.cockroachlabs.com/docs/stable/connection-pooling.html?filters=go
- https://www.cockroachlabs.com/docs/stable/cockroach-node.html

## 证书

# 主节点证书
```shell
mkdir /data/certs
mkdir /data/my-safe-directory
cockroach cert create-ca --certs-dir=/data/certs --ca-key=/data/my-safe-directory/ca.key

#生成客户端证书
cockroach cert create-client root --certs-dir=/data/certs --ca-key=/data/my-safe-directory/ca.key

#主节点node1证书
cp -rf /data/certs/* /data/certs01/
cockroach cert create-node node1ip localhost 127.0.0.1 --certs-dir=/data/certs01 --ca-key=/data/my-safe-directory/ca.key

#创建node2证书，赋值到node2
cp -rf /data/certs/* /data/certs02/
cockroach cert create-node node2ip localhost 127.0.0.1 --certs-dir=/data/certs02 --ca-key=/data/my-safe-directory/ca.key --overwrite

sudo scp -i poker.pem -r /data/certs02 ubuntu@172.31.41.182:/data


#创建node3证书，赋值到node3
cp -rf /data/certs/* /data/certs03/
cockroach cert create-node node3ip localhost 127.0.0.1 --certs-dir=/data/certs03 --ca-key=/data/my-safe-directory/ca.key --overwrite


#启动主节点
sudo cockroach start --certs-dir=/data/certs01 --advertise-addr=node1ip --http-addr=node1ip:8080 --join=node1ip:26257,node2ip:26257,node3ip:26257 --cache=.25 --max-sql-memory=.25

#节点都启动好后初始化
cockroach init --certs-dir=/data/certs01 --host=node1ip
sudo cockroach node status --certs-dir=/data/certs01 --host=node1ip
```

## 登录web后台

* 创建web登录用户
```shell
cockroach sql --certs-dir=/data/certs01 --host=node1ip
> CREATE USER root WITH PASSWORD '123456';
> GRANT admin TO dreamsxin;
> SHOW USERS;
> SHOW DATABASES;
```

## 备份恢复

```sql
BACKUP INTO 'http://localhost/db_backup';
RESTORE FROM LATEST IN 'http://localhost/db_backup';
```
* 配置 WebDAV
```nginx
server {
    listen 80;
    client_max_body_size 100m;
    location / {
      dav_methods PUT DELETE MKCOL COPY MOVE;
      sendfile           on;
      create_full_put_path    on;
      client_max_body_size    0;
    }
}
```

## 图形客户端

https://github.com/beekeeper-studio/beekeeper-studio
