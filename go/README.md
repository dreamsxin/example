# Go 入门

## 安裝

```shell
sudo apt-get install golang-go gccgo
```

## 配置环境变量

在当前用户文件 `~/.bashrc` 末尾加入（当bash shell被打开时，该文件被读取执行）：
```shell
export GOPATH=/usr/local/go
export PATH=$PATH:$GOPATH/bin
# or
export GOROOT=/usr/local/go
export GOPATH=~/gowork
export PATH=$GOROOT/bin:$GOPATH:$PATH
```
或者在文件 `~/.bash_profile` （当用户登录时，该文件被执行一次）

`/etc/profile`
用来设置系统环境参数，比如$PATH. 这里面的环境变量是对系统内所有用户生效的；

`/etc/bashrc`
这个文件设置系统 bash shell 相关的东西，对系统内所有用户生效。只要用户运行 bash 命令，那么这里面的东西就在起作用

`~/.bash_profile`
用来设置一些环境变量，功能和 /etc/profile 类似，但是这个是针对当前用户来设定的；

`~/.bashrc`
作用类似于 /etc/bashrc，只是针对用户自己而言，不对其他用户生效。

## 显示环境变量

```shell
 echo "GOPATH:$GOPATH"
 go env
 go version
 ```

## 包与导入

每个 Go 程序都是由包组成的。
程序运行的入口是包 `main`。 

我们可以编写多个导入语句，例如：

```go
import "fmt"
import "math"
```

还可以使用打包的导入方式：

```go
import (
	"fmt"
	"math"
)
```

## 安装第三方库

```shell
go get code.google.com/p/go.net/websocket
go get -u github.com/golang/net/websocket
```

## 编写 Hello world

```go
package main

import (
	"fmt"
	"time"
	"runtime"
)

func main() {
	fmt.Println("Hellow World!", runtime.Version(), ", The time is", time.Now())
}
```

## 解释执行

```shell
go run hello.go
```

## 编译执行

```shell
go build hello.go
./hello
# or
gccgo ./hello.go
./a.out
```

## 函数

函数也是值。

```go
package main

import (
	"fmt"
	"math"
)

func main() {
	hypot := func(x, y float64) float64 {
		return math.Sqrt(x*x + y*y)
	}

	fmt.Println(hypot(3, 4))
}
```

### 参数

函数可以没有参数或接受多个参数，参数类型在变量名之后。
当两个或多个连续的参数是同一类型时，则除了最后一个类型之外，其他都可以省略。

```go
package main

import "fmt"

func add(x, y int) int {
	return x + y
}

func main() {
	fmt.Println(add(42, 13))
}
```

在这个例子中，`add` 接受两个 int 类型的参数。

### 返回值

函数可以返回任意数量的返回值。返回值可以被命名，并且像变量那样使用。 

```go
package main

import "fmt"

func swap(x, y string) (string, string) {
	return y, x
}

func split(sum int) (x, y int) {
	x = sum * 4 / 9
	y = sum - x
	return
}

func main() {
	a, b := swap("hello", "world")
	fmt.Println(a, b)

	fmt.Println(split(17))
}
```

### 函数的闭包

Go 函数可以是闭包的。闭包是一个函数值，它来自函数体的外部的变量引用。函数可以对这个引用值进行访问和赋值；换句话说这个函数被“绑定”在这个变量上。
例如，函数 adder 返回一个闭包。每个闭包都被绑定到其各自的 sum 变量上。

```go
package main

import "fmt"

func adder() func(int) int {
	sum := 0
	return func(x int) int {
		sum += x
		return sum
	}
}

func main() {
	pos, neg := adder(), adder()
	for i := 0; i < 10; i++ {
		fmt.Println(
			pos(i),
			neg(-2*i),
		)
	}
}
```

## 变量

`var` 语句定义了一个变量的列表；跟函数的参数列表一样，类型在后面。
变量定义可以包含初始值，每个变量对应一个。如果初始化是使用表达式，则可以省略类型，从初始值中获得类型。
在定义一个变量不指定其类型时（使用没有类型的 var 或 := 语句），变量的类型由右值推导得出。

