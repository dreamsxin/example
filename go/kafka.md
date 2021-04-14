# 使用 Kafka

Kafka 是一个由 LinkedIn 开发的分布式消息系统，于2011年年初开源。Kafka 使用 Scala 实现，它是基于消息发布-订阅模式实现的消息系统，其主要设计目标：
- 消息持久化
- 高吞吐
- 分布式
- 跨平台
- 实时性
- 伸缩性

## 相关术语

- Broker

Kafka 集群包含一个或多个服务器，这种服务器被称作 Broker。

- Topic

逻辑上同 RabbitMQ 的 Queue 队列相似，每条发布到Kafka集群的消息都必须有一个 Topic （类别）。

- Partition

Partition 是物理概念上的分区，为了提高系统吞吐量会为每个 Topic 分配一个或多个 Partition。

- Producer

消息生产者，负责生产消息并发送到 Broker。

- Consumer

消息消费者，向 Broker 读取消息并处理的客户端。

- Consumer Group

每个 Consumer 都属于一个特定的组，可为每个Consumer指定组名，若不指定则属于默认组，组可以用来实现例如一条消息被组内多个成员消费等功能。

- Replica

主题副本数，不得超过 Broker 数量。

## sarama

https://github.com/Shopify/sarama

* 创建 Consumer 时将 offset 设置为最新
```go
config := cluster.NewConfig()
config.Version = sarama.V1_1_0_0
config.Consumer.Offsets.Initial = sarama.OffsetNewest
config.Consumer.Return.Errors = true
config.Group.Return.Notifications = true

// init consumer
consumer, err := cluster.NewConsumer(brokerAddrs, groupID, topics, config)
```


## confluent-kafka-go

https://github.com/confluentinc/confluent-kafka-go
