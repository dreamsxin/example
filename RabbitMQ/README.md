# RabbitMQ

RabbitMQ 是一个消息代理：它接收并转发消息。
可以把它当成一个邮局，当你把想发送的邮件投进邮箱时，可以确定邮递员最终会把邮件送到收件人手里。在上面的比喻中，我们把 RabbitMQ 比作 邮箱、邮局、邮递员。


RabbitMQ和消息传递(messaging)中一些常使用术语：

- Producing
仅仅指发送消息(message)。

- producer
发送消息的程序就称为生产者。

- queue
RabbitMQ内部的邮箱。虽然消息在 RabbitMQ 和你的应用程序之间流动，但它们只能存储在队列中。队列的大小只受主机的内存和磁盘的限制，他本质上是一个大的消息缓冲区。
多个生产者可以向一个队列发送消息，多个消费者也可以尝试从一个队列接收数据，这就是队列。

- consumer
消费和接收邮件相似，consumer 是一个主要等待接收消息的程序。

## 概念说明：

Broker：简单来说就是消息队列服务器实体。

Exchange：消息交换机，它指定消息按什么规则，路由到哪个队列。

Queue：消息队列载体，每个消息都会被投入到一个或多个队列。

Binding：绑定，它的作用就是把exchange和queue按照路由规则绑定起来。

Routing Key：路由关键字，exchange根据这个关键字进行消息投递。

vhost：虚拟主机，一个broker里可以开设多个vhost，用作不同用户的权限分离。

producer：消息生产者，就是投递消息的程序。

consumer：消息消费者，就是接受消息的程序。

channel：消息通道，在客户端的每个连接里，可建立多个channel，每个channel代表一个会话任务。

## 概括

发送到交换机，交换机根据路由发到相同队列

## hello world

* 生产者

```php
use PhpAmqpLib\Connection\AMQPStreamConnection;
use PhpAmqpLib\Message\AMQPMessage;

// 创建一个 RabbitMQ 的连接，完成协议版本的协商和认证等操作。
$connection = new AMQPStreamConnection('localhost', 5672, 'guest', 'guest');

// 新建一个 channel
$channel = $connection->channel();

// 申明一个队列，不存在时会被创建。
$channel->queue_declare('helloqueue', false, false, false, false);

// 创建消息
$msg = new AMQPMessage('Hello World!');

// 发送消息
$channel->basic_publish($msg, '', 'helloqueue');
 
echo " [x] Sent 'Hello World!'\n";

// 关闭 channel 和 connection
$channel->close();
$connection->close();
```

* 消费者

```php
use PhpAmqpLib\Connection\AMQPStreamConnection;

// 创建一个 RabbitMQ 的连接，完成协议版本的协商和认证等操作。
$connection = new AMQPStreamConnection('localhost', 5672, 'guest', 'guest');

// 新建一个 channel
$channel = $connection->channel();
 
// 这里本来可以不申明一个队列，但因为消费者可能比生产者先启动，所以为了确保队列存在才这样做。
$channel->queue_declare('hello', false, false, false, false);
 
echo ' [*] Waiting for messages. To exit press CTRL+C', "\n";

// 定义接收回调函数
$callback = function($msg) {
  echo " [x] Received ", $msg->body, "\n";
};

// 进行消费监听
$channel->basic_consume('hello', '', false, true, false, false, $callback);
 
while(count($channel->callbacks)) { // 回调函数不为空
    $channel->wait();
}

// 关闭 channel 和 connection
$channel->close();
$connection->close();
```

## 交换机、路由关键字使用

`exchange_declare`和`queue_declare`以及`queue_bind`其实也不是必须的，可以预先通过管理后台添加。

