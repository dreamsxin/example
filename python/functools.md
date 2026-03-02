# functools

`functools` 是 Python 标准库中的一个模块，专门用于**高阶函数**（即操作或返回函数的函数）以及**可调用对象**的处理。它提供了一系列实用的工具，帮助我们以函数式编程风格编写更简洁、高效且易于维护的代码。

## 1. 模块概述

`functools` 的核心目标是对函数进行包装、修饰和增强，使得函数可以像对象一样被灵活操作。它包含以下几类功能：

- **函数参数固定**（`partial`、`partialmethod`）
- **函数结果缓存**（`lru_cache`、`cached_property`）
- **函数元信息保留**（`wraps`、`update_wrapper`）
- **比较运算符自动生成**（`total_ordering`）
- **单分派泛型函数**（`singledispatch`、`singledispatchmethod`）
- **归约操作**（`reduce`）

---

## 2. 常用函数详解与示例

### 2.1 `partial` —— 冻结函数的部分参数

`partial` 用于创建一个新的可调用对象，原函数的部分参数被预先设置为固定值（即“冻结”参数）。这在需要频繁调用同一函数但某些参数固定时非常有用。

```python
from functools import partial

def power(base, exp):
    return base ** exp

# 创建一个专门计算平方的函数
square = partial(power, exp=2)
print(square(5))   # 25

# 创建一个以2为底的指数函数
exp_of_2 = partial(power, base=2)
print(exp_of_2(3)) # 8
```

**注意**：`partial` 也可以用于类方法，但对于实例方法需使用 `partialmethod`。

### 2.2 `reduce` —— 累积归约

`reduce` 将一个接受两个参数的函数以累积的方式应用到可迭代对象的元素上，最终将序列归约为单个值。Python 3 中将 `reduce` 移到了 `functools` 中（Python 2 是内置函数）。

```python
from functools import reduce

# 计算列表所有元素的乘积
numbers = [1, 2, 3, 4]
product = reduce(lambda x, y: x * y, numbers)
print(product)   # 24

# 带初始值
product = reduce(lambda x, y: x * y, numbers, 10)  # 10*1*2*3*4 = 240
```

### 2.3 `wraps` —— 保留被装饰函数的元信息

编写装饰器时，直接返回的内嵌函数会丢失原函数的名称、文档字符串等元信息。`wraps` 是一个装饰器，用于将原函数的元信息复制到包装函数上。

```python
from functools import wraps

def my_decorator(func):
    @wraps(func)   # 保留 func 的元信息
    def wrapper(*args, **kwargs):
        print("Before call")
        result = func(*args, **kwargs)
        print("After call")
        return result
    return wrapper

@my_decorator
def greet(name):
    """Say hello to someone."""
    print(f"Hello, {name}")

print(greet.__name__)   # 输出 'greet' 而非 'wrapper'
print(greet.__doc__)    # 输出 'Say hello to someone.'
```

### 2.4 `lru_cache` —— 最近最少使用缓存

`lru_cache` 是一个装饰器，用于缓存函数的返回值，避免重复计算。它使用 LRU（Least Recently Used）策略淘汰不常用的缓存项，非常适合计算密集型或 I/O 密集型且参数可哈希的函数。

```python
from functools import lru_cache
import time

@lru_cache(maxsize=32)
def fibonacci(n):
    """递归计算斐波那契数列"""
    if n < 2:
        return n
    return fibonacci(n-1) + fibonacci(n-2)

start = time.time()
print(fibonacci(35))   # 第一次计算较慢
print(f"耗时: {time.time()-start:.4f}s")

start = time.time()
print(fibonacci(35))   # 第二次直接返回缓存结果，极快
print(f"耗时: {time.time()-start:.4f}s")

# 查看缓存统计信息
print(fibonacci.cache_info())   # CacheInfo(hits=..., misses=..., maxsize=32, currsize=...)
```

