# pprof

全称是 performance profiles，是 google 官方提供的性能分析工具，项目地址：`https://github.com/google/pprof`。
配合 Graphviz 使用可以提供图形化的能力。

使用它的方式很简单，配合pprof 库来使用。只要在代码里增加两块代码。一块是 import:
```go
_ "net/http/pprof"

go func() {
  http.ListenAndServe("0.0.0.0:8899", nil)  //ip和端口可以更换
}()
```
然后就可以在浏览器里输入 `http://localhost:8899/debug/pprof/` 看到相关的性能维度

"_" 执行 init 如下：
```go
func init() {
	http.HandleFunc("/debug/pprof/", Index)
	http.HandleFunc("/debug/pprof/cmdline", Cmdline)
	http.HandleFunc("/debug/pprof/profile", Profile)
	http.HandleFunc("/debug/pprof/symbol", Symbol)
	http.HandleFunc("/debug/pprof/trace", Trace)
}
```

## Gin

https://github.com/gin-contrib/pprof

下载数据到本地
```shell
go tool pprof http://192.168.1.27:8080/debug/pprof/allocs
go tool pprof http://192.168.1.27:8080/debug/pprof/block
go tool pprof http://192.168.1.27:8080/debug/pprof/cmdline
go tool pprof -inuse_space --seconds=120 http://192.168.1.27:8080/debug/pprof/heap
go tool pprof http://192.168.1.27:8080/debug/pprof/mutex
go tool pprof http://192.168.1.27:8080/debug/pprof/profile
go tool pprof http://192.168.1.27:8080/debug/pprof/threadcreate
go tool pprof http://192.168.1.27:8080/debug/pprof/trace
```
开启本地服务
```shell
go tool pprof -http=192.168.1.27:8081 pprof.server.alloc_objects.alloc_space.inuse_objects.inuse_space.001.pb.gz
go tool pprof -http=192.168.1.27:8081 http://192.168.1.27:8080/debug/pprof/allocs
```

```go
package main

import (
  "github.com/gin-contrib/pprof"
  "github.com/gin-gonic/gin"
)

func main() {
  router := gin.Default()
  // default is "debug/pprof"
  pprof.Register(router)
  //pprof.Register(router, "dev/pprof")

  router.Run(":8080")
}
```

## 使用 go tool pprof 分析数据

有两种方式：

- 通过url
`go tool pprof http://localhost:8899/debug/pprof/profile`
`go tool pprof http://localhost:8899/debug/pprof/heap`
`go tool pprof https://localhost/debug/pprof/heap?debug=1&seconds=10`

进去之后输入「top」，就能很直观的看到哪个方法占用了内存。
我们可以再输入获得更详细的信息：
`list main.main`

- 通过文件 `go tool pprof cpuprofile 文件路径`
```shell
echo "ulimit -c unlimited" >> ~/.profile
echo "export GOTRACEBACK=crash " >> ~/.profile
export GOBACTRACE=crash
```

### 图形 graphviz

- https://www.graphviz.org/

### pprof 字段

allocs（内存分配采样）
描述：记录程序中所有过去的内存分配。allocs 不是针对某个特定时间点的快照，而是通过定期采样程序的内存分配来获取信息。
用途：帮助分析程序的内存使用情况，找出高频的内存分配和可能的内存泄漏。可以通过查看分配的内存块，了解哪些部分的代码导致了过多的内存分配。
示例：通过 pprof 你可以获取内存分配的历史采样数据，找到内存使用的瓶颈。

block（阻塞分析）
描述：记录导致阻塞的栈跟踪信息，通常与同步原语（如 sync.Mutex、sync.Cond 等）相关。阻塞是指某个 goroutine 因等待锁、条件变量等原因无法继续执行。
用途：帮助分析程序中阻塞的 goroutine，尤其是锁争用和并发问题。通过分析 block profile，你可以查看哪些同步操作导致了程序性能下降。
示例：如果程序存在多个 goroutine 并发访问共享资源且存在锁竞争，block profile 会显示这些锁争用和阻塞的堆栈信息。

cmdline（命令行）
描述：记录当前程序的命令行启动参数。包括程序的所有命令行参数。
用途：有助于了解当前运行的程序是如何启动的，哪些命令行参数被传递给程序。这对分析程序执行环境和上下文非常重要。
示例：可以帮助你识别程序的启动方式，比如使用了哪些配置文件、环境变量或特定的运行参数。

goroutine（goroutine栈分析）
描述：记录当前所有 goroutine 的栈跟踪信息。每个 goroutine 可能处于不同的状态（例如：运行中、等待中或阻塞中）。
用途：帮助分析程序中的并发行为，尤其是追踪那些长时间运行或阻塞的 goroutine。可以帮助发现死锁、锁竞争等并发问题。
示例：当程序有大量 goroutine 时，通过查看 goroutine profile，你可以看到所有 goroutine 的调用栈，并找出哪些 goroutine 在等待、阻塞或占用大量资源。

heap（堆内存分析）
描述：对程序中当前存活的对象进行内存分配采样。heap profile 给出了程序在某一时刻堆上所有活跃对象的内存使用情况。
用途：用于分析堆内存的使用情况，特别是帮助识别内存泄漏或不必要的内存分配。通过 heap profile，开发者可以找出程序中使用最多内存的对象。
示例：如果程序内存消耗过高，使用 heap profile 可以帮助找出内存使用最多的地方，或者确认是否存在未被释放的内存。

mutex（互斥锁分析）
描述：记录持有并争用互斥锁的 goroutine 栈跟踪。mutex profile 显示了锁争用情况，包括哪些 goroutine 当前持有锁，哪些 goroutine 在等待锁。
用途：帮助分析程序中的锁竞争和同步问题。对于性能瓶颈，特别是锁争用较为严重的程序，可以使用 mutex profile 来找出锁的争用点。
示例：如果有多个 goroutine 在争夺同一个锁，mutex profile 会显示哪个 goroutine 持有锁，哪些正在等待，从而帮助开发者优化锁的使用。

profile（CPU分析）
描述：记录程序的 CPU 性能分析数据。可以指定采样的持续时间（通过 GET 参数中的 duration），profile 是常用的性能分析工具，帮助定位 CPU 的瓶颈。
用途：用于分析程序的 CPU 使用情况，找出消耗大量 CPU 时间的函数或代码路径。常见的用途是分析程序中消耗最多时间的部分，并进行性能优化。
示例：通过 profile，你可以获得一个 CPU profile 文件，并使用 go tool pprof 命令来分析哪个函数消耗了最多的 CPU 时间。

threadcreate（线程创建分析）
描述：记录导致创建新操作系统线程的栈跟踪。操作系统线程是由 Go 运行时系统管理的线程，Go 会根据需要动态创建操作系统线程。
用途：帮助分析程序中是否存在大量的线程创建，可能影响性能。过多的线程创建可能导致上下文切换频繁，从而影响程序的整体性能。
示例：通过 threadcreate profile，你可以发现哪些操作会触发新的线程创建，并查看线程的堆栈信息。

trace（程序执行追踪）
描述：记录程序的执行轨迹。通过 trace，可以跟踪程序的所有执行过程，详细记录 goroutine 的调度、阻塞等事件。
用途：帮助分析程序的执行流程，查看 goroutine 的调度情况，尤其是在并发程序中，分析并发和同步的问题。
示例：使用 trace 你可以查看程序在特定时间段内的所有调度事件，包括 goroutine 的启动、调度、阻塞等情况。

##

https://github.com/cloudwego/goref
