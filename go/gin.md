# gin

## 使用流程

基于 `net/http` 开发 `web` 服务的一般流程:

利用 `http.HandleFunc` 函数注册路由，并指定处理函数，函数签名为 `func(w http.ResponseWriter, r *http.Request)`

设置响应状态码、设置响应头(如Content-Type)、处理结果序列化并写入响应体
调用http.ListenAndServer进行端口监听
```go
func main() {
    //注册路由
    http.HandleFunc("/time", func(w http.ResponseWriter, r *http.Request) {
        //需要在处理函数内部获取查询参数、路径参数、读取请求体并反序列化
        //设置响应状态码、设置响应头(如Content-Type)、处理结果序列化并写入响应体
        w.WriteHeader(http.StatusOK)
        w.Header().Set("Content-Type", "application/json")
        //业务逻辑处理
        tim := map[string]string{
            "time": time.Now().Format("2006-01-02"),
        }
        byts, err := json.Marshal(tim)
        if err != nil {
            //错误处理(设置错误响应状态码、错误信息等)
            panic(err)
        }
        w.Write(byts)
    })
    //调用http.ListenAndServer进行端口监听
    http.ListenAndServe("0.0.0.0:8080", nil)
}
```
gin 封装简化了操作：
```go
func main() {
    mux := gin.Default()
    mux.GET("/time", func(c *gin.Context) {
        m := map[string]string{
            "time": time.Now().Format("2006-01-02"),
        }
        c.JSON(http.StatusOK, m)
    })
    err := mux.Run("0.0.0.0:8080")
    if err != nil {
        panic(err)
    }
}
```

**总结**

1. 创建gin.Engine、注册middleware
2. 注册路由，编写处理函数，在函数内通过gin.Context获取参数，进行逻辑处理，通过gin.Context暴露的方法(如JSON())写回输出
3. 监听端口

## 内置中间件
在使用Gin框架开发Web应用时，常常需要自定义中间件，不过，Gin也内置一些中间件，我们可以直接使用，下面是内置中间件列表：
```go
func BasicAuth(accounts Accounts) HandlerFunc
func BasicAuthForRealm(accounts Accounts, realm string) HandlerFunc
func Bind(val interface{}) HandlerFunc
func ErrorLogger() HandlerFunc
func ErrorLoggerT(typ ErrorType) HandlerFunc
func Logger() HandlerFunc
func LoggerWithConfig(conf LoggerConfig) HandlerFunc
func LoggerWithFormatter(f LogFormatter) HandlerFunc
func LoggerWithWriter(out io.Writer, notlogged ...string) HandlerFunc
func Recovery() HandlerFunc  //自动捕获应用程序中的 panic 异常，防止程序崩溃，并返回500错误码，同时在控制台输出错误信息。
func RecoveryWithWriter(out io.Writer) HandlerFunc
func WrapF(f http.HandlerFunc) HandlerFunc
func WrapH(h http.Handler) HandlerFunc
```

## 路由注册流程

### 核心数据结构

#### gin.Engine
```go
type Handler interface {
    ServeHTTP(ResponseWriter, *Request)
}
```
`gin.Engine` 对象其实就是 `Handler` 接口的一个实现
`http.ListenAndServe` 函数的第二个参数是 `Handler` 接口类型，只要实现了该接口的 `ServeHTTP(ResponseWriter, *Request)` 方法，就能够对请求进行自定义处理。

```go
func (engine *Engine) ServeHTTP(w http.ResponseWriter, req *http.Request) {
  ...
}

func Default() *Engine {
    debugPrintWARNINGDefault() // 检测go版本是否在 1.14+
    engine := New() // 创建 Engine 对象
    engine.Use(Logger(), Recovery()) // 其实是先调用 New 方法，再调用 Use 注册 middleware，这里先忽略。
    return engine
}

func New() *Engine {
  ...
    engine := &Engine{
        // NOTE: 实例化RouteGroup，路由管理相关(Engine自身也是一个RouterGroup)
        RouterGroup: RouterGroup{
            Handlers: nil,
            basePath: "/",
            root:     true,
        },
        ...
        // NOTE: 负责存储路由和处理方法的映射，采用类似字典树的结构(这里构造了几棵树，每棵树对应一个http请求方法)
        trees:            make(methodTrees, 0, 9),
        ...
    }
  ...
    // NOTE: 基于sync.Pool实现的context池，能够避免context频繁销毁和重建
    engine.pool.New = func() any {
        return engine.allocateContext(engine.maxParams)
    }
    return engine
}
```

### 模板调试实时渲染

```go
	r.FuncMap = template.FuncMap{
		"html": func(s string) template.HTML {
			return template.HTML(s)
		},
		"upper": strings.ToUpper,
		"lower": strings.ToLower,
		"strip": func(str string) string {
			return strings.ReplaceAll(strip.StripTags(str), "&nbsp;", "")
		},
	}

	filenames := []string{}
	filenames1, err := filepath.Glob("./includes/*.html")
	if err == nil {
		filenames = append(filenames, filenames1...)
	}
	filenames2, err := filepath.Glob("./*.html")

	if err == nil {
		filenames = append(filenames, filenames2...)
	}
	if cfg.Server.Mode == "debug" {
		gin.SetMode(gin.DebugMode)
	}
```
