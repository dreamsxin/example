---
title: C 拓展 Python 实战（四）—— 类的高级属性
date: 2018-03-10 20:49:44
categories:
- 技术
tags:
- c++
- python
---

在之前的示例中我们使用了大量的 `Py_INCREF` 和 `Py_DECREF` 来管理 Python Object 的引用计数。 `Empty` 的 `value_setter` 里也使用了一种非常啰嗦的语法:
```c
static int record_object_set_value(record_object *self, PyObject *value, void *closure) {
  PyObject *tmp;
  if (value == NULL) {
    PyErr_SetString(PyExc_TypeError, "Cannot delete value");
    return -1;
  }

  // 使用了一个非常啰嗦的临时变量
  tmp = self->value;
  Py_INCREF(value);
  self->value = value;
  Py_DECREF(tmp);

  return 0;
}
```
这一切都是为了内存管理。  
本章我们将回顾 Python 的垃圾回收机制，包括引用计数、循环检测和弱引用这些概念；  
在这个基础上，我们再来讨论在设计我们的类的时候要如何做才能避免内存泄漏；  
接着，我们给类添加继承和被继承的功能，并展示如何复写特定的属性和函数；   
最后，我们给类添加那些锦上添花的高级功能，比如静态属性、静态方法、类方法，以及一些 `__magic_function__`。   
<!-- more -->

## 一、Python 的垃圾回收机制
Python 里面每一个对象都用 `malloc` 创建于 heap 上，所以每一个不再需要的对象都必须调用一次且仅一次 `free` 来释放内存，否则就会出问题(内存泄露、coredump或者其它未知问题)。  
以上事实并不难理解，但是很难落实，因为不好掌握 `free` 的时机。依靠人的自觉和记忆力来执行 `free` 操作总是不靠谱的。所以很多编程语言都使用了某种自动的垃圾回收策略。 Python 所使用的策略就是**引用计数**。  

**1.1 引用计数**  
每当 Python 创建一个 Object，就会在 heap 中申请一块内存。每个 Object 会记录自己的引用计数，当引用计数减少到 0 的时候，就会 1.触发 Object 的 Destructor 并 2.销毁对象回收内存。  
什么时候会引起引用计数的变化呢？在讨论这些情况之前，我们不妨先重申一下对象和变量的概念。  
大多时候我们都是使用的变量，但变量不等于对象，变量只是对象的一个引用。`r = 1` 时，我们习惯说 `r` 是一个值为 `1` 的 `int`，但更严格地说 `r` 只是一个指向 `int` 的变量。  
我们可以把任何左值看做变量，`r.name`，`a_list[0]` 或者 `a_dict["key"]` 都可以看做变量，因为它们都具有引用某个对象的能力。  
每个对象在被创建的时候，默认引用计数是 1。之后每当多一个变量指向引用的时候，我们希望引用计数加 1，每少一个变量指向引用计数的时候，引用计数减 1。只有一种情况例外，那就是新创建的对象第一次被一个变量引用的时候，引用计数不变。否则的话，`r = Record()` 就会造成新创建的 `Record` 对象有两个引用！  
于是我们就可以总结出以下引起引用计数变化的情况:
1. 创建对象, 比如 `r = Record()`, `Record` 对象的引用计数为 1；  
2. 通过变量 A 赋值变量 B，比如 `r2 = r` 把 `Record` 对象的引用计数从 1 加到 2，而 `r2` 原来所指的对象的引用计数减 1；  
3. 删除变量，比如显式地 `del r` 或者销毁 `v` 从而删除 `v.x`，使变量所指对象的引用计数减 1；  
4. 把对象作为参数传入函数时引用计数加 1，函数返回后作为参数的引用计数减 1；    