```php
use PhpAmqpLib\Connection\AMQPStreamConnection;
use PhpAmqpLib\Message\AMQPMessage;

$conf = [
	'host' => 'localhost',
	'port' => 5672,
	'user' => 'guest',
	'pwd' => 'guest',
	'vhost' => '/'
];

// 交换机名
$exchangeName = 'helloex';
// 队列名称
$queueName = 'helloqueue';
// 路由关键字(也可以省略)，如果一个交换机有不同的队列，则需要设定不同的路由关键字
$routingKey = 'routehello';


$conn = new AMQPStreamConnection(
    $conf['host'], $conf['port'], $conf['user'], $conf['pwd'], $conf['vhost']
);

$channel = $conn->channel();

// 声明交换机
$type = 'direct'; // 交换器类型 DIRECT("direct"), FANOUT("fanout"), TOPIC("topic"), HEADERS("headers");
$durable = false; // 是否持久化,durable设置为true表示持久化,反之是非持久化,持久化的可以将交换器存盘,在服务器重启的时候不会丢失信息.
$autoDelete = true; // 是否自动删除，设置为TRUE则表是自动删除，是至少有一个队列或者交换器与这交换器绑定，一般都设置为 false ，此时需要发送ack消息，才会删除
$internal = false; // 是否内置,如果设置 为true,则表示是内置的交换器,客户端程序无法直接发送消息到这个交换器中,只能通过交换器路由到交换器的方式
$arguments = NULL; // 其它一些结构化参数比如:alternate-exchange

$channel->exchange_declare($exchangeName, 'direct', $durable, $autoDelete, $internal, $arguments);

// 声明队列
$channel->queue_declare($queueName, $durable, $autoDelete, false, false);

// 将队列与某个交换机进行绑定，并使用路由关键字
$channel->queue_bind($queueName, $exchangeName, $routingKey);

// 生成消息
$msgBody = json_encode(["name" => "iGoo", "age" => 22]);
$msg = new AMQPMessage($msgBody, ['content_type' => 'text/plain', 'delivery_mode' => 2]);

// 推送消息到某个交换机，绑定的时候使用了路由关键字，这里也要指定，默认就推送到不使用路由关键字的队列了
$r = $channel->basic_publish($msg, $exchangeName, $routingKey);

$channel->close();
$conn->close();
```

## 交换机类型

1、匿名交换机，工作队列模式（不用显示申明交换机）
2、扇形交换机（fanout），发布订阅/广播模式 
3、直连交换机（direct），路由绑定/广播精确匹配 
4、主题交换机（topic），路由规则/广播模糊

