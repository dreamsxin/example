# vlang

See https://vlang.io/docs

* 性能（Performance）

- As fast as C
- C interop without any costs
- Minimal amount of allocations
- Built-in serialization without runtime reflection
- Compiles to native binaries without any dependencies: a simple web server is only 65 KB 

* 安全（Safety）

- No null
- No global variables
- No undefined values
- No undefined behavior
- No variable shadowing
- Bounds checking
- Option/Result types
- Generics
- Immutable variables by default
- Pure functions by default
- Immutable structs by default 

## 编译安装（Installing V from source）

Linux, macOS, Windows, *BSD, Solaris, WSL, Android, Raspbian

```shell
sudo apt install build-essential
git clone https://github.com/vlang/v
cd v
make
# sudo ln -s /vpath/v /usr/local/bin/
sudo ./v symlink
```
在当前目录下生成可执行文件 `v`。

## 更新 v

```shell
v up
```

## 进入命令行模式

```shell
$ v
V 0.1.x
Use Ctrl-C or `exit` to exit

>>> println('hello world')
hello world
>>>
```

命令行模式中，通过 `clear` 可以清理已经定义变量。

可以带上参数 `-g` 或 `-cg` 生成调试信息。
参数 `-cg` 与 `-g` 区别在于显示的行号是编译过程中间 `C` 代码文件 `.tmp.c` 的行号。
参数 `-keep_c` 将保留中间`C` 代码文件 `.tmp.c`

* Commands:
-  up                Update V. Run `v up` at least once per day, since V development is rapid and features/bugfixes are added constantly.
-  run <file.v>      Build and execute the V program in file.v. You can add arguments for the V program *after* the file name.
-  build <module>    Compile a module into an object file.
-  runrepl           Run the V REPL. If V is running in a tty terminal, the REPL is interactive, otherwise it just reads from stdin.
-  symlink           Useful on Unix systems. Symlinks the current V executable to /usr/local/bin/v, so that V is globally available.
-  test v            Run all V test files, and compile all V examples.
-  test folder/      Run all V test files located in the folder and its subfolders. You can also pass individual _test.v files too.
-  fmt               Run vfmt to format the source code. [wip]
-  doc               Run vdoc over the source code and produce documentation.
-  translate         Translates C to V. [wip, will be available in V 0.3]
-  create            Create a new v project interactively. Answer the questions, and run it with `v run projectname`

## 运行自带例子

```shell
sudo apt install libglfw3 libglfw3-dev libfreetype6-dev libssl-dev
cd examples
v hello_world.v && ./hello_world    # or simply
v run hello_world.v                 # this builds the program and runs it right away

v word_counter.v && ./word_counter cinderella.txt
v run news_fetcher.v
v run tetris/tetris.v
```

* 调试模式下运行

```shell
v -g run tetris/tetris.v
```

## Hello World

创建文件 `hello_world.v`：
```vlang
fn main() {
	println('hello world')
}
```

* 编译运行

```shell
v run hello_world.v
```

## 编译模块

创建文件 `mytext.v`：
```vlang
module mytext

pub fn hello() int {
	println('hello world')
	return 1
}
```

```shell
v build module /home/xxx/example/vlang/code/modules/mytext
```

* 测试

创建文件 `mytext_test.v`：
```vlang
import mytext

fn test_mytext() {
	assert mytext.hello() == 1
}
```

```shell
v test code/modules/mytext
```

 ## 编译动态库

```shell
v -shared -prod code/modules/mytext/mytext.v
```

## 注释范例（Comments）

```txt
// This is a single line comment.

/* This is a multiline comment.
   /* It can be nested. */
*/
```

## 函数（Functions）

范例1：
```vlang
fn main() {
	println(add(77, 33))
	println(sub(100, 50))
}

fn add(x int, y int) int {
	return x + y
}

fn sub(x, y int) int {
	return x - y
}
```
参数名之后跟着类型，函数不能重载，可以在声明之前使用，不需要考虑文件和声明的顺序。

范例2：
```vlang
fn foo() (int, int) {
	return 2, 3
}

a, b := foo()
println(a) // 2
println(b) // 3
```

函数可以返回多个值。

* 作用域

函数Functions、常量consts 和 变量默认都是私有未导出的（ private (not exported)）。要允许其他模块能使用它们需要加上前置 `pub`。

