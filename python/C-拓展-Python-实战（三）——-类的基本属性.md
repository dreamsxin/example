---
title: C 拓展 Python 实战（三）—— 类的基本属性
date: 2018-03-07 01:45:49
categories:
- 技术
tags:
- c++
- python
---

我们将用两章探索如何实现内置的 Python 类，并尝试去了解一些 Python 编程语言的底层实现。  
和之前一样，我们先从一个简单的例子开始，再逐步添加更多的特性。  
我们要实现的第一个类为 `Empty`，就像它的名字一样，这个类没有任何 `member` 和 `method`。在实现这个类的过程中我们将学习到创建一个类的基本流程。  
我们要实现的第二个类为 `Record`, 它有两个成员属性 `name`(公开，string) 和 `__value`(私有，任意值), 以及 `print` 和 `set_value` 两个成员方法，并且有一个 `value property`。  
由于 `Record` 的 `value` 可以设置为任意值(包括自己)，我们就得再深入考虑一下引用计数、`dealloc` 和 `weak refrence` 这些涉及到内存管理的问题。  
接着我们给 `Record` 添加类方法(class method) `get_count`、静态方法(static method) `get_purpose` 以及 `__repr__`, `__str__` 和 `__eq__`。  
最后，我们创建一个 `StringRecord` 类，它是 `Record` 的子类，但是它的 `value` 只能是 `str` 类型的数据。

本章我们先掌握类的基本属性。
<!-- more -->

## 一、类的基本骨骼
在为模块添加内置函数的时候，我们是先把所有函数收集到一个 Array 里面(function table)，再把这个 Array 设置为 Module `struct` 的一个属性。  
然而给模块添加内置类的时候就不能这么干了。直接原因是 Module `struct` 里面并没有 class table 这个属性，更深层的原因则可能是因为类的复杂性。由于编译器等各种原因(待考证)，类有太多属性必须要推迟到 `PyInit_<module_name>` 函数里面去手动设置。  
所以给模块添加类的流程就变成了: 1.定义类 —— 2.在 Init 函数里面设置类必要的属性 —— 3.把类添加到 Module 里面 —— 4.返回 Module。  
说了这么多，下面开始写代码吧。

**1.1 定义类**  
如果你熟悉 Python 的 MetaClass 概念的话，那你应该明白，Python 里面 Class 本身也是 Object，是 Type 的实例。因此我们在定义一个类的时候，其实是在定义一个 PyTypeObject。  
而且我认为 Python 对类的实现方式体现了一种常见的 `Implementation` 思想，即把数据和行为分开，在类内部只定义类的行为，再以一个指针指向底层数据。这样的好处是提高了赋值对象时的效率(实际上 Python 是反过来的，它在内部定义数据和一个指向行为类的指针)。  
所以定义类其实分了两个步骤: 定义类的数据结构(`Impl`)和定义类的行为(`Wrapper`)。在 Python C API 内部，则习惯把这两者称呼为 Object 和 Type。  
```c
#include <Python.h>

/* 定义类的 Impl(Object)，真正的数据定义在这里 */
typedef struct {
  PyObject_HEAD   // 所有 Object 都包含的宏，其包含了一个名为 `ob_base` 的 PyObject
                  // `ob_base` 包含 Object 的引用计数和它的 `type` 的指针
} empty_object;

/* 定义表面上的类(Type)，这是一个 PyTypeObject, 它有很多很多很多的 slot 来实现这个类的各种行为。
 * 为了逻辑上显得更清晰，我们只在这里定义必须的元素，之后再按需求在 `PyMODINIT_FUNC` 函数里面去设定。
 */
static PyTypeObject empty_type = {
    PyVarObject_HEAD_INIT(NULL, 0)  // 所有类都包含的宏，等效于 `PyVarObject_HEAD_INIT(&PyType_Type, size)`，最后没有逗号
    "myclass.Empty",                // 类的名字
    sizeof(empty_object),           // 类的 Object size
    0                               // 如果类是序列类型的话，这里定义每个元素的 size，不是序列类型的话使用默认值 0
};
```

