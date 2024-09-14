# Dgraph

一个开源的、事务性的、分布式的、原生的图形数据库。

https://github.com/dgraph-io/dgraph

## 运行 Dgraph
运行 docker 镜像是开始使用 dgraph 的最快方法。这个独立映像仅用于快速启动。

现在，只需运行下面的命令，Dgraph就已经启动并运行了。
```shell
docker run --rm -it -p 8000:8000 -p 8080:8080 -p 9080:9080 dgraph/standalone:v24.0.2
```
