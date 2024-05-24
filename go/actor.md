#

https://github.com/asynkron/protoactor-go

- Actor 模型：在 ProtoActor 中，每个 Actor 都是一个独立的实体，拥有自己的状态，并通过消息进行通信。这种方式可以避免共享状态，减少竞态条件，提高系统的可伸缩性和容错性。
- 轻量级进程：每个 Actor 实例都是一个独立的执行单元（goroutine），它们在内存中是隔离的，无需锁或其他同步机制，大大简化了并发编程。
- 事件驱动：ProtoActor 基于事件循环，每个 Actor 进程在其消息队列上等待新消息的到来，这使得系统能够快速响应并处理大量的并发请求。
- 远程通信：ProtoActor 支持跨节点的 Actor 通信，这意味着你可以轻松地构建分布式的微服务架构，而且它使用的是安全可靠的 Protobuf 协议进行数据序列化。
- 性能优化：由于其高效的并发模式和最小化的上下文切换，ProtoActor 在处理高负载时表现出色，降低了延迟并提高了吞吐量。

## Actor model 与 CSP

两者的概念极为相似，都符合生产者消费者模型。

在 CSP 里面，Producer 往 channel 里投递消息，comsumer 从 channel 里取出消息来处理。

在 Actor model 中，sender 往 actor 的 mail 里投递信息，actor 从 mail 里取出消息来处理。

但是两者的关注点不同，CSP 里面生产者关注的是消息队列本身，生产者只负责往消息队列里面投递消息，至于这些消息被谁消费（甚至至可能被多个消费者一起消费），它不 care。

但是 Actor model 中，生产者关注的是特定的消费者，生产者往特定的消费者投递消息，消息只能被之前所指定的消费者消费。


## 实例

```go
type Hello struct{ Who string }
type HelloActor struct{}

func (state *HelloActor) Receive(context actor.Context) {
    switch msg := context.Message().(type) {
    case Hello:
        fmt.Printf("Hello %v\n", msg.Who)
    }
}

func main() {
    context := actor.EmptyRootContext
    props := actor.PropsFromProducer(func() actor.Actor { return &HelloActor{} })
    pid, err := context.Spawn(props)
    if err != nil {
        panic(err)
    }
    context.Send(pid, Hello{Who: "Roger"})
    console.ReadLine()
}
```

### 新建一个 actor system
```go
system := actor.NewActorSystem()
rootContext := system.Root
```

### 新建一个 actor system
```go
system := actor.NewActorSystem()
rootContext := system.Root
```
### 内构造了一个 actor

用 PID 来标识这个 actor，该 actor 收到消息后的处理逻辑是 Receive()。
```go
props := actor.PropsFromFunc(Receive)
pid := rootContext.Spawn(props)
···

### 发送消息

对之前新建的 actor 发送一个消息，得到一个 future 对象，并利用 future.Result() 方法取到结果。
```go
future := rootContext.RequestFuture(pid, Hello{Who: "Roger"}, -1)
result, _ := future.Result() // await result
···
