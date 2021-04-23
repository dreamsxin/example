每种语言的反射模型都不同，并且有些语言根本不支持反射。Golang语言实现了反射，反射机制就是在运行时动态的调用对象的方法和属性，官方自带的reflect包就是反射相关的，只要包含这个包就可以使用。

多插一句，Golang的gRPC也是通过反射实现的。

interface 和 反射
在讲反射之前，先来看看Golang关于类型设计的一些原则

变量包括（type, value）两部分

理解这一点就知道为什么nil != nil了
type 包括 static type和concrete type. 简单来说 static type是你在编码是看见的类型(如int、string)，concrete type是runtime系统看见的类型

类型断言能否成功，取决于变量的concrete type，而不是static type. 因此，一个 reader变量如果它的concrete type也实现了write方法的话，它也可以被类型断言为writer.

接下来要讲的反射，就是建立在类型之上的，Golang的指定类型的变量的类型是静态的（也就是指定int、string这些的变量，它的type是static type），在创建变量的时候就已经确定，反射主要与Golang的interface类型相关（它的type是concrete type），只有interface类型才有反射一说。

在Golang的实现中，每个interface变量都有一个对应pair，pair中记录了实际变量的值和类型:

(value, type)
value是实际变量值，type是实际变量的类型。一个interface{}类型的变量包含了2个指针，一个指针指向值的类型【对应concrete type】，另外一个指针指向实际的值【对应value】。

例如，创建类型为*os.File的变量，然后将其赋给一个接口变量r：

tty, err := os.OpenFile("/dev/tty", os.O_RDWR, 0)

var r io.Reader
r = tty
接口变量r的pair中将记录如下信息：(tty, *os.File)，这个pair在接口变量的连续赋值过程中是不变的，将接口变量r赋给另一个接口变量w:

var w io.Writer
w = r.(io.Writer)
接口变量w的pair与r的pair相同，都是:(tty, *os.File)，即使w是空接口类型，pair也是不变的。

interface及其pair的存在，是Golang中实现反射的前提，理解了pair，就更容易理解反射。反射就是用来检测存储在接口变量内部(值value；类型concrete type) pair对的一种机制。

Golang的反射reflect
reflect的基本功能TypeOf和ValueOf
既然反射就是用来检测存储在接口变量内部(值value；类型concrete type) pair对的一种机制。那么在Golang的reflect反射包中有什么样的方式可以让我们直接获取到变量内部的信息呢？ 它提供了两种类型（或者说两个方法）让我们可以很容易的访问接口变量内容，分别是reflect.ValueOf() 和 reflect.TypeOf()，看看官方的解释

// ValueOf returns a new Value initialized to the concrete value
// stored in the interface i.  ValueOf(nil) returns the zero 
func ValueOf(i interface{}) Value {...}

翻译一下：ValueOf用来获取输入参数接口中的数据的值，如果接口为空则返回0


// TypeOf returns the reflection Type that represents the dynamic type of i.
// If i is a nil interface value, TypeOf returns nil.
func TypeOf(i interface{}) Type {...}

翻译一下：TypeOf用来动态获取输入参数接口中的值的类型，如果接口为空则返回nil

reflect.TypeOf()是获取pair中的type，reflect.ValueOf()获取pair中的value，示例如下：

package main

import (
	"fmt"
	"reflect"
)

func main() {
	var num float64 = 1.2345

	fmt.Println("type: ", reflect.TypeOf(num))
	fmt.Println("value: ", reflect.ValueOf(num))
}

运行结果:
type:  float64
value:  1.2345
说明

reflect.TypeOf： 直接给到了我们想要的type类型，如float64、int、各种pointer、struct 等等真实的类型

reflect.ValueOf：直接给到了我们想要的具体的值，如1.2345这个具体数值，或者类似&{1 "Allen.Wu" 25} 这样的结构体struct的值

也就是说明反射可以将“接口类型变量”转换为“反射类型对象”，反射类型指的是reflect.Type和reflect.Value这两种