变量在定义时没有明确的初始化时会赋值为空值。

空值是：

- 数值类型为 `0`，
- 布尔类型为 `false`，
- 字符串为 `""`（空字符串）。

```go
package main

import "fmt"

var i, j int = 1, 2

func main() {
	var c, python, java = true, false, "no!"
	fmt.Println(i, j, c, python, java)
}
```

就像在这个例子中看到的一样，`var` 语句可以定义在包或函数级别。

### 短声明变量

函数外的每个语句都必须以关键字开始（`var`、`func`、等等），`:=` 结构不能使用在函数外。
在函数中，`:=` 简洁赋值语句在明确类型的地方，可以用于替代 var 定义。

```go
package main

import "fmt"

func main() {
	var i, j int = 1, 2
	k := 3
	c, python, java := true, false, "no!"

	fmt.Println(i, j, k, c, python, java)
}
```

### 基本类型

Go 的基本类型有Basic types

- bool
- string
- int、int8、int16、int32、int64
- uint、uint8、uint16、uint32、uint64、uintptr
- byte						// uint8 的别名
- rune						// int32 的别名，代表一个Unicode码
- float32、float64
- complex64、complex128		// 复数类型

```go
package main

import (
	"fmt"
	"math/cmplx"
)

var (
	ToBe   bool	   = false
	MaxInt uint64	 = 1<<64 - 1
	z	  complex128 = cmplx.Sqrt(-5 + 12i)
)

func main() {
	// 这里定义了常量
	const f = "%T(%v)\n"
	fmt.Printf(f, ToBe, ToBe)
	fmt.Printf(f, MaxInt, MaxInt)
	fmt.Printf(f, z, z)
}
```

上面的例子中，变量的定义被“打包”在一个语法块中。

### 类型转换

表达式 T(v) 将值 v 转换为类型 `T`。

一些关于数值的转换：

```go
var i int = 42
var f float64 = float64(i)
var u uint = uint(f)
```

或者，更加简单的形式：

```go
i := 42
f := float64(i)
u := uint(f)
```

与 C 不同的是 Go 的在不同类型之间的项目赋值时需要显式转换。 

### 字符串中的每一个元素叫做“字符”

Go 语言的字符有以下两种：

- byte
代表了 ASCII 码的一个字符，实际上是 uint8 。

- rune
代表一个 `UTF-8` 字符。当需要处理中文、日文或者其他复合字符时，则需要用到 rune 类型。rune 类型实际是 int32。

使用 `fmt.Printf` 中的 `%T` 格式符可以输出变量的实际类型：

```go
var a byte = 'a'
fmt.Printf("%d %T\n", a, a)
var b rune = '你'
fmt.Printf("%d %T\n", b, b)
```

### `[]byte` 转换成 `string`

```go
package main
 
import (
    "fmt"
    _ "unsafe"
)
 
func main() {
    bytes := []byte("I am byte array !")
    str := string(bytes)
    bytes[0] = 'i' //注意这一行，bytes在这里修改了数据，但是str打印出来的依然没变化，
    fmt.Println(str) // I am byte array !
}
```

接着看下面的代码
```go	
package main
 
import (
    "fmt"
    "unsafe"
)
 
func main() {
    bytes := []byte("I am byte array !")
    str := (*string)(unsafe.Pointer(&bytes))
    bytes[0] = 'i'
    fmt.Println(*str) // i am byte array !
}
```

现在打印出来的信息已经是改变过的了，现在可以看出来str和bytes共用一片内存。
这样做的意义在于，在网络通信中，大多数的接受方式都是[]byte，如果[]byte的数据比较大，内存拷贝的话会影响系统的性能。

## 常量

常量的定义与变量类似，只不过使用 const 关键字。
常量可以是字符、字符串、布尔或数字类型的值。
常量不能使用 := 语法定义。

数值常量是高精度的值。

一个未指定类型的常量由上下文来决定其类型。 

```go
package main

import "fmt"

const (
	Big   = 1 << 100
	Small = Big >> 99
)

func needInt(x int) int { return x*10 + 1 }
func needFloat(x float64) float64 {
	return x * 0.1
}

func main() {
	fmt.Println(needInt(Small))
	fmt.Println(needFloat(Small))
	fmt.Println(needFloat(Big))
}
```

