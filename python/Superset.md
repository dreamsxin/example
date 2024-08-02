# Superset

https://github.com/apache/incubator-superset

## 什么是Superset

Superset的中文翻译是快船，而Superset其实是一个自助式数据分析工具，它的主要目标是简化我们的数据探索分析操作，它的强大之处在于整个过程一气呵成，几乎不用片刻的等待。

## Superset 的特性

Superset通过让用户创建并且分享仪表盘的方式为数据分析人员提供一个快速的数据可视化功能。
在你用这种丰富的数据可视化方案来分析你的数据的同时，Superset还可以兼顾数据格式的拓展性、数据模型的高粒度保证、快速的复杂规则查询、兼容主流鉴权模式（数据库、OpenID、LDAP、OAuth或者基于Flask AppBuilder的REMOTE_USER）
通过一个定义字段、下拉聚合规则的简单的语法层操作就让我们可以将数据源在U上丰富地呈现。Superset还深度整合了Druid以保证我们在操作超大、实时数据的分片和切分都能行云流水。

## 数据库支持

Superset 是基于 Druid.io （https://github.com/druid-io/druid/） 设计的，但是又支持横向到像 SQLAlchemy 这样的常见Python ORM框架上面。

## 那Druid又是什么呢？

Druid 是一个基于分布式的快速列式存储，也是一个为BI设计的开源数据存储查询工具。Druid提供了一种实时数据低延迟的插入、灵活的数据探索和快速数据聚合。现有的Druid已经可以支持扩展到TB级别的事件和PB级的数据了，Druid是BI应用的最佳搭档。

想必，你已经受够了Hive那个龟速查询，迫不及待想体验一下这种酣畅淋漓的快感了吧！

## 实战

主流可视化工具：Superset、DataEase、MetaBase

## 部署

https://superset.apache.org/docs/quickstart

```shell
git clone https://github.com/apache/superset
# Enter the repository you just cloned
$ cd superset

# Fire up Superset using Docker Compose
$ docker compose -f docker-compose-image-tag.yml up
```

### Log into Superset
Now head over to http://localhost:8088 and log in with the default created account:

- username: admin
- password: admin

### 本地跑Docker

下载镜像：

`docker pull index.tenxcloud.com/7harryprince/Superset`

生成新的key
```shell
python -c "import random; print(random.getrandbits(160))"
```

跑容器

`docker run -e "SQLALCHEMY_DATABASE_URI = 'sqlite:////home/superset/.superset/superset.db' -e "SUPERSET_SECRET_KEY=你生成的随机数" -d -p 8087:8088 -v /opt/docker/superset:/home/superset amancevice/superset`

`docker run -e "SUPERSET_SECRET_KEY=你生成的随机数" -p 8088:8088 -d index.tenxcloud.com/7harryprince/Superset`

查询一下你的docekr ip
```text
docker-machine ls
NAME      ACTIVE   DRIVER       STATE     URL                         SWARM   DOCKER    ERRORS
default   -        virtualbox   Running   tcp://192.168.99.100:2376           v1.9.1    
dev       -        virtualbox   Saved                                         Unknown   
```

这里可以看到我的默认Docker的IP是`192.168.99.100`，所以需要在浏览器中访问 `http://192.168.99.100:8088`。

这样我们又是三行代码搞定了一个大数据分析神器。

需要注意到，这个 Superset 容器里的默认鉴权配置是：
```text
superset  fab create-admin
username: admin
password: Superset_admin
```

## docker compose
执行
```shell
docker compose -f docker-compose-non-dev.yml pull
```
这个过程会需要几分钟

上一步完成后，执行
```shell
docker compose -f docker-compose-non-dev.yml up
```
等到控制台输出变慢后 就完成了。

此时打开浏览器 输入：http://IP:8088/登录即可。
默认用户名密码是admin

## 后端

整个项目的后端是基于Python的，用到了Flask、Pandas、SqlAlchemy。

- Flask AppBuilder(鉴权、CRUD、规则）
- Pandas（分析）
- SqlAlchemy（数据库ORM）

此外，也关注到Superset的缓存机制值得我们学习：

- 采用memcache和Redis作为缓存
- 级联超时配置
- UI具有时效性控制
- 允许强制刷新

## 前端

自然前端是JS的天下，用到了npm、react、webpack,这意味着你可以在手机也可以流畅使用。

- d3 (数据可视化)
- nvd3.org(可重用图表)

## 局限性

Superset的可视化，目前只支持每次可视化一张表，对于多表join的情况还无能为力。
依赖于数据库的快速响应，如果数据库本身太慢Superset也没什么办法。
语义层的封装还需要完善，因为 druid 原生只支持部分sql。