**1.2 设置类必要的属性**  
我们已经定义好了类的基本信息，比如类的名字，类占用的空间大小等。还有很多必要属性，比如这个类到底如何 `alloc` 内存，将在 Init 函数里面去设置。以下是我们 `PyMODINIT_FUNC` 函数的一个片段。  
```c
PyMODINIT_FUNC PyInit_myclass(void) {
  ...
  /* 对于我们的 Empty 类来说，只需要再设置这三个属性就好了 */
  empty_type.tp_doc = "built-in Empty type";   // 定义类的文档
  empty_type.tp_flags = Py_TPFLAGS_DEFAULT;    // 定义类的 flag，我们的类是空类，因此只包含一个默认 flag
  empty_type.tp_new = PyType_GenericNew;       // 定义类的 new 函数，我们的类是空类，因此使用默认的 new 函数
  ...
}
```

当所有的类属性都设置好以后，我们还需要再做一些收尾工作，比如根据编译器的要求修改一些杂七杂八的设置，比如添加继承来的方法和属性等。所幸这些事情都不用我们自己干，Python C API 提供了一个 `PyType_Ready` 函数我帮我们自动化这些琐事。  
```c
  if (PyType_Ready(&empty_type) < 0) {
    return NULL;
  }
```

**1.3 把类添加到 Module 里面** 
最后需要做的事情就是通过 `PyModule_AddObject` 把我们已经准备好的类添加到 Module 里面去了。
```c
static PyModuleDef myclass_module = {
    PyModuleDef_HEAD_INIT,
    "myclass",
    "a example module for defining built-in data type",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC PyInit_myclass(void) {
  PyObject *module;
  module = PyModule_Create(&myclass_module);
  if (module == NULL)
    return NULL;

  empty_type.tp_doc = "built-in Empty type";
  empty_type.tp_flags = Py_TPFLAGS_DEFAULT;
  empty_type.tp_new = PyType_GenericNew;
  if (PyType_Ready(&empty_type) < 0) {
    return NULL;
  }
  
  // 把 empty_type 添加到 module，并命名为 "Empty"
  PyModule_AddObject(module, "Empty", (PyObject *) &empty_type);
  return module;
}
```
像往常一样编译安装我们的 `myclass` 模块，然后进入 Interpreter 测试吧：
```python
root@8a57bfaacaf7:/tmp/Python-3.5.3# python3
Python 3.5.3 (default, Mar  6 2018, 12:34:13) 
[GCC 5.4.0 20160609] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import myclass
>>> help(myclass)
Help on built-in module myclass:

NAME
    myclass - a example module for defining built-in data type

CLASSES
    builtins.object
        Empty
    
    class Empty(builtins.object)
     |  built-in Empty type
     |  
     |  Methods defined here:
     |  
     |  __new__(*args, **kwargs) from builtins.type
     |      Create and return a new object.  See help(type) for accurate signature.

FILE
    (built-in)

(END)

>>> empty = myclass.Empty()
>>> dir(empty)
['__class__', '__delattr__', '__dir__', '__doc__', '__eq__', '__format__', '__ge__', '__getattribute__', '__gt__', '__hash__', '__init__', '__le__', '__lt__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__']
>>>
```

嗯，这就是我们的 `Empty` 类。简要概括一下步骤就是: 
1. 创建一个 object struct 专门用于存储底层数据
2. 创建一个 PyTypeObject 用于定义类的 metadata，并通过 PyTypeObject 的各种 slot 来拓展类属性
3. 通过 PyType_Ready 做一些自动的收尾工作
4. 通过 PyModule_AddObject 把定义好的类添加到模块里面去

不过此时还有两个疑惑点，即 object struct 什么时候和 PyTypeObject(Impl 和 Wrapper) 什么时候绑定的？怎么定义成员变量和方法？
别急，答案在下一节就会揭晓。