## `make` 和 `new`

make 跟 new都可以用于内存分配。

- new 用于各种类型的内存分配，返回了一个指针，指向新分配的类型 T 的零值。
new 可以用来初始化泛型，并且返回储存位址。所以通常我们会用指标变数来接 new 过后的型别。特别要注意的是，new 会自动用 zeroed value 来初始化型别，也就是字串会是""，number 会是 0，channel, func, map, slice 等等则会是 nil。

- make 只能用于给slice、map和channel类型的内存分配，并且返回一个有初始值(非零)的 T 类型，而不是指针 T。


使用new来创建map时，返回的内容是一个指针，需要初始化后才能使用，而使用make来创建map时，返回的内容是一个引用：

```go
//使用new创建一个map指针
ma := new(map[string]int)                                                                                                                                          
//第一种初始化方法
*ma = map[string]int{}
(*ma)["a"] = 44
fmt.Println(*ma)

//第二种初始化方法
*ma = make(map[string]int, 0)
(*ma)["b"] = 55
fmt.Println(*ma)

//第三种初始化方法
mb := make(map[string]int, 0)
mb["c"] = 66
*ma = mb
(*ma)["d"] = 77
fmt.Println(*ma)
```


## `for` 循环

Go 只有一种循环结构——`for` 循环。

基本的 for 循环除了没有了 `( )` 之外（甚至强制不能使用它们），看起来跟 C 或者 Java 中做的一样，而 `{ }` 是必须的。
跟 C 或者 Java 中一样，可以让前置、后置语句为空。基于此可以省略分号，类似 C 的 while。 
如果省略了循环条件，循环就不会结束，因此可以用更简洁地形式表达死循环。

```go
package main

import "fmt"

func main() {
	sum := 0
	for i := 0; i < 10; i++ {
		sum += i
	}
	fmt.Println(sum)

	sum = 1
	for ; sum < 1000; {
		sum += sum
	}
	fmt.Println(sum)

	sum = 1
	for sum {
		sum += sum
	}
	fmt.Println(sum)
}
```

## `if`

if 语句跟 `for` 语句一样，除了没有了 `( )` 之外（甚至强制不能使用它们），看起来跟 C 或者 Java 中的一样，而 `{ }` 是必须的。
`if` 语句可以在条件之前执行一个简单的语句。
由这个语句定义的变量的作用域仅在 if 范围之内，包括 else 块。

```go
package main

import (
	"fmt"
	"math"
)

func pow(x, n, lim float64) float64 {
	if v := math.Pow(x, n); v < lim {
		return v
	} else {
		fmt.Printf("%g >= %g\n", v, lim)
	}
	return lim
}

func main() {
	fmt.Println(
		pow(3, 2, 10),
		pow(3, 3, 20),
	)
}
```

## `switch`

Go的switch非常灵活，表达式不必是常量或整数，执行的过程从上至下，直到找到匹配项；而如果 switch 没有表达式，它会匹配 true，同 `switch true` 一样。
Go里面switch默认相当于每个case最后带有break，匹配成功后不会自动向下执行其他case，而是跳出整个switch, 但是可以使用fallthrough强制执行后面的case代码。

```go
package main

import (
	"fmt"
	"runtime"
	"time"
)

func main() {
	fmt.Print("Go runs on ")
	switch os := runtime.GOOS; os {
	case "darwin":
		fmt.Println("OS X.")
	case "linux":
		fmt.Println("Linux.")
	default:
		// freebsd, openbsd,
		// plan9, windows...
		fmt.Printf("%s.", os)
	}

	t := time.Now()
	switch {
	case t.Hour() < 12:
		fmt.Println("Good morning!")
	case t.Hour() < 17:
		fmt.Println("Good afternoon.")
	default:
		fmt.Println("Good evening.")
	}
}
```

## `defer`

`defer` 语句会延迟函数的执行直到上层函数返回。
延迟调用的参数会立刻生成，但是在上层函数返回前函数都不会被调用。 