从relfect.Value中获取接口interface的信息
当执行reflect.ValueOf(interface)之后，就得到了一个类型为”relfect.Value”变量，可以通过它本身的Interface()方法获得接口变量的真实内容，然后可以通过类型判断进行转换，转换为原有真实类型。不过，我们可能是已知原有类型，也有可能是未知原有类型，因此，下面分两种情况进行说明。

已知原有类型【进行“强制转换”】

已知类型后转换为其对应的类型的做法如下，直接通过Interface方法然后强制转换，如下：

realValue := value.Interface().(已知的类型)
示例如下：

package main

import (
	"fmt"
	"reflect"
)

func main() {
	var num float64 = 1.2345

	pointer := reflect.ValueOf(&num)
	value := reflect.ValueOf(num)

	// 可以理解为“强制转换”，但是需要注意的时候，转换的时候，如果转换的类型不完全符合，则直接panic
	// Golang 对类型要求非常严格，类型一定要完全符合
	// 如下两个，一个是*float64，一个是float64，如果弄混，则会panic
	convertPointer := pointer.Interface().(*float64)
	convertValue := value.Interface().(float64)

	fmt.Println(convertPointer)
	fmt.Println(convertValue)
}

运行结果：
0xc42000e238
1.2345
说明

转换的时候，如果转换的类型不完全符合，则直接panic，类型要求非常严格！
转换的时候，要区分是指针还是指
也就是说反射可以将“反射类型对象”再重新转换为“接口类型变量”
未知原有类型【遍历探测其Filed】

很多情况下，我们可能并不知道其具体类型，那么这个时候，该如何做呢？需要我们进行遍历探测其Filed来得知，示例如下:

package main

import (
	"fmt"
	"reflect"
)

type User struct {
	Id   int
	Name string
	Age  int
}

func (u User) ReflectCallFunc() {
	fmt.Println("Allen.Wu ReflectCallFunc")
}

func main() {

	user := User{1, "Allen.Wu", 25}

	DoFiledAndMethod(user)

}

// 通过接口来获取任意参数，然后一一揭晓
func DoFiledAndMethod(input interface{}) {

	getType := reflect.TypeOf(input)
	fmt.Println("get Type is :", getType.Name())

	getValue := reflect.ValueOf(input)
	fmt.Println("get all Fields is:", getValue)

	// 获取方法字段
	// 1. 先获取interface的reflect.Type，然后通过NumField进行遍历
	// 2. 再通过reflect.Type的Field获取其Field
	// 3. 最后通过Field的Interface()得到对应的value
	for i := 0; i < getType.NumField(); i++ {
		field := getType.Field(i)
		value := getValue.Field(i).Interface()
		fmt.Printf("%s: %v = %v\n", field.Name, field.Type, value)
	}

	// 获取方法
	// 1. 先获取interface的reflect.Type，然后通过.NumMethod进行遍历
	for i := 0; i < getType.NumMethod(); i++ {
		m := getType.Method(i)
		fmt.Printf("%s: %v\n", m.Name, m.Type)
	}
}

运行结果：
get Type is : User
get all Fields is: {1 Allen.Wu 25}
Id: int = 1
Name: string = Allen.Wu
Age: int = 25
ReflectCallFunc: func(main.User)

说明

通过运行结果可以得知获取未知类型的interface的具体变量及其类型的步骤为：

先获取interface的reflect.Type，然后通过NumField进行遍历
再通过reflect.Type的Field获取其Field
最后通过Field的Interface()得到对应的value
通过运行结果可以得知获取未知类型的interface的所属方法（函数）的步骤为：

先获取interface的reflect.Type，然后通过NumMethod进行遍历
再分别通过reflect.Type的Method获取对应的真实的方法（函数）
最后对结果取其Name和Type得知具体的方法名
也就是说反射可以将“反射类型对象”再重新转换为“接口类型变量”
struct 或者 struct 的嵌套都是一样的判断处理方式
通过reflect.Value设置实际变量的值
reflect.Value是通过reflect.ValueOf(X)获得的，只有当X是指针的时候，才可以通过reflec.Value修改实际变量X的值，即：要修改反射类型的对象就一定要保证其值是“addressable”的。

