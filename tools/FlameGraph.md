# 火焰图

软件的性能分析，往往需要查看 `CPU` 耗时，了解瓶颈在哪里。
火焰图是基于 `stack` 信息生成的 `SVG` 图片，用来展示 `CPU` 的调用栈。火焰图整个看起来就像一团跳动的火焰，这也正是其名字的由来。
火焰图的颜色没有特殊含义，因为火焰图表示的是 `CPU` 的繁忙程度，所以一般选择暖色调。
因为调用栈在横向会按照函数名字母排序，并且同样的调用栈会做合并（所以 `x` 轴代表总执行时间长短，不表示先后顺序）。
火焰图就是看顶层的哪个函数占据的宽度最大。只要有 "平顶"(`plateaus`)，就表示该函数可能存在性能问题。

`y` 轴表示调用栈，每一层都是一个函数。 调用栈越深，火焰就越高，顶部就是正在执行的函数，下方都是它的父函数。
`x` 轴表示抽样数，如果一个函数在 `x` 轴占据的宽度越宽，就表示它被抽到的次数多，即执行的时间长。 

## 火焰图类型

- [`On-CPU`](http://www.brendangregg.com/FlameGraphs/cpuflamegraphs.html)
- [`Off-CPU`](http://www.brendangregg.com/FlameGraphs/offcpuflamegraphs.html)
- [`Memory`](http://www.brendangregg.com/FlameGraphs/memoryflamegraphs.html)
- [`Hot/Cold`](http://www.brendangregg.com/FlameGraphs/hotcoldflamegraphs.html)
- [`Differential`](http://www.brendangregg.com/blog/2014-11-09/differential-flame-graphs.html)


要生成火焰图所需的堆栈信息，`Linux` 的话，那么选择通常是

- perf
跟踪事件从kernerl到user space，支持块设备、网络、CPU、文件系统、内存等，同时还支持系统调用，用户库的事件。
- stap
- dtrace

## 火焰图可视化生成器 Flame Graph

用 `git` 将其 `clone`下来

```cpp
git clone https://github.com/brendangregg/FlameGraph.git
```

生成和创建火焰图需要如下几个步骤

- 捕获堆栈
使用 `stap/perf/dtrace` 等工具抓取程序的运行堆栈
- 折叠堆栈
对堆栈信息进行分析组合，将重复的堆栈累计在一起，从而体现出负载和关键路径
- 生成火焰图

`Flame Graph` 提供了一系列常用的折叠堆栈(`stackcollapse`)工具。

| `stackcollapse` | 描述 |
|:---------------:|:----:|
| stackcollapse.pl | for DTrace stacks |
| stackcollapse-perf.pl | for Linux perf_events "perf script" output |
| stackcollapse-pmc.pl | for FreeBSD pmcstat -G stacks |
| stackcollapse-stap.pl | for SystemTap stacks |
| stackcollapse-instruments.pl | for XCode Instruments |
| stackcollapse-vtune.pl | for Intel VTune profiles |
| stackcollapse-ljp.awk | for Lightweight Java Profiler |
| stackcollapse-jstack.pl | for Java jstack(1) output |
| stackcollapse-gdb.pl | for gdb(1) stacks |
| stackcollapse-go.pl | for Golang pprof stacks |
| stackcollapse-vsprof.pl | for Microsoft Visual Studio profiles |



## perf 采集数据

让我们从 `perf` (`performance`) 讲起，它是 `Linux` 系统原生提供的性能分析工具，会返回 `CPU` 正在执行的函数名以及调用栈。

```shell
sudo perf record -F 90 -p xxx -g -- sleep 10
```
- `-e` 指定采集事件，默认采集 `cycles`（即 `CPU clock` 周期）
- `-F 90` 指定采样频率为 `90Hz`(每秒`90`次)
- `-p xxx` 指定要采集进程号
- `-g` 表示记录调用栈，`sleep 10` 则是持续 `10` 秒。

>> 如果90次都返回都是同一个函数，那就说明 `CPU` 这一秒钟都在执行同一个函数。

运行后会产生一个文本文件。如果有 `8` 个 `CPU`，每秒抽样 `90` 次，持续 `10` 秒，就得到 `7 200` 个调用栈。

为了便于阅读，`perf record` 命令可以统计每个调用栈出现的百分比，然后从高到低排列。

```cpp
sudo perf report -n --stdio
```

## 生成火焰图

首先用 `perf script` 工具对 `perf.data` 进行解析

```shell
perf script -i perf.data > perf.unfold
# 生成火焰图
./stackcollapse-perf.pl perf.unfold > perf.folded
```

最后生成 `svg` 图

```cpp
./flamegraph.pl perf.folded > perf.svg
```

## 局限
-------

两种情况下，无法画出火焰图，需要修正系统行为。

* 调用栈不完整

当调用栈过深时，某些系统只返回前面的一部分（比如前10层）。

* 函数名缺失

有些函数没有名字，编译器只用内存地址来表示（比如匿名函数）。


