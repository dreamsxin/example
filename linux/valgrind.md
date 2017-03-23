# Valgrind


## Valgrind 包含的工具

### memcheck

检查程序中的内存问题，如泄漏、越界、非法指针等。所有对内存的读写都会被检测到，一切对malloc、free、new、delete的调用都会被捕获。所以，它能检测以下问题：

- 1)对未初始化内存的使用；
- 2)读/写释放后的内存块；
- 3)读/写超出malloc分配的内存块；
- 4)读/写不适当的栈中内存块；
- 5)内存泄漏，指向一块内存的指针永远丢失；
- 6)不正确的malloc/free或new/delete匹配；
- 7)memcpy()相关函数中的dst和src指针重叠。

### callgrind

检测程序代码覆盖，以及分析程序性能。和gprof类似的分析工具，但它对程序的运行观察更是入微，能给我们提供更多的信息。和gprof不同，它不需要在编译源代码时附加特殊选项，但加上调试选项是推荐的。Callgrind收集程序运行时的一些数据，建立函数调用关系图，还可以有选择地进行cache模拟。在运行结束时，它会把分析数据写入一个文件。callgrind_annotate可以把这个文件的内容转化成可读的形式。

### cachegrind

 Cache分析器，分析CPU的cache命中率、丢失率，用于进行代码优化。它模拟CPU中的一级缓存I1，Dl和二级缓存，能够精确地指出程序中cache的丢失和命中。如果需要，它还能够为我们提供cache丢失次数，内存引用次数，以及每行代码，每个函数，每个模块，整个程序产生的指令数。这对优化程序有很大的帮助。作一下广告：valgrind自身利用该工具在过去几个月内使性能提高了25%-30%。据早先报道，kde的开发team也对valgrind在提高kde性能方面的帮助表示感谢。

### helgrind

用于检查多线程程序的竞态条件。它主要用来检查多线程程序中出现的竞争问题。Helgrind寻找内存中被多个线程访问，而又没有一贯加锁的区域，这些区域往往是线程之间失去同步的地方，而且会导致难以发掘的错误。Helgrind实现了名为“Eraser”的竞争检测算法，并做了进一步改进，减少了报告错误的次数。不过，Helgrind仍然处于实验阶段。

### massif

堆栈分析器，指示程序中使用了多少堆内存等信息。它能测量程序在堆栈中使用了多少内存，告诉我们堆块，堆管理块和栈的大小。Massif能帮助我们减少内存的使用，在带有虚拟内存的现代系统中，它还能够加速我们程序的运行，减少程序停留在交换区中的几率。Massif对内存的分配和释放做profile。程序开发者通过它可以深入了解程序的内存使用行为，从而对内存使用进行优化。这个功能对C++尤其有用，因为C++有很多隐藏的内存分配和释放。

### lackey：一个工具实例，以其为模板可以创建你自己的工具。
### nulgrind：为开发者展示如何创建一个工具。

## 使用 Valgrind

Valgrind使用起来非常简单，你甚至不需要重新编译你的程序就可以用它。当然如果要达到最好的效果，获得最准确的信息，还是需要按要求重新编译一下的。比如在使用memcheck的时候，最好关闭优化选项。

valgrind命令的格式如下：
- valgrind [valgrind-options] your-prog [your-prog options]

```shell
export USE_ZEND_ALLOC=0
export ZEND_DONT_UNLOAD_MODULES=1
valgrind --tool=memcheck php test.php
valgrind --leak-check=full --show-reachable=yes php7.1 test.php
```

一些常用的选项如下：
- -h --help									显示帮助信息。
- --version									显示valgrind内核的版本，每个工具都有各自的版本。
- -q --quiet								安静地运行，只打印错误信息。
- -v --verbose								打印更详细的信息。
- --tool=<toolname> [default: memcheck]		最常用的选项。运行valgrind中名为toolname的工具。如果省略工具名，默认运行memcheck。
- --db-attach=<yes|no> [default: no]		绑定到调试器上，便于调试错误。
