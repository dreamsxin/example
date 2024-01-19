# ETCD

使⽤Raft协议来维护集群内各个节点状态的⼀致性。简单说，ETCD集群是⼀个分布式系统，由多个节点相互通信构成整体对外服务，每个节点都存储了完整的数据，并且通过Raft协议保证每个节点维护的数据是⼀致的。

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

https://github.com/gohutool/boot4go-etcdv3-browser

https://github.com/cruvie/kk_etcd_ui

https://github.com/cruvie/kk_etcd_go

https://github.com/gtamas/etcdmanager

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

## 备份和恢复
主要用于管理节点的快照，其主要用法如下所示：

`etcdctl snapshot <subcommand> [flags]`
### 生成快照
其主要用法如下所示：

`etcdctl snapshot save <filename> [flags]`
示例如下所示：

`etcdctl snapshot save Surpass.db`
### 查看快照
其主要用法如下所示：

`etcdctl snapshot status <filename> [flags]`
示例如下所示：

`etcdctl snapshot status Surpass.db -w table`

### 恢复快照
其主要用法如下所示：

`etcdctl snapshot restore <filename> [options] [flags]`
示例如下所示：

`etcdctl snapshot restore Surpass.db \
--name=default \
--data-dir=/home/etcd_db \
--initial-advertise-peer-urls=http://localhost:2380`

## 查看告警

如果内部出现问题，会触发告警，可以通过命令查看告警引起原因，命令如下所示：

`etcdctl alarm <subcommand> [flags]`
常用的子命令主要有两个：
```shell
# 查看所有告警
etcdctl alarm list
# 解除所有告警
etcdctl alarm disarm
```

## 用户权限
    etcd默认是没有开启访问控制的，如果开启外网访问etcd的话就需要考虑访问控制的问题，etcd提供了两种访问控制的方式：

### 基于身份验证的访问控制
基于证书的访问控制
    etcd有一个特殊用户root和一个特殊角色root：

root用户：root用户是etcd的超级管理员，拥有etcd的所有权限，在开启角色认证之前为们必须要先建立好root用户
root角色：具有该root角色的用户既具有全局读写访问权限，具有更新集群的身份验证配置的权限。此外，该root角色还授予常规集群维护的特权，包括修改集群成员资格，对存储进行碎片整理以及拍摄快照。
    etcd的权限资源：

Users: user用来设置身份认证(user:passwd)，一个用户可以拥有多个角色，每个角色被分配一定的权限(只读、只写、可读写)，用户分为root用户和非root用户。
Roles: 角色用来关联权限，角色主要三类：
 root角色:默认创建root用户时即创建了root角色，该角色拥有所有权限；
 guest角色:默认自动创建，主要用于非认证使用。普通角色，
 由root用户创建角色，并分配指定权限。
Permissions: 权限分为只读、只写、可读写三种权限，权限即对指定目录或key的读写权限。
如果没有指定任何验证方式，即未显示指定以什么用户进行访问，那么默认会设定为 guest 角色。默认情况下 guest 也是具有全局访问权限的

### 用户管理
其主要用法如下所示：

`etcdctl user <subcommand> [flags]`

其主要子命etcdctl：

子命令	常用用法	功能描述
add	etcdctl user add < user name or user:password > [options] [flags]

### 添加新用户
delete	etcdctl user delete < user name > [flags]	删除用户
list	etcdctl user list [flags]	列出所有用户
get	etcdctl user get < user name > [options] [flags]	获取用户详细信息
passwd	etcdctl user passwd < user name > [options] [flags]	修改密码
grant-role	etcdctl user grant-role < user name > < role name > [flags]	赋予用户角色
revoke-role	etcdctl user revoke-role < user name > < role name > [flags]	删除用户角色

### 角色管理
其主要用法如下所示：

etcdctl role <subcommand> [flags]
    其主要子命令主要如下所示：

子命令	常用用法	功能描述
add	etcdctl role add < role name > [flags]	添加角色
delete	etcdctl role delete <role name > [flags]	删除角色
list	etcdctl role list [flags]	列出所有角色
get	etcdctl role get <role name > [flags]	获取角色详情
grant-permission	etcdctl role grant-permission [options] < role name > < permission type > < key > [endkey] [flags]	把key操作权限授予给一个角色
revoke-permission	etcdctl role revoke-permission < role name > < key > [endkey] [flags]	从角色中撤销key操作权限

### 开启root身份验证
在开启身份验证后，注意事项如下所示：

#### 开启身份验证：
所有etcdctl命令操作都需要指定用户参数--user，参数值为用户名:密码

#### 开启证书验证：
所有etcdctl命令操作都需要添加证书参数--cacert

开启root身份验证的步骤如下所示：
```shell
# 添加root 用户，密码为surpass
etcdctl user add root:surpass
# 开启身份验证，开启为enable，取消为disable
etcdctl auth enable
# 在开启身份验证后，需要带user相关信息
etcdctl get name --user=root:surpass
3.8.4 角色授权
    在开启了root身份验证后，就可以对普通用户和角色操作了。

1.用户增删改查
# 增加普通用户
etcdctl user add surpass:surpass --user=root:surpass
# 获取用户信息
etcdctl user get surpass --user=root:surpass
# 查看所有用户
etcdctl user list --user=root:surpass
# 修改用户密码
etcdctl user passwd surpass --user=root:surpass
# 删除用户
etcdctl user delete surpass --user=root:surpass
2.角色增删改查
# 添加角色
etcdctl role add etcd-test --user=root:surpass
# 获取角色详细信息
etcdctl role get etcd-test --user=root:surpass
# 获取所有角色
etcdctl role list --user=root:surpass
# 删除角色
etcdctl role delete etcd-test --user=root:surpass
3.绑定和授权
# 授权角色只读（read）、只写(write)和读写(readwrite)权限
# 按key进行授权
etcdctl role grant-permission etcd-test readwrite name  --user=root:surpass
# 按key的prefix进行授权
etcdctl role grant-permission etcd-test readwrite name --prefix=true --user=root:surpass
# 将授权绑定给指定用户
etcdctl user grant-role test etcd-test --user=root:surpass
# 撤消角色授权
etcdctl role revoke-permission etcd-test name --user=root:surpass
# 撤消绑定授权
etcdctl user revoke-role test etcd-test --user=root:surpass
```
