# 摘自

See https://www.cnblogs.com/hualou/category/1617965.html

## 基础入门

* 创建 UserService

```go
package Services

type IUserService interface {
    GetName(userid int) string
}

type UserService struct{}

func (this UserService) GetName(userid int) string {
    if userid == 101 {
        return "jerry"
    }
    return "guest"
}
```

* 创建UserEndPoint

```go
package Services

import (
    "context"
    "github.com/go-kit/kit/endpoint"
)

type UserRequest struct { //封装User请求结构体
    Uid int `json:"uid"`
}

type UserResponse struct {
    Result string `json:"result"`
}

func GenUserEnPoint(userService IUserService) endpoint.Endpoint {
    return func(ctx context.Context, request interface{}) (response interface{}, err error) {
        r := request.(UserRequest)           //通过类型断言获取请求结构体
        result := userService.GetName(r.Uid) //
        return UserResponse{Result: result}, nil
    }
}
```

* 创建UserTransport
```go
package Services

import (
    "context"
    "encoding/json"
    "errors"
    "net/http"
    "strconv"
)

func DecodeUserRequest(c context.Context, r *http.Request) (interface{}, error) { //这个函数决定了使用哪个request结构体来请求
    if r.URL.Query().Get("uid") != "" {
        uid, _ := strconv.Atoi(r.URL.Query().Get("uid"))
        return UserRequest{Uid: uid}, nil
    }
    return nil,errors.New("参数错误")/
}

func EncodeUserResponse(ctx context.Context,w http.ResponseWriter,response interface{}) error{
    w.Header().Set("Content-type","application/json") //设置响应格式为json，这样客户端接收到的值就是json，就是把我们设置的UserResponse给json化了

    return json.NewEncoder(w).Encode(response)//判断响应格式是否正确
}
```

* 搭建第一个http服务
```go
package main

import (
    "github.com/go-kit/kit/transport/http"
    "gomicro/Services"
)

func main() {
    user := Services.UserService{}
    endp := Services.GenUserEnPoint(user)

    serverHandler = http.NewServer(endp, Services.DecodeUserRequest, Services.EncodeUserResponse) // 使用go kit创建 serverHandler 传入我们之前定义的两个解析函数
}
```

* 让我们的服务支持外部路由

```go
package main

import (
    httptransport "github.com/go-kit/kit/transport/http"
    "github.com/gorilla/mux"
    "gomicro/Services"
    "net/http"
)

func DecodeUserRequest(c context.Context, r *http.Request) (interface{}, error) { //这个函数决定了使用哪个request来请求
    vars := mux.Vars(r) //通过这个返回一个map，map中存放的是参数key和值，因为我们路由地址是这样的/user/{uid:\d+}，索引参数是uid,访问Examp: http://localhost:8080/user/121，所以值为121
    if uid, ok := vars["uid"]; ok { //
        uid, _ := strconv.Atoi(uid)
        return UserRequest{Uid: uid}, nil
    }
    return nil, errors.New("参数错误")
}

func main() {
    user := Services.UserService{}
    endp := Services.GenUserEnPoint(user)

    serverHandler := httptransport.NewServer(endp, Services.DecodeUserRequest, Services.EncodeUserResponse) //使用go kit创建server传入我们之前定义的两个解析函数

    r := mux.NewRouter() //使用mux来使服务支持路由
    //r.Handle(`/user/{uid:\d+}`, serverHandler) //这种写法支持多种请求方法，访问Examp: http://localhost:8080/user/121便可以访问
    r.Methods("GET").Path(`/user/{uid:\d+}`).Handler(serverHandler) //这种写法限定了请求只支持GET方法
    http.ListenAndServe(":8080", r)

}
```

* 服务注册与发现Consul,简学API,手动注册和删除服务

```go
package main

import (
httptransport "github.com/go-kit/kit/transport/http"
mymux "github.com/gorilla/mux"
"gomicro/Services"
"net/http"
)

func main() {
    user := Services.UserService{}
    endp := Services.GenUserEnPoint(user)

    serverHandler := httptransport.NewServer(endp, Services.DecodeUserRequest, Services.EncodeUserResponse) //使用go kit创建server传入我们之前定义的两个解析函数

    r := mymux.NewRouter()
    //r.Handle(`/user/{uid:\d+}`, serverHandler) //这种写法支持多种请求方式
    r.Methods("GET", "DELETE").Path(`/user/{uid:\d+}`).Handler(serverHandler) //这种写法仅支持Get，限定只能Get请求
    r.Methods("GET").Path("/health").HandlerFunc(func(writer http.ResponseWriter, request *http.Request) {
        writer.Header().Set("Content-type", "application/json")
        writer.Write([]byte(`{"status":"ok"}`))
    })
    http.ListenAndServe(":8080", r)

}
```

