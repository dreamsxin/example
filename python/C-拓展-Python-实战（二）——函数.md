---
title: C 拓展 Python 实战（二）——函数
date: 2018-03-02 09:00:11
categories:
- 技术
tags:
- python
- c++
---

我们把用 C/C++ 实现的 Python 函数统称为 `PyCFunction`。  
上一章 `mymath` 模块的 `sum` 函数能接受两个 int 参数并对它们求和。然而靠这样的功能还不能写出真正可用的 Python Module。  
在本章我们将实现一个更复杂的`PyCFunction`以掌握更多必要的细节。  
<!--more-->
我们将实现一个 `myfunc` Module，其中有一个 `myfunc` 函数，这个函数在 Python 中对应的定义如下:
```python
def myfunc_py(items, operator, reporter=None):
    """
    :param items:        一个 list
    :param operator:     一个以 items 为参数的 Python callable 对象
    :param reporter:     当 reporter 不为 None 时，必须为一个 Python callable 对象
                         它以一个包含 myfunc 所有的参数和返回值的 dict 为参数
    :return:             operator(items) 的结果
    """
    if not isinstance(items, list):
        raise TypeError("items must be a list")

    if not callable(operator):
        raise TypeError("operator must be a callable object")

    result = operator(items)
    if reporter is not None and callable(reporter):
        report = {"args": (items, operator),
                  "kwargs": {"reporter": reporter},
                  "result": result}
        reporter(report)
    return result
```
也就是说我们实现的这个 `PyCFunction` 有以下特点:
1. 接受一个 `list` 作为第一个 **positional argument**
2. 接受 Python 里面的一个 **`callable`** object 为第二个 positional argument，并且会在 `PyCFunction` 里面调用它
3. 接受一个**可选**的 reporter 作为 **key-word argument**
4. 会检查 argument 的类型，并适时抛出**异常**
5. 会创建一个**新的 object**并返回到 Python Interpreter

这个函数恰好涵盖了我们需要特别注意的四个点： 1.处理参数，2.处理逻辑异常，3.调用其它函数，4.返回结果。
好了，不多说了，开始写代码吧。

## 一、`myfunc.c` 的源码
```c
/* myfunc.c */
#include <Python.h>

PyObject *myfunc(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject *items, *operator, *reporter = NULL;
    PyObject *result;
    PyObject * report;

    static char *kwlist[] = {"items", "operator", "reporter", NULL};
    int parse_state = PyArg_ParseTupleAndKeywords(args, kwargs, "OO|O", kwlist, 
                                                    &items, &operator, &reporter)
    if (!parse_state) return NULL;

    if (!PyList_Check(items)) {
        PyErr_SetString(PyExc_TypeError, "items must be a list");
        return NULL;
    }

    if (!PyCallable_Check(operator)) {
        PyErr_SetString(PyExc_TypeError, "operator must be a callable");
        return NULL;
    }

    if (reporter != NULL) {
        if (!PyCallable_Check(reporter)) {
        PyErr_SetString(PyExc_TypeError, "reporter must be a callable");
        return NULL;
        }
    }

    result = PyObject_CallObject(operator, Py_BuildValue("(O)", items));
    if (result == NULL)
        return NULL;
    Py_INCREF(result);

    if (reporter != NULL) {
        report = PyDict_New();
        if (args != NULL) PyDict_SetItemString(report, "args", args);
        if (kwargs != NULL) PyDict_SetItemString(report, "kwargs", kwargs);
        if (result != NULL) PyDict_SetItemString(report, "result", result);
        PyObject_CallObject(reporter, Py_BuildValue("(O)", report));
    }

    return result;
}

static PyMethodDef MymathMethods[] = {
    {"myfunc", myfunc, METH_VARARGS | METH_KEYWORDS, "a complex function"},
    {NULL}
};

static struct PyModuleDef myfunc_module = {
    PyModuleDef_HEAD_INIT,
    "myfunc",
    PyDoc_STR("built-in myfunc module"),
    -1,
    MymathMethods
};

PyMODINIT_FUNC PyInit_myfunc(void) {
    PyObject *m = PyModule_Create(&myfunc_module);
    return m;
}
```
由于有了前一章的基础，所以这份代码并不难理解，它的整体架构和 `mumath.c` 类似，只是函数定义更加复杂了。  
我们暂时不深入代码细节，而是先通过测试来感觉一下它的威力。  
Show Time!
```python
>>> from myfunc import myfunc
>>> myfunc([1,2], sum)
3
>>> myfunc([1, 2], sum, print)
{'result': 3, 'args': ([1, 2], <built-in function sum>, <built-in function print>)}
3
```
我们看到，第一个参数 `[1, 2]` 被第二个参数 `sum` 作为参数，其结果 `3` 被作为 `myfunc` 的结果返回。  
而如果传入了第三个参数 `print`，它将以 `myfunc` 的 `args`，`kwargs` 和返回值所构建成的 `dict` 为参数被调用(此时为被打印出来)。    