```go
package main

import "fmt"

func main() {
	defer fmt.Println("world")

	fmt.Println("hello")

	i := 1
	// 此时确定了 i 值
	defer fmt.Println("Deferred print:", i)
	i++
	fmt.Println("Normal print:", i)
}
```

延迟的函数调用被压入一个栈中。当函数返回时，会按照后进先出的顺序调用被延迟的函数调用，下面最后输出的数字将会是一个倒序。

```go
package main

import "fmt"

func main() {
	fmt.Println("counting")

	for i := 0; i < 10; i++ {
		defer fmt.Println(i)
	}

	fmt.Println("done")
}
```

## 指针

指针保存了变量的内存地址。类型 *T 是指向类型 T 的值的指针。其零值是 `nil`。
Go 与 C 不同之处是没有指针运算。

- `&` 符号会生成一个指向其作用对象的指针。
- `*` 符号表示指针指向的底层的值。 

```go
package main

import "fmt"

func main() {
	i, j := 42, 2701

	p := &i		 // point to i
	fmt.Println(*p) // read i through the pointer
	*p = 21		 // set i through the pointer
	fmt.Println(i)  // see the new value of i

	p = &j		 // point to j
	*p = *p / 37   // divide j through the pointer
	fmt.Println(j) // see the new value of j
}
```

## `struct`

一个结构体（`struct`）就是一个字段的集合。
结构体内的字段使用点号来访问。
通过指针间接的访问结构体字段是透明的。 

```go
package main

import "fmt"

type Vertex struct {
	X int
	Y int
}

func main() {
	fmt.Println(Vertex{1, 2})

	v.X = 4
	fmt.Println(v.X, v.Y)

	p := &v
	p.X = 1e9
	fmt.Println(v)
}
```

### `struct` 字面值语法

通过结构体字段的值作为列表来新分配一个结构体。
使用 Name: 语法可以仅列出部分字段。（字段名的顺序无关。） 

```go
package main

import "fmt"

type Vertex struct {
	X, Y int
}

// 分配方式
var (
	v1 = Vertex{1, 2}  // 类型为 Vertex
	v2 = Vertex{X: 1}  // Y:0 被省略
	v3 = Vertex{}	  // X:0 和 Y:0
	p  = &Vertex{1, 2} // 类型为 *Vertex
)

func main() {
	fmt.Println(v1, p, v2, v3)

	// 定义新的结构数组
	var vs = []struct {
		x string
		y string
		z int
	}{
		{"0", "0", 0},
		{"1", "1", 1},
		{"2", "2", 2},
	}

	fmt.Println(vs)

	for index, value := range vs {
		fmt.Println(index, value)
	}

	// 下划线表示那个值舍去，即舍去下标索引
	for _, value := range vs {
		fmt.Println(value)
	}
} 
```

## `array` 数组

类型 [n]T 是一个有 n 个类型为 T 的值的数组。
数组的长度是其类型的一部分，因此数组不能改变大小。
数组在使用的过程中都是值传递，将一个数组赋值给一个新变量或作为方法参数传递时，是将源数组在内存中完全复制了一份，而不是引用源数组在内存中的地址。

```go
package main

import "fmt"

func main() {
	// 定义变量 a 是一个有2个字符串的数组
	var a [2]string
	a[0] = "Hello"
	a[1] = "World"
	fmt.Println(a[0], a[1])
	fmt.Println(a)
}
```

## `slice` 切片

Slice 是对底层数组 Array 的封装，每个 Slice 都是都源数组在内存中的地址的一个引用，源数组可以衍生出多个 Slice，Slice 也可以继续衍生Slice。
Slice 可以通过两种方式定义，一种是从源数组中衍生，一种是通过 `make` 函数定义。
本质上来说都一样，都是在内存中通过数组的初始化的方式开辟一块内存，将其划分为若干个小块用来存储数组元素，然后Slice就去引用整个或者局部数组元素。
Slice 的空值是 `nil`，长度和容量都是 0。 