**1.2 循环检测**
```python
def circle_ref():
  r = Record()
  l = list()
  l.append(r)
  r.value = l

circle_ref()
```
在 `circle_def` 里面，`r` 包含对 `l` 的引用，`l` 又包含对 `r` 的引用。所以当函数执行完之后，本应该释放的变量由于循环引用而没有被释放。  
为了处理这种问题，Python 使用循环检测(cycle detector) 作为引用计数机制的补充。简单说就是即使 A、B 两个对象互相引用，但是除此之外没有任何对象指向它们中的任何一个时，就可以把它们当做垃圾来回收(我们不讨论标记清除这种垃圾回收机制)。  

**1.3 弱引用**
弱引用的作用是保持对对象的引用的同时又不增加对象的引用计数。比如有时候我们想要持有一份对对象的记录，当对象不再需要时，删除记录。  
如果我们直接使用强引用做记录，会造成对象的引用计数加 1，而对象不再被需要时，由于记录自身的存在，引用计数不会减到 0，从而导致内存泄漏。

## 二、设计对内存资源友好的类
引用计数、循环检测和弱引用是 Python 提供给我们的利器。当我们使用 C 来拓展 Python 的时候，也要实现 Python 所提供的这些便利性。  
在这一节，我们将优化 `Record` 类，顺引用计数的操作逻辑，并使它支持循环检测和弱引用。  

**2.1 管理引用计数**  
我们在第一节提到了 4 个改变引用计数的情况，现在我们依次从这四个方面检查我们的 `Record` 类:  
- 改变引用计数情况1. 创建对象, 比如 `r = Record()`, `Record` 对象的引用计数为 1
```python
Python 3.5.3 (default, Mar  8 2018, 14:52:49) 
[GCC 5.4.0 20160609] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> from myclass import Record
>>> import sys
>>> x = Record("x")
>>> sys.getrefcount(x)  
2  # 把 x 做参数传入函数会使引用计数加 1，所以此时得到 1+1 为正确结果
>>> 
```
由于新创建的对象默认引用计数是 1，所以只要我们不在 `new` 函数和 `init` 函数里面 `Py_INCREF` 就好了。
- 改变引用计数情况2. 通过变量 A 赋值变量 B，比如 `r2 = r` 把 `Record` 对象的引用计数从 1 加到 2，而 `r2` 原来所指的对象的引用计数减 1
```python
Python 3.5.3 (default, Mar  8 2018, 14:52:49) 
[GCC 5.4.0 20160609] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> from myclass import Record
>>> import sys
>>> a = Record("a")
>>> b = Record("b")
>>> c = Record("c")
>>> a.value = b
>>> sys.getrefcount(a)
2  # 加 1+1 后的结果，OK(以后不再解释为什么引用计数多了1)
>>> sys.getrefcount(b)
3  # a.value 指向了 b，b 的引用计数变为 2, OK
>>> a.value = c
>>> sys.getrefcount(b),sys.getrefcount(c)
(2, 3)  # a.value 指向对象 c，b 的引用计数变为减 1 变为 1, c 的引用计数加 1 变为 2， OK
```
赋值时涉及到两个引用计数操作，一是原来的对象引用计数要减 1，二是新的对象引用计数加 1。我们再来看看 `Record.value` 的 `setter`源码:  
```c
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
```
`return` 语句上面的四行代码，就实现了新旧对象的引用计数的增减。  
我们也注意到了 `tmp` 变量，为什么要多一步保存旧对象到 `tmp` 呢？为什么不直接写成如下语句:  
```c
  Py_DECREF(self->value);
  Py_INCREF(value);
  self->value = value;
```
如果写成这种简单的形式的话，执行第一句之后，`self->value` 的引用计数可能变成 0 从而触发它的 `destructor`，而 `destructor` 会执行什么样的操作我们并不知情，它有可能尝试读取 `self` 实例的 `value` (它并不知道自己就是 `self->value`)并修改某些状态，而这是一个正在被销毁的对象(我想象不到实际的情况，实际上我想了两天也每想到，Python 官方 Doc 也只是稍微提了一下。但确实有这种可能性)。

