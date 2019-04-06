---
title: C 拓展 Python 实战（一）——全局观
date: 2018-02-27 04:54:29
categories:
- 技术
tags:
- python
- c++
---

在本章我们会用 C 实现一个简单的的 Python 内置模块， 以了解用 C/C++ 拓展 Python 时的核心的组件和基本流程。  
假设这个模块叫 `mymath ` ， 其中有一个`sum` 函数:

```python
import mymath
number = mymath.sum(1, 2)
print(number) # 3
```
<!-- more -->
## 一、编写源码
首先我们要编写这份 `mymath.c` 源码， 这份源码要做的事情大致如下：
1. 从固定的接口返回一个 Python 的 Module 对象
2. 这个 Module 对象通过一个全局的静态 `struct`定义
3. 这个 Module 对象里包含(一些) Python 函数对象
4. 这些函数对象要接受 Python 对象组成的参数列表，并以 Python 对象为返回值

下面开始行动。  

**1. 从固定的接口返回一个 Python 的 Module 对象**  
就像一般 C 程序的入口是 `main` 函数一样， 用 C 编写 Python 的 Module 也有一个固定的接口:
```c
/* 使用 Python 的 C API 必须 include Python 的头文件，且放在第一行*/
#include <Python.h>

/* 返回 Module 对象的函数 prototype 统一为 PyObject* PyInit_<module_name> (void)。
 * 函数的返回值类型是 PyObject *, PyMODINIT_FUNC 这个宏会自动帮我们在 C++ 环境中添加 extern "C" 前缀；
 * 函数名称统一为 PyInit_<module_name> 的形式， 我的 Module 是 mymath， 所以写成了 PyInit_mymath；
 * 函数的参数为 void。
 */
PyMODINIT_FUNC PyInit_mymath(void) {

    /* 使用 PyModule_Create 这个宏创建 Python 的 Module 对象。
     * 你应该还记得 Python 中任何事物都是 Object，在 C 里面，它们就是 PyObject*。
     * 参数 mymath_module 是个静态 struct 的指针。我们稍后再来讨论如何声明这个 struct。
     */
    PyObject *m = PyModule_Create(&mymath_module);

    /* 最后， 返回创建好的 Module 对象。*/
    return m;
}
```

**2. 这个 Module 对象通过一个全局的静态 `struct`定义**  
刚才我们在创建 Module 对象的时候使用到了一个静态 `struct` 的指针。Module 的名字，文档，它所包含的函数等都由这个 `struct` 指定。那我们怎么定义这个 `struct` 呢？
```c
static struct PyModuleDef mymath_module = {
    PyModuleDef_HEAD_INIT,        // 这是一个约定俗成的宏，放在最前面
    "mymath",                     // 这一行定义 Module 的名字	
    PyDoc_STR("mymath module"),   // 这一行定义 Module 的文档
    -1,                           // 通常是 -1 ，全编译器只存在一个这个 Module 的实例
    MymathMethods                 // 这是一个 array， 包含 Module 所有的函数, 稍后解释
};
```
Python 里面万物皆 Object，且大多位于 heap 之中，但这个 Module 除外。  
我们不想要 Module 被莫名其妙地释放掉，所以这个 `struct` 是 **global static** 的。

**3. 这个 Module 对象里包含(一些) Python 函数对象**  
刚才在定义 Python Module struct 的时候提到 Module 所有的函数都由一个 array 提供。那这个 array 又怎么定义呢？
```c
static PyMethodDef MymathMethods[] = {
    {"sum", sum, METH_VARARGS, "add up two numbers and return their sum"},
    {NULL}
};
```
嗯， 这也是一个 **global static** 的数据。  
其实 `PyMethodDef` 是一个 `struct` 的别名， 为了保持**一致性**而特意命名成 `Py<xxx>Def` 的形式的。以下是这个 struct 的定义，看起来也是相当的 self-explained:
```c
struct PyMethodDef {
    const char  *ml_name;   // 这行指定函数名称
    PyCFunction ml_meth;    // 这行指定对应的 C 函数
    int         ml_flags;   // 这行指定函数的参数列表形式
    const char  *ml_doc;    // 这行指定函数的文档
};
```
现在再来看 `MymathMethods` 这个 `array` 的第一项。
字符串`"sum"` 是提供给 Python Interpreter 的函数名称，而 `sum`是当从 Python 调用这个函数时， 实际调用的 C 函数，`"add up two numbers and return their sum"` 自然是文档。  
`METH_VARARGS` 则影响 Python C API 如何解析从 Python Interpreter 传入的参数， 这是一个比较复杂的问题， 我们会在下一章函数的专题中仔细探讨。现在只需要记住，它把 Python 中传入的参数当做一个 `tuple`。  
最后一项又是一个约定俗称的规范， 这一项的函数名称为 `NULL`， 用以说明这是这个 `array` 的最后一项。  
接下来我们只剩下一个待解决的问题了， C 函数 `sum` 该怎么定义？

