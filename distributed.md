# 分布式系统

## Zookeeper 分布式服务框架
## etcd
## Consul
## Gearman 分布式任务处理系统
## Celery 分布式任务队列
## NSQ 分布式的实时消息平台
## Apache Kafka
## Disque
## RabbitMQ
## Apache ActiveMQ
## ZeroMQ
## Disque 分布式消息中间件
## Beanstalkd 分布式消息队列
## nanomsg
## Darner

## Druid
## PipelineDB

## Apache Storm 流式处理框架
## Apache Spark

## Apache Hadoop
## Apache Hive
## Apache HBase

## Disco - Massive data, Minimal code

## Apache Flume

## Zabbix 开源分布式监控解决方案

## ElasticSearch 分布式全文搜索引擎

## GFS、HDFS、Lustre 、Ceph 、GridFS 、MogileFS、TFS、FastDFS、MooseFS、GlusterFS

# 一致性算法 Paxos & raft & zab

## Raxos 算法（少数服从多数）

解决的问题：在一个可能发生异常的分布式系统中如何就某个值达成一致，让整个集群的节点对某个值的变更达成一致
任何一个节点都可以提出要修改某个数据的提案,是否通过这个提案取决于这个集群中是否有超过半数的节点同意（所以节点数总是单数）—— 版本标记。虽然一致性，但是只能对一个操作进行操作啊？？
当一个Server接收到比当前版本号小的提案时，则拒绝。当收到比当前大的版本号的提案时，则锁定资源，进行修改，返回OK.   也就是说收到超过一半的最大版本的提案才算成功。

* 核心思想：

在抢占式访问权的基础上引入多个acceptor，也就是说当一个版本号更大的提案可以剥夺版本号已经获取的锁。
后者认同前者的原则：

- 在肯定旧epoch 无法生成确定性取值时，新的 epoch 会提交自己的valu
- 一旦 旧epoch形成确定性取值，新的 epoch肯定可以获取到此取值，并且会认同此取值，不会被破坏。

## ZAB 协议 ( Zookeeper Atomic  Broadcast)

原子广播协议：保证了发给各副本的消息顺序相同

ZAB 是在 Paxos 算法基础上进行扩展而来的。Zookeeper 使用单一主进程 Leader用于处理客户端所有事务请求，采用 ZAB 协议将服务器状态以事务形式广播到所有 Follower 上，由于事务间可能存在着依赖关系，ZAB协议保证 Leader 广播的变更序列被顺序的处理，一个状态被处理那么它所依赖的状态也已经提前被处理

核心思想：保证任意时刻只有一个节点是Leader，所有更新事务由Leader发起去更新所有副本 Follower，更新时用的是两段提交协议，只要多数节点 prepare 成功，就通知他们commit。各个follower 要按当初 leader 让他们 prepare 的顺序来 apply 事务。

## Raft 算法

演示：http://thesecretlivesofdata.com/raft/

Raft 算法也是一种少数服从多数的算法，在任何时候一个服务器可以扮演以下角色之一：

- Leader：负责 Client 交互 和 log 复制，同一时刻系统中最多存在一个
- Follower：被动响应请求 RPC，从不主动发起请求 RPC
- Candidate : 由Follower 向Leader转换的中间状态

在选举Leader的过程中，是有时间限制的，raft 将时间分为一个个 Term，可以认为是“逻辑时间”：

- 每个 Term中至多存在1个 Leader
- 某些 Term由于不止一个得到的票数一样，就会选举失败，不存在Leader。则会出现 Split Vote  ，再由候选者发出邀票
- 每个 Server 本地维护 currentTerm