交换机类型CPU开销是不一样的，一般来说CPU开销顺序是:
`主题交换机 > 直连交换机 > 扇形交换机 > 匿名交换机

上面的代码中，当我们声明初始化交换机的时候第二个参数使用`direct`参数，其实还有另外3种参数可选，分别为：

- direct
精准推送

- fanout
广播。推送到绑定到此交换机下的所有队列

- topic
组播。比如上面我绑定的路由关键字是`routehello`，那么会把消息推送到此交换机绑定此路由关键字的所有队列。

- headers
首部交换机和扇形交换机都不需要路由键routingKey，交换机时通过`Headers`头部来将消息映射到队列。消费者在`queue_bind`的时候要求`$arguments`携带一个键“x-match”，以及一个或多个键值对。

	* any
	只要在发布消息时携带的有一对键值对headers满足队列定义的多个参数arguments的其中一个就能匹配上，注意这里是键值对的完全匹配，只匹配到键了，值却不一样是不行的；

	* all
	在发布消息时携带的所有 `Entry` 必须和绑定在队列上的所有 `Entry` 完全匹配

### 匿名交换机

应用场景：并行处理任务队列 

工作队列（又称：任务队列—Task Queues）是为了避免等待一些占用大量资源、时间的操作。当我们把任务（Task）当作消息发送到队列中，一个运行在后台的工作者（worker）进程就会取出任务然后处理。当你运行多个工作者（workers），任务就会在它们之间共享。

使用工作队列的一个好处就是它能够并行的处理队列。如果堆积了很多任务，我们只需要添加更多的工作者（workers）就可以了，扩展很简单。

默认来说，RabbitMQ会按顺序得把消息发送给每个消费者（consumer）。平均每个消费者都会收到同等数量得消息。这种发送消息得方式叫做——轮询（round-robin）。

代码示例

1）生产者
```php
//声明队列，$queue,$passive,$durable,$exclusive,$auto_delete
$channel->queue_declare('task_queue', false, true, false, false);
//发布消息到队列，$message,$exchange,$queue
$channel->basic_publish($msg, '', 'task_queue');
```
2）消费者
```php
//声明队列
$channel->queue_declare('task_queue', false, true, false, false);
//消费数据异步回调，$queue,$consumer_tag,$no_local,$no_ack,$exclusive,$nowait,$callback
$channel->basic_consume('task_queue', '', false, false, false, false, $callback);
```
* 相关特性

1）持久化。
如果你没有特意告诉RabbitMQ，那么在它退出或者崩溃的时候，将会丢失所有队列和消息。
已经定义过非持久化的队列不能再定义为持久化队列，我们得重新命名一个新的队列。必须把“队列”和“消息”都设为持久化。

```php
//交换机持久化
$channel->exchange_declare('exchange', 'fanout', false, true, false);
//对列持久化
$channel->queue_declare('task_queue', false, true, false, false);
//消息持久化
$msg = new AMQPMessage($body, ['delivery_mode' => 2]);
```

2）Ack消息确认。
我们不想丢失任何任务消息。如果一个工作者（worker）挂掉了，我们希望任务会重新发送给其他的工作者（worker）。为了防止消息丢失，RabbitMQ提供了消息响应（acknowledgments）。消费者会通过一个ack（响应），告诉RabbitMQ已经收到并处理了某条消息，然后RabbitMQ就会释放并删除这条消息。
```php
$channel->basic_consume('task_queue', '', false, false, false, false, $callback);
//在回调函数中发送ack消息
$msg->delivery_info['channel']->basic_ack($msg->delivery_info['delivery_tag']);
```

3）Qos公平调度。
如果多个worker进程中，某个worker处理比较慢，另一个worker比较快，默认RabbitMQ只管分发进入队列的消息，不会关心有多少消费者（consumer）没有作出响应，这样会使得比较慢的worker消息堆积过多，导致任务分配不均。Qos公平调度设置prefetch_count=1，即在同一时刻，不会发送超过1条消息给一个工作者（worker），直到它已经处理了上一条消息并且作出了响应。这样，RabbitMQ就会把消息分发给下一个空闲的工作者（worker）。
```php
//$prefetch_size,$prefetch_count,$global
$channel->basic_qos(null, 1, null);
```

4）消息事务。
将消息设为持久化并不能完全保证不会丢失。持久化只是告诉了RabbitMq要把消息存到硬盘，但从RabbitMq收到消息到保存之间还是有一个很小的间隔时间。因为RabbitMq并不是所有的消息都使用同步IO—它有可能只是保存到缓存中，并不一定会写到硬盘中。并不能保证真正的持久化，但已经足够应付我们的简单工作队列。如果你一定要保证持久化，我们需要改写代码来支持事务（transaction）。
```php
$channel->tx_select();
$channel->basic_publish($msg, '', 'task_queue');
$channel->tx_commit();
```

5）confirm消息确认。
AMQP消息协议提供了事务支持，不过事务机制会导致性能急剧下降，所以rabbitmq特别引入了 confirm 机制。

Confirm有三种编程方式：
	
1、普通confirm模式。每发送一条消息后，调用wait_for_pending_acks()方法，等待服务器端confirm。实际上是一种串行confirm。
2、批量confirm模式。每次发送一批消息后，调用wait_for_pending_acks()方法，等待服务器端confirm。
3、异步confirm模式。提供一个回调方法，服务器端confirm了一条(或多条)消息后客户端会回调这个方法。

代码示例：
```php
// 一旦消息被设为confirm模式，就不能设置事务模式，反之亦然
$channel->confirm_select();
// 阻塞等待消息确认
$channel->wait_for_pending_acks();
// 异步回调消息确认
$channel->set_ack_handle();
$channel->set_nack_handler();
```

### 扇形交换机

应用场景：简单的发布订阅模式，广播消息 

这种广播式交换机会忽略路由关键字，不使用任何绑定参数将队列和交换机绑定在一起。 
producer每向交换机发送一条消息，消息都会被无条件的传递到所有和这个交换机绑定的消息队列中。
 
代码示例
```php
//声明交换机类型，匿名交换机无需声明，$exchange,$type,$passive,$durable,$auto_delete
$channel->exchange_declare('logs', 'fanout', false, false, false);
//发布消息到fanout交换机，忽略routing_key
$channel->basic_publish($msg, 'logs');
//创建随机的临时队列
list($queue_name, ,) = $channel->queue_declare("");
//绑定队列到交换机
$channel->queue_bind($queue_name, 'logs');
```

### HEADERS 交换机

生产者发送带 header 的消息：
```php
$connection = new AMQPStreamConnection(HOST, PORT, USER, PASS, VHOST);
$channel = $connection->channel();