**4. 这些函数对象要接受 Python 对象组成的参数列表，并以 Python 对象为返回值**  
Python 中万物皆 Object，函数的参数从 Python Interpreter 传过来，计算结果又返回到 Interpreter，所以它们自然都是 PyObject。或者说，都是 PyObject 的指针。
```c
PyObject *sum(PyObject *self, PyObject *args) {	     // 第一个是 self， 第二个是 args tuple
    int num1, num2;                                  // 声明两个 int 值用以容纳参数
    PyArg_ParseTuple(args, "ii", &num1, &num2);      // 以两个 int ("ii") 的形式解析 args tuple
    return Py_BuildValue("i", num1 + num2);          // 计算和， 再以 int 形式转换成 PyObject 返回
}
```
在 C 里面定义的 Python 函数都有类似的 prototype。返回类型总是 `PyObject*`，参数总是 `(PyObject* self， PyObject* args, PyObject* kwargs)`，第二个和第三个分别会被转义成 Python 的 `tuple` 和 `dict`， 并且都是可选的。  
在函数体内，需要注意的就是解析参数以及在最后把返回值转义成 `PyObject*`(当然也有异常流程和引用计数的问题，不过我们在下一章讨论)。  
至此， 我们已经实现了一个完整的 Python Module 了。我们只需要把它编译到 Python 里面去， 就可以使用它啦。

## 二、编译安装
因为我们打算以内置(built-in)的形式使用我们的 Python Module，所以我们得把刚才编写好的源码放到 Python 源码中去，再重新编译和安装整个 Python(我们也能以 package 的形式安装我们的 C 拓展，当然那是以后的内容)。我建议在 Docker 中执行这些操作，以免破坏本机环境...  

**1. 获取 Python 源码**
你可以从搜狐镜像下载一份 [Python 3.5.3](http://mirrors.sohu.com/python/3.5.3/Python-3.5.3.tgz) 的源码， 并解压它。  
**2. 放入我们的模块**  
在源码文件夹内， 有一个 Modules 文件夹专门用来放置内置的 Module， 把我们的`mymath.c` 复制到里面来。  
接着 `vim Modules/Setup.local`, 文件末尾插入 `mymath mymath.o`。
```c
/* mymath.c */
#include <Python.h>

PyObject *sum(PyObject *self, PyObject *args) {
    int num1, num2;
    PyArg_ParseTuple(args, "ii", &num1, &num2);
    return Py_BuildValue("i", num1 + num2);
}

static PyMethodDef MymathMethods[] = {
    {"sum", sum, METH_VARARGS, "add up two numbers and return their sum"},
    {NULL}
};

static struct PyModuleDef mymath_module = {
    PyModuleDef_HEAD_INIT,
    "mymath",
    PyDoc_STR("A hello world example to demonstrate writing a python module in c language"),
    -1,
    MymathMethods
};

PyMODINIT_FUNC PyInit_mymath(void) {
    PyObject *m = PyModule_Create(&mymath_module);
    return m;
}
```

```shell
# Modules/Setup.local
# Edit this file for local setup changes
mymath mymath.o
```

**3. 编译和安装**
```shell
make -j 4
make install
```

**4. Let's Play**  
最后让我们进入 Python3 的 Interpreter 开始玩耍吧。
```shell
root@bab0ce9dfc54:/tmp/Python-3.5.3# python3
Python 3.5.3 (default, Feb 26 2018, 12:46:45) 
[GCC 5.4.0 20160609] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import mymath
>>> help(mymath)
```

将会看到如下输出
```shell
Help on built-in module mymath:

NAME
    mymath - mymath module

FUNCTIONS
    sum(...)
        add up two numbers and return their sum

FILE
    (built-in)

(END)

```
按 `q` 返回 Interpreter， 接着输入:
```shell
>>> number = mymath.sum(1, 2)
>>> print(number)
3
>>> 
```

**Congratulations！** 我们最初的目标实现了。

