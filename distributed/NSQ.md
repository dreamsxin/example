# NSQ 源码解读

NSQ是由知名短链接服务商bitly用Go语言开发的实时消息处理系统，具有高性能、高可靠、无视单点故障等优点，是一个非常不错的新兴的消息队列解决方案。

nsg易于配置和部署，所有参考都通过命令行指定，编译好的二进制文件，没有其它依赖项。而且支持多种消息格式。

源码下载地址：

https://github.com/bitly/nsq

## 安装 nsq

```shell
go get github.com/kr/godep
go get github.com/bmizerany/assert
godep get github.com/bitly/nsq/...
```

## 使用 nsq

总共需要打开五个命令行窗口。

第一个执行：
```shell
nsqlookupd
```

第二个执行：
```shell
nsqd --lookupd-tcp-address=127.0.0.1:4160
```

第三个执行：
```shell
nsqadmin --lookupd-http-address=127.0.0.1:4161
```

第四个执行：
```shell
curl -d "hello world 1" "http://127.0.0.1:4151/put?topic=test"
```

第五个执行：

```shell
nsq_to_file --topic=test --output-dir=F:\tmp --lookupd-http-address=127.0.0.1:4161
```

这个命令中的output-dir参数，表示数据文件保存在目录，可以根据需要修改。
再回到第四个窗口，执行

```shell
curl -d "hello world 2" "http://127.0.0.1:4151/put?topic=test"
```

```shell
curl -d "hello world 3" "http://127.0.0.1:4151/put?topic=test"
```

浏览器访问：`http://127.0.0.1:4171/`，可以看到数据统计分析的页面。

打开output-dir参数设置的目录，如本例中的F:\tmp，打开里面的一个记事本文件，看到有三行：
hello world 1
hello world 2
hello world 3

至此，安装和使用就结束了。

## nsqlookupd：

http://bitly.github.io/nsq/components/nsqlookupd.html

`nsqlookupd`管理拓扑信息，客户端根据特定的`topic`查询`nsqlookupd`来发现`nsqd`生产者及`nsqd`广播`topic`和`channel`信息。

它可以理解为整个nsq系统的神经中枢，启动以后，它默认会同时监听两个端口`4160`和`4161`。
在`4160`端口，建立一个`tcp server`，用于和`nsqd`进行数据沟通。
在`4161`上呢，会建立一个`http server`，用于和`nsqadmin`进行数据交互。

## nsqd

http://bitly.github.io/nsq/components/nsqd.html

`nsqd`接收、队列、发送消息到客户端。

命令 `nsqd –lookupd-tcp-address=127.0.0.1:4160`

表示启动一个nsqd 服务，同时告诉这个服务，`nsqlookupd`的地址在`127.0.0.1:4160`。
`nsqd`自己会默认监听`4151`端口，接收以http方式发送来的消息数据。

## nsqadmin

http://bitly.github.io/nsq/components/nsqadmin.html

提供一个浏览器可访问的web页面，用于实时查看集群状态和执行一些管理操作。

命令`nsqadmin –lookupd-http-address=127.0.0.1:4161`

表示启动`nsqadmin`，同时告诉`nsqlookupd`监听的http地址在`127.0.0.1:4161`。
`nsqadmin`监听4171端口，提供一个`web`服务，供浏览器访问，查看数据。

## curl

```shell
curl -d "hello world 1" "http://127.0.0.1:4151/put?topic=test"
```

表示向网址 `http://127.0.0.1:4151/put?topic=test` 发起POST请求，数据为`hello world 1`，这`4151`端口，刚好是`nsqd`监听的端口，`nsqd` 接收网络发来的信息，并放入队列保存起来，供消息的消费者来取出数据并处理。

## nsq_to_file

这是nsq提供一个简单的消息“消费者”，他把某个指定主题`topic`下的数据存在指定的文件中。

命令`nsq_to_file –topic=test –output-dir=/tmp –lookupd-http-address=127.0.0.1:4161`

表示把指定的主题`test`下的数据，放在目录`/tmp`下面。所以可以看到文件中保存了三个`hello world`，因为我们向`topic=test`的URL发送了三次请求。

## nsqlookupd

