# ldd (List Dynamic Dependencies) 

## 选项
- --version：打印指令版本号；
- -v：详细信息模式，打印所有相关信息；
- -u：打印未使用的直接依赖；
- -d：执行重定位和报告任何丢失的对象；
- -r：执行数据对象和函数的重定位，并且报告任何丢失的对象和函数；
- --help：显示帮助信息。

## 原理

ldd 不是个可执行程式，而只是个 shell 脚本； ldd 显示可执行模块的 dependency 的工作原理，其实质是通过 ld-linux.so（elf动态库的装载器）来实现的。ld-linux.so 模块会先于 executable 模块程式工作，并获得控制权，因此当上述的那些环境变量被设置时，ld-linux.so 选择了显示可执行模块的 dependency。

设置一系列的环境变量，如下：LD_TRACE_LOADED_OBJECTS、LD_WARN、LD_BIND_NOW、LD_LIBRARY_VERSION、LD_VERBOSE 等。当 LD_TRACE_LOADED_OBJECTS 环境变量不为空时，任何可执行程序在运行时，它都会只显示模块的 dependency，而程序并不真正执行。要不你可以在 shell 终端测试一下，如下：

`export LD_TRACE_LOADED_OBJECTS=1`

再执行任何的程序，如ls等，看看程序的运行结果。

ldd 显示可执行模块的 dependency 的工作原理，其实质是通过 ld-linux.so（elf 动态库的装载器）来实现的。我们知道，ld-linux.so 模块会先于 executable 模块程序工作，并获得控制权，因此当上述的那些环境变量被设置时，ld-linux.so 选择了显示可执行模块的 dependency。

实际上可以直接执行ld-linux.so模块，如：`/lib/ld-linux.so.2 --list program`（这相当于ldd program）
