# ctypes --- Python 的外部函数库

- https://docs.python.org/zh-cn/3.11/library/ctypes.html
- https://github.com/python/cpython/tree/3.11/Lib/ctypes

##　载入动态连接库
ctypes 导出了 cdll 对象，在 Windows 系统中还导出了 windll 和 oledll 对象用于载入动态连接库。

通过操作这些对象的属性，你可以载入外部的动态链接库。cdll 载入按标准的 cdecl 调用协议导出的函数，而 windll 导入的库按 stdcall 调用协议调用其中的函数。 
oledll 也按 stdcall 调用协议调用其中的函数，并假定该函数返回的是 Windows HRESULT 错误代码，并当函数调用失败时，自动根据该代码甩出一个 OSError 异常。

**windows 例子**
```python
from ctypes import *
print(windll.kernel32)  

print(cdll.msvcrt)      

libc = cdll.msvcrt
```

**linux 例子**
```python
cdll.LoadLibrary("libc.so.6")  

libc = CDLL("libc.so.6")       
libc                           
```

## 基础数据类型

| ctypes 类型    | C 类型                                  | Python 类型   |
|--------------|---------------------------------------|-------------|
| c_bool       | _Bool                                 | bool (1)    |
| c_char       | char                                  | 单字符字节串对象    |
| c_wchar      | wchar_t                               | 单字符字符串      |
| c_byte       | char                                  | int         |
| c_ubyte      | unsigned char                         | int         |
| c_short      | short                                 | int         |
| c_ushort     | unsigned short                        | int         |
| c_int        | int                                   | int         |
| c_uint       | unsigned int                          | int         |
| c_long       | long                                  | int         |
| c_ulong      | unsigned long                         | int         |
| c_longlong   | __int64 或 long long                   | int         |
| c_ulonglong  | unsigned __int64 或 unsigned long long | int         |
| c_size_t     | size_t                                | int         |
| c_ssize_t    | ssize_t 或 Py_ssize_t                  | int         |
| c_float      | float                                 | float       |
| c_double     | double                                | float       |
| c_longdouble | long double                           | float       |
| c_char_p     | char* (以 NUL 结尾)                      | 字节串对象或 None |
| c_wchar_p    | wchar_t* (以 NUL 结尾)                   | 字符串或 None   |
| c_void_p     | void*                                 | int 或 None  |

```python
i = c_int(42)
print(i)

print(i.value)

i.value = -99
print(i.value)

s = "Hello, World"
c_s = c_wchar_p(s)
print(c_s)

print(c_s.value)

c_s.value = "Hi, there"
print(c_s)              # the memory location has changed

print(c_s.value)

print(s)                # first object is unchanged
```

## 调用函数

```python
printf = libc.printf

printf(b"Hello, %s\n", b"World!")
printf(b"Hello, %S\n", "World!")
printf(b"%d bottles of beer\n", 42)
printf(b"%f bottles of beer\n", 42.5)
```

## 调用可变函数

```python
printf.argtypes = [c_char_p, c_char_p, c_int, c_double]
printf(b"String '%s', Int %d, Double %f\n", b"Hi", 10, 2.2)
```

## 返回类型

```python
strchr = libc.strchr
strchr(b"abcdef", ord("d"))  

strchr.restype = c_char_p    # c_char_p is a pointer to a string
strchr(b"abcdef", ord("d"))

print(strchr(b"abcdef", ord("x")))
```

## 传递指针（或以引用方式传递形参）

ctypes 暴露了 byref() 函数用于通过引用传递参数，使用 pointer() 函数也能达到同样的效果，只不过 pointer() 需要更多步骤，因为它要先构造一个真实指针对象。所以在 Python 代码本身不需要使用这个指针对象的情况下，使用 byref() 效率更高。

```python
i = c_int()
f = c_float()
s = create_string_buffer(b'\000' * 32)
print(i.value, f.value, repr(s.value))

libc.sscanf(b"1 3.14 Hello", b"%d %f %s",
            byref(i), byref(f), s)

print(i.value, f.value, repr(s.value))
```

## 结构体和联合

结构体和联合必须继承自 ctypes 模块中的 Structure 和 Union 。子类必须定义 _fields_ 属性。 _fields_ 是一个二元组列表，二元组中包含 field name 和 field type 。

```python
from ctypes import *
class POINT(Structure):
    _fields_ = [("x", c_int),
                ("y", c_int)]

point = POINT(10, 20)
print(point.x, point.y)

point = POINT(y=5)
print(point.x, point.y)

POINT(1, 2, 3)
```