- 改变引用计数情况3. 删除变量，比如显式地 `del r` 或者销毁 `v` 从而删除 `v.x`，使变量所指对象的引用计数减 1；  
```python
Python 3.5.3 (default, Mar  8 2018, 14:52:49) 
[GCC 5.4.0 20160609] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> from myclass import Record
>>> import sys
>>> a = Record("a")
>>> b = Record("b")
>>> c = a
>>> sys.getrefcount(a),sys.getrefcount(b),sys.getrefcount(c)
(3, 2, 3)  # a 和 c 都指向 Record("a") 对象， OK
>>> del c
>>> sys.getrefcount(a),sys.getrefcount(b)
(2, 2)  # 删除 c 后，a 的引用计数变回 2， OK
>>> a.value = b
>>> sys.getrefcount(a),sys.getrefcount(b)
(2, 3)  # a.value 指向 b，b 的引用计数加 1， OK
>>> del a
>>> sys.getrefcount(b)
3  # 删除 a 后，a.value 不存在了，然而 b 的引用计数还是 3，有问题！！！
>>> 
```
我们的 `Record` 对象在被销毁后，并没有释放其占有的资源。而如果要实现这个效果，我们还需要定义我们的 destructor(dealloc) 函数。
```c
static void record_dealloc(record_object *self) {
  Py_XDECREF(self->name);
  Py_XDECREF(self->value);
  Py_TYPE(self)->tp_free((PyObject*)self);
}
...
  ...
  record_type.tp_dealloc = (destructor)record_dealloc;
  ...
...
```
我们的 destructor 把自己所有的成员变量的引用计数都减去 1。由于成员变量有可能为 `NULL`，所以我们使用的是 `Py_XDECREF`。  
然后，我们通过 TypeObject 的 `tp_free` 函数来释放自己所占用的资源。
现在我们再来测试一下 `Record` 类:
```python
Python 3.5.3 (default, Mar  8 2018, 15:56:15) 
[GCC 5.4.0 20160609] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> from myclass import Record
>>> import sys
>>> a = Record("a")
>>> b = Record("b")
>>> a.value = b
>>> sys.getrefcount(a),sys.getrefcount(b)
(2, 3)  # OK
>>> del a
>>> sys.getrefcount(b)
2  # a 被销毁，a.value 也被销毁，而 b 的引用计数也从 3 减到 2，正常！
>>> 
```

- 改变引用计数情况4. 把对象作为参数传入函数时引用计数加 1，函数返回后作为参数的引用计数减 1；

这个在上面三种情况的每个测试中都有体现，就不再细说。  
至此，我们的 `Record` 类已经能够很好地支持引用计数的特性了。

**2.2 支持循环检测**  
一般来说只有容器类型的类才需要支持循环检测，而 `Record` 的 `value` 并不限制数据类型，因此就显得有必要。  
要使 `Record` 类支持循环检测，我们需要提供一个固定范式的 `traverse` 函数来遍历所有可能涉及到循环引用的成员属性:
```c
/* 这个函数的参数和函数体都是固定的 */
static int record_traverse(record_object *self, visitproc visit, void *arg) {
  int vret;

  if (self->name) {
    vret = visit(self->name, arg);
    if (vret != 0)
      return vret;
  }
  if (self->value) {
    vret = visit(self->value, arg);
    if (vret != 0)
      return vret;
  }

  return 0;
}
```
以及一个 `clear` 函数来清除所有可能涉及到循环引用的成员属性:
```c
static int record_clear(record_object *self) {
  PyObject *tmp;

  tmp = self->name;
  self->name = NULL;
  Py_XDECREF(tmp);

  tmp = self->value;
  self->value = NULL;
  Py_XDECREF(tmp);

  return 0;
}
```
以上两个函数可以使用宏来简化:
```c
static int record_traverse(record_object *self, visitproc visit, void *arg) {
  Py_VISIT(self->name);
  Py_VISIT(self->value);
  return 0;
}

static int record_clear(record_object *self) {
  Py_CLEAR(self->name);
  Py_CLEAR(self->value);
  return 0;
}
```
之后，再调整 `dealloc` 函数以避免循环 gc：
```c
static void record_dealloc(record_object *self) {
  PyObject_GC_UnTrack(self);
  record_clear(self);
  Py_TYPE(self)->tp_free((PyObject *) self);
}
```
做好这些准备后，最后再去修改对应的 `tp_slot`：
```c
...
  ...
  record_type.tp_doc = "built-in Record type";
  record_type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC;
  record_type.tp_dealloc = (destructor) record_dealloc;
  record_type.tp_traverse = (traverseproc) record_traverse;
  record_type.tp_clear = (inquiry) record_clear;
  ...
...
```
这样，我们的 `Record` 类就能支持循环检测了。 

