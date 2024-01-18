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

## 数据库操作命令
## put
添加一个键值，基本用法如下所示：

`etcdctl put [options] <key> <value> [flags]`

常用参数如下所示：
- --prev-kv	输出修改前的键值

注意事项如下所示：

其中value接受从stdin的输入内容
如果value是以横线-开始，将会被视为flag，如果不希望出现这种情况，可以使用两个横线代替--
若键已经存在，则进行更新并覆盖原有值，若不存在，则进行添加

### get
查询键值，基本用法如下所示：

`etcdctl get [options] <key> [range_end] [flags]`

常用参数如下所示：

参数	功能描述
- --hex	以十六进制形式输出
- --limit number	设置输出结果的最大值
- --prefix	根据prefix进行匹配key
- --order	对输出结果进行排序，ASCEND 或 DESCEND
- --sort-by	按给定字段排序，CREATE, KEY, MODIFY, VALUE, VERSION
- --print-value-only	仅输出value值
- --from-key	按byte进行比较，获取大于等于指定key的结果
- --keys-only	仅获取keys

### del
删除键值，基本用法如下所示：

`etcdctl del [options] <key> [range_end] [flags]`
常用参数如下所示：

参数	功能描述
- --prefix	根据prefix进行匹配删除
- --prev-kv	输出删除的键值
- --from-key	按byte进行比较，删除大于等于指定key的结果

## 租约相关命令

租约具有生命周期，需要为租约授予一个TTL(time to live)，将租约绑定到一个key上，则key的生命周期与租约一致，可续租，可撤销租约。其主要用法如下所示：

`etcdctl lease <subcommand> [flags]`

### 添加租约
主要用法如下所示：

`etcdctl lease grant <ttl> [flags]`
示例如下所示：

`etcdctl lease grant 600`

### 列出租约
主要用法如下所示：

`etcdctl lease list [flags]`
示例如下所示：

`etcdctl lease list`
### 删除租约
主要用法如下所示：

`etcdctl lease revoke <leaseID> [flags]`
示例如下所示：

`etcdctl lease revoke 694d81417acd4754`
### 查看租约详情
主要用法如下所示：

`etcdctl lease timetolive <leaseID> [options] [flags]`
示例如下所示：

`etcdctl lease timetolive 694d81417acd4759`
### 租约续约
主要用法如下所示：

`etcdctl lease keep-alive [options] <leaseID> [flags]`
示例如下所示：

`etcdctl lease keep-alive 694d81417acd4757`
