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
$autoDelete = true; // 是否自动删除,设置为TRUE则表是自动删除,自删除的前提是至少有一个队列或者交换器与这交换器绑定,之后所有与这个交换器绑定的队列或者交换器都与此解绑,一般都设置为fase
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

## 路由匹配

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

## HEADERS 交换机使用

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