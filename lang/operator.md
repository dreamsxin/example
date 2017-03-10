# 运算符分类

－ 算术运算符：+ - * / % ++ —
－ 关系运算符：== != > < >= <=
－ 逻辑运算符: && || !
－ 位运算符:  与& 或| 异或^ 补码运算符~ 左移<< 右移>>
－ 赋值运算符: = += -= *= /= %= <<= >>= &= ^= |=
－ 杂项运算符: sizeof cast ?: , . -> & *

## 位运算基础操作

设置某位为1
```c
number |= 1 << x; // 设置第x位为1
```

清除某位
```c
number &= ~(1 << x); // 置第x位为0
```
开关某位
```c
number ^= 1 << x;
```
检查某位
```c
if (number & (1 << x))
```

### 使用宏来封装
```c
#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))
```

### 使用位结构操作

```c
struct bits {
    unsigned int a:1;
    unsigned int b:1;
    unsigned int c:1;
};

struct bits mybits;

// set/clear a bit
mybits.b = 1;
mybits.c = 0;

// toggle a bit
mybits.a = !mybits.a;
mybits.b = ~mybits.b;
mybits.c ^= 1;

// check a bit
if (mybits.c)
```

C++ 可以使用STL的`std::bitset`

这个方法其实类似于使用位结构，只不过STL包装了这个结构定义，当然还提供了很多便捷的接口：
```c++
std::bitset<5> bits;
bits[0] = true;
bits[1] = false;
bits.set(2);
bits.flip(3);
bits.reset(2);
```
