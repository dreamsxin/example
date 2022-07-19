# go-kit

* 认证 - Basic 认证和 JWT 认证
* 传输 - HTTP、Nats、gRPC 等等。
* 日志记录 - 用于结构化服务日志记录的通用接口。
* 指标 - CloudWatch、Statsd、Graphite 等。
* 追踪 - Zipkin 和 Opentracing。
* 服务发现 - Consul、Etcd、Eureka 等等。
* 断路器 - Hystrix 的 Go 实现。

https://github.com/go-kit/kit

## 使用说明

- service
定义服务接口，以及实现服务处理逻辑。

- endpoint
定义 `endpoints` 集合 `set`， 实现服务接口，调用 `Endpoint`。`MakeXXXXEndpoint returns an endpoint that invokes XXXX on the service.`
定义Request、Response格式，并可以使用装饰器包装函数，依次来实现各个中间件的嵌套。比如在请求的时候添加日志。

- transport
`gprc`: 
	调用 `NewGRPCClient` 传递 `conn`， 构建并返回 `endpoint` 定义的 `set`，通过 `grpctransport.NewClient().Endpoint()` 创建 `Endpoint` 赋值给 `set`。
	调用 `NewGRPCServer` 传递 `service` 目录定义的服务实例。
	
`http`: 实现 `NewHTTPHandler`，传递 `endpoints` 集合 `set`。

* 注册中心

- 1、 etcd
- 2、 consul
- 3、 eureka
- 4、 zookeeper

* 负载均衡

- 1、 random
- 2、 roundRobin
- 3、实现 Balancer 接口，自定义

只需实现Balancer接口，我们可以很容易的增加其它负载均衡机制

## Demo

* server
```golang

package main
 
import (
	"MyKit"
	"context"
	"fmt"
	"github.com/go-kit/kit/endpoint"
	"github.com/go-kit/kit/log"
	"github.com/go-kit/kit/sd/etcdv3"
	grpc_transport "github.com/go-kit/kit/transport/grpc"
	"google.golang.org/grpc"
	"net"
	"time"
)
 
type BookServer struct {
	bookListHandler grpc_transport.Handler
	bookInfoHandler grpc_transport.Handler
}
 
//一下两个方法实现了 protoc生成go文件对应的接口：
/*
// BookServiceServer is the server API for BookService service.
type BookServiceServer interface {
	GetBookInfo(context.Context, *BookInfoParams) (*BookInfo, error)
	GetBookList(context.Context, *BookListParams) (*BookList, error)
}
*/
//通过grpc调用GetBookInfo时,GetBookInfo只做数据透传, 调用BookServer中对应Handler.ServeGRPC转交给go-kit处理
func (s *BookServer) GetBookInfo(ctx context.Context, in *book.BookInfoParams) (*book.BookInfo, error) {
 
	_, rsp, err := s.bookInfoHandler.ServeGRPC(ctx, in)
	if err != nil {
		return nil, err
 
	}
	/*
	if info,ok:=rsp.(*book.BookInfo);ok {
		return info,nil
	}
	return nil,errors.New("rsp.(*book.BookInfo)断言出错")
	*/
	return rsp.(*book.BookInfo), err //直接返回断言的结果
}
 
//通过grpc调用GetBookList时,GetBookList只做数据透传, 调用BookServer中对应Handler.ServeGRPC转交给go-kit处理
func (s *BookServer) GetBookList(ctx context.Context, in *book.BookListParams) (*book.BookList, error) {
	_, rsp, err := s.bookListHandler.ServeGRPC(ctx, in)
	if err != nil {
		return nil, err
	}
	return rsp.(*book.BookList), err
}
 
//创建bookList的EndPoint
func makeGetBookListEndpoint()endpoint.Endpoint  {
	return func(ctx context.Context, request interface{}) (response interface{}, err error) {
		b:=new(book.BookList)
		b.BookList=append(b.BookList,&book.BookInfo{BookId:1,BookName:"Go语言入门到精通"})
		b.BookList=append(b.BookList,&book.BookInfo{BookId:2,BookName:"微服务入门到精通"})
		b.BookList=append(b.BookList,&book.BookInfo{BookId:2,BookName:"区块链入门到精通"})
		return b,nil
	}
}
 
//创建bookInfo的EndPoint
func makeGetBookInfoEndpoint() endpoint.Endpoint {
	return func(ctx context.Context, request interface{}) (interface{}, error) {
		//请求详情时返回 书籍信息
		req := request.(*book.BookInfoParams)
		b := new(book.BookInfo)
		b.BookId = req.BookId
		b.BookName = "Go入门到精通"
		return b, nil
	}
}
 
func decodeRequest(_ context.Context, req interface{}) (interface{}, error) {
	return req, nil
}
 
func encodeResponse(_ context.Context, rsp interface{}) (interface{}, error) {
	return rsp, nil
}
 
func main() {
	var (
		etcdServer     = "127.0.0.1:2379"        //etcd服务的IP地址
		prefix         = "/services/book/"       //服务的目录
		ServerInstance = "127.0.0.1:50052"       //当前实例Server的地址
		key            = prefix + ServerInstance //服务实例注册的路径
		value          = ServerInstance
		ctx            = context.Background()
		//服务监听地址
		serviceAddress = ":50052"
	)
	//etcd连接参数
	option := etcdv3.ClientOptions{DialTimeout: time.Second * 3, DialKeepAlive: time.Second * 3}
	//创建连接
	client, err := etcdv3.NewClient(ctx, []string{etcdServer}, option)
	if err != nil {
		panic(err)
	}
	//创建注册
	registrar := etcdv3.NewRegistrar(client, etcdv3.Service{Key: key, Value: value}, log.NewNopLogger())
	registrar.Register() //启动注册服务
	bookServer := new(BookServer)
	bookListHandler := grpc_transport.NewServer(
		makeGetBookListEndpoint(),
		decodeRequest,
		encodeResponse,
	)
	bookServer.bookListHandler = bookListHandler
 
	bookInfoHandler := grpc_transport.NewServer(
		makeGetBookInfoEndpoint(),
		decodeRequest,
		encodeResponse,
	)
	bookServer.bookInfoHandler = bookInfoHandler
 
	listener, err := net.Listen("tcp", serviceAddress) //网络监听，注意对应的包为："net"
	if err != nil {
		fmt.Println(err)
		return
	}
	gs := grpc.NewServer(grpc.UnaryInterceptor(grpc_transport.Interceptor))
	book.RegisterBookServiceServer(gs, bookServer) //调用protoc生成的代码对应的注册方法
	gs.Serve(listener)                             //启动Server
 
}
````
* client
```golang
package main
 