范例3：
```vlang
pub fn public_function() {
}

fn private_function() {
}
```

### 默认是纯函数（Pure functions by default）

默认情况下所有函数参数都是不可变的，即使在传递引用时也是如此。
然而，V并不是一种纯粹的函数语言。可以使用关键字`mut`：
```vlang
struct User {
mut:
	is_registered bool
}

fn (u mut User) register() {
	u.is_registered = true
}

mut user := User{}
println(user.is_registered) // "false" 
user.register()
println(user.is_registered) // "true" 

// 普通函数
fn multiply_by_2(arr mut []int) {
	for i := 0; i < arr.len; i++ {
		arr[i] *= 2
	}
}

mut nums := [1, 2, 3]
// 调用此函数时必须在 nums 之前添加 mut。这表明被调用的函数将修改该值。
multiply_by_2(mut nums)
println(nums) // "[2, 4, 6]"
```
最好使用返回值，而不是修改参数。只应在应用程序的性能关键部分中通过修改参数，以减少内存分配和复制。
```vlang
fn register(u User) User {
	return { u | is_registered: true }
}

user = register(user)
```

## 引用（References）

如果函数参数是不可变的，编译器会自行判断是通过值还是引用传递它，开发人员不需要考虑它。
在 `V` 中引用类似于转到指针和C++引用。例如，树结构定义如下：
```vlang
struct Node<T> {
	val   T
	left  &Node
	right &Node
}
```

## 常量（Constants）

使用 `const` 申明，只能在函数外定义。
常量用永远不可修改，由于没有全局变量，所以常量常常当做全局变量使用。

```vlang
const (
	pi    = 3.14
	world = '世界'
)

println(pi)
println(world)
```

可以定义更复杂的值：
```vlang
struct Color {
        r int
        g int
        b int
}

fn (c Color) str() string { return '{$c.r, $c.g, $c.b}' }

fn rgb(r, g, b int) Color { return Color{r: r, g: g, b: b} }

const (
        numbers = [1, 2, 3]

        red  = Color{r: 255, g: 0, b: 0}
        blue = rgb(0, 0, 255)
)

println(numbers)
println(red)
println(blue)
```

## println/print

`println` 是一个简单而强大的内置函数，它可以打印任何东西：字符串、数字、数组、映射、结构。
如果不想打印换行符，请使用`print（）`代替。
如果要为类型定义自定义打印值，只需定义一个`.str（）`字符串方法。


## 变量 Variables

只能使用 `:=` 进行变量的申明和初始化。这意味着变量总是有一个初始值，会自动根据右边的值推断变量类型。
变量只能定义在函数内，不允许全局变量。
可以进行强制类型转换：`string(v)`。

范例1：
```vlang
name := 'Bob'
age := 20
large_number := i64(9999999999)
println(name)
println(age)
println(large_number)
```

* 变量的值默认是不可变得（immutable）

要允许更改变量的值，必须用 `mut` 进行声明：

范例2：
```vlang
mut age := 20
println(age)
age = 21
println(age)
```

改变变量的时候，使用 `=` 进行赋值。

* 变量名掩盖(Variable Shadowing)

不允许变量名掩盖，编译将抛出异常。

范例3：
```vlang
fn main() {
	a := 10
	if true {
		a := 20
	}
}
```
以上代码将出现编译异常。

## 基础类型（Basic types）

- bool

- string

- i8    i16  int  i64      i128 (soon)
- byte  u16  u32  u64      u128 (soon)

- rune // represents a Unicode code point 

- f32 f64

- byteptr
- voidptr

* `int` 类型总是 32位

### 字符串（Strings）

在 `V` 中，字符串是字节的只读数组（因此不可替换字符串上的任意字符，只可追加），数据使用UTF-8编码。
单引号`'`和双引号`"`都可以用来表示字符串。为了保持一致性，vfmt将双引号转换为单引号，除非字符串包含单引号字符。

范例1：
```vlang
name := 'Bob'
println('Hello, $name!')  // `$` is used for string interpolation
println(name.len)

bobby := name + 'by' // + is used to concatenate strings
println(bobby) // "Bobby" 

println(bobby[1..3]) // "ob" 
mut s := 'hello '
s += 'world' // `+=` is used to append to a string
println(s) // "hello world" 
```

