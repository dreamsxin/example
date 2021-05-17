# 原子操作

`sync/atomic`

## 类型

提供的原子操作共有5种：增或减、比较并交换、载入、存储和交换。

- int32
  `newi32 := atomic.AddInt32(&i32, 3)`
  `newi32 := atomic.AddInt32(&i32, -3)`


- int64
- uint32
```go
var b uint64 = 10
c := int64(-3)
d := uint64(c)
atomic.AddUint64(&b,d)
fmt.Println(b)

# 一个负整数的补码可以通过对它按位（除了符号位之外）求反码并加一得到。我们还知道，一个负整数可以由对它的绝对值减一并求补码后得到的数值的二进制表示来代表。
var b uint64 = 10
var e int64 = -3
atomic.AddUint64(&b,^uint64(-e-1))
fmt.Println(a,b)

atomic.AddUint32(&b, -3 & math.MaxUint32)
```
- uint64
- uintptr
- unsafe.Pointer