示例如下：

package main

import (
	"fmt"
	"reflect"
)

func main() {

	var num float64 = 1.2345
	fmt.Println("old value of pointer:", num)

	// 通过reflect.ValueOf获取num中的reflect.Value，注意，参数必须是指针才能修改其值
	pointer := reflect.ValueOf(&num)
	newValue := pointer.Elem()

	fmt.Println("type of pointer:", newValue.Type())
	fmt.Println("settability of pointer:", newValue.CanSet())

	// 重新赋值
	newValue.SetFloat(77)
	fmt.Println("new value of pointer:", num)

	
	// 如果reflect.ValueOf的参数不是指针，会如何？
	pointer = reflect.ValueOf(num)
	//newValue = pointer.Elem() // 如果非指针，这里直接panic，“panic: reflect: call of reflect.Value.Elem on float64 Value”
}

运行结果：
old value of pointer: 1.2345
type of pointer: float64
settability of pointer: true
new value of pointer: 77
说明

需要传入的参数是* float64这个指针，然后可以通过pointer.Elem()去获取所指向的Value，注意一定要是指针。
如果传入的参数不是指针，而是变量，那么
通过Elem获取原始值对应的对象则直接panic
通过CanSet方法查询是否可以设置返回false
newValue.CantSet()表示是否可以重新设置其值，如果输出的是true则可修改，否则不能修改，修改完之后再进行打印发现真的已经修改了。
reflect.Value.Elem() 表示获取原始值对应的反射对象，只有原始对象才能修改，当前反射对象是不能修改的
也就是说如果要修改反射类型对象，其值必须是“addressable”【对应的要传入的是指针，同时要通过Elem方法获取原始值对应的反射对象】
struct 或者 struct 的嵌套都是一样的判断处理方式
通过reflect.ValueOf来进行方法的调用
这算是一个高级用法了，前面我们只说到对类型、变量的几种反射的用法，包括如何获取其值、其类型、如果重新设置新值。但是在工程应用中，另外一个常用并且属于高级的用法，就是通过reflect来进行方法【函数】的调用。比如我们要做框架工程的时候，需要可以随意扩展方法，或者说用户可以自定义方法，那么我们通过什么手段来扩展让用户能够自定义呢？关键点在于用户的自定义方法是未可知的，因此我们可以通过reflect来搞定

示例如下：

package main

import (
	"fmt"
	"reflect"
)

type User struct {
	Id   int
	Name string
	Age  int
}

func (u User) ReflectCallFuncHasArgs(name string, age int) {
	fmt.Println("ReflectCallFuncHasArgs name: ", name, ", age:", age, "and origal User.Name:", u.Name)
}

func (u User) ReflectCallFuncNoArgs() {
	fmt.Println("ReflectCallFuncNoArgs")
}

// 如何通过反射来进行方法的调用？
// 本来可以用u.ReflectCallFuncXXX直接调用的，但是如果要通过反射，那么首先要将方法注册，也就是MethodByName，然后通过反射调动mv.Call

func main() {
	user := User{1, "Allen.Wu", 25}
	
	// 1. 要通过反射来调用起对应的方法，必须要先通过reflect.ValueOf(interface)来获取到reflect.Value，得到“反射类型对象”后才能做下一步处理
	getValue := reflect.ValueOf(user)

	// 一定要指定参数为正确的方法名
	// 2. 先看看带有参数的调用方法
	methodValue := getValue.MethodByName("ReflectCallFuncHasArgs")
	args := []reflect.Value{reflect.ValueOf("wudebao"), reflect.ValueOf(30)}
	methodValue.Call(args)

	// 一定要指定参数为正确的方法名
	// 3. 再看看无参数的调用方法
	methodValue = getValue.MethodByName("ReflectCallFuncNoArgs")
	args = make([]reflect.Value, 0)
	methodValue.Call(args)
}


运行结果：
ReflectCallFuncHasArgs name:  wudebao , age: 30 and origal User.Name: Allen.Wu
ReflectCallFuncNoArgs