## 二、members 和 methods
现在我们开始定义 `Record` 类。 

**2.1 添加类的成员属性**  
`Record` 类不同于 `Empty`，它有两个成员属性 `name` 和 `string`。根据我们之前的经验可知这两个属性应该在 `Impl` 里面去定义。
```c
#include <Python.h>

/* 相比于 Empty 多了两个属性 */
typedef struct {
  PyObject_HEAD
  PyObject *name;    // 虽然我们希望 name 为一个 string，但 Python 里面的 string 也是对象
  PyObject *value;   // value 可以为任何 Python 对象
} record_object;
```
此时我们只是在 `Impl` 里面定义了成员变量的数据类型，我们还要告知 `Wrapper` 它有这两个成员变量，建立二者的联系。
我们需要建立一个 member table，为此需要 include 一个新的头文件 `structmember.h`:  
```c
#include <Python.h>
#include <structmember.h>

...

static PyMemberDef record_members[] = {
    {"name", T_OBJECT_EX, offsetof(record_object, name), 0, "name of the record"},
    {"value", T_OBJECT, offsetof(record_object, value), READONLY, "value of the record"},
    {NULL}
};

...

```
member table 和 method table 有着类似的结构，它们都是一个 Array，都以一个 `{NULL}` 作为结束的标志。不同的是 Array 的元素。
member table 的元素是定义如下的 `struct`：
```c
typedef struct PyMemberDef {
    char *name;            // member 的名字
    int type;              // member 的类型，T_OBJECT 和 T_OBJECT_EX 都是 Python 对象
                           // 区别在后者会对 NULL 值抛出 AttributeErro 异常
    Py_ssize_t offset;     // 可能用于定义 member 在 `Impl` 中的内存位置
    int flags;             // 定义 member 的读写权限, 0 为默认，
                           // 除此之外还有 READONLY，READ_RESTRICTED，WRITE_RESTRICTED，RESTRICTED，支持 OR 操作
    char *doc;             // 定义 member 的文档
} PyMemberDef
```
建立 member table 之后，我们在 `PyMODINIT_FUNC` 里面设置 Type 的 `tp_members` slot:
```c
  ...
  record_type.tp_members = record_members;
  ...
```
所以完整的代码是这样的:
```c
#include <Python.h>
#include <structmember.h>

typedef struct {
  PyObject_HEAD
  PyObject *name;
  PyObject *value;
} record_object;

static PyMemberDef record_members[] = {
    {"name", T_OBJECT_EX, offsetof(record_object, name), 0, "name of the record"},
    {"value", T_OBJECT, offsetof(record_object, value), READONLY, "value of the record"},
    {NULL}
};

static PyTypeObject record_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "myclass.Record",
    sizeof(record_object),
    0
};

static PyModuleDef myclass_module = {
    PyModuleDef_HEAD_INIT,
    "myclass",
    "a example module for defining built-in data type",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC PyInit_myclass(void) {
  PyObject *module;
  module = PyModule_Create(&myclass_module);
  if (module == NULL)
    return NULL;

  record_type.tp_doc = "built-in Record type";
  record_type.tp_flags = Py_TPFLAGS_DEFAULT;
  record_type.tp_new = PyType_GenericNew;
  record_type.tp_members = record_members;
  if (PyType_Ready(&record_type) < 0) {
    return NULL;
  }

  PyModule_AddObject(module, "Record", (PyObject *) &record_type);
  return module;
}
```
在 Interpreter 中测试:
```python
Python 3.5.3 (default, Mar  6 2018, 13:53:21) 
[GCC 5.4.0 20160609] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> from myclass import Record
>>> help(Record)
Help on class Record in module myclass:

class Record(builtins.object)
 |  built-in Record type
 |  
 |  Methods defined here:
 |  
 |  __new__(*args, **kwargs) from builtins.type
 |      Create and return a new object.  See help(type) for accurate signature.
 |  
 |  ----------------------------------------------------------------------
 |  Data descriptors defined here:
 |  
 |  name
 |      name of the record
 |  
 |  value
 |      value of the record
(END)
>>> # 我们看到 Record 类有 name 和 value 两个成员，并且都有我们写好的 Doc
... 
>>> r = Record()
>>> r
<myclass.Record object at 0x7f2127a047b0>
>>> r.name  # 由于我们在 member table 里面给 name 的 type 为 T_OBJECT_EX，而我们又没有初始化 name 的值，所以会异常
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
AttributeError: name
>>> r.name = "Year"
>>> r.value  # 我们在 member table 里面给 value 的 type 为 T_OBJECT，而我们又没有初始化 value 的值，所以会返回 None
>>> r.value = 2018  # 由于我们给 value 的 flag 为 READONLY， 所以无法写入置它的值
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
AttributeError: readonly attribute
>>> r.name
'Year'
>>> r.name = 233333
>>> r.name
233333
>>> r.name = list()  # name 的类型可以为任意 Object
>>> r.name
[]
```
至此，我们基本实现了带有两个成员变量的类。  
但是这个类还有些行为不是我们所期望的，比如 `name` 的类型应该限定为 `string`，而 `value` 的 Readonly 属性并不完全等同于 private 的属性。

