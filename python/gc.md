# Python垃圾回收机制

本文主要结合CPython源码分析一下Python的GC机制（面试的时候被问到这个问题，之前理解不深，答得不好，一波大坑啊。。。）

Python GC主要使用引用计数（reference counting）来跟踪和回收垃圾。在引用计数的基础上，通过“标记-清除”（mark and sweep）解决容器对象可能产生的循环引用问题，通过“分代回收”（generation collection）以空间换时间的方法提高垃圾回收效率。
引用计数

引用计数法在对象内部维护了一个被其他对象引用数的引用计数值，当这个引用计数值为0时，说明这个对象不再被其他对象引用，就可以被回收了。

结合源码来看，所有Python对象的头部包含了这样一个结构PyObject（相当于继承自PyObject）：

```python
// object.h
struct _object {
    Py_ssize_t ob_refcnt;
    struct PyTypeObject *ob_type;
} PyObject;
```

ob_refcnt就是引用计数值。

例如，下面是int型对象的定义：

```python
// intobject.h
typedef struct {
        PyObject_HEAD
        long ob_ival;
} PyIntObject;
```

引用计数法有很明显的优点：

- 高效
- 运行期没有停顿
- 对象有确定的生命周期
- 易于实现

原始的引用计数法也有明显的缺点：

- 维护引用计数的次数和引用赋值成正比，而不像mark and sweep等基本与回收的内存数量有关。
- 无法解决循环引用的问题。A和B相互引用而再没有外部引用A与B中的任何一个，它们的引用计数都为1，但显然应该被回收。

为了解决这两个致命弱点，Python又引入了以下两种GC机制。
标记-清除

“标记-清除”法是为了解决循环引用问题。可以包含其他对象引用的容器对象（如list, dict, set，甚至class）都可能产生循环引用，为此，在申请内存时，所有容器对象的头部又加上了PyGC_Head来实现“标记-清除”机制。

```python
// objimpl.h
typedef union _gc_head {
    struct {
        union _gc_head *gc_next;
        union _gc_head *gc_prev;
        Py_ssize_t gc_refs;
    } gc;
    long double dummy;  /* force worst-case alignment */
} PyGC_Head;
```

在为对象申请内存的时候，可以明显看到，实际申请的内存数量已经加上了PyGC_Head的大小

```python
// gcmodule.c
PyObject *
_PyObject_GC_Malloc(size_t basicsize)
{
    PyObject *op;
    PyGC_Head *g = (PyGC_Head *)PyObject_MALLOC(
                sizeof(PyGC_Head) + basicsize);
    if (g == NULL)
        return PyErr_NoMemory();

    ......

    op = FROM_GC(g);
    return op;
}
```

举例来说，从list对象的创建中，有如下主要逻辑：

```python
// listobject.c
PyObject *
PyList_New(Py_ssize_t size)
{
    PyListObject *op;
    ......
    op = PyObject_GC_New(PyListObject, &PyList_Type);
    ......
    _PyObject_GC_TRACK(op);
    return (PyObject *) op;
}
```

_PyObject_GC_TRACK就将对象链接到了第0代对象集合中（后文详述分代回收）。

垃圾标记时，先将集合中对象的引用计数复制一份副本(以免在操作过程中破坏真实的引用计数值)：

```python
// gcmodule.c
static void
update_refs(PyGC_Head *containers)
{
    PyGC_Head *gc = containers->gc.gc_next;
    for (; gc != containers; gc = gc->gc.gc_next) {
        assert(gc->gc.gc_refs == GC_REACHABLE);
        gc->gc.gc_refs = FROM_GC(gc)->ob_refcnt;
        assert(gc->gc.gc_refs != 0);
    }
}
```

然后操作这个副本，遍历对象集合，将被引用对象的引用计数副本值减1：

```python
// gcmodule.c
static void
subtract_refs(PyGC_Head *containers)
{
    traverseproc traverse;
    PyGC_Head *gc = containers->gc.gc_next;
    for (; gc != containers; gc=gc->gc.gc_next) {
        traverse = FROM_GC(gc)->ob_type->tp_traverse;
        (void) traverse(FROM_GC(gc),
                   (visitproc)visit_decref,
                   NULL);
    }
}
```

这个traverse是对象类型定义的函数，用来遍历对象，通过传入的回调函数visit_decref来操作引用计数副本。

例如dict就要在key和value上都用visit_decref操作一遍：

```python
// dictobject.c
static int
dict_traverse(PyObject *op, visitproc visit, void *arg)
{
    Py_ssize_t i = 0;
    PyObject *pk;
    PyObject *pv;

    while (PyDict_Next(op, &i, &pk, &pv)) {
        visit(pk);
        visit(pv);
    }
    return 0;
}
```

然后根据引用计数副本值是否为0将集合内的对象分成两类，reachable和unreachable，其中unreachable是可以被回收的对象：

```python
// gcmodule.c
static void
move_unreachable(PyGC_Head *young, PyGC_Head *unreachable)
{
    PyGC_Head *gc = young->gc.gc_next;
    while (gc != young) {
        PyGC_Head *next;
        if (gc->gc.gc_refs) {
            PyObject *op = FROM_GC(gc);
            traverseproc traverse = op->ob_type->tp_traverse;
            assert(gc->gc.gc_refs > 0);
            gc->gc.gc_refs = GC_REACHABLE;
            (void) traverse(op,
                            (visitproc)visit_reachable,
                            (void *)young);
            next = gc->gc.gc_next;
        }
        else {
            next = gc->gc.gc_next;
            gc_list_move(gc, unreachable);
            gc->gc.gc_refs = GC_TENTATIVELY_UNREACHABLE;
        }
        gc = next;
    }
}
```

在处理了weak reference和finalizer等琐碎细节后（本文不展开讲述，有兴趣的童鞋请参考python源码），就可以回收unreachable中的对象了。
分代回收

分代回收的整体思想是：将系统中的所有内存块根据其存活时间划分为不同的集合，每个集合就成为一个“代”，垃圾收集频率随着“代”的存活时间的增大而减小，存活时间通常利用经过几次垃圾回收来度量。

用来表示“代”的结构体是gc_generation， 包括了当前代链表表头、对象数量上限、当前对象数量：

```python
// gcmodule.c
struct gc_generation {
    PyGC_Head head;
    int threshold; /* collection threshold */
    int count; /* count of allocations or collections of younger
              generations */
};
```

Python默认定义了三代对象集合，索引数越大，对象存活时间越长。

```python
#define NUM_GENERATIONS 3
#define GEN_HEAD(n) (&generations[n].head)

/* linked lists of container objects */
static struct gc_generation generations[NUM_GENERATIONS] = {
    /* PyGC_Head,               threshold,  count */
    {{{GEN_HEAD(0), GEN_HEAD(0), 0}},   700,        0},
    {{{GEN_HEAD(1), GEN_HEAD(1), 0}},   10,     0},
    {{{GEN_HEAD(2), GEN_HEAD(2), 0}},   10,     0},
};
```

新生成的对象会被加入第0代，前面_PyObject_GC_Malloc中省略的部分就是Python GC触发的时机。每新生成一个对象都会检查第0代有没有满，如果满了就开始着手进行垃圾回收：

```python
 g->gc.gc_refs = GC_UNTRACKED;
 generations[0].count++; /* number of allocated GC objects */
 if (generations[0].count > generations[0].threshold &&
     enabled &&
     generations[0].threshold &&
     !collecting &&
     !PyErr_Occurred()) {
          collecting = 1;
          collect_generations();
          collecting = 0;
 }
```