代码位于源码根目录的nsqlookupd下。

`nsqlookupd\nsqlookupd.go`

```go
package nsqlookupd

import (
"log"
"net"

"github.com/bitly/nsq/util"
)

type NSQLookupd struct {
//在文件nsqlookupd\options.go中定义，记录NSQLookupd的配置信息
options *nsqlookupdOptions

//nsqlookupd监听TCP数据的地址
tcpAddr *net.TCPAddr

//nsqlookupd监听HTTP数据的地址
httpAddr *net.TCPAddr

//使用上面的tcpAddr建立的Listener
tcpListener net.Listener

//使用上面的httpAddr建立的Listener
httpListener net.Listener

//在util\wait_group_wrapper.go文件中定义,与sync.WaitGroup相关，用于线程同步。
waitGroup util.WaitGroupWrapper

//在nsqlookupd\registration_db.go文件中定义，看字面意思DB(database)就可知道这涉及到数据的存取
DB *RegistrationDB
}
//
//根据配置的nsqlookupdOptions创建一个NSQLookupd的实例
//
func NewNSQLookupd(options *nsqlookupdOptions) *NSQLookupd {

//使用配置参数的TCPAddress创建TCP地址，用于和nsqd通信。
tcpAddr, err := net.ResolveTCPAddr("tcp", options.TCPAddress)
if err != nil {
log.Fatal(err)
}

//使用配置参数的HTTPAddress参数，创建http链接，可以供nsqadmin访问，以读取统计数据
httpAddr, err := net.ResolveTCPAddr("tcp", options.HTTPAddress)
if err != nil {
log.Fatal(err)
}

return &amp;NSQLookupd{
options: options,
tcpAddr: tcpAddr,
httpAddr: httpAddr,
DB: NewRegistrationDB(),
}
}

//
//Main函数，启动时首先执行本函数
//补注：阅读options.go时，发现nsqlookupd启动时，首先运行的并不是这个Main方法。而是apps\nsqlookupd\nsqlookupd.go里的main方法，这个下篇文章会提到。
//
func (l *NSQLookupd) Main() {
//定义了Context的实例，Context在nsqlookupd\context.go文件中定义，其中只包含了一个nsqlookupd的指针,注意花括号里是字符L的小写，不是数字一.
context := &amp;Context{l}

//监听TCP
tcpListener, err := net.Listen("tcp", l.tcpAddr.String())
if err != nil {
log.Fatalf("FATAL: listen (%s) failed - %s", l.tcpAddr, err.Error())
}

//把Listener存在NSQLookupd的struct里
l.tcpListener = tcpListener

//创建tcpServer的实例，tcpServer在nsqlookupd\tcp.go文件中定义，用于处理TCP连接中接收到的数据。通过前面阅读知道，context里只是一个NSQLookupd类型的指针。
tcpServer := &amp;tcpServer{context: context}

//调用util.TCPServer方法（在util\tcp_server.go中定义）开始接收监听并注册handler。 //传入的两个参数第一个是tcpListener
//第二个tcpServer实现了util\tcp_server.go中定义的TCPHandler接口。
//tcpServer接到TCP数据时，会调用其Handle方法(见nsqlookupd\tcp.go)来处理。
//此处为何要用到waitGroup，目前还比较迷糊
l.waitGroup.Wrap(func() { util.TCPServer(tcpListener, tcpServer) })

//监听HTTP
httpListener, err := net.Listen("tcp", l.httpAddr.String())
if err != nil {
log.Fatalf("FATAL: listen (%s) failed - %s", l.httpAddr, err.Error())
}

//把Listener存在NSQLookupd的struct里
l.httpListener = httpListener

//创建httpServer的实例，httpServer在nsqlookupd\http.go文件中定义
httpServer := &amp;httpServer{context: context}

//调用util.HTTPServer方法（在util\http_server.go中定义）开始在指定的httpListener上接收http连接。
//传入的两个参数第一个是httpListener
//第二个httpServer定义了http handler，用于处理HTTP请求。
//同样，对waitGroup的用法还不是很理解。
l.waitGroup.Wrap(func() { util.HTTPServer(httpListener, httpServer) })

//经过以上阅读，基本上会有两个发现：
//1、tcpServer和httpServer的代码很相似。
//2、util\tcp_server.go在注册handler之前，先定义了一个接口，而tuil\http_server.go却没有。
//如果再仔细研究这两个文件，还会发现，tcp_server里，通过go handler.Handle(clientConn)这段代码，把连接clientConn做为变量，传给了handler
//而在http_server，是把handler传给了HTTPServer
//这主要是因为net/http包和net包用法不一样，net/http做了进一步有封装。
}

//
//退出 关闭两个Listener
//
func (l *NSQLookupd) Exit() {
if l.tcpListener != nil {
l.tcpListener.Close()
}

if l.httpListener != nil {
l.httpListener.Close()
}
l.waitGroup.Wait()
}
```