```go
package main

import "fmt"

func main() {
	// 空值
	var z []int
	fmt.Println(z, len(z), cap(z))
	if z == nil {
		fmt.Println("nil!")
	}

	// 从源数组中衍生
	p := []int{2, 3, 5, 7, 11, 13}
	fmt.Println("p ==", p)

	fmt.Println("p[1:4] ==", p[1:4])

	// 省略下标代表从 0 开始
	fmt.Println("p[:3] ==", p[:3])

	// 省略上标代表到 len(s) 结束
	fmt.Println("p[4:] ==", p[4:])

	// 从源数组中切片
	a := [10]int{1, 2, 3, 4, 5, 6, 7, 8, 9, 0}
	s := a[2:8]
	fmt.Println(s)

	for i := 0; i < len(p); i++ {
		fmt.Printf("p[%d] == %d\n", i, p[i])
	}

	// 构造slice，make函数第一个参数表示构建的数组的类型，第二个参数为数组的长度，第三个参数可选，是slice的容量，默认为第二个参数值。
	s2 := make([]int, 10, 20)
	fmt.Println(s2) //输出：[0 0 0 0 0 0 0 0 0 0]
	fmt.Printf("len=%d cap=%d\n", len(s2), cap(s2))
}
```

### 向 slice 添加元素

使用内建函数 `append`

```go
package main

import "fmt"

func main() {
	var a []int
	printSlice("a", a)

	// append works on nil slices.
	a = append(a, 0)
	printSlice("a", a)

	// the slice grows as needed.
	a = append(a, 1)
	printSlice("a", a)

	// we can add more than one element at a time.
	a = append(a, 2, 3, 4)
	printSlice("a", a)
}

func printSlice(s string, x []int) {
	fmt.Printf("%s len=%d cap=%d %v\n", s, len(x), cap(x), x)
}
```

### 分片与数组的区别

数组是固定长度的，而分片确实可动态增长的，以定义为例：

```go
// 定义数组, 一定要指定长度
var names [5]string

// 定义分片, 不需要指定长度
var names []string
```

在函数调用时, 数组是值传递，而分片是引用传递。

其实对于 golang 来讲，函数调用的时候都是值传递，拷贝一个副本，之所以表现为值传递和引用传递，在于一个拷贝的是数据值，另一个拷贝的是数据指针，两个指针值指向的是同一个内存地址。

### 分片的实现

分片的底层数据还是使用的数组，它一共包含 3 个字段：

- 地址指针
- 长度
- 容量

```go
// source 是一个分片， 大小为 4， 容量为： 5
source := make([]string, 4, 5)
fmt.Println(source, len(source), cap(source))
// 输出： [   ] 4 5

// 注意这里不会进行内存分配，因为 source 还有剩余空间可以新加数据
source = append(source, "1")
fmt.Println(source, len(source), cap(source))
// 输出： [    1] 5 5
```

在使用 `append` 来为分片添加数据时，没有内存分配时会修改源数组：

```go
source := []string{"1", "2", "3", "4", "5"}

// 拷贝 source 的第二到第三个元素（不包括第三个）这时的容量将使用源数组的。
copied := source[2:3]

fmt.Println("source-->", source)
fmt.Println("copied-->", copied)

copied = append(copied, "mike")
fmt.Println("source-->", source)
fmt.Println("copied-->", copied)
```
输出：
```text
source--> [1 2 3 4 5]
copied--> [3]
source--> [1 2 3 mike 5]
copied--> [3 mike]
```

当 append 有内存分配时：
```go
source := []string{"1", "2", "3", "4", "5"}

// 拷贝 source 的第二到第三个元素（不包括第三个），容量 3
copied := source[2:3:3]
// 此时 copied 会和 source 共享底层数组

fmt.Println("source-->", source)
fmt.Println("copied-->", copied)

copied = append(copied, "mike")
fmt.Println("source-->", source)
fmt.Println("copied-->", copied)
```
输出：
```text
source--> [1 2 3 4 5]
copied--> [3]
source--> [1 2 3 4 5]
copied--> [3 mike]
```

## `range`

`for` 循环的 range 格式可以对 slice 或者 map 进行迭代循环。
可以通过赋值给 `_` 来忽略序号和值。
如果只需要索引值，去掉“, value”的部分即可。

