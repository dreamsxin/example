# CAF_C++ Actor Framework

http://actor-framework。org/

https://actor-framework。readthedocs。io

CAF 是一个 Actor 模型的开源编程框架， 强 C++ 11 风格的实现，早先的名字并不叫 CAF，而是CPPA。改名的时间发生在版本(0。9 >> 0。10)。
默认的 Actor 调度是基于线程池方式，而不是协程（CAF的实现参考了 Akka 和 Erlang，更像 Akka）。

在 CAF 中大量使用了 lambda 表达式来替换通常散落一地的回调函数。

## 编译安装

```shell
git clone https://github.com/actor-framework/actor-framework.git 
cd actor-framework

```
编译成功之后生成两个库：libcaf_core.so 和 libcaf_io.so，如果你不打算使用网络相关的功能，只要链接上 `libcaf_core` 就可以了。

## CAF 主要特性

1、Actor

Actor 分为两种，无类型和强类型。Actor 的创建非常简单，简单到不能再简单了。
将一个自由函数作为参数传递 spawn，一个 Actor 就生成了，在下面的例子中 actorx 内部持有一个 actor 的句柄 self。所有与这个 Actor 有关的消息都从这里开始。

```cpp
void actorx(event_based_actor* self)
{

}
spawn(actorx);
```

2、模式匹配

模式匹配是 CAF 中一个十分重要的特性，它直接导致了 CAF 基于 lambda 表达式的消息回调列表成为可能。caf为此自己内建了 DSL 语言来达成这个目的。
```cpp
void actorx(event_based_actor* self)
{
    self->become(	
    [=](const string& what)
    {
        LOG_DEBUG("got a string msg: %s\n"，what。c_str())
    },
    [=](const int& what)
    {
	    LOG_DEBUG("got integer msg: %d\n"，what)
    });
}
auto a = spawn(actorx);
anon_send(a，"string"); /* 向actor发送一个字符串。*/
anon_send(a，0x10); /* 向actor发送一个整数。*/
```

3、消息发送

消息发送可以分为以下几种:

a) 发完就忘，就像上面 anon_send。
b) 同步发送，等待响应，等待是异步的，相当于只是期待一个响应。
c) 同步发送，等待响应，超时后收到一个系统消息。
d) 同步发送，同步等待，适用阻塞的 Actor API。
e) 消息延迟发送，这一点比 Akka 做得漂亮很多。
f) 消息前转 forward。
g) 消息优先级选择。


4、消息接收

主要有两种特性:

a) 等待超时，超时后收到一个系统消息。
b) 消息跳过，skip。

5、行为（behavior）

如果 Actor 消息流程是一个有限状态机，那行为就是用来控制流程的标志。behavior 是一个栈，通过 `keep_behavior` 压栈，`unbecome` 恢复。

6、链接（link）

两个(或多个)actor可以相互link在一起，生死相关，一起活着或者默认一起死去。
当其中一个actor异常退出时，另一个(可能是多个)会收到一个"exit_msg"消息，如果不捕获这个消息，默认的行为就是当前actor也退出。
如果选择捕获，那就自己定义行为。

7、监视（monitor）

一个actor可选择监视另一个或另几个actor。甚至可以同时监视同一个actor多次。当被监视的actor退出时，监视者就会收到一个"down_msg"。
监视者仅仅是收到这个消息而已。不会有其它对自己的响应。需自己定义行为。


8、在网络上发布一个 actor

这个功能貌似还非常骨感，其做法是调用 pubish 函数将一个 actor 绑定在某个本地端口上，作为服务器向外提供服务。如下面的端口8080，端口
后面还有一个默认的字符串参数，允许指定本地服务ip。
```cpp
auto a = spawn(actorx);
publish(a，8080);

actor r = remote_actor("127。0。0。1"，8080);
anon_send(r，"information");
```

上面的 `remote_actor` 则用于连上publish的actor，这样分布在不同机器上的actor就可以互相通信了。这个功能似乎仅限于此了。
从实际的代码运行来看。作为服务器的actor只能收到客户端发送的业务消息，包括连接到来，断开等事件都无消息通知。
并且双方之间的通信似乎还遵循某种协议(自己写一个socket，向服务器发送消息时，它无动于衷)。

9、代理（broker）

broker的功能要比publish更强。 有点类似于我的前一篇博客中介绍的akka.io。

比akka有一点好的地方在是，broker提供了有限的手动控制消息缓冲区的api。 

相信过不了多久。用broker作服务器就会成为不错的选择.


10、组通信

组可以分为匿名组，本地组和远程组，前两个用于进程内通信，远程组则可能适合一些分布式的应用场景，一个 actor 可以加入或选择离开组。