**2.2 定义类的 `new` 函数**  
`new` 函数相当于 C++ 里面的 Constructor，对应 Python 中的 `__new__` 函数，我们在这一步创建并返回类的实例。`new` 函数和 Python 中的 `__new__` 有相同的 prototype，相当于 `def new(type, bases=tuple(), attributes=dict())`。  
```c
/* new 函数的参数分别为 type，base 元组和 attributes 字典 */
static PyObject *record_new(PyTypeObject *type, PyObject *bases, PyObject *attributes) {
  // self 是我们创建的并返回的实例
  record_object *self;
  // type->tp_alloc 申请内存
  self = (record_object *) type->tp_alloc(type, 0);
  if (self != NULL) {
    // 初始化 name 为空字符串
    self->name = PyUnicode_FromString("");
    if (self->name == NULL) {
      Py_DECREF(self);
      return NULL;
    }
    // 初始化 value 为 None
    self->value = Py_None;
  }

  // 把 record_object * 转换 成 PyObject * 并返回
  // 由此我们可知返回的其实是 record_object(Impl) 的指针，而不是 record_type 的指针
  // record_object 里面的 PyObject_HEAD 宏会帮我们加入一个 `ob_base` 属性，其中包含 Object 的引用计数和它的 type 指针
  // 所以我们猜想 Python 里面 object.member 和 object.method 其实都绕了一层，即先从 object 取 type 指针，再从 type 指针取对应的属性和方法
  return (PyObject *) self;
}
```
`new` 函数做的其实就是创建变量和申请内存空间，虽然能为成员变量赋予初始值，但这个过程对类的使用者来说是封闭的，即用户不能通过传参给 `new` 函数来控制成员变量的初始化过程。  
其实这属于 `init` 函数的工作。

**2.3 定义类的 `init` 函数**  
`init` 函数给用户提供了一个初始化 Object 的接口。它和 `__init__` 函数有类似的 prototype，相当于 `def __init__(self, *args, **kwargs)`。  
```c
// init 函数不再返回 self，除此之外它和普通的有 PyCFunction 一样参数列表形式
static int record_init(record_object *self, PyObject *args, PyObject *kwargs) {
  PyObject *name=NULL, *value=NULL, *tmp;
  static char *kwlist[] = {"name", "value",NULL};

  // 从参数列表解析 name 和 value 以初始化 Object
  if (! PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", kwlist,
                                    &name, &value))
    return -1;

  // 赋值的时候要额外小心，我们先把旧值保存到 tmp，再赋予新值，再减少旧值的引用计数。
  // 详情我们在下一章再讨论
  if (name) {
    tmp = self->name;
    Py_INCREF(name);
    self->name = name;
    Py_XDECREF(tmp);
  }

  if (value) {
    tmp = self->value;
    Py_INCREF(value);
    self->value = value;
    Py_XDECREF(tmp);
  }

  return 0;
}
```
定义 `new` 函数之后再把它插入到 `tp_init` slot里面去：`record_type.tp_init = (initproc)record_init;`
在 Interpreter 里面测试:
```python
Python 3.5.3 (default, Mar  6 2018, 15:35:52) 
[GCC 5.4.0 20160609] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> from myclass import Record
>>> r = Record(name="Year", value=2018)
>>> r.name
'Year'
>>> r.value
2018
>>> r = Record()  # 现在我们的 init 函数要求至少提供 name 参数
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: Required argument 'name' (pos 1) not found
>>> r = Record(name="Year")
>>> r.name
'Year'
>>> 
```
嗯，现在我们已经给 `Record` 类添加了两个成员变量，并且定义了 `new` 函数 和 `init` 成员函数来控制类成员的创建和初始化。   
接下来给我们 `Record` 添加一些成员函数。  

