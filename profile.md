## Python

对于python程序，比较出名的profile工具有三个：profile、cprofile 和 hotshot。其中 profile 是纯python语言实现的，Cprofile 将 profile 的部分实现native化，hotshot也是C语言实现，hotshot与Cprofile的区别在于：hotshot对目标代码的运行影响较小，代价是更多的后处理时间，而且hotshot已经停止维护了。需要注意的是，profile（Cprofile hotshot）只适合单线程的python程序。
对于多线程，可以使用yappi，yappi不仅支持多线程，还可以精确到CPU时间。
对于协程（greenlet），可以使用 greenletprofiler，基于 yappi 修改，用 greenlet context hook 住 thread context。

- ncalls 函数总的调用次数
- tottime 函数内部（不包括子函数）的占用时间
- percall（第一个） tottime/ncalls
- cumtime 函数包括子函数所占用的时间
- percall（第二个）cumtime/ncalls
- filename:lineno(function)  文件：行号（函数）

### GUI tools

虽然 Cprofile 的输出已经比较直观，但我们还是倾向于保存profile的结果，然后用图形化的工具来从不同的维度来分析，或者比较优化前后的代码。查看profile结果的工具也比较多，比如，visualpytune、qcachegrind、runsnakerun

qcachegrind 和 runsnakerun 这两个colorful的工具比 visualpytune 强大得多。

qcachegrind 确实要比 visualpytune 强大。大致分为三部：第一部分同 visualpytune 类似，是每个函数占用的时间，其中 Incl 等同于cumtime， Self 等同于tottime。第二部分和第三部分都有很多标签，不同的标签标示从不同的角度来看结果，第三部分的“call graph”展示了该函数的call tree并包含每个子函数的时间百分比，一目了然。