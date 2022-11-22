# Kafka

在高并发的应用场景中，由于来不及同步处理请求，接收到的请求往往会发生阻塞。例如，大量的插入、更新请求同时到达数据库，这会导致行或表被锁住，最后会因为请求堆积过多而触发“连接数过多的异常”（Too Many Connections）错误。

因此，在高并发的应用场景中需要一个缓冲机制，而消息队列则可以很好地充当这样一个角色。消息队列通过异步处理请求来缓解系统的压力。

“消息队列”（Message Queue,MQ）从字面来理解，是一个队列，拥有先进先出（First Input First Output,FIFO）的特性。它主要用于不同进程或线程之间的通信，用来处理一系列的输入请求。

消息队列采用异步通信机制。即，消息的发送者和接收者无须同时与消息队列进行数据交互，消息会一直保存在队列中，直至被接收者读取。每一条消息记录都包含详细的数据说明，包括数据产生的时间、数据类型、特定的输入参数。

在实际的应用中，消息队列主要有以下作用。

- 应用解耦：多个应用可通过消息队列对相同的消息进行处理，应用之间相互独立，互不影响；
- 异步处理：相比于串行和并行处理，异步处理可以减少处理的时间；
- 数据限流：流量高峰期，可通过消息队列来控制流量，避免流量过大而引起应用系统崩溃；
- 消息通信：实现点对点消息队列或聊天室等。

## docker
```shell
docker pull wurstmeister/kafka
```
git
```shell
git clone https://github.com/wurstmeister/kafka-docker
cd kafka-docker
```
Start a cluster:
```shell
docker-compose up -d
```
Add more brokers:
```shell
docker-compose scale kafka=3
```
Destroy a cluster:
```shell
docker-compose stop
```

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


## Kafka 设计目标

以时间复杂度为O(1)的方式提供消息持久化能力，即使对TB级以上数据也能保证常数时间的访问性能高吞吐率。即使在非常廉价的商用机器上也能做到单机支持每秒100K条消息的传输支持Kafka Server间的消息分区，及分布式消费，同时保证每个partition内的消息顺序传输同时支持离线数据处理和实时数据处理

## 为什么要用消息系统

### 解耦

在项目启动之初来预测将来项目会碰到什么需求，是极其困难的。消息队列在处理过程中间插入了一个隐含的、基于数据的接口层，两边的处理过程都要实现这一接口。这允许你独立的扩展或修改两边的处理过程，只要确保它们遵守同样的接口约束

### 冗余

有些情况下，处理数据的过程会失败。除非数据被持久化，否则将造成丢失。消息队列把数据进行持久化直到它们已经被完全处理，通过这一方式规避了数据丢失风险。在被许多消息队列所采用的”插入-获取-删除”范式中，在把一个消息从队列中删除之前，需要你的处理过程明确的指出该消息已经被处理完毕，确保你的数据被安全的保存直到你使用完毕。

### 扩展性

因为消息队列解耦了你的处理过程，所以增大消息入队和处理的频率是很容易的；只要另外增加处理过程即可。不需要改变代码、不需要调节参数。扩展就像调大电力按钮一样简单。

### 灵活性 & 峰值处理能力

在访问量剧增的情况下，应用仍然需要继续发挥作用，但是这样的突发流量并不常见；如果为以能处理这类峰值访问为标准来投入资源随时待命无疑是巨大的浪费。使用消息队列能够使关键组件顶住突发的访问压力，而不会因为突发的超负荷的请求而完全崩溃。

### 可恢复性

当体系的一部分组件失效，不会影响到整个系统。消息队列降低了进程间的耦合度，所以即使一个处理消息的进程挂掉，加入队列中的消息仍然可以在系统恢复后被处理。而这种允许重试或者延后处理请求的能力通常是造就一个略感不便的用户和一个沮丧透顶的用户之间的区别。

### 送达保证

消息队列提供的冗余机制保证了消息能被实际的处理，只要一个进程读取了该队列即可。在此基础上，IronMQ提供了一个”只送达一次”保证。无论有多少进程在从队列中领取数据，每一个消息只能被处理一次。这之所以成为可能，是因为获取一个消息只是”预定”了这个消息，暂时把它移出了队列。除非客户端明确的表示已经处理完了这个消息，否则这个消息会被放回队列中去，在一段可配置的时间之后可再次被处理。

### 顺序保证

在大多使用场景下，数据处理的顺序都很重要。消息队列本来就是排序的，并且能保证数据会按照特定的顺序来处理。IronMO保证消息通过FIFO（先进先出）的顺序来处理，因此消息在队列中的位置就是从队列中检索他们的位置。

### 缓冲