**2.3 支持弱引用**  
如果现在对我们的 `Record` 类使用弱引用是会报错的:
```python
Python 3.5.3 (default, Mar  8 2018, 15:56:15) 
[GCC 5.4.0 20160609] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import weakref
>>> from myclass import Record
>>> r = Record("r")
>>> ref = weakref.ref(r)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: cannot create weak reference to 'myclass.Record' object
>>> 
```
接下来我们让 `Record` 类支持弱引用。概括地说，要使一个类支持弱引用，需要 4 个步骤:
1. Object(`Impl`) 里面添加一个 `PyObject*` 作为 weakref list  
2. `new` 函数里面把 `weakref_list` 初始化为 `NULL`  
3. `dealloc` 函数里面用 `PyObject_ClearWeakRefs` 清空 weakref list
4. Type 里面设置 `tp_weaklistoffset`
以下是对 `Record` 类进行的修改:
```c
#include <Python.h>
#include <structmember.h>

typedef struct {
  PyObject_HEAD
  PyObject *name;
  PyObject *value;
  PyObject *weakref_list;  // 添加 weakref_list
} record_object;

...

static void record_dealloc(record_object *self) {
  PyObject_GC_UnTrack(self);
  record_clear(self);

  // 清空 weakref list
  if (self->weakref_list != NULL) {
    PyObject_ClearWeakRefs((PyObject *) self);
  }

  Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *record_new(PyTypeObject *type, PyObject *bases, PyObject *attributes) {
  record_object *self;
  self = (record_object *) type->tp_alloc(type, 0);
  if (self != NULL) {
    self->weakref_list = NULL;  // 初始化 weakref_list 为 NULL

    self->name = PyUnicode_FromString("");
    if (self->name == NULL) {
      Py_DECREF(self);
      return NULL;
    }

    self->value = Py_None;

  }
  return (PyObject *) self;
}

...

PyMODINIT_FUNC PyInit_myclass(void) {
  PyObject *module;
  module = PyModule_Create(&myclass_module);
  if (module == NULL)
    return NULL;

  record_type.tp_doc = "built-in Record type";
  record_type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC;
  record_type.tp_dealloc = (destructor) record_dealloc;
  record_type.tp_traverse = (traverseproc) record_traverse;
  record_type.tp_clear = (inquiry) record_clear;
  record_type.tp_weaklistoffset = offsetof(record_object, weakref_list);  // 设置 tp_weaklistoffset slot
  record_type.tp_new = record_new;
  record_type.tp_init = (initproc) record_init;
  record_type.tp_members = record_members;
  record_type.tp_methods = record_methods;
  record_type.tp_getset = record_getseters;
  if (PyType_Ready(&record_type) < 0) {
    return NULL;
  }

  PyModule_AddObject(module, "Record", (PyObject *) &record_type);
  return module;
}

```
其它部分的代码保持不变。我们重新编译和安装后，在 Interpreter 里面测试如下
```python
Python 3.5.3 (default, Mar 10 2018, 06:41:24) 
[GCC 5.4.0 20160609] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import weakref
>>> from myclass import Record
>>> r = Record("r")
>>> ref = weakref.ref(r)
>>> r
<myclass.Record object at 0x7ff8f2b8b848>
>>> ref
<weakref at 0x7ff8f325f5e8; to 'myclass.Record' at 0x7ff8f2b8b848>
>>> ref()
<myclass.Record object at 0x7ff8f2b8b848>
>>> del r
>>> ref
<weakref at 0x7ff8f325f5e8; dead>
>>> ref()  # 删除 r 后，弱引用 ref 状态为 dead，调用 ref 返回 None
>>>
```
终于，我们的 `Record` 类经过引用计数管理、循环检测和弱引用三方面的打磨，已经是一个内存友好、使用方便的类了！