**2.4 定义类的成员函数**  
我们个 `Record` 类添加一个成员函数 `print` 来显示其 name 和 value， 添加一个 set_value 来改变 value 的值。
```c
/* 不同于 init 函数，成员函数都要以 PyObject * 为返回值 */
static PyObject *record_print(record_object *self) {
  return PyUnicode_FromFormat("%S %S", self->name, self->value);
}

static PyObject *record_set_value(record_object *self, PyObject *args) {
  PyObject *value = NULL, *tmp;
  if (!PyArg_ParseTuple(args, "O", &value)) {
    return NULL;
  }

  if (value) {
    tmp = self->value;
    Py_INCREF(value);
    self->value = value;
    Py_XDECREF(tmp);
  }

  return Py_None;
}
```
类的成员函数和普通函数有相同的 `prototype`，第一个参数是 `self` 表示 instance。
定义好成员函数后，把它们加入到一个 function table 里面去，再把这个 table 插入 `tp_methods` slot(是不是很熟悉的流程了？)
```c
static PyMethodDef record_methods[] = {
    {"print", (PyCFunction) record_print, METH_NOARGS, "Print name and value"},
    {"set_value", (PyCFunction) record_set_value, METH_VARARGS, "set value"},
    {NULL}
};

...
  ...
  record_type.tp_methods = record_methods;
  ...
...

```
在 Interpreter 里面测试:
```python
Python 3.5.3 (default, Mar  6 2018, 16:20:01) 
[GCC 5.4.0 20160609] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> from myclass import Record
>>> r = Record("year")
>>> r.print()
'year None'
>>> r.set_value(2018)
>>> r.print()
'year 2018'
```