```python
>>> myfunc(items=[1, 2, 3], reporter=print, operator=sum)
{'result': 6, 'args': (), 'kwargs': {'operator': <built-in function sum>, 'items': [1, 2, 3], 'reporter': <built-in function print>}}
6
```
如上所示，我们也能全部以 kwargs 的形式提供参数，并且此时可以忽略它们在 `PyCFunction` 中声明的顺序。  

```python
>>> myfunc(1, 2, 3)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: items must be a list
>>> myfunc([1,2], 2, 3)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: operator must be a callable
>>> myfunc([1,2], sum, 3)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: reporter must be a callable
>>> 
```
而如果我们提供的参数不符合规范，函数也能适时抛出异常。  
嗯，完美。  
下面我们按之前提到的四点深入分析 `PyCFunction` 的细节。

## 二、处理参数
我们的 myfunc 的参数列表为 `(PyObject *self, PyObject *args, PyObject *kwargs)` 。  
这已经是最复杂的 `PyCFunction` 的参数列表的形式。`PyCFunction` 在处理参数时有两个最重要的特点：
 - `PyCFunction` 不关注参数列表的形式，它们的数量固定(1-3个)，类型统一(`PyObject *`)
 - `PyCFunction` 需要额外的步骤去解析参数

**2.1 `PyCFunction` 不关注参数列表的形式**
`PyCFunction` 不关注参数的数量，也不关注参数的数据类型。参数的类型都是 `PyObject *`，数量最多3个：
1. 第一个永远为 `self`， Module 函数的 self 是 Module，Class 函数的 `self` 是 object instance；  
2. 第二个是可选的，如果被定义了，那就是 `args` 元组 
3. 第三个也是可选的，如果被定义了，那就是 `kwargs` 字典  

为什么 `PyCFunction` 不关注参数的数据类型？ 即使我们要在 `PyCFunction` 中限定参数类型，那也只能是 `PyObject` 或者其子类，而 Python Interpreter 传递的任何对象都是 `PyObject`，所以想限制也限制不了。  
那为什么也统一了参数的数量呢？就不能是 `(PyObject* self, PyObject* arg1, PyObject * arg2, PyObject *arg3=NULL)` 吗？实际上如果这样的话，Python 编译安装好后，`PyCFunction` 的参数形式就固定了，在 Python Interpreter 里面使用这个函数时，参数顺序只能是 `arg1`, `arg2`, `arg3`。而 Python 是可以使用 `(arg3=3, arg1=1, arg2=2)` 这种打乱了顺序的 `kwargs` 的形式提供参数的。