## 三、类的继承
我们希望我们的类可以被继承，也能继承别的类。  
假设我们现在要实现一个功能大体和 `Record` 类相同的类，唯一的区别是它的 `value` 只能是 `str`。  
简单的做法是在 Python 中定义一个 `StringRecord` 类，复写其 `value property`。  
复杂的做法是在 C 里面定义一个 `Record` 类的子类，我们自己实现 `StringRecord` 类对 `Record` 类的继承细节。  

**3.1 简单的方法**  
我们不妨先这么做:
```python
from myclass import Record

>>> class StringRecord(Record):
...     @property
...     def value():
...         return super().value
...     @value.setter
...     def value(self, v):
...         if not isinstance(v, str):
...             raise AttributeError("value must be str type")
...         super(StringRecord, StringRecord).value.__set__(self, v)
... 

Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: type 'myclass.Record' is not an acceptable base type
>>> 
```
然而我们发现 `Record` 类竟然不能用作基类来继承。 要想让我们的 `Record` 类能够被继承，需要给 `tp_flag` slot 添加 `Py_TPFLAGS_BASETYPE`：
```python
...
  ...
  record_type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC | Py_TPFLAGS_BASETYPE;
  ...
...
```
是的，从接口的要求上来说只需要这样就可以了。我们在 Interpreter 中再来测试:
```python
>>> class StringRecord(Record):
...     @property
...     def value():
...         return super().value
...     @value.setter
...     def value(self, v):
...         if not isinstance(v, str):
...             raise AttributeError("value must be str type")
...         super(StringRecord, StringRecord).value.__set__(self, v)
...

# 很好，我们现在能够继承 Record 类了
>>> sr = StringRecord("John")

# 显然，Record 类的方法和属性都被继承了 
>>> sr.print()
'John None'

# 尝试给 value 设置一个 int 值，预料之中的报错
>>> sr.value = 22
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
  File "<stdin>", line 8, in value
AttributeError: value must be str type

# value 只能是 str，完美！(啊，好想玩 Switch...)
>>> sr.value = "I want to play Switch"
>>> sr.print()
'John I want to play Switch'
```