```go
package main

import "fmt"

var pow = []int{1, 2, 4, 8, 16, 32, 64, 128}

func main() {
	for i, v := range pow {
		fmt.Printf("2**%d = %d\n", i, v)
	}

	for _, value := range pow {
		fmt.Printf("%d\n", value)
	}

	pow2 := make([]int, 10)
	for i := range pow2 {
		pow2[i] = 1 << uint(i)
	}

	for _, value := range pow2 {
		fmt.Printf("%d\n", value)
	}
}
```

## `map`

map 在使用之前必须用 make 而不是 new 来创建；值为 nil 的 map 是空的，并且不能赋值。 

- map 是无序的，必须通过 key 获取
- map 的长度是不固定的，也就是和 slice 一样，也是一种引用类型
- 内置的 len 函数同样适用于 map，返回 map 拥有的 key 的数量
- map 的初始化可以通过 key:val 的方式初始化值，比如：map_variable := map[string]string{"a": "1", "b": "2", "c": "3"}
- 通过 delete 删除 map 的元素，函数原型delete(map_variable, key)

```go
package main

import "fmt"

type Vertex struct {
	Lat, Long float64
}

var m map[string]Vertex

func main() {
	m = make(map[string]Vertex)
	if m != nil {
		fmt.Println("not nil!")
	}

	// 赋值
	m["Bell Labs"] = Vertex{
		40.68433, -74.39967,
	}
	fmt.Println(m["Bell Labs"])

	m2 := make(map[string]int)

	// 插入元素
	m2["Answer"] = 42
	fmt.Println("The value:", m2["Answer"])

	// 修改元素
	m2["Answer"] = 48
	fmt.Println("The value:", m2["Answer"])

	// 删除元素
	delete(m2, "Answer")
	fmt.Println("The value:", m2["Answer"])

	// 检测元素
	v, ok := m2["Answer"]
	fmt.Println("The value:", v, "Present?", ok)
}
```

## `method` 方法

Go 没有类。然而，仍然可以在结构体类型上定义方法。方法接收者 出现在 func 关键字和方法名之间的参数中。 
方法就是有接收者的函数。

在 Go 中，类型可以定义接收此类型的函数，即方法。每个类型都有接口，意味着对那个类型定义了方法集合。
下面定义了结构体类型 S 以及它的两个方法：

```go
type S struct { i int }
func (p *S) Get() int { return p.i }
func (p *S) Put(v int) { p.i = v }
```

可以在非本地类型（本地类型包括内建类型，比如 int）的任意类型上定义方法。
但是我们可以为内建类型定义别名，然后就可以为别名定义方法。如

```go
type Foo int // 为 int 定义别名 Foo
func (self Foo) Emit() {
	fmt.Printf("%v", self)
}
```

可以对包中的 任意 类型定义任意方法，而不仅仅是针对结构体。但是，不能对来自其他包的类型或基础类型定义方法。 

```go
package main

import (
	"fmt"
	"math"
)

type Vertex struct {
	X, Y float64
}

func (v *Vertex) Scale(f float64) {
	v.X = v.X * f
	v.Y = v.Y * f
}

func (v *Vertex) Abs() float64 {
	return math.Sqrt(v.X*v.X + v.Y*v.Y)
}

func main() {
	v := &Vertex{3, 4}
	v.Scale(5)
	fmt.Println(v, v.Abs())
}
```

## 接口

接口定义为一个方法的集合。方法包含实际的代码。换句话说，一个接口就是定义，而方法就是实现。
因此，接收者不能定义为接口类型，这样做的话会引起 invalid receiver type … 的编译器错误。

```go
package main

import (
	"fmt"
	"math"
)

type Abser interface {
	Abs() float64
}

func main() {
	var a Abser
	f := MyFloat(-math.Sqrt2)
	v := Vertex{3, 4}

	a = f  // a MyFloat 实现了 Abser
	a = &v // a *Vertex 实现了 Abser

	// 下面一行，v 是一个 Vertex（而不是 *Vertex）所以没有实现 Abser。
	a = v

	fmt.Println(a.Abs())
}

type MyFloat float64

func (f MyFloat) Abs() float64 {
	if f < 0 {
		return float64(-f)
	}
	return float64(f)
}

type Vertex struct {
	X, Y float64
}

func (v *Vertex) Abs() float64 {
	return math.Sqrt(v.X*v.X + v.Y*v.Y)
}
```