import (
	"MyKit"
	"context"
	"fmt"
	"github.com/go-kit/kit/endpoint"
	"github.com/go-kit/kit/log"
	"github.com/go-kit/kit/sd"
	"github.com/go-kit/kit/sd/etcdv3"
	"github.com/go-kit/kit/sd/lb"
	"google.golang.org/grpc"
	"io"
	"time"
)

func main() {
 
	var (
		//注册中心地址
		etcdServer = "127.0.0.1:2379"
		//监听的服务前缀
		prefix = "/services/book/"
		ctx    = context.Background()
	)
	options := etcdv3.ClientOptions{
		DialTimeout:   time.Second * 3,
		DialKeepAlive: time.Second * 3,
	}
	//连接注册中心
	client, err := etcdv3.NewClient(ctx, []string{etcdServer}, options)
	if err != nil {
		panic(err)
	}
	logger := log.NewNopLogger()
	//创建实例管理器, 此管理器会Watch监听etc中prefix的目录变化更新缓存的服务实例数据
	instancer, err := etcdv3.NewInstancer(client, prefix, logger)
	if err != nil {
		panic(err)
	}
	//创建端点管理器， 此管理器根据Factory和监听的到实例创建endPoint并订阅instancer的变化动态更新Factory创建的endPoint
	endpointer := sd.NewEndpointer(instancer, reqFactory, logger) //reqFactory自定义的函数，主要用于端点层（endpoint）接受并显示数据
	//创建负载均衡器
	balancer := lb.NewRoundRobin(endpointer)
 
	/**
	我们可以通过负载均衡器直接获取请求的endPoint，发起请求
	reqEndPoint,_ := balancer.Endpoint()
	*/
 
	/**
	也可以通过retry定义尝试次数进行请求
	*/
	reqEndPoint := lb.Retry(3, 3*time.Second, balancer)
 
	//现在我们可以通过 endPoint 发起请求了
	req := struct{}{}
	if _, err = reqEndPoint(ctx, req); err != nil {
		panic(err)
	}
}
 