**3.2 复杂的方法**  
如果我们希望复写的函数也用 C 来实现，那么我们可能就要在 C 里面实现整个子类。其主要事项有三:  
1. Object(`Impl`) 第一项不再是 `PyObject_HEAD`, 改成父类的 `Impl`;  
2. 设置 `tp_base` slot 为父类的 Type
3. 根据需要复写父类的方法  
下面我们来在 C 里面实现 `StringRecord` 类:  
```c
...  

// 在 `record_type` 的定义之后添加 `string_record_type` 的定义
typedef struct {
  record_object base_record;  // 继承其它类时，Object 第一项必须为父类
} string_record_object;

/* 重新定义 value setter */
static int string_record_object_set_value(record_object *self, PyObject *value, void *closure) {
  if (value == NULL) {
    PyErr_SetString(PyExc_TypeError, "Cannot delete value");
    return -1;
  }

  if (!PyUnicode_Check(value)) {
    PyErr_SetString(PyExc_TypeError, "The value attribute value must be a string");
    return -1;
  }

  // 我们 100% 肯定 value 一定是 str，所以不会有 destructor 会回调回来，所以就不再需要 tmp 临时变量
  Py_DECREF(self->value);
  self->value = value;
  Py_INCREF(value);

  return 0;
}

// 重新设置 getter_setter 表，只需要替换掉 value sster，其它不变
static PyGetSetDef string_record_getseters[] = {
    {"name", (getter) record_object_get_name, (setter) record_object_set_name, "name property", NULL},
    {"value", (getter) record_object_get_value, (setter) string_record_object_set_value, "value property", NULL},
    {NULL}
};

// 定义新的 Type
static PyTypeObject string_record_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "myclass.StringRecord",
    sizeof(string_record_object),
    0
};

// 在 PyMODINIT_FUNC 中设置 slot 等最后的操作
PyMODINIT_FUNC PyInit_myclass(void) {
  PyObject *module;
  module = PyModule_Create(&myclass_module);
  if (module == NULL)
    return NULL;

  record_type.tp_doc = "built-in Record type";
  record_type.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC | Py_TPFLAGS_BASETYPE;
  record_type.tp_dealloc = (destructor) record_dealloc;
  record_type.tp_traverse = (traverseproc) record_traverse;
  record_type.tp_clear = (inquiry) record_clear;
  record_type.tp_weaklistoffset = offsetof(record_object, weakref_list);
  record_type.tp_new = record_new;
  record_type.tp_init = (initproc) record_init;
  record_type.tp_members = record_members;
  record_type.tp_methods = record_methods;
  record_type.tp_getset = record_getseters;
  if (PyType_Ready(&record_type) < 0) {
    return NULL;
  }
  PyModule_AddObject(module, "Record", (PyObject *) &record_type);

  // 大部分属性都继承父类的，只需要额外设置 tp_base 以及需要修改的 tp_getset
  string_record_type.tp_base = &record_type;
  string_record_type.tp_getset = string_record_getseters;
  if (PyType_Ready(&string_record_type) < 0) {
    return NULL;
  }
  PyModule_AddObject(module, "StringRecord", (PyObject *) &string_record_type);

  return module;
}
```
以上就是 `StringRecord` 在 C 里面的定义。在 Interpreter 里面测试如下:
```python
>>> from myclass import StringRecord
>>> sr = StringRecord()  # new 和 init 等方法仍然是继承来的
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: Required argument 'name' (pos 1) not found
>>> sr = StringRecord(name="John")
>>> sr.value = 22  # value setter 使用了新的函数
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
TypeError: The value attribute value must be a string
>>> sr.value = "I really want to play Switch!!!"
>>> sr.print()
'John I really want to play Switch!!!'
>>> 
```

## 四、静态属性、静态方法、类属性以及 `__magic_functions__`
`Record` 类最重要的属性都已经实现了，还剩下一些锦上添花的高级属性，我们简要地过一下。