* 字符串插入（String interpolation）

插值语法非常简单，使用 `$` 或者适用于复杂表达式的 `${}`：

范例2：
```vlang
struct User {
	name          string
	age           int
mut:
	is_registered bool
}

user := User{
	name: 'Bob'
	age: 10
}
age := 'age = $user.age'
println(age) // age = 10
can_register := 'can register = ${user.age > 13}'
println(can_register) // can register = 0
```

* int 转 string

```vlang
println('age = ' + age.str())
```

* 字符
使用 ``` 表示字符。

```vlang
a := `a`
assert 'aloha!'[0] == `a`
```

* 原始字符串（raw strings）

原始字符串将不会被转码：

```vlang
s := r'hello\nworld'
println(s) // "hello\nworld"
```

### 数组（Arrays）

数组类型由第一个元素决定，所有的元素类型必须相同，否则将会编译失败，例如：
`[1，2，3]`是一个int数组（`[]int`）。
`['a'，'b']`是一个字符串数组（`[]string`）。 

范例1：
```vlang
mut nums := [1, 2, 3]
println(nums) // "[1, 2, 3]"
println(nums[1]) // "2"

mut names := ['John']
```

* 追加值
```vlang
nums << 4
println(nums) // "[1, 2, 3, 4]"

nums << [5, 6, 7]
println(nums) // "[1, 2, 3, 4, 5, 6, 7]"

names << 'Peter'
names << 'Sam'
// names << 10  <-- This will not compile. `names` is an array of strings.
```

* 获取长度

```vlang
println(names.len) // "3"
```

* 是否包含

```vlang
println('Alex' in names) // "false"
```

* 打印数组，转换为 string

```vlang
println(names) // auto converted to a string

s := names.str()
println(s)
```

* 清空数组与预分配

```vlang
names = [] // The array is now empty

// We can also preallocate a certain amount of elements.
ids := [0].repeat(50) // This creates an array with 50 zeros
```

* 数组的过滤和映射（filtered and mapped）

使用 `.filter()` 和 `.map()` 方法：
```vlang
nums := [1, 2, 3, 4, 5, 6]
even := nums.filter(it % 2 == 0)
println(even) // [2, 4, 6]

words := ['hello', 'world']
upper := words.map(it.to_upper())
println(upper) // ['HELLO', 'WORLD']
```
这里的 `it` 是一个特殊的变量，引用 `filter/map` 方法中的元素。

### Maps

```vlang
mut m := map[string]int // Only maps with string keys are allowed for now 
m['one'] = 1
m['two'] = 2
println(m['one']) // "1" 
println(m['bad_key']) // "0" 
println('bad_key' in m) // Use `in` to detect whether such key exists
m.delete('two')

numbers := {
	'one': 1,
	'two': 2,
}
```

### 枚举（Enums）

```vlang
enum Color {
	red green blue
}

fn main() {
	mut color := Color.red
	// V knows that `color` is a `Color`. No need to use `color = Color.green` here.
	color = .green
	println(color) // "1"  TODO: print "green"? 
	if color == .green {
		println("it's green")
	}
}
```

### 结构体（Structs）

结构是在堆栈上分配的，要在堆上分配，并获取对它的引用，请使用前缀 `&`：

```vlang
struct Point {
	x int
	y int
}

p := Point{
	x: 10
	y: 20
}
println(p.x) // Struct fields are accessed using a dot

 // Alternative initialization syntax for structs with 3 fields or fewer
p2 := &Point{10, 10}
// References have the same syntax for accessing fields 
println(p2.x) 
```
这里的 `p2` 是 `&Point` 类型。

* 默认值

```vlang
struct Foo {
  a int
  b int = 10
}

foo := Foo{}
assert foo.a == 0
assert foo.b == 10
```

* 访问修饰符（Access modifiers）
默认情况下，结构字段是私有的和不可变的（也使结构不可变）。它们的访问修饰符可以通过pub和mut进行更改。总共有5种可能的选择：
```vlang
struct Foo {
	a int     // private immutable (default)
mut:
	b int     // private mutable
	c int     // (you can list multiple fields with the same access modifier) 
pub:
	d int     // public immmutable (readonly)
pub mut:
	e int     // public, but mutable only in parent module 
__global:
	f int 	  // public and mutable both inside and outside parent module 
}                 // (not recommended to use, that's why the 'global' keyword 
                  // starts with __)