说明

要通过反射来调用起对应的方法，必须要先通过reflect.ValueOf(interface)来获取到reflect.Value，得到“反射类型对象”后才能做下一步处理

reflect.Value.MethodByName这.MethodByName，需要指定准确真实的方法名字，如果错误将直接panic，MethodByName返回一个函数值对应的reflect.Value方法的名字。

[]reflect.Value，这个是最终需要调用的方法的参数，可以没有或者一个或者多个，根据实际参数来定。

reflect.Value的 Call 这个方法，这个方法将最终调用真实的方法，参数务必保持一致，如果reflect.Value'Kind不是一个方法，那么将直接panic。

本来可以用u.ReflectCallFuncXXX直接调用的，但是如果要通过反射，那么首先要将方法注册，也就是MethodByName，然后通过反射调用methodValue.Call

Golang的反射reflect性能
Golang的反射很慢，这个和它的API设计有关。在 java 里面，我们一般使用反射都是这样来弄的。

Field field = clazz.getField("hello");
field.get(obj1);
field.get(obj2);
这个取得的反射对象类型是 java.lang.reflect.Field。它是可以复用的。只要传入不同的obj，就可以取得这个obj上对应的 field。

但是Golang的反射不是这样设计的:

type_ := reflect.TypeOf(obj)
field, _ := type_.FieldByName("hello")
这里取出来的 field 对象是 reflect.StructField 类型，但是它没有办法用来取得对应对象上的值。如果要取值，得用另外一套对object，而不是type的反射

type_ := reflect.ValueOf(obj)
fieldValue := type_.FieldByName("hello")
这里取出来的 fieldValue 类型是 reflect.Value，它是一个具体的值，而不是一个可复用的反射对象了，每次反射都需要malloc这个reflect.Value结构体，并且还涉及到GC。

小结
Golang reflect慢主要有两个原因

涉及到内存分配以及后续的GC；

reflect实现里面有大量的枚举，也就是for循环，比如类型之类的。

总结
上述详细说明了Golang的反射reflect的各种功能和用法，都附带有相应的示例，相信能够在工程应用中进行相应实践，总结一下就是：

反射可以大大提高程序的灵活性，使得interface{}有更大的发挥余地

反射必须结合interface才玩得转
变量的type要是concrete type的（也就是interface变量）才有反射一说
反射可以将“接口类型变量”转换为“反射类型对象”

反射使用 TypeOf 和 ValueOf 函数从接口中获取目标对象信息
反射可以将“反射类型对象”转换为“接口类型变量

reflect.value.Interface().(已知的类型)
遍历reflect.Type的Field获取其Field
反射可以修改反射类型对象，但是其值必须是“addressable”

想要利用反射修改对象状态，前提是 interface.data 是 settable,即 pointer-interface
通过反射可以“动态”调用方法

因为Golang本身不支持模板，因此在以往需要使用模板的场景下往往就需要使用反射(reflect)来实现

原文链接：https://studygolang.com/articles/12348?fr=sidebar

取消
首页
编程
手机
软件
硬件
安卓
苹果
手游
教程
平面
服务器
首页 > 脚本专栏 > Golang > Go语言反射三定律
谈谈Go语言的反射三定律
2016-08-08 09:08:01 投稿：daisy

本文中，我们将解释Go语言中反射的运作机制。每个编程语言的反射模型不大相同，很多语言索性就不支持反射（C、C++）。由于本文是介绍Go语言的，所以当我们谈到“反射”时，默认为是Go语言中的反射。
简介
Reflection（反射）在计算机中表示 程序能够检查自身结构的能力，尤其是类型。它是元编程的一种形式，也是最容易让人迷惑的一部分。
虽然Go语言没有继承的概念，但为了便于理解，如果一个struct A 实现了 interface B的所有方法时，我们称之为“继承”。

类型和接口
反射建立在类型系统之上，因此我们从类型基础知识说起。

