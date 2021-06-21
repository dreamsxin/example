# go-ki

https://github.com/go-kit/kit

## Demo

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
