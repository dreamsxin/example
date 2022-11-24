# sarama 的消费者组分析、使用

## 一、官方样例

官方样例比较简单：

1、通过 sarama.NewConfig 创建一个配置

2、通过 NewConsumerGroup 创建一个消费者组

3、通过 Consume 创建消费者组的会话，该函数的第三个参数即为该会话三个阶段的回调： Setup Cleanup 和 ConsumeClaim ，分别在创建会话之前、会话结束之后 和 会话生存中（主要就是在此阶段进行消息读取）进行调用。

## 二、问题

1、当指定的topic在kafka中不存的时候，kafka会新建该topic，如果只想让用户消费已存在的topic，那么该如何获取kafka中已经存在的topic？

2、 setup 和 Cleanup 的调用流程是怎样的？会在哪些情况下被调用？

3、既然是消费者组，那如何查看组里某个消费者拥有哪些topic和partition？

4、如何使用指定的 offset 来消费某个 topic ？

5、如何实现消费的 Exactly-once？

注：以上测试使用的示例代码是自己写的样例代码的部分内容，完整的样例代码见文章最后

三、分析
1、在 sarama 中，获取 topic 的接口在 Client interface 中，所以需要先通过 NewClient 接口创建一个 client，然后就可以通过该 client 的 Topics 接口获取到 kafka 中所有的 topic。但消费者组使用的类型是 ConsumerGroup ，那该如何获取该类型呢？sarama 中提供 NewConsumerGroupFromClient 接口，可以从一个现存的 client 创建一个 ConsumerGroup ，所以，修改后的流程，由原先的 NewConsumerGroup 直接创建，变成：

a、使用 NewClient 创建一个 client

b、使用 NewConsumerGroupFromClient 创建 ConsumerGroup 。

具体代码实现如下：
```go
// 创建client
newClient, err := sarama.NewClient(brokers, config)
if err != nil {
	log.Fatal(err)
}

// 获取所有的topic
topics, err := newClient.Topics()
if err != nil {
	log.Fatal(err)
}
log.Info("topics: ", topics)

// 根据client创建consumerGroup
client, err := sarama.NewConsumerGroupFromClient(k.group, newClient)
if err != nil {
	log.Fatalf("Error creating consumer group client: %v", err)
}
```
这么做的好处就是：可以使用 client 的接口，获取一些信息，例如 kafka 的当前配置有哪些，controller 有哪些，brokers 有哪些，topic 总共有哪些，特定的 topic 有哪些 partitions，partition 当前的 offset 是多少 等等，具体功能可查看 Client interface ：
```go
type Client interface {
	// Config returns the Config struct of the client. This struct should not be
	// altered after it has been created.
	Config() *Config

	// Controller returns the cluster controller broker. It will return a
	// locally cached value if it's available. You can call RefreshController
	// to update the cached value. Requires Kafka 0.10 or higher.
	Controller() (*Broker, error)

	// RefreshController retrieves the cluster controller from fresh metadata
	// and stores it in the local cache. Requires Kafka 0.10 or higher.
	RefreshController() (*Broker, error)

	// Brokers returns the current set of active brokers as retrieved from cluster metadata.
	Brokers() []*Broker
  ......
}
```

2、 setup 、 Cleanup 和 ConsumeClaim 是 s.handler.ConsumeClaim 的三个接口，需要用户自己实现。可以简单理解为：当需要创建一个会话时，先运行 setup ，然后在 ConsumerClaim 中处理消息，最后运行 Cleanup 。

setup 会在一个新会话开始之前运行，且也在 ConsumerClaim 接口之前运行。调用流程为： Consume —> newSession —> newConsumerGroupSession —> handler.Setup 。

在调用了 Setup 之后，后面会创建一个协程，该协程里面其实调用的就是 ConsumeClaim 接口，所以我们实现的 ConsumerClaim 其实是一个单独的协程，其调用流程为： Consume —> newSession —> newConsumerGroupSession —> consume —> s.handler.ConsumeClaim 。

Cleanup 会在一个会话结束的时候运行。调用流程为：Consume —>release —> s.handler.Cleanup 。

了解了调用流程之后，哪些情况又会调用到他们呢？—> 1、新建consumeGroup的时候。2、发生rebalance的时候。

我们可以在setup和cleanup中加一个打印：
```go
func (k *Kafka) Setup(session sarama.ConsumerGroupSession) error {
	log.Info("setup")
  close(k.ready)
	return nil
}

func (k *Kafka) Cleanup(sarama.ConsumerGroupSession) error {
	log.Info("cleanup")
	return nil
}
```

然后启动一个consumer，可以观察到打印：
`INFO[0000] setup`

然后按 Ctrl + C 关闭 consumer，可以观察到打印：
`INFO[0101] cleanup`

说明新建consumer然后退出时，会调用 setup 和 cleanup。