* 编写注册函数

```go
package utils

import (
    consulapi "github.com/hashicorp/consul/api"
    "log"
)

func RegService() {
    config := consulapi.DefaultConfig()
    config.Address = "192.168.3.14:8500"
    reg := consulapi.AgentServiceRegistration{}
    reg.Name = "userservice" //注册service的名字
    reg.Address = "192.168.3.14" //注册service的ip
    reg.Port = 8080//注册service的端口
    reg.Tags = []string{"primary"}

    check := consulapi.AgentServiceCheck{} //创建consul的检查器
    check.Interval="5s" //设置consul心跳检查时间间隔
    check.HTTP = "http://192.168.3.14:8080/health" //设置检查使用的url

    reg.Check = &check

    client, err := consulapi.NewClient(config) //创建客户端
    if err != nil {
        log.Fatal(err)
    }
    err = client.Agent().ServiceRegister(&reg)
    if err != nil {
        log.Fatal(err)
    }
}
```

* 调用注册函数
```go
package main

import (
    httptransport "github.com/go-kit/kit/transport/http"
    mymux "github.com/gorilla/mux"
    "gomicro/Services"
    "gomicro/utils"
    "net/http"
)

func main() {
    user := Services.UserService{}
    endp := Services.GenUserEnPoint(user)

    serverHandler := httptransport.NewServer(endp, Services.DecodeUserRequest, Services.EncodeUserResponse) //使用go kit创建server传入我们之前定义的两个解析函数

    r := mymux.NewRouter()
    //r.Handle(`/user/{uid:\d+}`, serverHandler) //这种写法支持多种请求方式
    r.Methods("GET", "DELETE").Path(`/user/{uid:\d+}`).Handler(serverHandler) //这种写法仅支持Get，限定只能Get请求
    r.Methods("GET").Path("/health").HandlerFunc(func(writer http.ResponseWriter, request *http.Request) {
        writer.Header().Set("Content-type", "application/json")
        writer.Write([]byte(`{"status":"ok"}`))
    })
    utils.RegService() //调用注册服务程序
    http.ListenAndServe(":8080", r)

}
```
* 注册和反注册代码
```go
package utils

import (
    consulapi "github.com/hashicorp/consul/api"
    "log"
)

var ConsulClient *consulapi.Client

func init() {
    config := consulapi.DefaultConfig()
    config.Address = "192.168.3.14:8500"
    client, err := consulapi.NewClient(config) //创建客户端
    if err != nil {
        log.Fatal(err)
    }
    ConsulClient = client
}

func RegService() {
    reg := consulapi.AgentServiceRegistration{}
    reg.Name = "userservice"     //注册service的名字
    reg.Address = "192.168.3.14" //注册service的ip
    reg.Port = 8080              //注册service的端口
    reg.Tags = []string{"primary"}

    check := consulapi.AgentServiceCheck{}         //创建consul的检查器
    check.Interval = "5s"                          //设置consul心跳检查时间间隔
    check.HTTP = "http://192.168.3.14:8080/health" //设置检查使用的url
    reg.Check = &check

    err := ConsulClient.Agent().ServiceRegister(&reg)
    if err != nil {
        log.Fatal(err)
    }

}

func UnRegService() {
    ConsulClient.Agent().ServiceDeregister("userservice")
}
```
* service退出时优雅反注册service
```go
package main

import (
    "fmt"
    httptransport "github.com/go-kit/kit/transport/http"
    mymux "github.com/gorilla/mux"
    "gomicro/Services"
    "gomicro/utils"
    "log"
    "net/http"
    "os"
    "os/signal"
    "syscall"
)

func main() {
    user := Services.UserService{}
    endp := Services.GenUserEnPoint(user)

    serverHandler := httptransport.NewServer(endp, Services.DecodeUserRequest, Services.EncodeUserResponse) //使用go kit创建server传入我们之前定义的两个解析函数

    r := mymux.NewRouter()
    //r.Handle(`/user/{uid:\d+}`, serverHandler) //这种写法支持多种请求方式
    r.Methods("GET", "DELETE").Path(`/user/{uid:\d+}`).Handler(serverHandler)

    r.Methods("GET").Path("/health").HandlerFunc(func(writer http.ResponseWriter, request *http.Request) {
        writer.Header().Set("Content-type", "application/json")
        writer.Write([]byte(`{"status":"ok"}`))
    })//这种写法仅支持Get限定只能Get,DELETE请求
    errChan := make(chan error)
    go func() {
        utils.RegService() //调用注册服务程序
        err := http.ListenAndServe(":8080", r)
        if err != nil {
            log.Println(err)
            errChan <- err
        }
    }()
    go func() {
        sigChan := make(chan os.Signal)
        signal.Notify(sigChan, syscall.SIGINT, syscall.SIGTERM)
        errChan <- fmt.Errorf("%s", <-sigChan)
    }()
    getErr := <-errChan //只要报错 或者service关闭阻塞在这里的会进行下去
    utils.UnRegService()
    log.Println(getErr)
}
```