**参数说明**：
- `maxsize`：最大缓存条目数，设为 `None` 表示无限制（需注意内存）。
- `typed`：若设为 `True`，则不同类型的参数（如 `3` 和 `3.0`）将分别缓存。

### 2.5 `cached_property` —— 缓存属性值

Python 3.8 引入的 `cached_property` 是一个装饰器，它将一个方法转换为属性，并在第一次访问后缓存结果，后续直接返回缓存值，适合那些计算开销大但结果不变的属性。

```python
from functools import cached_property

class DataProcessor:
    def __init__(self, data):
        self.data = data

    @cached_property
    def expensive_computation(self):
        print("正在执行复杂计算...")
        return sum(self.data) ** 2  # 模拟耗时操作

dp = DataProcessor([1, 2, 3, 4])
print(dp.expensive_computation)   # 输出：正在执行复杂计算... 100
print(dp.expensive_computation)   # 直接输出 100，不再打印
```

### 2.6 `total_ordering` —— 自动生成比较方法

定义一个类时，若至少提供了 `__eq__` 和另一个比较方法（如 `__lt__`、`__le__`、`__gt__`、`__ge__`），`total_ordering` 装饰器会自动填充剩余的比较方法。

```python
from functools import total_ordering

@total_ordering
class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age

    def __eq__(self, other):
        return self.age == other.age

    def __lt__(self, other):
        return self.age < other.age

p1 = Person("Alice", 30)
p2 = Person("Bob", 25)
p3 = Person("Charlie", 30)

print(p1 > p2)   # True，自动生成 __gt__
print(p1 >= p3)  # True，自动生成 __ge__
```

### 2.7 `singledispatch` —— 单分派泛型函数

`singledispatch` 用于实现泛型函数，即根据第一个参数的类型调用不同的实现。非常适合替代繁琐的 `if/elif` 类型检查。

```python
from functools import singledispatch

@singledispatch
def process(obj):
    """默认处理方式"""
    raise NotImplementedError("不支持的类型")

@process.register(int)
def _(obj):
    print(f"处理整数: {obj}")

@process.register(str)
def _(obj):
    print(f"处理字符串: {obj}")

@process.register(list)
def _(obj):
    print(f"处理列表，长度: {len(obj)}")

# 还可以注册到 collections.abc 的类型
from collections.abc import Sequence
@process.register(Sequence)
def _(obj):
    print(f"处理序列，第一个元素: {obj[0]}")

process(10)        # 处理整数: 10
process("hello")   # 处理字符串: hello
process([1,2,3])   # 处理列表，长度: 3（因为 list 是具体的类型，优先匹配具体类型）
```

类似地，`singledispatchmethod` 用于类中的方法。

### 2.8 `update_wrapper` —— 手动更新包装器信息

`wraps` 实际上是 `update_wrapper` 的装饰器版本。`update_wrapper` 可以直接用于函数，将原函数的元信息复制到包装器函数。

```python
from functools import update_wrapper

def my_decorator(func):
    def wrapper(*args, **kwargs):
        return func(*args, **kwargs)
    update_wrapper(wrapper, func)   # 手动复制元信息
    return wrapper
```

---

## 3. 其他实用工具

- **`partialmethod`**：用于在类中定义部分应用的方法。
- **`cmp_to_key`**：将旧式的比较函数转换为键函数（用于 `sorted`、`min` 等）。
- **`singledispatchmethod`**：泛型方法，与 `singledispatch` 类似，但用于类的方法。

---

## 4. 总结

`functools` 是 Python 函数式编程的利器，它通过简洁的装饰器和函数，极大地提升了代码的复用性、性能和可读性。在实际开发中，合理运用这些工具可以：

- 减少重复代码（`partial`、`lru_cache`）
- 优化性能（`lru_cache`、`cached_property`）
- 保持函数元数据正确（`wraps`）
- 实现灵活的类型分派（`singledispatch`）
- 自动生成冗余的比较方法（`total_ordering`）

掌握 `functools` 的用法，能让你的 Python 代码更加 Pythonic。
