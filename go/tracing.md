# 通过实例理解 Go Execution Tracer

在 Go 工具链中，go tool pprof（与runtime/pprof或net/http/pprof联合使用）便是一个基于采样（sampling）的性能剖析(profiing)辅助工具。它基于定时器对运行的go程序进行各种采样，包括诸如CPU时间、内存分配等方面。但go pprof也具有上面所说的基于采样的工具的不足，那就是采样的频度不足导致的精确性问题，在Go运行时内部，CPU分析使用操作系统计时器来定期（每秒约100次，即10ms一次）中断执行。在每个中断（也称为样本）上，它同时收集当时的调用堆栈。当为了实现更高频度采样时（比如微秒级别的采样），目前的go profile无法支持（为此uber工程师提了一个名为pprof++的高精度、更精确并支持硬件监控的提案）。

Go 语言同样也提供了基于追踪（tracing）策略的工具，一旦开启trace，Go应用中发生的所有特定事件（event）便会被记录下来，并支持将其保存在文件中以备后续分析，这个工具由谷歌工程师Dmitry Vyukov提出设计方案并实现，并在Go 1.5版本发布时加入Go工具链，这个工具被称为Go Execution Tracer，中文直译就是Go执行跟踪器。

## 为 Go 应用添加 Tracer


### 手工通过runtime/trace包在Go应用中开启和关闭Tracer
无论使用哪一种方法，runtime/trace 包都是基础与核心。我们可以直接使用runtime/trace包提供的API在Go应用中手工开启和关闭Tracer：
```go
package main

import (
    "os"
    "runtime/trace"
)

func main() {
    trace.Start(os.Stdout)
    defer trace.Stop()
    // 下面是业务代码
    ... ...
}

func main() {
    f, _ := os.Create("trace.out")
    defer f.Close()
    trace.Start(f)
    defer trace.Stop()
    // 下面是业务代码
    ... ...
}
```

### 通过net/http/pprof提供基于http进行数据传输的Tracer服务
如果一个Go应用通过net/http/pprof包提供对pprof采样的支持，那么我们就可以像获取cpu或heap profile数据那样，通过/debug/pprof/trace端点来开启Tracer并获取Tracer数据：
```shell
$wget -O trace.out http://localhost:6060/debug/pprof/trace?seconds=5
net/http/pprof包中的Trace函数负责处理发向/debug/pprof/trace端点的http请求，见下面代码：
```
```go
// $GOROOT/src/net/http/pprof/pprof.go
func Trace(w http.ResponseWriter, r *http.Request) {
    w.Header().Set("X-Content-Type-Options", "nosniff")
    sec, err := strconv.ParseFloat(r.FormValue("seconds"), 64)
    if sec <= 0 || err != nil {
        sec = 1
    }

    if durationExceedsWriteTimeout(r, sec) {
        serveError(w, http.StatusBadRequest, "profile duration exceeds server's WriteTimeout")
        return
    }

    // Set Content Type assuming trace.Start will work,
    // because if it does it starts writing.
    w.Header().Set("Content-Type", "application/octet-stream")
    w.Header().Set("Content-Disposition", `attachment; filename="trace"`)
    if err := trace.Start(w); err != nil {
        // trace.Start failed, so no writes yet.
        serveError(w, http.StatusInternalServerError,
            fmt.Sprintf("Could not enable tracing: %s", err))
        return
    }
    sleep(r, time.Duration(sec*float64(time.Second)))
    trace.Stop()
}
```
我们看到在该处理函数中，函数开启了Tracer：trace.Start，并直接将w作为io.Writer的实现者传给了trace.Start函数，接下来Tracer采集的数据便会源源不断地通过http应答发回客户端，处理完后，Trace函数关闭了Tracer。

我们看到通过这种方式实现的动态开关Tracer是相对理想的一种方式，生产环境可以采用这种方式，这样可以将Tracer带来的开销限制在最小范围。

### 通过go test -trace获取Tracer数据
如果要在测试执行时开启Tracer，我们可以通过go test -trace来实现：

```shell
go test -trace trace.out ./...
```
命令执行结束后，trace.out中便存储了测试执行过程中的Tracer数据，后续我们可以用go tool trace对其进行展示和分析。

### Tracer数据分析
有了Tracer输出的数据后，我们接下来便可以使用go tool trace工具对存储Tracer数据的文件进行分析了：

$go tool trace trace.out
go tool trace会解析并验证Tracer输出的数据文件，如果数据无误，它接下来会在默认浏览器中建立新的页面并加载和渲染这些数据，如下图所示：

## User annotation

```go
trace.WithRegion(ctx, "makeCappuccino", func() {

   // orderID allows to identify a specific order
   // among many cappuccino order region records.
   trace.Log(ctx, "orderID", orderID)

   trace.WithRegion(ctx, "steamMilk", steamMilk)
   trace.WithRegion(ctx, "extractCoffee", extractCoffee)
   trace.WithRegion(ctx, "mixMilkCoffee", mixMilkCoffee)
})

ctx, task := trace.NewTask(ctx, "makeCappuccino")
trace.Log(ctx, "orderID", orderID)

milk := make(chan bool)
espresso := make(chan bool)

go func() {
        trace.WithRegion(ctx, "steamMilk", steamMilk)
        milk <- true
}()
go func() {
        trace.WithRegion(ctx, "extractCoffee", extractCoffee)
        espresso <- true
}()
go func() {
        defer task.End() // When assemble is done, the order is complete.
        <-espresso
        <-milk
        trace.WithRegion(ctx, "mixMilkCoffee", mixMilkCoffee)
}()
```