```

## 定义类型方法

`V` 没有类，但可以为类型定义方法，下面定义结构类型的方法：

```vlang
struct User {
	age int
}

fn (u User) can_register() bool {
	return u.age > 16
}

user := User{age: 10}
println(user.can_register()) // "false" 

user2 := User{age: 20}
println(user2.can_register()) // "true" 
```

方法是具有特殊接收器参数的函数。
接收者出现在`fn`关键字和方法名之间的自变量列表中。
在本例中，`can_register`方法有一个`User`类型为u的接收器。
惯例不是使用`self`或`this`这样的接收器名称，而是一个短的、最好是一个字母长的名称。

## 有限运算符重载

为了提高安全性和可维护性，运算符重载有几个限制：

- 只能重载`+`、`-`、`*`、`/`运算符。
- 不允许在运算符函数内调用其他函数。
- 运算符函数无法修改其参数。
- 两个参数必须具有相同的类型（就像V中的所有运算符一样）

为了提高可读性，运算符重载非常重要，例如：

```vlang
a + b + c * d
```
可读性比
```vlang
a.add(b).add(c.mul(d))
```
高多了。

例如，内置模块中字符串类型`string`：
```vlang
pub struct string {
	// mut:
	// hash_cache int
pub:
	str byteptr // points to a C style 0 terminated string of bytes.
	len int // the length of the .str field, excluding the ending 0 byte. It is always equal to strlen(.str).
}
```

重载的操作符：
```vlang

// ==
fn (s string) eq(a string) bool {
	if isnil(s.str) {
		// should never happen
		panic('string.eq(): nil string')
	}
	if s.len != a.len {
		return false
	}
	for i := 0; i < s.len; i++ {
		if s[i] != a[i] {
			return false
		}
	}
	return true
}

// !=
fn (s string) ne(a string) bool {
	return !s.eq(a)
}

// s < a
fn (s string) lt(a string) bool {
	for i := 0; i < s.len; i++ {
		if i >= a.len || s[i] > a[i] {
			return false
		}
		else if s[i] < a[i] {
			return true
		}
	}
	if s.len < a.len {
		return true
	}
	return false
}

// s <= a
fn (s string) le(a string) bool {
	return s.lt(a) || s.eq(a)
}

// s > a
fn (s string) gt(a string) bool {
	return !s.le(a)
}

// s >= a
fn (s string) ge(a string) bool {
	return !s.lt(a)
}

// TODO `fn (s string) + (a string)` ? To be consistent with operator overloading syntax.
fn (s string) add(a string) string {
	new_len := a.len + s.len
	mut res := string{
		len: new_len
		str: malloc(new_len + 1)
	}
	for j := 0; j < s.len; j++ {
		res[j] = s[j]
	}
	for j := 0; j < a.len; j++ {
		res[s.len + j] = a[j]
	}
	res[new_len] = `\0` // V strings are not null terminated, but just in case
	return res
}
```

## 接口（Interfaces）

类型通过实现其方法来实现接口。没有明确的意图声明，没有“implements”关键字。

```vlang
struct Dog {}
struct Cat {}

fn (d Dog) speak() string {
	return 'woof'
}

fn (c Cat) speak() string {
	return 'meow' 
}

interface Speaker {
	speak() string
}

fn perform(s Speaker) {
	println(s.speak())
}

dog := Dog{}
cat := Cat{}
perform(dog) // "woof" 
perform(cat) // "meow"
```

## 基本操作

### If

条件周围没有括号，可用于表达式中：
```vlang
a := 10
b := 20
if a < b {
	println('$a < $b')
} else if a > b {
	println('$a > $b')
} else {
	println('$a == $b')
}

num := 777
s := if num % 2 == 0 {
	'even'
}
else {
	'odd'
}
println(s) // "odd"
```

### In operator

允许检查 `array` 或 `map` 是否包含元素。
```vlang
if parser.token == .plus || parser.token == .minus ||
	parser.token == .div || parser.token == .mult {
	...
}