### Stringer

一个普遍存在的接口是 fmt 包中定义的 Stringer。

```go
type Stringer interface {
	String() string
}
```

Stringer 是一个可以用字符串描述自己的类型。`fmt`包 （还有许多其他包）使用这个来进行输出。

```go
package main

import "fmt"

type Person struct {
	Name string
	Age  int
}

func (p Person) String() string {
	return fmt.Sprintf("%v (%v years)", p.Name, p.Age)
}

func main() {
	a := Person{"Arthur Dent", 42}
	z := Person{"Zaphod Beeblebrox", 9001}
	fmt.Println(a, z)
}
```

## 错误

Go 程序使用 error 值来表示错误状态。

与 fmt.Stringer 类似，`error` 类型是一个内建接口：

```go
type error interface {
	Error() string
}
```

通常函数会返回一个 error 值，调用的它的代码应当判断这个错误是否等于 `nil`， 来进行错误处理。 

```go
i, err := strconv.Atoi("42")
if err != nil {
	fmt.Printf("couldn't convert number: %v\n", err)
}
fmt.Println("Converted integer:", i)
```

error 为 nil 时表示成功；非 nil 的 error 表示错误。 

```go
package main

import (
	"fmt"
	"time"
)

type MyError struct {
	When time.Time
	What string
}

func (e *MyError) Error() string {
	return fmt.Sprintf("at %v, %s",
		e.When, e.What)
}

func run() error {
	return &MyError{
		time.Now(),
		"it didn't work",
	}
}

func main() {
	if err := run(); err != nil {
		fmt.Println(err)
	}
}
```

## Reader

io 包指定了 io.Reader 接口，它表示从数据流结尾读取。

```go
package main

import (
	"fmt"
	"io"
	"strings"
)

func main() {
	// 创建了一个 strings.Reader
	r := strings.NewReader("Hello, Reader!")

	// 以每次 8 字节的速度读取输出
	b := make([]byte, 8)
	for {
		n, err := r.Read(b)
		fmt.Printf("n = %v err = %v b = %v\n", n, err, b)
		fmt.Printf("b[:n] = %q\n", b[:n])
		if err == io.EOF {
			break
		}
	}
}
```

## Web 服务器

包 http 通过任何实现了 http.Handler 的值来响应 HTTP 请求：

```go
package http

type Handler interface {
	ServeHTTP(w ResponseWriter, r *Request)
}
```

实例：
```go
package main

import (
	"fmt"
	"log"
	"net/http"
)

type Hello struct{}

func (h Hello) ServeHTTP(w http.ResponseWriter,	r *http.Request) {
	switch req.URL.Path {
	default:
		fmt.Fprint(w, "Hello!")
	}
}

func main() {
	var h Hello
	err := http.ListenAndServe("localhost:4000", h)

	http.Handle("/string", String("I'm a frayed knot."))
	http.Handle("/struct", &Struct{"Hello", ":", "Gophers!"})
	if err != nil {
		log.Fatal(err)
	}
}
```

另外一个写法：
```go
package main

import (
	"fmt"
	"log"
	"net/http"
)

type String string

func (s String) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	switch r.URL.Path {
	case "/":
		fmt.Fprint(w, s)
	case "/hello":
		fmt.Fprint(w, s)
	default:
		fmt.Fprint(w, "Hello")
	}
}

func main() {
	http.Handle("/", String("I'm a root."))
	http.Handle("/hello", String("I'm a hello."))

	err := http.ListenAndServe("localhost:4000", nil)

	if err != nil {
		log.Fatal(err)
	}
}
```

## goroutine

goroutine 是由 Go 运行时环境管理的轻量级线程。
goroutine 在相同的地址空间中运行，因此访问共享内存必须进行同步。sync 提供了这种可能，不过在 Go 中并不经常用到，因为有其他的办法，比如 channel。

