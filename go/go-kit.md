# go-kit 入门

为了了解 [go-kit](https://github.com/go-kit/kit) 的用法及以后的源码解析做准备，先从最简单的实例开始。

## 简单使用

微服务是为了让开发人员能更专注业务的实现及系统解耦合（自己的见解）。[go-kit](https://github.com/go-kit/kit) 是怎么做的呢？
- 专注业务服务实现
- 通过组合把业务服务发布出去

### 业务实现

#### 定义业务服务接口
```go
// StringService provides operations on strings.
type StringService interface {
	Uppercase(string) (string, error)
	Count(string) int
}
```
#### 实现业务服务接口
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

### 通过组合把业务服务发布出去

#### 实现 Endpoint

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
#### 执行（这里通过 HTTP 发布业务服务）
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

## 小结

使用 [go-kit](https://github.com/go-kit/kit) 写代码很有函数式编程的感觉，分离关注点，并通过组合方式把整个流程组合起来。

## 参考

https://github.com/go-kit/kit/blob/master/examples/stringsvc1/main.go