//通过传入的 实例地址  创建对应的请求endPoint
func reqFactory(instanceAddr string) (endpoint.Endpoint, io.Closer, error) {
	return func(ctx context.Context, request interface{}) (interface{}, error) {
		fmt.Println("请求服务: ", instanceAddr)
		conn, err := grpc.Dial(instanceAddr, grpc.WithInsecure())
		if err != nil {
			fmt.Println(err)
			panic("connect error")
		}
		defer conn.Close()
		bookClient := book.NewBookServiceClient(conn)
		bi, _ := bookClient.GetBookInfo(context.Background(), &book.BookInfoParams{BookId: 1})
		fmt.Println("获取书籍详情")
		fmt.Println("bookId: 1", " => ", "bookName:", bi.BookName)
 
		bl, _ := bookClient.GetBookList(context.Background(), &book.BookListParams{Page: 1, Limit: 10})
		fmt.Println("获取书籍列表")
		for _, b := range bl.BookList {
			fmt.Println("bookId:", b.BookId, " => ", "bookName:", b.BookName)
		}
		return nil, nil
	}, nil, nil
}
```

* 定义业务服务接口

```go
// StringService provides operations on strings.
type StringService interface {
	Uppercase(string) (string, error)
	Count(string) int
}
```
* 实现业务服务接口
```go
// stringService is a concrete implementation of StringService
type stringService struct{}

func (stringService) Uppercase(s string) (string, error) {
	if s == "" {
		return "", ErrEmpty
	}
	return strings.ToUpper(s), nil
}

func (stringService) Count(s string) int {
	return len(s)
}
```

* 实现 Endpoint

```go
// Endpoints are a primary abstraction in go-kit. An endpoint represents a single RPC (method in our service interface)
func makeUppercaseEndpoint(svc StringService) endpoint.Endpoint {
	return func(_ context.Context, request interface{}) (interface{}, error) {
		req := request.(uppercaseRequest)
		v, err := svc.Uppercase(req.S)
		if err != nil {
			return uppercaseResponse{v, err.Error()}, nil
		}
		return uppercaseResponse{v, ""}, nil
	}
}

func makeCountEndpoint(svc StringService) endpoint.Endpoint {
	return func(_ context.Context, request interface{}) (interface{}, error) {
		req := request.(countRequest)
		v := svc.Count(req.S)
		return countResponse{v}, nil
	}
}
```
* 启动服务
```go
// Transports expose the service to the network. In this first example we utilize JSON over HTTP.
func main() {
	svc := stringService{}

	uppercaseHandler := httptransport.NewServer(
		makeUppercaseEndpoint(svc),
		decodeUppercaseRequest,
		encodeResponse,
	)

	countHandler := httptransport.NewServer(
		makeCountEndpoint(svc),
		decodeCountRequest,
		encodeResponse,
	)

	http.Handle("/uppercase", uppercaseHandler)
	http.Handle("/count", countHandler)
	log.Fatal(http.ListenAndServe(":8080", nil))
}
```

## A gRPC server in Go kit

目录结构

```txt
.
├── cmd
│   └── main.go         # main entrypoint file          
├── endpoints
│   └── endpoints.go    # contains the endpoint definition
├── pb
│   ├── math.pb.go      # our gRPC generated code
│   └── math.proto      # our protobuf definitions
├── service
│   └── api.go          # contains the service's core business logic
└── transports
    └── grpc.go         # contains the gRPC transport
```

### 定义协议接口 Protobuf

```proto
syntax = "proto3";

option go_package = "github.com/dreamsxin/gokit-grpc-demo/pb";

service MathService {
  rpc Add(MathRequest) returns (MathResponse) {}
}

message MathRequest {
  float numA = 1;
  float numB = 2;
}

message MathResponse {
  float result = 1;
}
```

* 生成 Go 文件

```shell
protoc --go_out=plugins=grpc:. pb/math.proto
```

### 定义业务逻辑

```go
package service

import (
    "context"

    "github.com/go-kit/kit/log"
)

type service struct {
    logger log.Logger
}

// Service interface describes a service that adds numbers
type Service interface {
    Add(ctx context.Context, numA, numB float32) (float32, error)
}

// NewService returns a Service with all of the expected dependencies
func NewService(logger log.Logger) Service {
    return &service{
        logger: logger,
    }
}

// Add func implements Service interface
func (s service) Add(ctx context.Context, numA, numB float32) (float32, error) {
    return numA + numB, nil
}
```

### Endpoint

```go
package endpoints

import (
    "context"

    "github.com/go-kit/kit/endpoint"
    "github.com/junereycasuga/gokit-grpc-demo/service"
)

// Endpoints struct holds the list of endpoints definition
type Endpoints struct {
    Add endpoint.Endpoint
}

// MathReq struct holds the endpoint request definition
type MathReq struct {
    NumA float32
    NumB float32
}