```go
package main

import (
	"fmt"
	"time"
)

func say(s string) {
	for i := 0; i < 5; i++ {
		time.Sleep(100 * time.Millisecond)
		fmt.Println(s)
	}
}

func main() {
	go say("world")
	say("hello")
}
```

### channel

channel 是有类型的管道，可以用 channel 操作符 <- 对其发送或者接收值。
channel 使用前必须创建：`ch := make(chan int)`。
默认情况下，在另一端准备好之前，发送和接收都会阻塞。这使得 goroutine 可以在没有明确的锁或竞态变量的情况下进行同步。
Channel 可以作为一个先入先出(FIFO)的队列，接收的数据和发送的数据的顺序是一致的。

```go
package main

import "fmt"

func sum(a []int, c chan int) {
	sum := 0
	for _, v := range a {
		sum += v
	}
	c <- sum // 将和送入 c
}

func main() {
	a := []int{7, 2, 8, -9, 4, 0}

	c := make(chan int)
	go sum(a[:len(a)/2], c)
	go sum(a[len(a)/2:], c)
	x, y := <-c, <-c // 从 c 中获取

	fmt.Println(x, y, x+y)
}
```

channel 可以是 _带缓冲的_。为 make 提供第二个参数作为缓冲长度来初始化一个缓冲 channel：`ch := make(chan int, 100)`。

```go
package main

import "fmt"

func main() {
	c := make(chan int, 2)
	c <- 1
	c <- 2
	fmt.Println(<-c)
	fmt.Println(<-c)
}
```

#### range 和 close

 发送者可以 close 一个 channel 来表示再没有值会被发送了。
 接收者可以通过赋值语句的第二参数来测试 channel 是否被关闭：当没有值可以接收并且 channel 已经被关闭，那么经过`v, ok := <-ch` 之后 ok 会被设置为 `false`。

循环 `for i := range c` 会不断从 channel 接收值，直到它被关闭。

> 只有发送者才能关闭 channel，而不是接收者。向一个已经关闭的 channel 发送数据会引起 panic。
> 还要注意：channel 与文件不同；通常情况下无需关闭它们。只有在需要告诉接收者没有更多的数据的时候才有必要进行关闭，例如中断一个 `range`。 

```go
package main

import (
	"fmt"
)

func fibonacci(n int, c chan int) {
	x, y := 0, 1
	for i := 0; i < n; i++ {
		c <- x
		x, y = y, x+y
	}
	close(c)
}

func main() {
	c := make(chan int, 10)
	go fibonacci(cap(c), c)
	for i := range c {
		fmt.Println(i)
	}
}
```

#### select

select 语句使得一个 goroutine 在多个通讯操作上等待。
select 会阻塞，直到条件分支中的某个可以继续执行，这时就会执行那个条件分支。当多个都准备好的时候，会随机选择一个。

```go
package main

import (
	"fmt"
	"time"
)

func fibonacci(c, quit chan int) {
	x, y := 0, 1
	for {
		select {
		case c <- x:
			// x = y and y = x + y
			x, y = y, x+y
		case <-quit:
			fmt.Println("quit")
			return
		default:
			fmt.Println("	.")
			time.Sleep(50 * time.Millisecond)
		}
	}
}

func main() {
	// 使用两个通道
	c := make(chan int)
	quit := make(chan int)
	go func() {
		for i := 0; i < 10; i++ {
			fmt.Println(<-c)
		}
		quit <- 0
	}()
	fibonacci(c, quit)
}
```

## 启动脚本

```shell
#! /bin/bash
 
export GOPATH=/home/ficow/gopath
echo "GOPATH:$GOPATH"
 
export GOROOT=/home/ficow/go
echo "GOROOT:$GOROOT"
 
PATH=$PATH:$HOME/bin:usr/local/git/bin:$GOROOT/bin:$GOPATH/bin:
echo "PATH:$PATH"
 
(go run main.go >> /home/ficow/ficowblog/src/blogLog.txt 2>&1 &)
 
echo "*** 服务器重启完成！ ***"
```

## 问题

https://www.educative.io/blog/50-golang-interview-questions
