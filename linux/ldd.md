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


## rpath 和 runpath

在 Linux 系统中，ldd 命令用于打印共享库的依赖关系，而 rpath 和 runpath 是与动态链接库相关的两个环境变量，它们用于指定程序运行时搜索动态库的路径。

rpath (Run-time Library Path)：在编译时设置，用于告诉系统在程序运行时查找动态库的路径。

runpath (Run-Path)：类似于 rpath，但在某些情况下（如使用了 -Wl,--enable-new-dtags 链接选项时），它会被优先考虑。

### 查看现有的 rpath 和 runpath

你可以使用 readelf 工具来查看一个 ELF 文件的 rpath 和 runpath 设置：
```shell
readelf -d /path/to/your/executable | grep '(RPATH\|RUNPATH)'
```

### 编译设置 rpath

```shell
gcc -o your_program your_program.c -Wl,-rpath,/your/library/path -L/your/library/path -llibraryname
# --enable-new-dtags
gcc -o your_program your_program.c -Wl,--enable-new-dtags,-rpath,/your/library/path -L/your/library/path -llibraryname
```

### 修改

```shell
sudo apt-get install patchelf
patchelf --set-rpath /usr/glib-2.31/lib common_shell_exec
```

- export LD_LIBRARY_PATH=/your/library/path:$LD_LIBRARY_PATH
