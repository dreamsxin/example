# 编译选项

## 常规选项

1、没有任何选项
`gcc   helloworld.c`
结果会在与helloworld.c相同的目录下产生一个a.out的可执行文件。

2、-o选项
指定输出文件名：`gcc -o helloworld helloworld.c`
-o意思是Output即需要指定输出的可执行文件的名称。这里的名称为helloworld。

3、-c选项
只编译，不汇编连接：`gcc -c helloworld.c`
-c意思就是Compile，产生一个叫helloworld.o的目标文件

4、-S选项
产生汇编源文件：`gcc -S helloworld.c`
-S意思就是aSsemble，产生一个叫helloworld.s的汇编源文件

5、-E选项
预处理C源文件：`gcc -E helloworld.c`
输出到标准输出。可以对它进行重定向：
gcc -E helloworld.c > helloworld.txt

## 优化选项

1) -O选项
基本优化：`gcc -O helloworld.c`
-O 意思就是Optimize，产生一个经过优化的叫作a.out的可执行文件。也可以同时使用-o选项，以指定输出文件名。
如：`gcc -O -o test helloworld.c`即会产生一个叫test的经过优化的可执行文件。

2) -O2选项
最大优化：`gcc -O2 helloworld.c`产生一个经过最大优化的叫作a.out的可执行文件。

## 调试选项

1) -g选项
产生供gdb调试用的可执行文件：`gcc -g helloworld.c` 产生一个叫作a.out的可执行文件，大小明显比只用-o选项编译汇编连接后的文件大。

2) -pg选项
产生供gprof剖析用的可执行文件：`gcc -pg helloworld.c` 产生一个叫作a.out的执行文件，大小明显比用-g选项后产生的文件还大。

# __attribute__

## alias

设置一个函数的别名。

```c
#include <stdio.h>  
int __fun()
{
  printf("in %s\n",__FUNCTION__);
  return 0;
}

int fun() __attribute__((alias("__fun")));
int main()
{
  fun();
  return 0;
}
```

## visibility

visibility 用于设置动态链接库中函数的可见性，将变量或函数设置为hidden，则该符号仅在本so中可见，在其他库中则不可见。

编写大型程序时，可用-fvisibility=hidden设置符号默认隐藏，针对特定变量和函数，在代码中使用 `__attribute__ ((visibility("default")))` 令该符号外部可见，这种方法可用有效避免库文件之间的符号冲突。

`visibility1.cc`

```c
#include <stdio.h>
void fun1()
{
  printf("in %s\n",__FUNCTION__);
}
// 若编译此文件时使用了参数-fvisibility=hidden，则此行可以省略
__attribute__ ((visibility("hidden"))) void fun1();
```

`visibility2.cc`
```c
#include <stdio.h>
extern "C" void fun1();
extern "C" void fun2()
{
  fun1();
  printf("in %s\n",__FUNCTION__);
}
// 若编译此文件时没有使用参数-fvisibility或设置参数-fvisibility=default，则此行可以省略
__attribute__ ((visibility("default"))) void fun2();
```

# --wrap

覆盖系统函数，例如覆盖 `atoi`

`program.c`
```c
#include <stdlib.h>  
#include <stdio.h>  

int __real_atoi(const char *nptr);
int __wrap_atoi(const char *str)
{
  return __real_atoi(str) + 1;
}

int main()
{
        int a = atoi("12");  
        printf("atoi(12)=%d\n",a);  
        return 0;  
}
```

```shell
gcc program.c -Wl,-wrap,atoi -o program
```
