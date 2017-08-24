# Kafka

## 安装ZooKeeper，并设置dataDir

```shell
wget http://mirrors.cnnic.cn/apache/zookeeper/zookeeper-3.4.6/zookeeper-3.4.6.tar.gz
tar zxvf zookeeper-3.4.6.tar.gz
cd zookeeper-3.4.6
cp -rf conf/zoo_sample.cfg conf/zoo.cfg
vim conf/zoo.cfg
```

## 启动ZooKeeper

```shell
sh bin/zkServer.sh start
```

## 安装并运行Kafka

```shell
wget http://apache.fayea.com/kafka/0.8.2.1/kafka_2.10-0.8.2.1.tgz
tar -zxf kafka_2.10-0.8.2.1.tgz
cd kafka_2.10-0.8.2.1
sh bin/kafka-server-start.sh config/server.properties
```

## 创建Topic

```shell
sh bin/kafka-topics --create --topic kafkatopic --replication-factor 1 --partitions 1 --zookeeper localhost:2181
```

## 查看Topic

```shell
sh bin/kafka-topics --list --zookeeper localhost:2181
```

## 启动Producer 生产消息

```shell
sh bin/kafka-console-producer --broker-list localhost:9092 --topic kafkatopic
```

## 启动Consumer 消费消息

```shell
sh bin/kafka-console-consumer --zookeeper localhost:2181 --topic kafkatopic --from-beginning
```

## 删除Topic

```shell
sh bin/kafka-run-class kafka.admin.TopicCommand --delete --topic kafkatopic --zookeeper localhost:2181
```

## 查看Topic 的offset

```shell
sh bin/kafka-consumer-offset-checker  --zookeeper localhost:2181 --topic kafkatopic --group consumer
```

Kafka 的数据在Zookeeper 节点的 /var/local/kafka/data 目录中，以topic 作为子目录名。


## Kafka 专用术语

- Broker
Kafka 集群包含一个或多个服务器，这种服务器被称为 broker。

- Topic
每条发布到 Kafka 集群的消息都有一个类别，这个类别被称为 Topic。（物理上不同 Topic 的消息分开存储，逻辑上一个 Topic 的消息虽然保存于一个或多个 broker 上，但用户只需指定消息的 Topic 即可生产或消费数据而不必关心数据存于何处）。

- Partition
Partition 是物理上的概念，每个 Topic 包含一个或多个 Partition。

- Producer
负责发布消息到 Kafka broker。

- Consumer
消息消费者，向 Kafka broker 读取消息的客户端。

- Consumer Group
每个 Consumer 属于一个特定的 Consumer Group（可为每个 Consumer 指定 group name，若不指定 group name 则属于默认的 group）。

## Kafka 交互流程

Kafka 是一个基于分布式的消息发布-订阅系统，它被设计成快速、可扩展的、持久的。与其他消息发布-订阅系统类似，Kafka 在主题当中保存消息的信息。生产者向主题写入数据，消费者从主题读取数据。由于 Kafka 的特性是支持分布式，同时也是基于分布式的，所以主题也是可以在多个节点上被分区和覆盖的。

信息是一个字节数组，程序员可以在这些字节数组中存储任何对象，支持的数据格式包括 String、JSON、Avro。Kafka 通过给每一个消息绑定一个键值的方式来保证生产者可以把所有的消息发送到指定位置。属于某一个消费者群组的消费者订阅了一个主题，通过该订阅消费者可以跨节点地接收所有与该主题相关的消息，每一个消息只会发送给群组中的一个消费者，所有拥有相同键值的消息都会被确保发给这一个消费者。

Kafka 设计中将每一个主题分区当作一个具有顺序排列的日志。同处于一个分区中的消息都被设置了一个唯一的偏移量。Kafka 只会保持跟踪未读消息，一旦消息被置为已读状态，Kafka 就不会再去管理它了。Kafka 的生产者负责在消息队列中对生产出来的消息保证一定时间的占有，消费者负责追踪每一个主题 (可以理解为一个日志通道) 的消息并及时获取它们。基于这样的设计，Kafka 可以在消息队列中保存大量的开销很小的数据，并且支持大量的消费者订阅。