## log

```go
log.SetPrefix("TRACE: ")
log.SetFlags(log.Ldate | log.Lmicroseconds | log.Llongfile)
//log.SetFlags(log.LstdFlags | log.Lshortfile)
    
logFileLocation, _ := os.OpenFile("/var/log/test.log", os.O_CREATE|os.O_APPEND|os.O_RDWR, 0744)
log.SetOutput(logFileLocation)
    
log.Printf("Debug %s", "hello")
log.Fatal("Error %s", "exit") //defer函数不会执行
```

## zap

https://github.com/uber-go/zap
非常快的、结构化的，分日志级别的Go日志库。

同时提供了结构化日志记录和printf风格的日志记录
根据Uber-go Zap的文档，它的性能比类似的结构化日志包更好——也比标准库更快。 以下是Zap发布的基准测试信息

* 安装
```shell
go get -u go.uber.org/zap
```

* 配置
Zap提供了两种类型的日志记录器：Sugared Logger和Logger。

在性能很好但不是很关键的上下文中，使用SugaredLogger。它比其他结构化日志记录包快4-10倍，并且支持结构化和printf风格的日志记录。

在每一微秒和每一次内存分配都很重要的上下文中，使用Logger。它甚至比SugaredLogger更快，内存分配次数也更少，但它只支持强类型的结构化日志记录。

* 创建 Logger
通过调用zap.NewProduction()/zap.NewDevelopment()或者zap.Example()创建一个Logger。
上面的每一个函数都将创建一个logger。唯一的区别在于它将记录的信息不同。例如production logger默认记录调用函数信息、日期和时间等。
通过Logger调用Info/Error等。
默认情况下日志都会打印到应用程序的console界面。
