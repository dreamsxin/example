#

- https://github.com/HDT3213/godis
- https://github.com/nalgeon/redka
- https://github.com/oliver006/redis_exporter
- https://github.com/tair-opensource/RedisShake
- https://github.com/alicebob/miniredis
- https://github.com/redis-windows/redis-windows/releases
用于测试
- https://github.com/yuyan-sec/RedisEXP

## 认证

```shell
redis-cli
auth mypassword
```

## 查看内存

```shell
redis-cli
info memory
```

## 查看指定key过期时间

```shell
ttl keyname
```

## Redis数据回写机制

数据回写分同步和异步两种方式

- 同步回写(SAVE), 主进程直接向磁盘回写数据. 在数据量大的情况下会导致系统假死很长时间
- 异步回写(BGSAVE), 主进程fork后, 复制自身并通过这个新的进程回写磁盘, 回写结束后新进程自行关闭.

由于 BGSAVE 不需要主进程阻塞, 系统也不会假死, 一般会采用 BGSAVE 来实现数据回写.

## 清除redis的特定前缀的key

* 批量删除Key

Redis 中有删除单个 Key 的指令 DEL，但好像没有批量删除 Key 的指令，不过我们可以借助 Linux 的 xargs 指令来完成这个动作：
```shell
redis-cli keys "*"| xargs redis-cli del
```
如果要指定 Redis 数据库访问密码，使用下面的命令：
```shell
redis-cli -a password keys "*"| xargs redis-cli -a password del
```
如果要访问 Redis 中特定的数据库，使用下面的命令：
```shell
redis-cli -n 0 keys "*"| xargs redis-cli -n 0 del
```

>> keys 指令可以进行模糊匹配，但如果 Key 含空格，就匹配不到了，暂时还没发现好的解决办法。

* 删除所有Key

删除所有Key，可以使用Redis的flushdb和flushall命令：
```shell
// 删除当前数据库中的所有Key
flushdb
// 删除所有数据库中的key
flushall
```

## 错误 error

### Can't save in background: fork: Cannot allocate memory

* 故障分析

    在小内存的进程上做fork, 不需要太多资源. 但当这个进程的内存空间以G为单位时, fork就成为一件很恐怖的操作.
    在16G内存的足迹上fork 14G的进程, 系统肯定Cannot allocate memory.
    主机的Redis 改动的越频繁 fork进程也越频繁, 所以一直在Cannot allocate memory

* 解决方案

直接修改内核参数 vm.overcommit_memory = 1, Linux内核会根据参数vm.overcommit_memory参数的设置决定是否放行。

- vm.overcommit_memory = 1，直接放行
- vm.overcommit_memory = 0：则比较 此次请求分配的虚拟内存大小和系统当前空闲的物理内存加上swap，决定是否放行。
- vm.overcommit_memory = 2：则会比较进程所有已分配的虚拟内存加上此次请求分配的虚拟内存和系统当前的空闲物理内存加上swap，决定是否放行。

```shell
sudo echo 'vm.overcommit_memory = 1' >> /etc/sysctl.conf
sudo sysctl -p
```