**4.1 静态属性**  
静态属性，或者我们把它称之为类的属性，显然并非定义在 Object(`Impl`) 里面，而应该在 Type 里面。Type 有一个 `tp_dict` slot 可以用来保存 Type 的属性，也就是类的属性。  
如果我们要给 `Record` 类添加一个静态属性 `count` 来记录实例数量，我们可以这么做:
```c
...
  ...
  record_type.tp_dict = PyDict_New();
  PyDict_SetItem(record_type.tp_dict, PyUnicode_FromString("count"), PyLong_FromLong(0));
  ...
...
```
当然，我们也要在实例的 `new` 和 `dealloc` 函数里面做对应的修改:
```c
static void record_dealloc(record_object *self) {
  long count;
  PyObject_GC_UnTrack(self);
  record_clear(self);

  if (self->weakref_list != NULL) {
    PyObject_ClearWeakRefs((PyObject *) self);
  }

  // 销毁一个实例的时候，count 要减 1
  count = PyLong_AsLong(PyDict_GetItem(Py_TYPE(self)->tp_dict, PyUnicode_FromString("count")));
  if (count > 0) {
    PyDict_SetItem(Py_TYPE(self)->tp_dict, PyUnicode_FromString("count"), PyLong_FromLong(count - 1));
  }

  Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *record_new(PyTypeObject *type, PyObject *bases, PyObject *attributes) {
  record_object *self;
  long count;

  self = (record_object *) type->tp_alloc(type, 0);
  if (self != NULL) {
    self->weakref_list = NULL;

    self->name = PyUnicode_FromString("");
    if (self->name == NULL) {
      Py_DECREF(self);
      return NULL;
    }

    self->value = Py_None;

    // 新建一个实例的时候，count 要加 1
    count = PyLong_AsLong(PyDict_GetItem(type->tp_dict, PyUnicode_FromString("count")));
    PyDict_SetItem(type->tp_dict, PyUnicode_FromString("count"), PyLong_FromLong(count + 1));
  }
  return (PyObject *) self;
}
```
让我们编译后来测试一下:
```python
Python 3.5.3 (default, Mar 10 2018, 10:19:23) 
[GCC 5.4.0 20160609] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> from myclass import Record
>>> help(Record)  # Record 类有了一个 count 属性，初始值为 0

Help on class Record in module myclass:

class Record(builtins.object)
 |  built-in Record type
 |  
 |  Methods defined here:
 |  
 |  __init__(self, /, *args, **kwargs)
 |      Initialize self.  See help(type(self)) for accurate signature.
 |  
 |  __new__(*args, **kwargs) from builtins.type
 |      Create and return a new object.  See help(type) for accurate signature.
 |  
 |  print(...)
 |      Print name and value
 |  
 |  ----------------------------------------------------------------------
 |  Data descriptors defined here:
 |  
 |  name
 |      name property
 |  
 |  value
 |      value property
 |  
 |  ----------------------------------------------------------------------
 |  Data and other attributes defined here:
 |  
 |  count = 0
(END)

>>> a = Record("a")
>>> a.count  
1  # 新建一个实例后，count 变成 1， 正常
>>> b = Record("b")
>>> b.count  
1  # count 还是 1，理论上应该是 2，不正常！
>>> Record.count
1
>>> help(Record)

>>> Record.count
2  # 对 Record 类调用一个 help 函数后，count 值才更新为 2
>>> 
```
在以上的测试中，我们发现虽然我们成功添加了 count 这个静态属性，但是它好像没有按预期及时更新，而是需要手动调用 `help` 后才更新。这看起来像是某种缓存机制所导致的 bug。  
实际上，Python 的确缓存了内部属性的查找结果，所以为了得到正确的结果，每当我们对内部的某个属性进行更改后，应该要删除缓存。怎么删除呢？通过 `PyType_Modified` 这个函数。  
我们在 `new` 和 `dealloc` 函数里加入这步操作:
```c
static void record_dealloc(record_object *self) {
  long count;
  PyObject_GC_UnTrack(self);
  record_clear(self);

  if (self->weakref_list != NULL) {
    PyObject_ClearWeakRefs((PyObject *) self);
  }

  count = PyLong_AsLong(PyDict_GetItem(Py_TYPE(self)->tp_dict, PyUnicode_FromString("count")));
  if (count > 0) {
    PyDict_SetItem(Py_TYPE(self)->tp_dict, PyUnicode_FromString("count"), PyLong_FromLong(count - 1));
    PyType_Modified(Py_TYPE(self));  // 删除缓存
  }

  Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *record_new(PyTypeObject *type, PyObject *bases, PyObject *attributes) {
  record_object *self;
  long count;

  self = (record_object *) type->tp_alloc(type, 0);
  if (self != NULL) {
    self->weakref_list = NULL;

    self->name = PyUnicode_FromString("");
    if (self->name == NULL) {
      Py_DECREF(self);
      return NULL;
    }

    self->value = Py_None;

    count = PyLong_AsLong(PyDict_GetItem(type->tp_dict, PyUnicode_FromString("count")));
    PyDict_SetItem(type->tp_dict, PyUnicode_FromString("count"), PyLong_FromLong(count + 1));
    PyType_Modified(type);  // 删除缓存
  }
  return (PyObject *) self;
}
```
这样我们的 `count` 属性就能及时反应正确的数值了。

**4.3 类方法**  
定义类方法和定义成员方法一样，所不同的是第一个参数不再是实例，而是类，为了达到这个目的，我们只需要把函数的参数 flag 中再加上 `METH_CLASS` 就好了:
```c
static PyObject *record_get_count(PyTypeObject *type, PyObject *args) {
  PyObject *count = PyDict_GetItem(type->tp_dict, PyUnicode_FromString("count"));
  Py_INCREF(count);
  return count;
}

static PyMethodDef record_methods[] = {
    {"print", (PyCFunction) record_print, METH_NOARGS, "Print name and value"},
    {"get_count", (PyCFunction) record_get_count, METH_VARARGS | METH_CLASS, "Get instance count of this class"},
    {NULL}
};
```
如上我们就定义了一个 `get_count` 的类方法。  