Go是静态类型语言。每个变量都有且只有一个静态类型，在编译时就已经确定。比如 int、float32、*MyType、[]byte。 如果我们做出如下声明：

type MyInt int

var i int
var j MyInt
上面的代码中，变量 i 的类型是 int，j 的类型是 MyInt。 所以，尽管变量 i 和 j 具有共同的底层类型 int，但它们的静态类型并不一样。不经过类型转换直接相互赋值时，编译器会报错。

关于类型，一个重要的分类是 接口类型（interface），每个接口类型都代表固定的方法集合。一个接口变量就可以存储（或“指向”，接口变量类似于指针）任何类型的具体值，只要这个值实现了该接口类型的所有方法。一组广为人知的例子是 io.Reader 和 io.Writer， Reader 和 Writer 类型来源于 io包，声明如下：

// Reader is the interface that wraps the basic Read method.
type Reader interface {
 Read(p []byte) (n int, err error)
}

// Writer is the interface that wraps the basic Write method.
type Writer interface {
 Write(p []byte) (n int, err error)
}
任何实现了 Read（Write）方法的类型，我们都称之为继承了 io.Reader（io.Writer）接口。换句话说， 一个类型为 io.Reader 的变量 可以指向（接口变量类似于指针）任何类型的变量，只要这个类型实现了Read 方法：

var r io.Reader
r = os.Stdin
r = bufio.NewReader(r)
r = new(bytes.Buffer)
// and so on
要时刻牢记：不管变量 r 指向的具体值是什么，它的类型永远是 io.Reader。再重复一次：Go语言是静态类型语言，变量 r 的静态类型是 io.Reader。

一个非常非常重要的接口类型是空接口，即：

interface{}
它代表一个空集，没有任何方法。由于任何具体的值都有 零或更多个方法，因此类型为interface{} 的变量能够存储任何值。

有人说，Go的接口是动态类型的。这个说法是错的！接口变量也是静态类型的，它永远只有一个相同的静态类型。如果在运行时它存储的值发生了变化，这个值也必须满足接口类型的方法集合。

由于反射和接口两者的关系很密切，我们必须澄清这一点。

接口变量的表示
Russ Cox 在2009年写了一篇文章介绍 Go中接口变量的表示形式，这里我们不需要重复所有的细节，只做一个简单的总结。

Interface变量存储一对值：赋给该变量的具体的值、值类型的描述符。更准确一点来说，值就是实现该接口的底层数据，类型是底层数据类型的描述。举个例子：

var r io.Reader
tty, err := os.OpenFile("/dev/tty", os.O_RDWR, 0)
if err != nil {
 return nil, err
}
r = tty
在这个例子中，变量 r 在结构上包含一个 (value, type) 对：(tty, os.File)。注意：类型 os.File 不仅仅实现了 Read 方法。虽然接口变量只提供 Read 函数的调用权，但是底层的值包含了关于这个值的所有类型信息。所以我们能够做这样的类型转换：

var w io.Writer
w = r.(io.Writer)
上面代码的第二行是一个类型断言，它断定变量 r 内部的实际值也继承了 io.Writer接口，所以才能被赋值给 w。赋值之后，w 就指向了 (tty, *os.File) 对，和变量 r 指向的是同一个 (value, type) 对。不管底层具体值的方法集有多大，由于接口的静态类型限制，接口变量只能调用特定的一些方法。

我们继续往下看：

var empty interface{}
empty = w
这里的空接口变量 empty 也包含 (tty, *os.File) 对。这一点很容易理解：空接口变量可以存储任何具体值以及该值的所有描述信息。

细心的朋友可能会发现，这里没有使用类型断言，因为变量 w 满足 空接口的所有方法（传说中的“无招胜有招”）。在前一个例子中，我们把一个具体值 从 io.Reader 转换为 io.Writer 时，需要显式的类型断言，是因为 io.Writer 的方法集合 不是 io.Reader 的子集。

另外需要注意的一点是，(value, type) 对中的 type 必须是 具体的类型（struct或基本类型），不能是 接口类型。 接口类型不能存储接口变量。