上面的代码里共涉及到几个外部文件：
- nsqlookupd\options.go
- nsqlookupd\context.go
- nsqlookupd\tcp.go
- util\tcp_server.go
- nsqlookupd\http.go
- util\http_server.go
- util\wait_group_wrapper.go
- nsqlookupd\registration_db.go

`options.go`：
```go
package nsqlookupd

import (
    "log"
    "os"
    "time"
)

type nsqlookupdOptions struct {
    Verbose bool `flag:"verbose"`   //是否开启啰嗦模式，开启后，会打很多LOG，一般在调试或定位问题时使用。

    TCPAddress       string `flag:"tcp-address"`    //TCP监听地址
    HTTPAddress      string `flag:"http-address"`   //HTTP监听地址
    BroadcastAddress string `flag:"broadcast-address"`  //这个lookup节点的对外地址

    //producer的交互超时时间，默认是5分钟。就是说，如果5分钟内nsqlookupd没有收到producer的PING(类似心跳包),则会认为producer已掉线。
    InactiveProducerTimeout time.Duration `flag:"inactive-producer-timeout"`
    //字面直译是墓碑时间
    //在nsqadmin的http界面中访问/tombstone_topic_producer URL时，nsqlookupd会给producer TombstoneLifetime长度的时间来注销
    //默认为45秒，在这45秒内，producer不会再被任何consumer通过nsqadmin的/lookup操作找到，同时producer还会进行删除topic等操作。
    //45秒之后，producer就会与nsqlookupd断开连接，同时通过nsqlookupd TCP连接中的UNREGISTER操作在数据记录中把该producer删除。
    TombstoneLifetime       time.Duration `flag:"tombstone-lifetime"`
}

//
//新建nsqlookupdOptions类型的变量的指针
//
func NewNSQLookupdOptions() *nsqlookupdOptions {
    //获取主机名
    hostname, err := os.Hostname()
    if err != nil {
        log.Fatal(err)
    }

    //返回nsqlookupdOptions类型的变量，并指定默认参数。
    return &nsqlookupdOptions{
        //TCP监听本机的4160端口
        TCPAddress:       "0.0.0.0:4160",
       
        //HTTP监听本机的4161端口
       
        HTTPAddress:      "0.0.0.0:4161",
       
        //主机名
        BroadcastAddress: hostname,

        //5分钟超时
        InactiveProducerTimeout: 300 * time.Second,
       
        //45秒
        TombstoneLifetime:       45 * time.Second,
    }
}
```

`context.go`：
```go
package nsqlookupd

//
//根据Context的命名，指环境、上下文的意思。通俗来讲，就是保存一些运行环境的信息
//从下面的定义可以看出，Context只是包含了NSQLookupd的指针
//
type Context struct {
    nsqlookupd *NSQLookupd
}
```

`wait_group_wrapper.go`：
```go
package util

import (
    "sync"
)

//
//本文件是对WaitGroup的封装，关于WaitGroup，根据字义，wait是等待的意思，group是组、团体的意思，合起来就是指等待一个组。
//即指，当一个组里所有的操作都完成后，才会继续执行。
//可以参考http://www.baiyuxiong.com/?p=913理解WaitGroup用法
//

type WaitGroupWrapper struct {
    sync.WaitGroup
}

func (w *WaitGroupWrapper) Wrap(cb func()) {
    w.Add(1)
    go func() {
        cb()
        w.Done()
    }()
}
```