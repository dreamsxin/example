ETCD使⽤Raft协议来维护集群内各个节点状态的⼀致性。简单说，ETCD集群是⼀个分布式系统，由多个节点相互通信构成整体对外服务，每个节点都存储了完整的数据，并且通过Raft协议保证每个节点维护的数据是⼀致的。

etcd是一个服务发现系统，具备以下的特点：

简单：安装配置简单，而且提供了Http api进行交互，使用也很简单

安全：支持SSL证书验证

快速：根据官方提供的benchmark数据，单实例支持每秒2k+读操作

可靠：采用raft算法，实现分布式系统数据的可用性和一致性

ETCD提供HTTP协议，在最新版本中支持Google grpc方式访问。具体支持接口情况如下：

ETCD是一个高可靠的KV存储系统，支持PUT/GET/DELETE接口；

为了支持服务注册与发现，支持WATCH接口（通过http long poll实现）；

- 支持KEY持有TTL属性；

- CAS（compare and swap)操作;

- 支持多key的事务操作；

- 支持目录操作

## 管理命令

### 添加成员
主要用法如下所示：

`etcdctl member add <memberName> [options] [flags]`

示例如下所示：
```shell
# 示例：将目标节点etcd4添加到集群
etcdctl member add etcd4 http://192.168.3.104:2380
# 启动目标集群时需要设置启动参数如下
etcd --name=etcd4 --data-dir=/etcd-data \
  --listen-peer-urls=http://192.168.3.104:2380 \
  --listen-client-urls=http://192.168.3.104:2379 \
  --initial-advertise-peer-urls=http://192.168.3.104:2380 \
  --advertise-client-urls=http://192.168.3.104:2379 \
  --initial-cluster=etcd1=http://192.168.3.101:2380,etcd2=http://192.168.3.102:2380,etcd3=http://192.168.3.103:2380,etcd4=http://192.168.3.104:2380 \
  --initial-cluster-state=existing
### 更新成员

主要用法如下所示：

`etcdctl member update <memberID> [options] [flags]`
示例如下所示：
```shell
etcdctl member update ade526d28b1f92f7 --peer-urls=http://192.168.3.111:2380
```
### 删除成员
主要用法如下所示：

`etcdctl member remove <memberID> [flags]`
示例如下所示：
```shell
etcdctl member remove ade526d28b1f92f7
```