// MathResp struct holds the endpoint response definition
type MathResp struct {
    Result float32
}

// MakeEndpoints func initializes the Endpoint instances
func MakeEndpoints(s service.Service) Endpoints {
    return Endpoints{
        Add: makeAddEndpoint(s),
    }
}

func makeAddEndpoint(s service.Service) endpoint.Endpoint {
    return func(ctx context.Context, request interface{}) (response interface{}, err error) {
        req := request.(MathReq)
        result, _ := s.Add(ctx, req.NumA, req.NumB)
        return MathResp{Result: result}, nil
    }
}
```

### Transport

```go
package transport

import (
    "context"

    "github.com/go-kit/kit/log"
    gt "github.com/go-kit/kit/transport/grpc"
    "github.com/junereycasuga/gokit-grpc-demo/endpoints"
    "github.com/junereycasuga/gokit-grpc-demo/pb"
)

type gRPCServer struct {
    add gt.Handler
}

// NewGRPCServer initializes a new gRPC server
func NewGRPCServer(endpoints endpoints.Endpoints, logger log.Logger) pb.MathServiceServer {
    return &gRPCServer{
        add: gt.NewServer(
            endpoints.Add,
            decodeMathRequest,
            encodeMathResponse,
        ),
    }
}

func (s *gRPCServer) Add(ctx context.Context, req *pb.MathRequest) (*pb.MathResponse, error) {
    _, resp, err := s.add.ServeGRPC(ctx, req)
    if err != nil {
        return nil, err
    }
    return resp.(*pb.MathResponse), nil
}

func decodeMathRequest(_ context.Context, request interface{}) (interface{}, error) {
    req := request.(*pb.MathRequest)
    return endpoints.MathReq{NumA: req.NumA, NumB: req.NumB}, nil
}

func encodeMathResponse(_ context.Context, response interface{}) (interface{}, error) {
    resp := response.(endpoints.MathResp)
    return &pb.MathResponse{Result: resp.Result}, nil
}
```

### Main entrypoint

```go
package main

import (
    "fmt"
    "net"
    "os"
    "os/signal"
    "syscall"

    "github.com/go-kit/kit/log"
    "github.com/go-kit/kit/log/level"
    "github.com/junereycasuga/gokit-grpc-demo/endpoints"
    "github.com/junereycasuga/gokit-grpc-demo/pb"
    "github.com/junereycasuga/gokit-grpc-demo/service"
    transport "github.com/junereycasuga/gokit-grpc-demo/transports"
    "google.golang.org/grpc"
)

func main() {
    var logger log.Logger
    logger = log.NewJSONLogger(os.Stdout)
    logger = log.With(logger, "ts", log.DefaultTimestampUTC)
    logger = log.With(logger, "caller", log.DefaultCaller)

    addservice := service.NewService(logger)
    addendpoint := endpoints.MakeEndpoints(addservice)
    grpcServer := transport.NewGRPCServer(addendpoint, logger)

    errs := make(chan error)
    go func() {
        c := make(chan os.Signal)
        signal.Notify(c, syscall.SIGINT, syscall.SIGTERM, syscall.SIGALRM)
        errs <- fmt.Errorf("%s", <-c)
    }()

    grpcListener, err := net.Listen("tcp", ":50051")
    if err != nil {
        logger.Log("during", "Listen", "err", err)
        os.Exit(1)
    }

    go func() {
        baseServer := grpc.NewServer()
        pb.RegisterMathServiceServer(baseServer, grpcServer)
        level.Info(logger).Log("msg", "Server started successfully 🚀")
        baseServer.Serve(grpcListener)
    }()

    level.Error(logger).Log("exit", <-errs)
}
```

## kitgen

生成代码

* 安装

```shell
go get github.com/nyarly/inlinefiles
cd $GOPATH/src/github.com/go-kit/kit/cmd/kitgen
go install

# Check installation by running:
kitgen -h
```

* 定义服务接口

```go
// service.go
package profilesvc // don't forget to name your package

type Service interface {
    PostProfile(ctx context.Context, p Profile) error
    // ...
}
type Profile struct {
    ID        string    `json:"id"`
    Name      string    `json:"name,omitempty"`
    // ...
}
```

* 生成代码

```shell
kitgen ./service.go
# kitgen has a couple of flags that you may find useful

# keep all code in the root directory
kitgen -repo-layout flat ./service.go

# put generated code elsewhere
kitgen -target-dir ~/go/src/myleft.org/kitchenservice/brewcoffee
```