* TransPoint代码
```go
package Services

import (
    "context"
    "encoding/json"
    "errors"
    "net/http"
    "strconv"
)

func GetUserInfo_Request(_ context.Context, request *http.Request, r interface{}) error {
    user_request := r.(UserRequest)
    request.URL.Path += "/user/" + strconv.Itoa(user_request.Uid)
    return nil
}

func GetUserInfo_Response(_ context.Context, res *http.Response) (response interface{}, err error) {
    if res.StatusCode > 400 {
        return nil, errors.New("no data")
    }
    var user_response UserResponse
    err = json.NewDecoder(res.Body).Decode(&user_response)
    if err != nil {
        return nil, err
    }
    return user_response, err
}
```
* 调用代码
```go
package main

import (
    "context"
    "fmt"
    httptransport "github.com/go-kit/kit/transport/http"
    . "gomicro2/Services"
    "net/url"
    "os"
)
func main()  {
    tgt,_:=url.Parse("http://127.0.0.1:8080")
    //创建一个直连client，这里我们必须写两个func,一个是如何请求,一个是响应我们怎么处理
    client:=httptransport.NewClient("GET",tgt,GetUserInfo_Request,GetUserInfo_Response)

    getUserInfo:=client.Endpoint() //通过这个拿到了定义在服务端的endpoint也就是上面这段代码return出来的函数，直接在本地就可以调用服务端的代码

    ctx:=context.Background()//创建一个上下文

    //执行
    res,err:=getUserInfo(ctx,UserRequest{Uid:101}) //使用go-kit插件来直接调用服务
    if err!=nil{
        fmt.Println(err)
        os.Exit(1)
    }
    userinfo:=res.(UserResponse)
    fmt.Println(userinfo.Result)

}
```

* consul 获取服务实例

```go
package main

import (
    "context"
    "fmt"
    "github.com/go-kit/kit/endpoint"
    "github.com/go-kit/kit/log"
    "github.com/go-kit/kit/sd"
    "github.com/go-kit/kit/sd/consul"
    httptransport "github.com/go-kit/kit/transport/http"
    consulapi "github.com/hashicorp/consul/api"
    "gomicro2/Services"
    "io"
    "net/url"
    "os"
)

func main() {
    //第一步创建client
    {
        config := consulapi.DefaultConfig()
        config.Address = "localhost:8500"
        api_client, _ := consulapi.NewClient(config)
        client:= consul.NewClient(api_client)

        var logger log.Logger
        {
            logger = log.NewLogfmtLogger(os.Stdout)
            var Tag = []string{"primary"}
            //第二部创建一个consul的实例
            instancer := consul.NewInstancer(client, logger, "userservice", Tag, true) //最后的true表示只有通过健康检查的服务才能被得到
            {
                factory := func(service_url string) (endpoint.Endpoint, io.Closer, error) { //factory定义了如何获得服务端的endpoint,这里的service_url是从consul中读取到的service的address我这里是192.168.3.14:8000
                    tart, _ := url.Parse("http://" + service_url) //server ip +8080真实服务的地址
                    return httptransport.NewClient("GET", tart, Services.GetUserInfo_Request, Services.GetUserInfo_Response).Endpoint(), nil, nil //我再GetUserInfo_Request里面定义了访问哪一个api把url拼接成了http://192.168.3.14:8000/v1/user/{uid}的形式
                }
                endpointer := sd.NewEndpointer(instancer, factory, logger)
                endpoints, _ := endpointer.Endpoints()
                fmt.Println("服务有", len(endpoints), "条")
                getUserInfo := endpoints[0] //写死获取第一个
                ctx := context.Background() //第三步：创建一个context上下文对象

                //第四步：执行
                res, err := getUserInfo(ctx, Services.UserRequest{Uid: 101})
                if err != nil {
                    fmt.Println(err)
                    os.Exit(1)
                }
                //第五步：断言，得到响应值
                userinfo := res.(Services.UserResponse)
                fmt.Println(userinfo.Result)
            }
        }
    }

}
```