**2.5 定义类的 property**  
我们的 `Record` 类还有两个令人不爽的地方，其一是我们希望 `name` 成员属性只能是 `string` 类型，其二是我们希望通过更简单的方式来读写 `value`。  
这两点不爽都能通过 `property` 来解决。  
在实现 `property` 之前，我们先在 `init` 函数中强制 parse `name` 为 str:
```c
static int record_init(record_object *self, PyObject *args, PyObject *kwargs) {
  PyObject *name = NULL, *value = NULL, *tmp;

  static char *kwlist[] = {"name", "value", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", kwlist,
                                   &name, &value))
    return -1;

  if (name) {
    if (!PyUnicode_Check(name)) {
      PyErr_SetString(PyExc_TypeError, "The name attribute must be a string");
      return -1;
    }
    tmp = self->name;
    Py_INCREF(name);
    self->name = name;
    Py_XDECREF(tmp);
  }

  if (value) {
    tmp = self->value;
    Py_INCREF(value);
    self->value = value;
    Py_XDECREF(tmp);
  }

  return 0;
}
```
在 Interpreter 测试:
```python
Python 3.5.3 (default, Mar  6 2018, 16:46:51) 
[GCC 5.4.0 20160609] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> from myclass import Record
>>> r = Record(1)  # name 必须为 str
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: argument 1 must be str, not int
>>> r = Record("year")

>>> r.print()
'year None'
>>> 
```
在初始化 `Record` 类的实例时 `name` 必须为 `str`，但是我们仍然可以之后直接通过 `r.name = 2` 来把它改成其它类型。这时候，`property` 就派上用场了。  
定义 `property` 的过程是这样的，先定义 `getter` 和 `setter`, 再把它们放入到 getset 表里面去，然后把这张表插入到 `tp_getset` slot 里。  
我们打算把 name 和 value 都变成 property，所以我们就不再在 member 表里面定义它们了:
```c
static PyMemberDef record_members[] = {
    {NULL}
};
```
接着定义 `name` 和 `value` 的 `getter` 和 `setter` 函数:
```c
/* getter 函数 prototype 统一为 static PyObject* func_name(object* self, void *closure)
 * 其中 closure 为固定参数，我们对它兴趣不大 */
static PyObject *record_object_get_name(record_object *self, void *closure) {
  // 在返回之前记得把引用计数加 1
  Py_INCREF(self->name);
  return self->name;
}

/* setter 函数 prototype 统一为 static int func_name(object* self, PyObject *value, void *closure)
 * 返回值为 int，setter 失败时返回 -1，否则返回 0，第二个参数不需要再解析，直接可以拿来赋值用 */
static int record_object_set_name(record_object *self, PyObject *value, void *closure) {
  // 当 value 为 NULL 时相当于调用 `del instance.attribute_name`，即 `deletter`。
  // 我们不希望设置 deleter，所以此时抛出异常
  if (value == NULL) {
    PyErr_SetString(PyExc_TypeError, "Cannot delete name");
    return -1;
  }

  // 对 name 而言，value 的值必须为 str
  if (!PyUnicode_Check(value)) {
    PyErr_SetString(PyExc_TypeError, "The name attribute value must be a string");
    return -1;
  }

  Py_DECREF(self->name);
  Py_INCREF(value);
  self->name = value;

  return 0;
}

static PyObject *record_object_get_value(record_object *self, void *closure) {
  Py_INCREF(self->value);
  return self->value;
}

static int record_object_set_value(record_object *self, PyObject *value, void *closure) {
  PyObject *tmp;
  if (value == NULL) {
    PyErr_SetString(PyExc_TypeError, "Cannot delete value");
    return -1;
  }

  tmp = self->value;
  Py_INCREF(value);
  self->value = value;
  Py_DECREF(tmp);

  return 0;
}

/* 设置 getter 和 setter 表，这个表是一个 array
 * 里面每一项代表一个 `property`，
 * 每一项 5 个属性分别为: property_name, property_getter, property_setter/deletter, property_doc 和 closure */
static PyGetSetDef record_getseters[] = {
    {"name", (getter) record_object_get_name, (setter) record_object_set_name, "name property", NULL},
    {"value", (getter) record_object_get_value, (setter) record_object_set_value, "value property", NULL},
    {NULL}
};

...
  ...
  // 把 getter/setter 表插入 `tp_getset` slot
  record_type.tp_getset = record_getseters;
  ...
...

```
在 Interpreter 里面测试:
```python
Python 3.5.3 (default, Mar  6 2018, 17:11:08) 
[GCC 5.4.0 20160609] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> from myclass import Record
>>> r = Record("year")
>>> r.print()
'year None'
>>> r.name = 2  # name 必须为 str
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: The name attribute value must be a string
>>> r.value = 222  # 使用 property 而不是 set_value 来改变 value 的值
>>> r.print()
'year 222'
>>> r.name = "YEAR"
>>> r.print()
'YEAR 222'
>>> del r.name  # 不能删除 name 属性
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: Cannot delete name
>>> 
```

至此，我们已经实现了一个拥有成员变量和函数，定义了 `new` 和 `init`，并且有 `property` 的 `Record` 类。类的基本属性全部都在我们的掌握之中。  
我们将在下一章专门探索类的高级特性——类的其它特殊函数、内存管理和类的继承。

