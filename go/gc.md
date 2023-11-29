## 常见的几种gc算法

### 引用计数
对每个对象维护一个引用计数，当引用该对象的对象被销毁时，引用计数减1，当引用计数器为0是回收该对象。

优点：对象可以很快的被回收，不会出现内存耗尽或达到某个阀值时才回收。

缺点：不能很好的处理循环引用，而且实时维护引用计数，有也一定的代价。

代表语言：Python、PHP、Swift

### 标记-清除
从根变量开始遍历所有引用的对象，引用的对象标记为"被引用"，没有被标记的进行回收。

优点：解决了引用计数的缺点。

缺点：需要STW，即要暂时停掉程序运行。

代表语言：Golang(其采用三色标记法)

### 分代收集

按照对象生命周期长短划分不同的代空间，生命周期长的放入老年代，而短的放入新生代，不同代有不能的回收算法和回收频率。

优点：回收性能好

缺点：算法复杂

代表语言： JAVA

每种算法都不是完美的，都是折中的产物。

## 优化GC的开销

Go GC触发时机大体分为三种：

- 手动触发：调用runtime.GC()
- 常规触发：Target heap memory = Live heap + (Live heap + GC roots) * GOGC / 100
- sysmon后台周期性强制触发GC

而常规触发的公式中，可以调整的只有GOGC这个参数(等价于debug.SetGCPercent())。GOGC默认值为100，也就是说当新分配heap内存的数量是上一周期的活跃heap内存的一倍的时候触发GC。

如果我们将GOGC改为200，那么GC的触发间隔将增加，频度会下降，CPU开销会降低(6.4%->3.8%)。

但修改 GOGC 有风险，可以使用 ballast(压舱石)技术，其原理就是在程序初始化时先分配一块大内存：
```go
func main() {

 // Create a large heap allocation of 10 GiB
 ballast := make([]byte, 10<<30)

 // Application execution continues
 // ...
 runtime.KeepAlive(ballast) // make sure the ballast won't be collected 
}
```
Go 1.19 版本引入了 `Soft memory limit`，这个方案在 `runtime/debug` 包中添加了一个名为 `SetMemoryLimit` 的函数以及GOMEMLIMIT环境变量，通过他们任意一个都可以设定Go应用的 `Memory limit`。

### 减少堆内存的分配和释放
GC开销大的根源在于 heap object 多，Go 的每轮 GC 都是 FullGC，每轮都要将所有 heap object 标记(mark)一遍，即便大多数heap object都是长期alive的，因此，一个直观的降低GC开销的方法就是减少 heap object 的数量，即减少 alloc。
沿着这样的思路，我们可以很直接的想出如下两种手段：
- 把小对象聚合到一个结构体中，然后做一次分配即可
- 重用 - sync.Pool
  
