# Python 入门

## Python的语言特性

Python是一门具有强类型(即变量类型是强制要求的)、动态性、隐式类型(不需要做变量声明)、大小写敏感(var和VAR代表了不同的变量)以及面向对象(一切皆为对象)等特点的编程语言。
 
## 获取帮助

你可以很容易的通过Python解释器获取帮助。如果你想知道一个对象是如何工作的，那么你所需要做的就是调用`help(<object>)`！另外还有一些有用的方法，`dir()`会显示该对象的所有方法，还有`<object>.__doc__`会显示其文档：

```text
>>> help(5)
Help on int object:
(etc etc)
 
>>> dir(5)
['__abs__', '__add__', ...]
 
>>> abs.__doc__
'abs(number) -> number
 
Return the absolute value of the argument.'
```

## 语法

Python中没有强制的语句终止字符，且代码块是通过缩进来指示的。缩进表示一个代码块的开始，逆缩进则表示一个代码块的结束。声明以冒号(:)字符结束，并且开启一个缩进级别。单行注释以井号字符(#)开头，多行注释则以多行字符串的形式出现。赋值（事实上是将对象绑定到名字）通过等号(“=”)实现，双等号(“==”)用于相等判断，”+=”和”-=”用于增加/减少运算(由符号右边的值确定增加/减少的值)。这适用于许多数据类型，包括字符串。你也可以在一行上使用多个变量。例如：

```text
>>> myvar = 3
>>> myvar += 2
>>> myvar
5
>>> myvar -= 1
>>> myvar
4
"""This is a multiline comment.
The following lines concatenate the two strings."""
>>> mystring = "Hello"
>>> mystring += " world."
>>> print mystring
Hello world.
# This swaps the variables in one line(!).
# It doesn't violate strong typing because values aren't
# actually being assigned, but new objects are bound to
# the old names.
>>> myvar, mystring = mystring, myvar
```

## 数据类型

Python具有列表（list）、元组（tuple）和字典（dictionaries）三种基本的数据结构，而集合(sets)则包含在集合库中(但从Python2.5版本开始正式成为Python内建类型)。列表的特点跟一维数组类似（当然你也可以创建类似多维数组的“列表的列表”），字典则是具有关联关系的数组（通常也叫做哈希表），而元组则是不可变的一维数组（Python中“数组”可以包含任何类型的元素，这样你就可以使用混合元素，例如整数、字符串或是嵌套包含列表、字典或元组）。数组中第一个元素索引值(下标)为0，使用负数索引值能够从后向前访问数组元素，-1表示最后一个元素。数组元素还能指向函数。来看下面的用法：

```text
>>> sample = [1, ["another", "list"], ("a", "tuple")]
>>> mylist = ["List item 1", 2, 3.14]
>>> mylist[0] = "List item 1 again" # We're changing the item.
>>> mylist[-1] = 3.21 # Here, we refer to the last item.
>>> mydict = {"Key 1": "Value 1", 2: 3, "pi": 3.14}
>>> mydict["pi"] = 3.15 # This is how you change dictionary values.
>>> mytuple = (1, 2, 3)
>>> myfunction = len
>>> print myfunction(mylist)
3
```

你可以使用:运算符访问数组中的某一段，如果:左边为空则表示从第一个元素开始，同理:右边为空则表示到最后一个元素结束。负数索引则表示从后向前数的位置（-1是最后一个项目），例如：

```text
>>> mylist = ["List item 1", 2, 3.14]
>>> print mylist[:]
['List item 1', 2, 3.1400000000000001]
>>> print mylist[0:2]
['List item 1', 2]
>>> print mylist[-3:-1]
['List item 1', 2]
>>> print mylist[1:]
[2, 3.14]
# Adding a third parameter, "step" will have Python step in
# N item increments, rather than 1.
# E.g., this will return the first item, then go to the third and
# return that (so, items 0 and 2 in 0-indexing).
>>> print mylist[::2]
['List item 1', 3.14]
```

## 字符串

Python中的字符串使用单引号(‘)或是双引号(“)来进行标示，并且你还能够在通过某一种标示的字符串中使用另外一种标示符(例如 “He said ‘hello’.”)。而多行字符串可以通过三个连续的单引号(”’)或是双引号(“””)来进行标示。Python可以通过u”This is a unicode string”这样的语法使用Unicode字符串。如果想通过变量来填充字符串，那么可以使用取模运算符(%)和一个元组。使用方式是在目标字符串中从左至右使用%s来指代变量的位置，或者使用字典来代替，示例如下：

```text
>>>print "Name: %s\
Number: %s\
String: %s" % (myclass.name, 3, 3 * "-")
Name: Poromenos
Number: 3
String: ---
 
strString = """This is
a multiline
string."""
 
# WARNING: Watch out for the trailing s in "%(key)s".
>>> print "This %(verb)s a %(noun)s." % {"noun": "test", "verb": "is"}
This is a test.
```

## 流程控制

Python中可以使用if、for和while来实现流程控制。Python中并没有select，取而代之使用if来实现。使用for来枚举列表中的元素。如果希望生成一个由数字组成的列表，则可以使用range(<number>)函数。以下是这些声明的语法示例：

```text
rangelist = range(10)
>>> print rangelist
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
for number in rangelist:
    # Check if number is one of
    # the numbers in the tuple.
    if number in (3, 4, 7, 9):
        # "Break" terminates a for without
        # executing the "else" clause.
        break
    else:
        # "Continue" starts the next iteration
        # of the loop. It's rather useless here,
        # as it's the last statement of the loop.
        continue
else:
    # The "else" clause is optional and is
    # executed only if the loop didn't "break".
    pass # Do nothing
 
if rangelist[1] == 2:
    print "The second item (lists are 0-based) is 2"
elif rangelist[1] == 3:
    print "The second item (lists are 0-based) is 3"
else:
    print "Dunno"
 
while rangelist[1] == 1:
    pass
```

## 函数

函数通过“def”关键字进行声明。可选参数以集合的方式出现在函数声明中并紧跟着必选参数，可选参数可以在函数声明中被赋予一个默认值。已命名的参数需要赋值。函数可以返回一个元组（使用元组拆包可以有效返回多个值）。Lambda函数是由一个单独的语句组成的特殊函数，参数通过引用进行传递，但对于不可变类型(例如元组，整数，字符串等)则不能够被改变。这是因为只传递了该变量的内存地址，并且只有丢弃了旧的对象后，变量才能绑定一个对象，所以不可变类型是被替换而不是改变（译者注：虽然Python传递的参数形式本质上是引用传递，但是会产生值传递的效果）。例如：

```text
# 作用等同于 def funcvar(x): return x + 1
funcvar = lambda x: x + 1
>>> print funcvar(1)
2
 
# an_int 和 a_string 是可选参数，它们有默认值
# 如果调用 passing_example 时只指定一个参数，那么 an_int 缺省为 2 ，a_string 缺省为 A default string。如果调用 passing_example 时指定了前面两个参数，a_string 仍缺省为 A default string。
# a_list 是必备参数，因为它没有指定缺省值。
def passing_example(a_list, an_int=2, a_string="A default string"):
    a_list.append("A new item")
    an_int = 4
    return a_list, an_int, a_string
 
>>> my_list = [1, 2, 3]
>>> my_int = 10
>>> print passing_example(my_list, my_int)
([1, 2, 3, 'A new item'], 4, "A default string")
>>> my_list
[1, 2, 3, 'A new item']
>>> my_int
10
```

## 类

Python支持有限的多继承形式。私有变量和方法可以通过添加至少两个前导下划线和最多尾随一个下划线的形式进行声明（如“__spam”，这只是惯例，而不是Python的强制要求）。当然，我们也可以给类的实例取任意名称。例如：

```text
class MyClass(object):
    common = 10
    def __init__(self):
        self.myvariable = 3
    def myfunction(self, arg1, arg2):
        return self.myvariable
 
    # This is the class instantiation
>>> classinstance = MyClass()
>>> classinstance.myfunction(1, 2)
3
# This variable is shared by all classes.
>>> classinstance2 = MyClass()
>>> classinstance.common
10
>>> classinstance2.common
10
# Note how we use the class name
# instead of the instance.
>>> MyClass.common = 30
>>> classinstance.common
30
>>> classinstance2.common
30
# This will not update the variable on the class,
# instead it will bind a new object to the old
# variable name.
>>> classinstance.common = 10
>>> classinstance.common
10
>>> classinstance2.common
30
>>> MyClass.common = 50
# This has not changed, because "common" is
# now an instance variable.
>>> classinstance.common
10
>>> classinstance2.common
50
 
# This class inherits from MyClass. The example
# class above inherits from "object", which makes
# it what's called a "new-style class".
# Multiple inheritance is declared as:
# class OtherClass(MyClass1, MyClass2, MyClassN)
class OtherClass(MyClass):
    # The "self" argument is passed automatically
    # and refers to the class instance, so you can set
    # instance variables as above, but from inside the class.
    def __init__(self, arg1):
        self.myvariable = 3
        print arg1
 
>>> classinstance = OtherClass("hello")
hello
>>> classinstance.myfunction(1, 2)
3
# This class doesn't have a .test member, but
# we can add one to the instance anyway. Note
# that this will only be a member of classinstance.
>>> classinstance.test = 10
>>> classinstance.test
10
```

## 异常

Python中的异常由 try-except [exceptionname] 块处理，例如：

```text
def some_function():
    try:
        # Division by zero raises an exception
        10 / 0
    except ZeroDivisionError:
        print "Oops, invalid."
    else:
        # Exception didn't occur, we're good.
        pass
    finally:
        # This is executed after the code block is run
        # and all exceptions have been handled, even
        # if a new exception is raised while handling.
        print "We're done with that."
 
>>> some_function()
Oops, invalid.
We're done with that.
```

## 导入

外部库可以使用 import [libname] 关键字来导入。同时，你还可以用 from [libname] import [funcname] 来导入所需要的函数。例如：

```text
import random
from time import clock
 
randomint = random.randint(1, 100)
>>> print randomint
64
```

## 文件I / O

Python针对文件的处理有很多内建的函数库可以调用。例如，这里演示了如何序列化文件(使用pickle库将数据结构转换为字符串)：

```text
import pickle
mylist = ["This", "is", 4, 13327]
# Open the file C:\\binary.dat for writing. The letter r before the
# filename string is used to prevent backslash escaping.
myfile = open(r"C:\\binary.dat", "w")
pickle.dump(mylist, myfile)
myfile.close()
 
myfile = open(r"C:\\text.txt", "w")
myfile.write("This is a sample string")
myfile.close()
 
myfile = open(r"C:\\text.txt")
>>> print myfile.read()
'This is a sample string'
myfile.close()
 
# Open the file for reading.
myfile = open(r"C:\\binary.dat")
loadedlist = pickle.load(myfile)
myfile.close()
>>> print loadedlist
['This', 'is', 4, 13327]
```

## 其它杂项

* 数值判断可以链接使用，例如 1<a<3 能够判断变量 a 是否在1和3之间。
* 可以使用 del 删除变量或删除数组中的元素。
* 列表推导式(List Comprehension)提供了一个创建和操作列表的有力工具。列表推导式由一个表达式以及紧跟着这个表达式的for语句构成，for语句还可以跟0个或多个if或for语句，来看下面的例子：

```text
>>> lst1 = [1, 2, 3]
>>> lst2 = [3, 4, 5]
>>> print [x * y for x in lst1 for y in lst2]
[3, 4, 5, 6, 8, 10, 9, 12, 15]
>>> print [x for x in lst1 if 4 > x > 1]
[2, 3]
# Check if an item has a specific property.
# "any" returns true if any item in the list is true.
>>> any([i % 3 for i in [3, 3, 4, 4, 3]])
True
# This is because 4 % 3 = 1, and 1 is true, so any()
# returns True.
 
# Check how many items have this property.
>>> sum(1 for i in [3, 3, 4, 4, 3] if i == 4)
2
>>> del lst1[0]
>>> print lst1
[2, 3]
>>> del lst1
```

* 全局变量在函数之外声明，并且可以不需要任何特殊的声明即能读取，但如果你想要修改全局变量的值，就必须在函数开始之处用global关键字进行声明，否则Python会将此变量按照新的局部变量处理（请注意，如果不注意很容易被坑）。例如：

```text
number = 5
 
def myfunc():
    # This will print 5.
    print number
 
def anotherfunc():
    # This raises an exception because the variable has not
    # been bound before printing. Python knows that it an
    # object will be bound to it later and creates a new, local
    # object instead of accessing the global one.
    print number
    number = 3
 
def yetanotherfunc():
    global number
    # This will correctly change the global.
    number = 3
```

## 小结

本教程并未涵盖Python语言的全部内容(甚至连一小部分都称不上)。Python有非常多的库以及很多的功能特点需要学习，所以要想学好Python你必须在此教程之外通过其它方式，例如阅读Dive into Python。我希望这个教程能给你一个很好的入门指导。如果你觉得本文还有什么地方值得改进或添加，或是你希望能够了解Python的哪方面内容，请留言。

## 库的搜索路径

```python
import sys
sys.path.append('/home/myleft/python')
```

或者在dist-packages（即/usr/lib/python2.7/dist-packages）添加一个路径文件，如`myPath.pth`，必须以`.pth`为后缀，内容如下：
```text
/home/myleft/python
```