我们再试一下发生rebalance的情况：先启动一个consumer，然后再启动一个同一组的consumer，第一个启动的 consumer 打印为：
```txt
INFO[0000] setup
INFO[0006] cleanup
INFO[0006] setup
```

第二个启动的 consumer 打印为：
```txt
INFO[0002] setup
```
说明在发生 reblance 的时候，会先关闭原先的会话，并调用 cleanup，然后再调用 setup，最后生成一个新的会话。
3、在 ConsumerGroupSession 接口中，有一个 Claims 接口，可以用来查看当前 consumer 被分配到哪些 topic 和 partition。我们可以在 Setup 接口中进行打印：
```go
func (k *Kafka) Setup(session sarama.ConsumerGroupSession) error {
	log.Info("setup")
	log.Info(session.Claims())
	close(k.ready)
	return nil
}
```
这里使用 range 分区策略，订阅的topic为t1p4和t2p4，每个topic都有4个分区，然后创建3个consumer，产生的打印为：
```txt
consumer1:
INFO[0000] setup
INFO[0000] map[t1p4:[0 1 2 3] t2p4:[0 1 2 3]]
INFO[0009] cleanup
INFO[0009] setup
INFO[0009] map[t1p4:[0 1] t2p4:[2 3]]
INFO[0015] cleanup
INFO[0015] setup
INFO[0015] map[t1p4:[0] t2p4:[3]]

consumer2:
INFO[0002] setup
INFO[0002] map[t1p4:[2 3] t2p4:[0 1]]
INFO[0009] cleanup
INFO[0009] setup
INFO[0009] map[t1p4:[1 2] t2p4:[0]]

consumer3:
INFO[0000] setup
INFO[0000] map[t1p4:[3] t2p4:[1 2]]
```
当只有consumer1的时候，它被分配到所有的分区： t1p4:[0 1 2 3] t2p4:[0 1 2 3]。

当consumer2加入的时候，consumer1被分配的是： t1p4:[0 1] t2p4:[2 3]， consumer2被分配的是： t1p4:[2 3] t2p4:[0 1] 。

当consumer3加入的时候，consumert1被分配的是： t1p4:[0] t2p4:[3] ，consumer2被分配的是： t1p4:[1 2] t2p4:[0]， consumer3被分配的是： t1p4:[3] t2p4:[1 2] 。

有兴趣的可以再依次删除consumer1，consumer2。

4、kafka的config配置中，指定消费的offset只有两个： OffsetNewest 和 OffsetOldest ，如果想指定 offset 进行消费，该怎么做呢？

前面说过， Setup 是运行在会话最一开始的地方，且这个时候已经能够获取到所有的 topic 和 partition，所以这里可以使用 ConsumerGroupSession 的 ResetOffset 接口进行设置，具体实现如下：（这里使用的主题：t2p4 已存在，且0分区中的offset已经到18）
```go
func (k *Kafka) Setup(session sarama.ConsumerGroupSession) error {
	log.Info("setup")
	session.ResetOffset("t2p4", 0, 13, "")
	log.Info(session.Claims())
	close(k.ready)
	return nil
}

func (k *Kafka) ConsumeClaim(session sarama.ConsumerGroupSession, claim sarama.ConsumerGroupClaim) error {
	for message := range claim.Messages() {
		log.Infof("[topic:%s] [partiton:%d] [offset:%d] [value:%s] [time:%v]",
			message.Topic, message.Partition, message.Offset, string(message.Value), message.Timestamp)
		session.MarkMessage(message, "")
	}
	return nil
}
```

此时，无论运行多少次，都可以消费 13 到 18 之间的消息：
```txt
INFO[0000] setup
INFO[0000] map[t1p4:[0 1 2 3] t2p4:[0 1 2 3]]
INFO[0000] [topic:t2p4] [partiton:0] [offset:13] [value:a] [time:2021-10-12 23:02:35.058 -0400 EDT]
INFO[0000] [topic:t2p4] [partiton:0] [offset:14] [value:b] [time:2021-10-12 23:02:35.087 -0400 EDT]
INFO[0000] [topic:t2p4] [partiton:0] [offset:15] [value:c] [time:2021-10-12 23:02:35.092 -0400 EDT]
INFO[0000] [topic:t2p4] [partiton:0] [offset:16] [value:d] [time:2021-10-12 23:03:18.882 -0400 EDT]
INFO[0000] [topic:t2p4] [partiton:0] [offset:17] [value:e] [time:2021-10-12 23:03:18.898 -0400 EDT]
INFO[0000] [topic:t2p4] [partiton:0] [offset:18] [value:f] [time:2021-10-12 23:03:18.903 -0400 EDT]
```

5、前面已经分析了 Setup 的调用流程，以及可以在 Setup 中可以做的事情，那么就可以手动记录 topic 的 offset 到磁盘中（比如文本、数据库等），在 Setup 的接口中，读取之前记录的 offset ，通过 ResetOffset 接口进行重新设置即可。当然，更新 offset 与 消息处理这部分的一致性，需要业务自己保证（例如使用数据库的事务来实现）。

