## log

```go
log.SetPrefix("TRACE: ")
log.SetFlags(log.Ldate | log.Lmicroseconds | log.Llongfile)
//log.SetFlags(log.LstdFlags | log.Lshortfile)
    
logFileLocation, _ := os.OpenFile("/var/log/test.log", os.O_CREATE|os.O_APPEND|os.O_RDWR, 0744)
log.SetOutput(logFileLocation)
    
log.Printf("Debug v %s", "hello")
log.Fatal("Error %s", "exit") //defer函数不会执行
```
多文件写入
```go
func main() {
   f, err := os.OpenFile("log.log", os.O_CREATE|os.O_APPEND|os.O_RDWR, os.ModePerm)
   if err != nil {
      return
   }
   defer func() {
      f.Close()
   }()

   // 组合一下即可，os.Stdout代表标准输出流
   multiWriter := io.MultiWriter(os.Stdout, f)
   log.SetOutput(multiWriter)

   log.SetFlags(log.Ldate | log.Ltime | log.Lshortfile)

   log.Println("line1")
   log.Print("line2")
   log.Printf("line%d \n", 3)
}
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

Zap提供了两种类型的日志记录器：Sugared Logger和Logger。

在性能很好但不是很关键的上下文中，使用SugaredLogger。它比其他结构化日志记录包快4-10倍，并且支持结构化和printf风格的日志记录。

在每一微秒和每一次内存分配都很重要的上下文中，使用Logger。它甚至比SugaredLogger更快，内存分配次数也更少，但它只支持强类型的结构化日志记录。

* 使用
通过调用zap.NewProduction()/zap.NewDevelopment()或者zap.Example()创建一个Logger。
上面的每一个函数都将创建一个logger。唯一的区别在于它将记录的信息不同。例如production logger默认记录调用函数信息、日期和时间等。
通过Logger调用Info/Error等。
默认情况下日志都会打印到应用程序的console界面。

`func (log *Logger) Info(msg string, fields ...Field)`

- `Info` 是一个可变参数函数，可以是`Info / Error/ Debug / Panic`等。每个方法都接受一个消息字符串和任意数量的`zapcore.Field`场参数。
- `zapcore.Field` 是一组键值对参数。
```go
logger, _ = zap.NewProduction()
logger.Info("Debug", zap.String("v", "hello"))
logger.Error("Error", zap.Error(err))
```
* Sugared Logger

```go
logger, _ := zap.NewProduction() //通过调用主 `logger.Sugar()` 方法来获取一个SugaredLogger。
sugarLogger = logger.Sugar()

sugarLogger.Debugf("v %s", "hello")
sugarLogger.Infof("v %s", "world")
sugarLogger.Errorf("Error %s", err)
```

* 配置

```go
file, _ := os.Create("/var/log/zap.log")
writeSyncer := zapcore.AddSync(file)

encoderConfig := zap.NewProductionEncoderConfig()
encoderConfig.EncodeTime = zapcore.ISO8601TimeEncoder
encoderConfig.EncodeLevel = zapcore.CapitalLevelEncoder
    
encoder := zapcore.NewJSONEncoder(zap.NewProductionEncoderConfig()) //zapcore.NewConsoleEncoder(zap.NewProductionEncoderConfig())
core := zapcore.NewCore(encoder, writeSyncer, zapcore.DebugLevel)

logger := zap.New(core) //logger := zap.New(core, zap.AddCaller())
sugarLogger = logger.Sugar()
```

## 日志切割归档

Lumberjack Logger采用以下属性作为输入:

- Filename: 日志文件的位置
- MaxSize：在进行切割之前，日志文件的最大大小（以MB为单位）
- MaxBackups：保留旧文件的最大个数
- MaxAges：保留旧文件的最大天数
- Compress：是否压缩/归档旧文件

* 安装
```shell
go get -u github.com/natefinch/lumberjack
```

```go
//默认
log.SetOutput(&lumberjack.Logger{
    Filename:   "/var/log/myapp/foo.log",
    MaxSize:    500, // megabytes
    MaxBackups: 3,
    MaxAge:     28, //days
    Compress:   true, // disabled by default
})

//zap
lumberJackLogger := &lumberjack.Logger{
    Filename:   "./test.log",
    MaxSize:    10,
    MaxBackups: 5,
    MaxAge:     30,
    Compress:   false,
}
writeSyncer := zapcore.AddSync(lumberJackLogger)
```
    
