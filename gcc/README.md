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
