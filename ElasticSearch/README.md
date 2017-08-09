## 系统要求

如果仅作测试用, 不需要两天机器, 可以将两个节点部署在同一台机器上, 对磁盘/cpu要求不高, 内存大于2g基本足够了

如果是正式环境, 需要根据日志量进行评估, 例如, 每天日志量占硬盘约约10G, 且保留30天日志, 则磁盘会占用约300g, es设定的阈值是磁盘空间占满85%则日志开始告警. 所以, 需要至少 300/0.85=354g.

准备两台机器, 在同一个局域网内(可ping通), 分别在每台机器上部署相应es节点, 搭建一套日志集群.

两台机器, 最少的资源了, 但是没法做到高可用, 所以, 还需要再加一台机器, 防止脑裂, 具体见最后(两台主力机器+一台稳定的机器就行)

- 集群节点: 最少两台机器
- 内存: 16G及以上
- cpu: 4核及以上
- 硬盘: 800G及以上, 建议1T, 集群容量约10亿级(取决于对应日志大小)
- 操作系统: centos

这里假设, 两台机器ip分别为

- 第一台机器: 10.0.0.1
- 第二台机器: 10.0.0.2

## 部署

1. 确认JDK版本及安装

es依赖java的版本最小为1.7
```shell
java -version
```

安装及升级java

```shell
# Redhat/Centos/Fedora
sudo yum install java-1.7.0-openjdk
```

2. 下载es

命令行中的下载命令:
```shell
curl -L -O https://download.elastic.co/elasticsearch/release/org/elasticsearch/distribution/tar/elasticsearch/2.3.3/elasticsearch-2.3.3.tar.gz
```

解压:
```shell
tar -xzf elasticsearch-2.3.3.tar.gz
```

3. 用户/目录/权限设置

新建用户, 假设为es
```shell
sudo useradd es
```
新建目录, 假设/data/目录挂载的硬盘最大(500G以上)
```shell
mkdir -p /data/LogTool
mkdir -p /data/LogData
```
将解压后的目录移动至新建的目录/data/LogTool下, 并改名为elasticsearch
```shell
mv elasticsearch-2.3.3 /data/LogTool/elasticsearch
```
将目录所有者修改为es
```shell
chown -R es:es /data/LogTool
chown -R es:es /data/LogData
```

5. 切换用户

切换到es用户, 并进入elasticsearch目录
```shell
su es
cd /data/LogTool/elasticsearch/
```
以用户es的身份进行后续操作

6. 修改配置文件

以用户es的身份进行操作

文件路径: `config/elasticsearch.yml`

修改该文件中配置项: (注意, 原始文件中都是被#号注释掉了, 需要去掉对应注释并修改配置值)

* 集群名: cluster.name, 注意: 两台机器配置一致

> cluster.name: inner_es_cluster

* 节点名: node.name, 注意: 两台机器配置不同, 一台为01, 另一台为02

第一台机器

> node.name: inner_es_node_01

第二台机器

> node.name: inner_es_node_02

* 数据路径: path.data, 为新建立的目录

> path.data: /data/LogData/

* 日志路径: path.logs

> path.logs: /data/LogData/logs

* LockMemory:

* bootstrap.mlockall: true

* 本机ip: network.host, 注意两台机器配置不同, 分贝配置为对应机器的内网ip

第一台机器
> network.host: 10.0.0.1

第二台机器
> network.host: 10.0.0.2

* Discovery配置: 注意这里是两台机器内网ip+9300端口, 注意这里minimum_master_nodes=2, 见最后一点防脑裂说明

> discovery.zen.ping.unicast.hosts: ["10.0.0.1:9300", "10.0.0.2:9300"]
> discovery.zen.minimum_master_nodes: 2

* gatewary配置:

> gateway.recover_after_nodes: 2
> gateway.recover_after_time: 5m
> gateway.expected_nodes: 1

新增其他配置到文件末尾, 根据需求加, 这里用到了script, 同时增大了recovery的配置(要大些保证recovery速度, 但是又不能太大, 会将带宽占满)

> script.engine.groovy.inline.search: on
> script.engine.groovy.inline.aggs: on
> indices.recovery.max_bytes_per_sec: 100mb
> indices.recovery.concurrent_streams: 10

7. 设置es占用内存

修改文件`bin/elasticsearch.in.sh`, 将文件如下变量变更为4g(根据自身机器配置, 配置的内存最大不超过机器物理内存的75%. 两个变量值相等, 以获取最大的性能). 当然, 实际使用中4g可能远远不够, 这个值仅是个示例

> ES_MIN_MEM=4g
> ES_MAX_MEM=4g

修改centos配置: /etc/security/limits.conf, 以便启用memlock, 提升性能

加入, 注意, 示例中用户为es

> es soft memlock unlimited
> es hard memlock unlimited

确认max descriptiors

查看系统数量

如果结果是unlimited, 则无需任何处理, 直接进入下一步
```shell
ulimit -n
> unlimited
```

如果结果是一个整数, 且小于204800
```shell
ulimit -n
> 4096
```

此时, 需要编辑`/etc/security/limits.conf`, 加入
```conf
es soft nofile 204800
es hard nofile 204800
```
另一种方法, 修改bin/elasticsearch, 在文件的前半部分加入下面这行代码, 保证在启动前执行即可.
```shell
ulimit -n 204800
```

8. 启动测试

以用户es的身份进行操作

在命令行中执行启动命令
```shell
cd /data/elasticsearch/
./bin/elasticsearch
```
确认集群是否启动成功
```shell
curl http://10.0.0.1:9200/
```
输出
```json
{
  "name" : "inner_es_node_01",
  "cluster_name" : "inner_es_cluster",
  "version" : {
    "number" : "2.3.3",
    "build_hash" : "218bdf10790eef486ff2c41a3df5cfa32dadcfde",
    "build_timestamp" : "2016-05-17T15:40:04Z",
    "build_snapshot" : false,
    "lucene_version" : "5.5.0"
  },
  "tagline" : "You Know, for Search"
}
```
启动第二个节点时日志

9. 正式启动

ctrl+c 关掉原先的进程

使用命令, 以daemon形式启动, 进程pid写入es.pid, 可以用于重启等
```shell
bin/elasticsearch -d -p es.pid
```

使用supervisord管理进程, 以下为supervisord.conf示例
```conf
[program:es]
directory=/data/LogTool/elasticsearch
command=/data/LogTool/elasticsearch/bin/elasticsearch
autostart=true
autorestart=true
stdout_logfile=/data/LogTool/elasticsearch/log/supervisord_es_out.log
stderr_logfile=/data/LogTool/elasticsearch/log/supervisord_es_err.log
```