关于接口，我们就介绍到这里，下面我们看看Go语言的反射三定律。

反射第一定律：反射可以将“接口类型变量”转换为“反射类型对象”。
注：这里反射类型指 reflect.Type 和 reflect.Value。

从用法上来讲，反射提供了一种机制，允许程序在运行时检查接口变量内部存储的 (value, type) 对。在最开始，我们先了解下 reflect 包的两种类型：Type 和 Value。这两种类型使访问接口内的数据成为可能。它们对应两个简单的方法，分别是 reflect.TypeOf 和 reflect.ValueOf，分别用来读取接口变量的 reflect.Type 和 reflect.Value 部分。当然，从 reflect.Value 也很容易获取到 reflect.Type。目前我们先将它们分开。

首先，我们下看 reflect.TypeOf：

package main

import (
 "fmt"
 "reflect"
)

func main() {
 var x float64 = 3.4
 fmt.Println("type:", reflect.TypeOf(x))
}
这段代码会打印出：

type: float64
你可能会疑惑：为什么没看到接口？这段代码看起来只是把一个 float64类型的变量 x 传递给 reflect.TypeOf，并没有传递接口。事实上，接口就在那里。查阅一下TypeOf 的文档，你会发现 reflect.TypeOf 的函数签名里包含一个空接口：

// TypeOf returns the reflection Type of the value in the interface{}.
func TypeOf(i interface{}) Type
我们调用 reflect.TypeOf(x) 时，x 被存储在一个空接口变量中被传递过去； 然后reflect.TypeOf 对空接口变量进行拆解，恢复其类型信息。

函数 reflect.ValueOf 也会对底层的值进行恢复（这里我们忽略细节，只关注可执行的代码）：

var x float64 = 3.4
fmt.Println("value:", reflect.ValueOf(x))
上面这段代码打印出：

value: <float64 Value>
类型 reflect.Type 和 reflect.Value 都有很多方法，我们可以检查和使用它们。这里我们举几个例子。类型 reflect.Value 有一个方法 Type()，它会返回一个 reflect.Type 类型的对象。Type和 Value都有一个名为 Kind 的方法，它会返回一个常量，表示底层数据的类型，常见值有：Uint、Float64、Slice等。Value类型也有一些类似于Int、Float的方法，用来提取底层的数据。Int方法用来提取 int64, Float方法用来提取 float64，参考下面的代码：

var x float64 = 3.4
v := reflect.ValueOf(x)
fmt.Println("type:", v.Type())
fmt.Println("kind is float64:", v.Kind() == reflect.Float64)
fmt.Println("value:", v.Float())
上面这段代码会打印出：

type: float64
kind is float64: true
value: 3.4
还有一些用来修改数据的方法，比如SetInt、SetFloat，在讨论它们之前，我们要先理解“可修改性”（settability），这一特性会在“反射第三定律”中进行详细说明。

反射库提供了很多值得列出来单独讨论的属性。首先是介绍下Value 的 getter 和 setter 方法。为了保证API 的精简，这两个方法操作的是某一组类型范围最大的那个。比如，处理任何含符号整型数，都使用 int64。也就是说 Value 类型的Int 方法返回值为 int64类型，SetInt 方法接收的参数类型也是 int64 类型。实际使用时，可能需要转化为实际的类型：

var x uint8 = 'x'
v := reflect.ValueOf(x)
fmt.Println("type:", v.Type())       // uint8.
fmt.Println("kind is uint8: ", v.Kind() == reflect.Uint8) // true.
x = uint8(v.Uint())    // v.Uint returns a uint64.
第二个属性是反射类型变量（reflection object）的 Kind 方法 会返回底层数据的类型，而不是静态类型。如果一个反射类型对象包含一个用户定义的整型数，看代码：

type MyInt int
var x MyInt = 7
v := reflect.ValueOf(x)
上面的代码中，虽然变量 v 的静态类型是MyInt，不是 int，Kind 方法仍然返回 reflect.Int。换句话说， Kind 方法不会像 Type 方法一样区分 MyInt 和 int。