**2.2 `PyCFunction` 需要额外的步骤去解析参数**
参数都是 `PyObject*`，如果要参与到 C 的逻辑中去进行计算，那它就要被转换成 C 数据类型。 而由于种种限制，这种转换不能在参数列表中自动实现，因此只能我们自己来实现。  
Python C API 专门提供了两个常用的解析参数的函数：`PyArg_ParseTuple` 和 `PyArg_ParseTupleAndKeywords`。前者只用于解析 `args` 元组, 后者同时解析 `args` 元组 和 `kwargs` 字典。它们以类似的解析逻辑参数:
1. 首先传入要解析的 `args`(和 `kwargs`)；
2. 再传入一个字符串作为模板，以此来指示 `args`(和 `kwargs`)应该解析出的参数数量以及它们的类型。比如 `"ii"` 表示要解析两个 `int` 数据类型；
3. 再依次传入参数数量的 C 指针，指针的类型要与参数的类型匹配；
4. 所有参数解析成功，返回 `true`，参数值被存入对应指针中，否则返回 `false`。

我们还是以代码来说明。
```c
PyObject *sum(PyObject *self, PyObject *args) {
    // 先声明两个 int 以存放参数的值
    int num1, num2;

    // 首先传入 args 作为需要解析的对象，再传入 "ii" (两个 i) 表面要解析两个 int，再依次传入 两个 int 指针
    PyArg_ParseTuple(args, "ii", &num1, &num2);

    // 解析成功后，两个 int 的值分别存到了 num1 和 num2 里面
    return Py_BuildValue("i", num1 + num2);
}

PyObject *myfunc(PyObject *self, PyObject *args, PyObject *kwargs) {
    // 同样的，先声明 C 数据以存放参数的值
    PyObject *items, *operator, *reporter = NULL;

    // 如果我们要解析 kwargs，则必须构建一个以 NULL 结尾的 char* array
    // 除最后的 NULL 外，每一个 char* 代表一个参数的名字， args tuple 里面的参数也要指定名字
    static char *kwlist[] = {"items", "operator", "reporter", NULL};
    // 同样的，先传入 args 和 kwargs， 再传入"OO|O"(代表解析3个 Python Object，| 之后的是可选的)
    // 在传入各项 C 指针之前，要先传入 kwlist
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|O", kwlist, &items, &operator, &reporter)) {
        return NULL;
    }
}
```
我们不去列举所有的字符和 C 数据类型的对应关系，这些在官方文档里面可以查到。
最后还需要注意一点，如果 `PyCFunction` 要支持按 `kwargs` 的形式传入和解析参数，那么在 Function table 里面必须把函数的 flag 加上 `METH_KEYWORDS`，否则会出现 `TypeError: myfunc() takes no keyword arguments` 错误。

```c
static PyMethodDef MymathMethods[] = {
    // sum 函数不需要支持 kwargs，所以不用 METH_KEYWORDS flag
    {"sum", sum, METH_VARARGS, "add up two numbers and return their sum"},
    {"myfunc", myfunc, METH_VARARGS | METH_KEYWORDS, "a complex function"},
    {NULL}
};
```

## 三、处理逻辑异常
在 `myfunc.c` 里面我们用诸如 `PyList_Check(obj)` 的函数来检查 `obj` 的类型，当类型不符合要求时，抛出异常：
```c
  if (!PyList_Check(items)) {
    PyErr_SetString(PyExc_TypeError, "items must be a list");
    return NULL;
  }
```
异常状态由三个全局的静态变量保存。第一个变量用作 flag，它如果不为 `NULL`，则编译器判断当前有异常发生；第二个保存了异常的消息，第三个变量保存了异常的 traceback。
我们一般使用 `PyErr_SetString` 设置异常，第一个参数是异常的类型，第二个参数是异常的消息。 Python C API 以 `PyExc_XXX` 的形式提供了所有内置的异常类型。我们也能使用 `PyErr_SetFromErrno` 来自动根据操作系统的 `errno` 来生成和设置异常。
当我们的函数设置异常消息后，需要返回 NULL，以告知调用者有异常发生了。
而当我们调用的 Python C API 提供的函数(除了 `PyArg_ParseXXX`)返回了 `NULL` 时，说明调用的函数中发生了异常(也能用 `PyErr_Occurred` 手动检测)，我们也需要返回 `NULL` 以传递状态(不需要再次设置异常)，或者调用 `PyErr_Clear` 以清除全局的异常状态并尝试其它的操作。