* 注册初始化代码
```go
package utils

import (
    "fmt"
    "github.com/google/uuid"
    consulapi "github.com/hashicorp/consul/api"
    "log"
    "strconv"
)

var ConsulClient *consulapi.Client
var ServiceID string
var ServiceName string
var ServicePort int

func init() {
    config := consulapi.DefaultConfig()
    config.Address = "192.168.3.14:8500"
    client, err := consulapi.NewClient(config) //创建客户端
    if err != nil {
        log.Fatal(err)
    }
    ConsulClient = client
    ServiceID = "userservice" + uuid.New().String() //因为最终这段代码是在不同的机器上跑的，是分布式的，有好几台机器提供相同的server，所以这里存到consul中的id必须是唯一的，否则只有一台服务器可以注册进去，这里使用uuid保证唯一性
}

func SetServiceNameAndPort(name string, port int) {
    ServiceName = name
    ServicePort = port
}

func RegService() {
    reg := consulapi.AgentServiceRegistration{}
    reg.ID = ServiceID //设置不同的Id，即使是相同的service name也得有不同的id
    reg.Name = ServiceName       //注册service的名字
    reg.Address = "localhost" //注册service的ip
    reg.Port = ServicePort       //注册service的端口
    fmt.Println(ServicePort)
    reg.Tags = []string{"primary"}

    check := consulapi.AgentServiceCheck{}                                      //创建consul的检查器
    check.Interval = "5s"                                                       //设置consul心跳检查时间间隔
    check.HTTP = "http://192.168.3.14:" + strconv.Itoa(ServicePort) + "/health" //设置检查使用的url
    fmt.Println(check.HTTP)
    reg.Check = &check

    err := ConsulClient.Agent().ServiceRegister(&reg)
    if err != nil {
        log.Fatal(err)
    }
}

func UnRegService() {
    ConsulClient.Agent().ServiceDeregister("userservice")
}
```
调用代码
```go
package main

import (
    "context"
    "fmt"
    "github.com/go-kit/kit/endpoint"
    "github.com/go-kit/kit/log"
    "github.com/go-kit/kit/sd"
    "github.com/go-kit/kit/sd/consul"
    httptransport "github.com/go-kit/kit/transport/http"
    consulapi "github.com/hashicorp/consul/api"
    "gomicro2/Services"
    "io"
    "net/url"
    "os"
)

func main() {
    //第一步创建client
    {
        config := consulapi.DefaultConfig()
        config.Address = "localhost:8500"
        api_client, _ := consulapi.NewClient(config)
        client:= consul.NewClient(api_client)

        var logger log.Logger
        {
            logger = log.NewLogfmtLogger(os.Stdout)
            var Tag = []string{"primary"}
            instancer := consul.NewInstancer(client, logger, "userservice", Tag, true) //最后的true表示只有通过健康检查的服务才能被得到
            {
                factory := func(service_url string) (endpoint.Endpoint, io.Closer, error) { //factory定义了如何获得服务端的endpoint,这里的service_url是从consul中读取到的service的address我这里是192.168.3.14:8000
                    tart, _ := url.Parse("http://" + service_url) //server ip +8080真实服务的地址
                    return httptransport.NewClient("GET", tart, Services.GetUserInfo_Request, Services.GetUserInfo_Response).Endpoint(), nil, nil //我在GetUserInfo_Request里面定义了访问哪一个api把url拼接成了http://192.168.3.14:8000/v1/user/{uid}的形式
                }
                endpointer := sd.NewEndpointer(instancer, factory, logger)
                endpoints, _ := endpointer.Endpoints()
                fmt.Println("服务有", len(endpoints), "条")
                getUserInfo := endpoints[0] //写死获取第一个
                ctx := context.Background() //第三步：创建一个context上下文对象

                //第四步：执行
                res, err := getUserInfo(ctx, Services.UserRequest{Uid: 101})
                if err != nil {
                    fmt.Println(err)
                    os.Exit(1)
                }
                //第五步：断言，得到响应值
                userinfo := res.(Services.UserResponse)
                fmt.Println(userinfo.Result)
            }
        }
    }

}
```

* 使用负载均衡