反射第二定律：反射可以将“反射类型对象”转换为“接口类型变量”。
和物理学中的反射类似，Go语言中的反射也能创造自己反面类型的对象。

根据一个 reflect.Value 类型的变量，我们可以使用 Interface 方法恢复其接口类型的值。事实上，这个方法会把 type 和 value 信息打包并填充到一个接口变量中，然后返回。其函数声明如下：

// Interface returns v's value as an interface{}.
func (v Value) Interface() interface{}
然后，我们可以通过断言，恢复底层的具体值：

y := v.Interface().(float64) // y will have type float64.
fmt.Println(y)
上面这段代码会打印出一个 float64 类型的值，也就是 反射类型变量 v 所代表的值。

事实上，我们可以更好地利用这一特性。标准库中的 fmt.Println 和 fmt.Printf 等函数都接收空接口变量作为参数，fmt 包内部会对接口变量进行拆包（前面的例子中，我们也做过类似的操作）。因此，fmt 包的打印函数在打印 reflect.Value 类型变量的数据时，只需要把 Interface 方法的结果传给 格式化打印程序：

fmt.Println(v.Interface())
你可能会问：问什么不直接打印 v ，比如 fmt.Println(v)？ 答案是 v 的类型是 reflect.Value，我们需要的是它存储的具体值。由于底层的值是一个 float64，我们可以格式化打印：

fmt.Printf("value is %7.1e\n", v.Interface())
上面代码的打印结果是：

3.4e+00
同样，这次也不需要对 v.Interface() 的结果进行类型断言。空接口值内部包含了具体值的类型信息，Printf 函数会恢复类型信息。

简单来说，Interface 方法和 ValueOf 函数作用恰好相反，唯一一点是，返回值的静态类型是 interface{}。

我们重新表述一下：Go的反射机制可以将“接口类型的变量”转换为“反射类型的对象”，然后再将“反射类型对象”转换过去。

反射第三定律：如果要修改“反射类型对象”，其值必须是“可写的”（settable）。

这条定律很微妙，也很容易让人迷惑。但是如果你从第一条定律开始看，应该比较容易理解。

下面这段代码不能正常工作，但是非常值得研究：

var x float64 = 3.4
v := reflect.ValueOf(x)
v.SetFloat(7.1) // Error: will panic.
如果你运行这段代码，它会抛出抛出一个奇怪的异常：

panic: reflect.Value.SetFloat using unaddressable value
这里问题不在于值 7.1 不能被寻址，而是因为变量 v 是“不可写的”。“可写性”是反射类型变量的一个属性，但不是所有的反射类型变量都拥有这个属性。

我们可以通过 CanSet 方法检查一个 reflect.Value 类型变量的“可写性”。对于上面的例子，可以这样写：

var x float64 = 3.4
v := reflect.ValueOf(x)
fmt.Println("settability of v:", v.CanSet())
上面这段代码打印结果是：

settability of v: false
对于一个不具有“可写性”的 Value类型变量，调用 Set 方法会报出错误。首先，我们要弄清楚什么“可写性”。

“可写性”有些类似于寻址能力，但是更严格。它是反射类型变量的一种属性，赋予该变量修改底层存储数据的能力。“可写性”最终是由一个事实决定的：反射对象是否存储了原始值。举个代码例子：

var x float64 = 3.4
v := reflect.ValueOf(x)
这里我们传递给 reflect.ValueOf 函数的是变量 x 的一个拷贝，而非 x 本身。想象一下，如果下面这行代码能够成功执行：

v.SetFloat(7.1)
答案是：如果这行代码能够成功执行，它不会更新 x ，虽然看起来变量 v 是根据 x 创建的。相反，它会更新 x 存在于 反射对象 v 内部的一个拷贝，而变量 x 本身完全不受影响。这会造成迷惑，并且没有任何意义，所以是不合法的。“可写性”就是为了避免这个问题而设计的。

这看起来很诡异，事实上并非如此，而且类似的情况很常见。考虑下面这行代码：

