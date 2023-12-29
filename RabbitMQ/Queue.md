# Queue

Queue（队列）是消息的缓冲区，用于存储待处理的消息。它是RabbitMQ中最基本的消息传递模型。Queue具有以下特点：

    队列是消息的容器：队列用于存储待处理的消息，消息按照先进先出（FIFO）的顺序进行处理。
    队列是有界的：队列具有最大容量限制，当队列已满时，新的消息将无法进入队列，直到队列中的消息被消费或被手动删除。
    队列是持久化的：队列中的消息可以被持久化到磁盘上，以防止消息丢失。当RabbitMQ服务器重启时，持久化的消息将被恢复。
    队列是可配置的：队列可以通过设置不同的属性进行配置，例如队列的名称、是否持久化、是否自动删除等。

## 类型
```go
const (
	QueueTypeClassic              = "classic"
	QueueTypeQuorum               = "quorum"
	QueueTypeStream               = "stream"
	QueueOverflowDropHead         = "drop-head"
	QueueOverflowRejectPublish    = "reject-publish"
	QueueOverflowRejectPublishDLX = "reject-publish-dlx"
)
```
RabbitMQ提供了三种类型的队列：

- Classic
- Quorum
- Stream

官方文档 对于 Stream 流队列的描述是：高性能、可持久化、可复制、非破坏性消费、只追加写入的日志

使用场景：

- 一个队列将同一条消息分发给不同消费者
- 可重复消费消息
- 更高的性能
- 存储大量消息而不影响性能
- 更高的吞吐

### 简单一对一

简单队列的生产者和消费者关系一对一

### Work

一个生产者、2个消费者。
但MQ中一个消息只能被一个消费者获取。即消息要么被C1获取，要么被C2获取。这种模式适用于类似集群，能者多劳。性能好的可以安排多消费，性能低的可以安排低消费。

### 订阅模式

1、1个生产者，多个消费者
2、每一个消费者都有自己的一个队列
3、生产者没有将消息直接发送到队列，而是发送到了交换机
4、每个队列都要绑定到交换机
5、生产者发送的消息，经过交换机，到达队列，实现，一个消息被多个消费者获取的目的
这种模式可以满足消费者发布一个消息，多个消费者消费同一信息的需求，但C1、C2消费的都是相同的数据，有时我们需要C1和C2消费的信息只有部分差异，比如我们需求：C1消费增加的数据，C2消费编辑、增加和删除的数据。
这是可以引入第四种模式：

### 路由模式

路由模式是在订阅模式基础上的完善，可以在生产消息的时候，加入Key值，与key值匹配的消费者消费信息。

但路由模式中，就如三中提到的C1、C2、如果C2对应的类型更多，就需要写很多key值。但其实它只与C1有一点差别。那就可以考虑第五种模式：

### 通配符模式

通配符模式是在路由模式的升级，他允许key模糊匹配。*代表一个词，#代表一个或多个词。通过通配符模式我们就可以将C1对应的一个key准确定为item.add。而C2我们就不需要一一写出key值，而是用item.#代替即可。

## 属性

​Type：队列类型

Name：队列名称，就是一个字符串，随便一个字符串就可以；

Durability：声明队列是否持久化，代表队列在服务器重启后是否还存在；

Auto delete： 是否自动删除，如果为true，当没有消费者连接到这个队列的时候，队列会自动删除；

Exclusive：exclusive属性的队列只对首次声明它的连接可见，并且在连接断开时自动删除；基本上不设置它，设置成false ​

Arguments：队列的其他属性，例如指定DLX（死信交换机等）；

### 其他属性

1、x-expires：Number

当Queue（队列）在指定的时间未被访问，则队列将被自动删除；

2、x-message-ttl：Number

发布的消息在队列中存在多长时间后被取消（单位毫秒）；

3、x-overflow：String

设置队列溢出行为，当达到队列的最大长度时，消息会发生什么，有效值为Drop Head或Reject Publish；

4、x-max-length：Number

队列所能容下消息的最大长度，当超出长度后，新消息将会覆盖最前面的消息，类似于Redis的LRU算法；

5、 x-single-active-consumer：默认为false

激活单一的消费者，也就是该队列只能有一个消息者消费消息；

6、x-max-length-bytes：Number

限定队列的最大占用空间，当超出后也使用类似于Redis的LRU算法；

7、x-dead-letter-exchange：String

指定队列关联的死信交换机，有时候我们希望当队列的消息达到上限后溢出的消息不会被删除掉，而是走到另一个队列中保存起来；

8.x-dead-letter-routing-key：String

指定死信交换机的路由键，一般和6一起定义；

9.x-max-priority：Number

如果将一个队列加上优先级参数，那么该队列为优先级队列；

（1）、给队列加上优先级参数使其成为优先级队列 x-max-priority=10【0-255取值范围】 
（2）、给消息加上优先级属性 通过优先级特性，将一个队列实现插队消费；

10、x-queue-mode：String（理解下即可）

队列类型x-queue-mode=lazy懒队列，在磁盘上尽可能多地保留消息以减少RAM使用，如果未设置，则队列将保留内存缓存以尽可能快地传递消息；

11、x-queue-master-locator：String（用的较少，不讲）

在集群模式下设置队列分配到的主节点位置信息；

每个queue都有一个master节点，所有对于queue的操作都是事先在master上完成，之后再slave上进行相同的操作；

每个不同的queue可以坐落在不同的集群节点上，这些queue如果配置了镜像队列，那么会有1个master和多个slave。

基本上所有的操作都落在master上，那么如果这些queues的master都落在个别的服务节点上，而其他的节点又很空闲，这样就无法做到负载均衡，那么势必会影响性能；

关于master queue host 的分配有几种策略，可以在queue声明的时候使用x-queue-master-locator参数，或者在policy上设置queue-master-locator，或者直接在rabbitmq的配置文件中定义queue_master_locator，有三种可供选择的策略：

（1）min-masters：选择master queue数最少的那个服务节点host；
（2）client-local：选择与client相连接的那个服务节点host；
（3）random：随机分配；