**4.2 静态方法**  
定义静态方法和定义成员方法也是一样的，区别在于第一个参数是 `NULL`。而为了实现这个效果，我们只需要 `METH_STATIC` 这个 flag：
```c
static PyObject* record_get_purpose(PyObject * null, PyObject *args) {
  return Py_BuildValue("s", "Using a pair of name and value to record anything you want");
}

static PyMethodDef record_methods[] = {
    {"print", (PyCFunction) record_print, METH_NOARGS, "Print name and value"},
    {"get_count", (PyCFunction) record_get_count, METH_VARARGS | METH_CLASS, "Get instance count of this class"},
    {"get_purpose", (PyCFunction) record_get_purpose, METH_VARARGS | METH_STATIC, "Get the purpose this class"},
    {NULL}
};
```
**4.4 `__str__` 和 `__repr__`**  
Type 有两个 slot `tp_str` 和 `tp_repr`，分别用于 `__str__` 和 `__repr`。我们定义 `repr` 和 `str` 时，需要注意的只有两点: 1. 返回 str，2. 只接受 self 一个参数：  
```c
static PyObject *record_str(record_object *self) {
  return PyUnicode_FromFormat("name: %S, value: %S", self->name, self->value);
}

static PyObject *record_repr(record_object *self) {
  return PyUnicode_FromFormat("myclass.Record(name=%S, value=%S)", self->name, self->value);
}

...
  ...
  record_type.tp_doc = "built-in Record type";
  record_type.tp_str = (reprfunc)record_str;
  record_type.tp_repr = (reprfunc)record_repr;
  ...
...

```

**4.5 比较操作符**  
我们希望 `Record` 支持比较的操作， 当 `name` 和 `value` 都相等时，两个 `Record` 实例相等。  
我们只需要实现比较操作符的函数，并把它插入 `tp_richcompare` 即可:
```c
static PyObject *record_richcmp(record_object *record1, record_object *record2, int op) {
  switch (op) {
    case Py_EQ: {
      if (record1->name == record2->name && record1->value == record2->value) {
        return Py_True;
      }
      return Py_False;
    }
    default: {
      PyErr_SetString(PyExc_Exception, "Not supported operation");
      return NULL;
    }
  }
}

...
  ...
  record_type.tp_richcompare = (richcmpfunc) record_richcmp;
  ...
...
```
以下是上述几个特性的测试:
```python
Python 3.5.3 (default, Mar 10 2018, 12:11:52) 
[GCC 5.4.0 20160609] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> from myclass import Record
>>> Record.get_purpose()
'Using a pair of name and value to record anything you want'
>>> a = Record("A")
>>> a
myclass.Record(name=A, value=None)
>>> str(a)
'name: A, value: None'
>>>
>>> b = Record("B")
>>> a == b
False
>>> c = Record("A")
>>> a == c
True
>>> a.get_count()
3
>>> del a
>>> del b
>>> del c
>>> Record.count
0
>>> Record.get_count()
0
>>> 
```

## 五、总结
在 C 里面定义类的步骤，不管你想要实现到多少细节，大体上的行为是分为 3 步的:  
1. 定义 Object(data impl)
2. 定义 Type(behavior wrapper)
3. 定义各方面具体的行为和属性，并设置到 Type 的 `tp_xxx` slot 里面。  

由于我们是在用 C 编程，所以需要格外小心内存问题，Python 提供的那些内存便利机制，像引用计数、循环检测和弱引用这些，也要我们自己实现。  
我们对类的高级特性的探索就到这里了。对其它更多的细节感兴趣的朋友可以自己再去查看官方文档，或者给我留言讨论。  