f(x)
上面的代码中，我们把变量 x 的一个拷贝传递给函数，因此不期望它会改变 x 的值。如果期望函数 f 能够修改变量 x，我们必须传递 x 的地址（即指向 x 的指针）给函数 f，如下：

f(&x)
你应该很熟悉这行代码，反射的工作机制是一样的。如果你想通过反射修改变量 x，就咬吧想要修改的变量的指针传递给 反射库。

首先，像通常一样初始化变量 x，然后创建一个指向它的 反射对象，名字为 p：

var x float64 = 3.4
p := reflect.ValueOf(&x) // Note: take the address of x.
fmt.Println("type of p:", p.Type())
fmt.Println("settability of p:", p.CanSet())
这段代码的输出是：

type of p: *float64
settability of p: false
反射对象 p 是不可写的，但是我们也不像修改 p，事实上我们要修改的是 *p。为了得到 p 指向的数据，可以调用 Value 类型的 Elem 方法。Elem 方法能够对指针进行“解引用”，然后将结果存储到反射 Value类型对象 v中：

v := p.Elem()
fmt.Println("settability of v:", v.CanSet())
在上面这段代码中，变量 v 是一个可写的反射对象，代码输出也验证了这一点：

settability of v: true
由于变量 v 代表 x， 因此我们可以使用 v.SetFloat 修改 x 的值：

v.SetFloat(7.1)
fmt.Println(v.Interface())
fmt.Println(x)
上面代码的输出如下：

7.1
7.1
反射不太容易理解，reflect.Type 和 reflect.Value 会混淆正在执行的程序，但是它做的事情正是编程语言做的事情。你只需要记住：只要反射对象要修改它们表示的对象，就必须获取它们表示的对象的地址。

结构体（struct）
在前面的例子中，变量 v 本身并不是指针，它只是从指针衍生而来。把反射应用到结构体时，常用的方式是 使用反射修改一个结构体的某些字段。只要拥有结构体的地址，我们就可以修改它的字段。

下面通过一个简单的例子对结构体类型变量 t 进行分析。

首先，我们创建了反射类型对象，它包含一个结构体的指针，因为后续会修改。

然后，我们设置 typeOfT 为它的类型，并遍历所有的字段。

注意：我们从 struct 类型提取出每个字段的名字，但是每个字段本身也是常规的 reflect.Value 对象。

type T struct {
 A int
 B string
}
t := T{23, "skidoo"}
s := reflect.ValueOf(&t).Elem()
typeOfT := s.Type()
for i := 0; i < s.NumField(); i++ {
 f := s.Field(i)
 fmt.Printf("%d: %s %s = %v\n", i,
  typeOfT.Field(i).Name, f.Type(), f.Interface())
}
上面这段代码的输出如下：

0: A int = 23
1: B string = skidoo
这里还有一点需要指出：变量 T 的字段都是首字母大写的（暴露到外部），因为struct中只有暴露到外部的字段才是“可写的”。

由于变量 s 包含一个“可写的”反射对象，我们可以修改结构体的字段：

f.Interface())s.Field(0).SetInt(77)
s.Field(1).SetString("Sunset Strip")
fmt.Println("t is now", t)
上面代码的输出如下：

t is now {77 Sunset Strip}
如果变量 s 是通过 t ，而不是 &t 创建的，调用 SetInt 和 SetString 将会失败，因为 t 的字段不是“可写的”。

结论

最后再次重复一遍反射三定律：

    1.反射可以将“接口类型变量”转换为“反射类型对象”。
    2.反射可以将“反射类型对象”转换为“接口类型变量”。
    3.如果要修改“反射类型对象”，其值必须是“可写的”（settable）。
一旦你理解了这些定律，使用反射将会是一件非常简单的事情。它是一件强大的工具，使用时务必谨慎使用，更不要滥用。
关于反射，我们还有很多内容没有讨论，包括基于管道的发送和接收、内存分配、使用slice和map、调用方法和函数，这些话题我们会在后续的文章中介绍。请大家继续关注脚本之家。

原作者 Rob Pike，翻译Oscar