## 四、调用 Python 函数
我们接下来看看怎么调用 Python 中传入的函数对象。
```c
  if (!PyCallable_Check(operator)) {
    PyErr_SetString(PyExc_TypeError, "operator must be a callable");
    return NULL;
  }
  result = PyObject_CallObject(operator, Py_BuildValue("(O)", items));
```
我们首先使用 `PyCallable_Check` 来检查对象是否为 callable 对象。然后再以 `PyObject_CallObject` 来调用对象。第一个参数为 callable，第二个参数为 `args` 元组。这个函数调用的效果等同于 Python 里面的 `callable_object(*args)`。  
如果想以 `callable_object(*args, **kwargs)` 的形式调用函数，我们可以使用 `PyObject_Call`，它接受第三个 `PyObject*` 作为 `kwargs`。不过此时 `args` 不能为 `NULL` 了(`kwargs` 可以为 `NULL`)。
我们还能看到，为了构建一个元组，我们使用了 `Py_BuildValue("(O)", items)`。这个函数调用的作用是，把 items 这个 C 数据(PyObject *) 转换为一个 Python 对象(字符O),并且包含在一个元组里(字母 O 放在括号里面)。

## 五、返回结果
我们的函数在重重运算中成功避开了各种异常并拿到了最终的结果。现在我们只剩下最后一件事情了，就是把结果返回给 Python Interpreter。  
如果我们的结果是 C 数据，那么我们还需要把它转换成 Python 数据。转换的函数就是我们刚才看见的 `Py_BuildValue`。  
这个函数怎么用，我们看下面这些例子就明白了:  
| 函数调用       | 返回的 Python 对象 |
| -------------|:-------------:|
|Py_BuildValue("")                        |None|
|Py_BuildValue("iii", 123, 456, 789)      |(123, 456, 789)
|Py_BuildValue("ss", "hello", "world")    |('hello', 'world')
|Py_BuildValue("()")                      |()
|Py_BuildValue("(i)", 123)                |(123,)
|Py_BuildValue("[i,i]", 123, 456)         |[123, 456]
|Py_BuildValue("{s:i,s:i}","abc", 123, "def", 456)    |{'abc': 123, 'def': 456}
|Py_BuildValue("((ii)(ii)) (ii)", 1, 2, 3, 4, 5, 6)          |(((1, 2), (3, 4)), (5, 6))

我们可以参考官方文档以获得更相近的解释，在此就不去详细说明了。  
数据转换完成 Python Object 后，我们并不能直接返回。  因为 Python 中所有的对象都存在于 heap 中，而所有的变量其实都是对某一个对象的引用。 Python 根据对象的引用计数来决定是否需要回收它。我们再看我们的代码:
```c
  ...
  result = PyObject_CallObject(operator, Py_BuildValue("(O)", items));
  if (result == NULL)
    return NULL;
  Py_INCREF(result);
  ,,,
  return result;
```
`PyObject_CallObject` 调用成功的话，会返回一个非 `NULL` 的对象。这个新对象目前的引用计数为 0。我们使 `result` 指向了这个对象，此时引用计数成为 1。如果这时候我们马上返回结果的话，函数返回后，result 被销毁，对象的引用计数就会从 1 减到 0，从而导致结果被垃圾回收。我们就返回了一个被销毁的对象！  
为了避免这种情况，我们通过 `Py_INCREF(result)` 把这个对象的引用计数加 1 后再返回。  
引用计数涉及到内存管理，是使用 C 拓展 Python 时需要格外小心的地方。我们在下一章展示用 C 实现 Python 的 `Class` 的时候再详细说明。  

好了，我们对 `PyCFunction` 的四个重要特点(参数处理、逻辑异常、函数调用和返回值)有了更深入的了解, 也能写出更强大的函数了。接下来，我们就要挑战更强大的类了。