## 四、完整样例代码

```go
package main

import (
	"context"
	"os"
	"os/signal"
	"sync"
	"syscall"

	"github.com/Shopify/sarama"
	log "github.com/sirupsen/logrus"
)

type Kafka struct {
	brokers           []string
	topics            []string
	startOffset       int64
	version           string
	ready             chan bool
	group             string
	channelBufferSize int
	assignor          string
}

var brokers = []string{"192.168.1.101:9092"}
var topics = []string{"t1p4", "t2p4"}
var group = "grp1"
var assignor = "range"

func NewKafka() *Kafka {
	return &Kafka{
		brokers:           brokers,
		topics:            topics,
		group:             group,
		channelBufferSize: 1000,
		ready:             make(chan bool),
		version:           "2.8.0",
		assignor:          assignor,
	}
}

func (k *Kafka) Connect() func() {
	log.Infoln("kafka init...")

	version, err := sarama.ParseKafkaVersion(k.version)
	if err != nil {
		log.Fatalf("Error parsing Kafka version: %v", err)
	}

	config := sarama.NewConfig()
	config.Version = version
	// 分区分配策略
	switch assignor {
	case "sticky":
		config.Consumer.Group.Rebalance.Strategy = sarama.BalanceStrategySticky
	case "roundrobin":
		config.Consumer.Group.Rebalance.Strategy = sarama.BalanceStrategyRoundRobin
	case "range":
		config.Consumer.Group.Rebalance.Strategy = sarama.BalanceStrategyRange
	default:
		log.Panicf("Unrecognized consumer group partition assignor: %s", assignor)
	}
	config.Consumer.Offsets.Initial = sarama.OffsetNewest
	config.ChannelBufferSize = k.channelBufferSize // channel长度

	// 创建client
	newClient, err := sarama.NewClient(brokers, config)
	if err != nil {
		log.Fatal(err)
	}
	// 获取所有的topic
	topics, err := newClient.Topics()
	if err != nil {
		log.Fatal(err)
	}
	log.Info("topics: ", topics)

	// 根据client创建consumerGroup
	client, err := sarama.NewConsumerGroupFromClient(k.group, newClient)
	if err != nil {
		log.Fatalf("Error creating consumer group client: %v", err)
	}

	ctx, cancel := context.WithCancel(context.Background())
	wg := &sync.WaitGroup{}
	wg.Add(1)
	go func() {
		defer wg.Done()
		for {
			if err := client.Consume(ctx, k.topics, k); err != nil {
				// 当setup失败的时候，error会返回到这里
				log.Errorf("Error from consumer: %v", err)
				return
			}
			// check if context was cancelled, signaling that the consumer should stop
			if ctx.Err() != nil {
				log.Println(ctx.Err())
				return
			}
			k.ready = make(chan bool)
		}
	}()
	<-k.ready
	log.Infoln("Sarama consumer up and running!...")
	// 保证在系统退出时，通道里面的消息被消费
	return func() {
		log.Info("kafka close")
		cancel()
		wg.Wait()
		if err = client.Close(); err != nil {
			log.Errorf("Error closing client: %v", err)
		}
	}
}

// Setup is run at the beginning of a new session, before ConsumeClaim
func (k *Kafka) Setup(session sarama.ConsumerGroupSession) error {
	log.Info("setup")
	session.ResetOffset("t2p4", 0, 13, "")
	log.Info(session.Claims())
	// Mark the consumer as ready
	close(k.ready)
	return nil
}

// Cleanup is run at the end of a session, once all ConsumeClaim goroutines have exited
func (k *Kafka) Cleanup(sarama.ConsumerGroupSession) error {
	log.Info("cleanup")
	return nil
}

// ConsumeClaim must start a consumer loop of ConsumerGroupClaim's Messages().
func (k *Kafka) ConsumeClaim(session sarama.ConsumerGroupSession, claim sarama.ConsumerGroupClaim) error {

	// NOTE:
	// Do not move the code below to a goroutine.
	// The `ConsumeClaim` itself is called within a goroutine, see:
	// https://github.com/Shopify/sarama/blob/master/consumer_group.go#L27-L29
	// 具体消费消息
	for message := range claim.Messages() {
		log.Infof("[topic:%s] [partiton:%d] [offset:%d] [value:%s] [time:%v]",
			message.Topic, message.Partition, message.Offset, string(message.Value), message.Timestamp)
		// 更新位移
		session.MarkMessage(message, "")
	}
	return nil
}

func main() {
	k := NewKafka()
	c := k.Connect()

	sigterm := make(chan os.Signal, 1)
	signal.Notify(sigterm, syscall.SIGINT, syscall.SIGTERM)
	select {
	case <-sigterm:
		log.Warnln("terminating: via signal")
	}
	c()
}
```