Client
```go
package main

import (
    "context"
    "fmt"
    "github.com/go-kit/kit/endpoint"
    "github.com/go-kit/kit/log"
    "github.com/go-kit/kit/sd"
    "github.com/go-kit/kit/sd/consul"
    "github.com/go-kit/kit/sd/lb"
    httptransport "github.com/go-kit/kit/transport/http"
    consulapi "github.com/hashicorp/consul/api"
    "gomicro2/Services"
    "io"
    "net/url"
    "os"
)

func main() {
    //第一步创建client
    {
        config := consulapi.DefaultConfig() //初始化consul的配置
        config.Address = "localhost:8500" //consul的地址
        api_client, _ := consulapi.NewClient(config) //根据consul的配置初始化client
        client := consul.NewClient(api_client) //根据client创建实例

        var logger log.Logger
        {
            logger = log.NewLogfmtLogger(os.Stdout)
            var Tag = []string{"primary"}
            instancer := consul.NewInstancer(client, logger, "userservice", Tag, true) //最后的true表示只有通过健康检查的服务才能被得到
            {
                factory := func(service_url string) (endpoint.Endpoint, io.Closer, error) { //factory定义了如何获得服务端的endpoint,这里的service_url是从consul中读取到的service的address我这里是192.168.3.14:8000
                    tart, _ := url.Parse("http://" + service_url)                                                                                 //server ip +8080真实服务的地址
                    return httptransport.NewClient("GET", tart, Services.GetUserInfo_Request, Services.GetUserInfo_Response).Endpoint(), nil, nil //我再GetUserInfo_Request里面定义了访问哪一个api把url拼接成了http://192.168.3.14:8000/v1/user/{uid}的形式
                }
                endpointer := sd.NewEndpointer(instancer, factory, logger)
                endpoints, _ := endpointer.Endpoints() //获取所有的服务端当前server的所有endpoint函数
                fmt.Println("服务有", len(endpoints), "条")

                mylb := lb.NewRoundRobin(endpointer) //使用go-kit自带的轮询
                for {
                    getUserInfo, err := mylb.Endpoint() //写死获取第一个
                    ctx := context.Background()         //第三步：创建一个context上下文对象

                    //第四步：执行
                    res, err := getUserInfo(ctx, Services.UserRequest{Uid: 101})
                    if err != nil {
                        fmt.Println(err)
                        os.Exit(1)
                    }
                    //第五步：断言，得到响应值
                    userinfo := res.(Services.UserResponse)
                    fmt.Println(userinfo.Result)
                }

            }
        }
    }

}
```
Server
```go
package main

import (
    "flag"
    "fmt"
    httptransport "github.com/go-kit/kit/transport/http"
    mymux "github.com/gorilla/mux"
    "gomicro/Services"
    "gomicro/utils"
    "log"
    "net/http"
    "os"
    "os/signal"
    "strconv"
    "syscall"
)

func main() {
    name := flag.String("name", "", "服务名称")
    port := flag.Int("port", 0, "服务端口")
    flag.Parse()
    if *name == "" {
        log.Fatal("请指定服务名")
    }
    if *port == 0 {
        log.Fatal("请指定端口")
    }
    utils.SetServiceNameAndPort(*name, *port) //设置服务名和端口

    user := Services.UserService{}
    endp := Services.GenUserEnPoint(user)
    serverHandler := httptransport.NewServer(endp, Services.DecodeUserRequest, Services.EncodeUserResponse) //使用go kit创建server传入我们之前定义的两个解析函数

    r := mymux.NewRouter()
    //r.Handle(`/user/{uid:\d+}`, serverHandler) //这种写法支持多种请求方式
    r.Methods("GET", "DELETE").Path(`/user/{uid:\d+}`).Handler(serverHandler) //这种写法仅支持Get，限定只能Get请求
    r.Methods("GET").Path("/health").HandlerFunc(func(writer http.ResponseWriter, request *http.Request) {
        writer.Header().Set("Content-type", "application/json")
        writer.Write([]byte(`{"status":"ok"}`))
    })
    errChan := make(chan error)
    go func() {
        utils.RegService()                                                 //调用注册服务程序
        err := http.ListenAndServe(":"+strconv.Itoa(utils.ServicePort), r) //启动http服务
        if err != nil {
            log.Println(err)
            errChan <- err
        }
    }()
    go func() {
        sigChan := make(chan os.Signal)
        signal.Notify(sigChan, syscall.SIGINT, syscall.SIGTERM)
        errChan <- fmt.Errorf("%s", <-sigChan)
    }()
    getErr := <-errChan
    utils.UnRegService()
    log.Println(getErr)
}
```
