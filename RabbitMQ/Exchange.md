# Exchange
在RabbitMQ中，生产者发送消息不会直接将消息投递到队列中，而是先将消息投递到交换机中，在由交换机转发到具体的队列，队列再将消息以推送或者拉取方式给消费者进行消费。

## 类型

```go
	ExchangeDirect  = "direct"
	ExchangeFanout  = "fanout"
	ExchangeTopic   = "topic"
	ExchangeHeaders = "headers"
```

### 直连交换机
直连交换机是一种带路由功能的交换机，一个队列会和一个交换机绑定，除此之外再绑定一个routing_key，当消息被发送的时候，需要指定一个binding_key，这个消息被送达交换机的时候，就会被这个交换机送到指定的队列里面去。同样的一个binding_key也是支持应用到多个队列中的。

### 主题交换机
主题交换机的routing_key需要有一定的规则，交换机和队列的binding_key需要采用*.#.*.....的格式，每个部分用.分开，其中
     *表示一个单词 
     #表示任意数量（零个或多个）单词。

     示例：
     队列Q1绑定键为 *.TT.*
     队列Q2绑定键为TT.#

     如果一条消息携带的路由键为 A.TT.B，那么队列Q1将会收到 
     如果一条消息携带的路由键为TT.AA.BB，那么队列Q2将会收到
### 扇形交换机：Fanout Exchange

扇形交换机是最基本的交换机类型，它所能做的事情非常简单———广播消息。
     扇形交换机会把能接收到的消息全部发送给绑定在自己身上的队列。因为广播不需要“思考”，
     所以扇形交换机处理消息的速度也是所有的交换机类型里面最快的。 

     这个交换机没有路由键概念，就算你绑了路由键也是无视的。 

### 首部交换机：Headers exchange
headers属性是一个键值对，键值对的值可以是任何类型，而fanout，direct，topic 的路由键都需要要字符串形式的。

头交换机匹配规则x-match有下列两种类型：

x-match = all，表示所有的键值对都匹配才能接受到消息。

x-match = any，表示只要有键值对匹配就能接受到消息。
### 默认交换机：预先声明的默认空字符串

简单应用特别有用处：那就是每个新建队列（queue）都会自动绑定到默认交换机上，绑定的路由键（routing key）名称与队列名称相同。

     如：当你声明了一个名为”hello”的队列，RabbitMQ会自动将其绑定到默认交换机上，绑定（binding）的路由键名称也是为”hello”。
     因此，当携带着名为”hello”的路由键的消息被发送到默认交换机的时候，此消息会被默认交换机路由至名为”hello”的队列中
  
### 系统内部交换机
类似amq.*的名称的交换机：
     这些是RabbitMQ默认创建的交换机。这些队列名称被预留做RabbitMQ内部使用，不能被应用使用，否则抛出403

### Dead Letter Exchange（死信交换机）

     在默认情况，如果消息在投递到交换机时，交换机发现此消息没有匹配的队列，则这个消息将被悄悄丢弃。
     为了解决这个问题，RabbitMQ中有一种交换机叫死信交换机。当消费者不能处理接收到的消息时，将这个消息重新发布到另外一个队列中，
     等待重试或者人工干预。这个过程中的exchange和queue就是所谓的”Dead Letter Exchange 和 Queue

## 属性

- Name：交换机名称；就是一个字符串
- Type：交换机类型，direct, topic, fanout, headers四种
- Durability：持久化，声明交换机是否持久化，代表交换机在服务器重启后是否还存在；
- Auto delete：是否自动删除，曾经有队列绑定到该交换机，后来解绑了，那就会自动删除该交换机；
- Internal：内部使用的，如果是yes，客户端无法直接发消息到此交换机，它只能用于交换机与交换机的绑定。
- Arguments：只有一个取值alternate-exchange，表示备用交换机；
