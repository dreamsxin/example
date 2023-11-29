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

## 使用 go tool pprof 分析数据

有两种方式：

- 通过url
`go tool pprof http://localhost:8899/debug/pprof/profile`
`go tool pprof http://localhost:8899/debug/pprof/heap`
进去之后输入「top」，就能很直观的看到哪个方法占用了内存。
我们可以再输入获得更详细的信息：
`list main.main`

- 通过文件 `go tool pprof cpuprofile 文件路径`
```shell
echo "ulimit -c unlimited" >> ~/.profile
echo "export GOTRACEBACK=crash " >> ~/.profile
export GOBACTRACE=crash
```