// 声明头交换机类型 'header' 可以用 AMQPExchangeType::HEADERS 代替
$channel->exchange_declare($exchangeName, 'header', $durable, $autoDelete, $internal, $arguments);

//传递带 header 消息属性
$headers = new Wire\AMQPTable(['who' => 'myleft', 'name' => 'myleft']);

$message = new AMQPMessage($body);
$message->set('application_headers', $headers);

$channel->basic_publish($message, $exchangeName);
```
消费者：
```php
$connection = new AMQPStreamConnection(HOST, PORT, USER, PASS, VHOST);
$channel = $connection->channel();

// 声明头交换机类型
$channel->exchange_declare($exchangeName, AMQPExchangeType::HEADERS, $durable, $autoDelete, $internal, $arguments);

$arguments = [
	'x-match' => 'any', // 注意之类
	'who' => 'myleft', // 这里匹配，所以可以接收到上面生产者消息
	'color' => 'red'
];

$arguments = [
	'x-match' => 'all', // 注意之类
	'who' => 'myleft', // 只有这个匹配，所以无法接收到上面生产者接收到消息
	'color' => 'red'
];

list($queue_name, ,) = $channel->queue_declare('', false, false, true);

$channel->queue_bind($queue_name, $exchangeName, '', false, $arguments);

$callback = function (AMQPMessage $message) {
    echo PHP_EOL . ' [x] ', $message->delivery_info['routing_key'], ':', $message->body, "\n";
    echo 'Message headers follows' . PHP_EOL;
    var_dump($message->get('application_headers')->getNativeData());
    echo PHP_EOL;
};

$channel->basic_consume($queue_name, '', false, true, false, false, $callback);

while ($channel->is_consuming()) {
    try {
        $channel->wait(null, false, 2);
    } catch (AMQPTimeoutException $exception) {
    }
    echo '*' . PHP_EOL;
}
```

## Golang

tag是一个64位的long值，最大值是 9223372036854775807

- channel.basicReject(deliveryTag, true);
basic.reject 方法拒绝 deliveryTag对应的消息，第二个参数是否`requeue`，`true`则重新入队列，否则丢弃或者进入死信队列。重新入队后，该消费者还是会消费到该条被reject的消息。

- channel.basicNack(deliveryTag, false, true);
basic.nack方法为不确认deliveryTag对应的消息，第二个参数是否应用于多消息，第三个参数是否requeue，与basic.reject区别就是同时支持多个消息，可以nack该消费者先前接收未ack的所有消息。nack后的消息也会被自己消费到。

- channel.basicRecover(true);
basic.recover是否恢复消息到队列，参数是是否`requeue`，`true`则重新入队列，并且尽可能的将之前recover的消息投递给其他消费者消费，而不是自己再次消费。`false`则消息会重新被投递给自己。
	
```go
connection = factory.newConnection();

final Channel channel = connection.createChannel();
channel.queueDeclare("队列名", true, false, false, null);

//第二个参数设为true为自动应答，false为手动ack
channel.basicConsume("队列名", true, new DefaultConsumer(channel){
	  @Override
	public void handleDelivery(String consumerTag, Envelope envelope, AMQP.BasicProperties properties, byte[] body) throws IOException {
		   try {
			    Thread.sleep(10000);
			    System.out.println(new String(body, "UTF-8"));
				//模拟异常
				int i = 1/0;
				//手动ack
				//channel.basicAck(envelope.getDeliveryTag(), false); 
			} catch (Exception e) {
				
				//重新放入队列
				//channel.basicNack(envelope.getDeliveryTag(), false, true);
				//抛弃此条消息
				//channel.basicNack(envelope.getDeliveryTag(), false, false);
				e.printStackTrace();

			}finally {
				
			}
	    
	}
});
```