if parser.token in [.plus, .minus, .div, .mult] {
	...
}
```
`V` 会优化上面的表达式，上面的两个`if`语句都产生相同的机器代码，不会新建数组。

### For loop

`V` 只有 `for` 这个循环控制方法。

* 遍历数组元素：
```vlang
mut numbers := [1, 2, 3, 4, 5]
for i, num in numbers {
	println(num)
	numbers[i] = 0
}

names := ['Sam', 'Peter']
for i, name in names {
	println('$i) $name')  // Output: 0) Sam
}                             //         1) Peter
```

* 类似 while

```vlang
mut sum := 0
mut i := 0
for i <= 100 {
	sum += i
	i++
}
println(sum) // "5050"

// 条件省略，将导致无限循环。
mut num := 0
for {
	num++
	if num >= 10 {
		break
	}
}
println(num) // "10"
```

* 传统的 C 风格`for`循环

在这里，不需要用`mut`声明`i`，因为根据定义，它总是可变的。

```vlang
for i := 0; i < 10; i++ {
	// Don't print 6
	if i == 6 {
		continue
	}
	println(i)
}
```

### 匹配（取代 switch 的 Match）

`match` 语句是编写一系列`if-else`语句的较短方法。

范例1：
```vlang
os := 'windows'
print('V is running on ')
match os {
	'darwin' { println('macOS.') }
	'linux'  { println('Linux.') }
	else     { println(os) }
}

s := match number {
	1    { 'one' }
	2    { 'two' }
	else { 
		println('this works too')
		'many' 
	}
}

enum Color {
	red
	blue
	green
}

// 使用简便语法 `.variant_here`
fn is_red_or_blue(c Color) bool {
	return match c {
		.red { true }
		.blue { true }
		else { false }
	}
}
```

## 可选返回类型与错误处理（Option/Result types & error handling）

函数返回类型加个 `?` 前置，即表示可能返回不同值，可选值可以是 `return none` or `return error('some error')`。

```vlang
struct User {
	id int
	name string
}

struct Repo {
	users []User
}

fn new_repo() Repo {
	return Repo {
		users: [User{1, 'Andrew'}, User {2, 'Bob'}, User {10, 'Charles'}]
	}
}

fn (r Repo) find_user_by_id(id int) ?User {
	for user in r.users {
		if user.id == id {
			// V automatically wraps this into an option type 
			return user
		}
	}
	return error('User $id not found')
}

fn main() {
	repo := new_repo()
	user := repo.find_user_by_id(10) or { // Option types must be handled by `or` blocks 
		return  // `or` block must end with `return`, `break`, or `continue` 
	}
	println(user.id) // "10" 
	println(user.name) // "Charles"
}
```

我们可以不使用 `or` 捕捉错误，而是继续抛出错误：
```vlang
fn test(url string) ?string { 
	resp := http.get(url)?
	return resp.body
}
```
如果调用函数不是可选函数，将导致程序异常停止，所以一般可以如下处理：
```vlang
fn test(url string) string { 
	resp := http.get(url) or { panic(err) }
	return resp.body
}
```

## 泛型（Generics）

```vlang
struct Repo<T> {
	db DB
}

fn new_repo<T>(db DB) Repo<T> {
	return Repo<T>{db: db}
}

// This is a generic function. V will generate it for every type it's used with.
fn (r Repo<T>) find_by_id(id int) ?T {
	table_name := T.name // in this example getting the name of the type gives us the table name
	return r.db.query_one<T>('select * from $table_name where id = ?', id)
}

db := new_db()
users_repo := new_repo<User>(db)
posts_repo := new_repo<Post>(db)
user := users_repo.find_by_id(1)?
post := posts_repo.find_by_id(1)?
```

## 并发（Concurrency）

并发模型与Go非常相似。要同时运行`foo（）`，只需使用`go foo（）`调用它。现在，它在一个新的系统线程中启动该函数。
很快将实现协同路由和调度程序。

## JSON 编码和解码

V 生成用于 JSON 编码和解码的代码。不使用运行时反射。这会带来更好的性能。

```vlang
Decoding JSON

import json

struct User {
	name string
	age  int

	// Use the `skip` attribute to skip certain fields
	foo Foo [skip]  

	// 字段名跟 JSON 中的key 不一致，可以自定义
	last_name string [json:lastName]  
}