在任何重要的系统中，都会有需要不同的处理时间的元素。例如,加载一张图片比应用过滤器花费更少的时间。消息队列通过一个缓冲层来帮助任务最高效率的执行–写入队列的处理会尽可能的快速，而不受从队列读的预备处理的约束。该缓冲有助于控制和优化数据流经过系统的速度。

### 理解数据流

在一个分布式系统里，要得到一个关于用户操作会用多长时间及其原因的总体印象，是个巨大的挑战。消息队列通过消息被处理的频率，来方便的辅助确定那些表现不佳的处理过程或领域，这些地方的数据流都不够优化。

### 异步通信

很多时候，你不想也不需要立即处理消息。消息队列提供了异步处理机制，允许你把一个消息放入队列，但并不立即处理它。你想向队列中放入多少消息就放多少，然后在你乐意的时候再去处理它们。

## 常用Message Queue对比

### RabbitMQ

RabbitMQ是使用Erlang编写的一个开源的消息队列，本身支持很多的协议：AMQP，XMPP, SMTP, STOMP，也正因如此，它非常重量级，更适合于企业级的开发。同时实现了Broker构架，这意味着消息在发送给客户端时先在中心队列排队。对路由，负载均衡或者数据持久化都有很好的支持。

### Redis

Redis是一个基于Key-Value对的NoSQL数据库，开发维护很活跃。虽然它是一个Key-Value数据库存储系统，但它本身支持MQ功能，所以完全可以当做一个轻量级的队列服务来使用。对于RabbitMQ和Redis的入队和出队操作，各执行100万次，每10万次记录一次执行时间。测试数据分为128Bytes、512Bytes、1K和10K四个不同大小的数据。实验表明：入队时，当数据比较小时Redis的性能要高于RabbitMQ，而如果数据大小超过了10K，Redis则慢的无法忍受；出队时，无论数据大小，Redis都表现出非常好的性能，而RabbitMQ的出队性能则远低于Redis。

### ZeroMQ

ZeroMQ号称最快的消息队列系统，尤其针对大吞吐量的需求场景。ZMQ能够实现RabbitMQ不擅长的高级/复杂的队列，但是开发人员需要自己组合多种技术框架，技术上的复杂度是对这MQ能够应用成功的挑战。ZeroMQ具有一个独特的非中间件的模式，你不需要安装和运行一个消息服务器或中间件，因为你的应用程序将扮演了这个服务角色。你只需要简单的引用ZeroMQ程序库，可以使用NuGet安装，然后你就可以愉快的在应用程序之间发送消息了。但是ZeroMQ仅提供非持久性的队列，也就是说如果宕机，数据将会丢失。其中，Twitter的Storm 0.9.0以前的版本中默认使用ZeroMQ作为数据流的传输（Storm从0.9版本开始同时支持ZeroMQ和Netty作为传输模块）。

### ActiveMQ

ActiveMQ是Apache下的一个子项目。 类似于ZeroMQ，它能够以代理人和点对点的技术实现队列。同时类似于RabbitMQ，它少量代码就可以高效地实现高级应用场景。

### Kafka/Jafka

Kafka是Apache下的一个子项目，是一个高性能跨语言分布式发布/订阅消息队列系统，而Jafka是在Kafka之上孵化而来的，即Kafka的一个升级版。具有以下特性：快速持久化，可以在O(1)的系统开销下进行消息持久化；高吞吐，在一台普通的服务器上既可以达到10W/s的吞吐速率；完全的分布式系统，Broker、Producer、Consumer都原生自动支持分布式，自动实现负载均衡；支持Hadoop数据并行加载，对于像Hadoop的一样的日志数据和离线分析系统，但又要求实时处理的限制，这是一个可行的解决方案。Kafka通过Hadoop的并行加载机制来统一了在线和离线的消息处理。Apache Kafka相对于ActiveMQ是一个非常轻量级的消息系统，除了性能非常好之外，还是一个工作良好的分布式系统。

## Kafka 解析

专业术语 Terminology

### Broker

Kafka集群包含一个或多个服务器，这种服务器被称为broker

### Topic

每条发布到Kafka集群的消息都有一个类别，这个类别被称为topic。（物理上不同topic的消息分开存储，逻辑上一个topic的消息虽然保存于一个或多个broker上但用户只需指定消息的topic即可生产或消费数据而不必关心数据存于何处）

### Partition

parition是物理上的概念，每个topic包含一个或多个partition，创建topic时可指定parition数量。每个partition对应于一个文件夹，该文件夹下存储该partition的数据和索引文件

### Producer

负责发布消息到Kafka broker

### Consumer

消费消息。每个consumer属于一个特定的consumer group（可为每个consumer指定group name，若不指定group name则属于默认的group）。使用consumer high level API时，同一topic的一条消息只能被同一个consumer group内的一个consumer消费，但多个consumer group可同时消费这一消息。