data := '{ "name": "Frodo", "lastName": "Baggins", "age": 25 }'
user := json.decode(User, data) or {
	eprintln('Failed to decode json')
	return
}
println(user.name)
println(user.last_name)
println(user.age)
```

## 延迟处理（Defer）

`defer` 语句将语句块的执行推迟到作用域结束或周围函数返回。

## ORM

V 有一个支持 Postgres 的内置 ORM，很快就会支持MySQL和SQLite。

```vlang
struct Customer { // struct name has to be the same as the table name for now
	id int // an integer id must be the first field
	name string
	nr_orders int
	country string
}

db := pg.connect(db_name, db_user)

// select count(*) from Customer
nr_customers := db.select count from Customer
println('number of all customers: $nr_customers')

// V syntax can be used to build queries
// db.select returns an array
uk_customers := db.select from Customer where country == 'uk' && nr_orders > 0
println(uk_customers.len)
for customer in uk_customers {
	println('$customer.id - $customer.name')
}

// by adding `limit 1` we tell V that there will be only one object
customer := db.select from Customer where id == 1 limit 1
println('$customer.id - $customer.name')

// insert a new customer
new_customer := Customer{name: 'Bob', nr_orders: 10}
db.insert(new_customer)
```

## 格式化 vfmt

```shell
v fmt file.v
```

## 文档
每个函数/类型/常量的文档必须放在声明之前：
```vlang
// clearall clears all bits in the array
fn clearall() {
}
```
生成
```shell
v doc path/to/module
```

## 调用 C 函数（Calling C functions from V）

`call_c_functions.v` 代码如下：
```vlang
#flag -lsqlite3
#include "sqlite3.h"

struct C.sqlite3
struct C.sqlite3_stmt

fn C.sqlite3_column_text(voidptr, int) byteptr
fn C.sqlite3_column_int(voidptr, int) int
fn C.sqlite3_open()
fn C.sqlite3_step() int
fn C.sqlite3_prepare_v2()
fn C.sqlite3_finalize()

fn main() {
        path := 'users.db'
        db := &C.sqlite3(0) // a temporary hack meaning `sqlite3* db = 0`
        C.sqlite3_open(path.str, &db)

        query := 'select count(*) from users'
        stmt := &C.sqlite3_stmt(0)
        C.sqlite3_prepare_v2(db, query.str, - 1, &stmt, 0)
        C.sqlite3_step(stmt)
        nr_users := C.sqlite3_column_int(stmt, 0)
        C.sqlite3_finalize(stmt)
        println(nr_users)
}
```
在文件最上面增加 `#flag` 指令提供 `C` 编译指令，比如链接 `-l`、头文件路径 `-I`、编译时定义 `-D`等。

可以给不同目标环境设置不同指令（目前支持linux , darwin , and windows）：
```vlang
#flag linux -lsdl2
#flag linux -Ivig
#flag linux -DCIMGUI_DEFINE_ENUMS_AND_STRUCTS=1
#flag linux -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1
#flag linux -DIMGUI_IMPL_API=
```

显示 C 编译命令：
```shell
v -show_c_cmd call_c_functions.v
```
out:
```txt
==========
cc  -std=gnu11 -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-unused-result -Wno-unused-function -Wno-missing-braces -Wno-unused-label -Werror=implicit-function-declaration -o "call_c_functions" "/tmp/v/call_c_functions.tmp.c"  -lsqlite3  -lm -lpthread   -ldl 
cc took 238 ms
=========
```

## 编译时 `if`（Compile time if）

类似 C 里面预定义宏的判断。

```vlang
$if windows {
	println('Windows')
}
$if linux {
	println('Linux')
}
$if mac {
	println('macOS')
}

$if debug {
	println('debugging')
}
```

## 将 C 代码转换成 V（Translating C/C++ to V）

TODO: translating C to V will be available in V 0.3. C++ to V will be available later this year. 

```shell
v translate test.cpp
```

`test.cpp` 内容如下：
```cpp
#include <vector>
#include <string>
#include <iostream>

int main() {
        std::vector<std::string> s;
        s.push_back("V is ");
        s.push_back("awesome");
        std::cout << s.size() << std::endl;
        return 0;
}
```
生成文件`test.v`：
```vlang
fn main {
        mut s := []
	s << 'V is '
	s << 'awesome'
	println(s.len)
}
```
