# Rust 入门

参考文档：

- https://www.rust-lang.org/zh-CN/
- https://kaisery.gitbooks.io/rust-book-chinese/content/

## 安装

```shell
curl https://sh.rustup.rs -sSf | sh
```

在 Rust 开发环境中，所有工具都安装到 ~/.cargo/bin 目录， 并且您能够在这里找到 Rust 工具链，包括 rustc、cargo 及 rustup。

## 导入环境变量

```shell
source ~/.cargo/env
```

## 卸载

```shell
rustup self uninstall
```

## 显示版本号

```shell
rustc --version
```

## 创建文件 `hello_world.rs`

```rust
fn main() {
    println!("Hello, world!"); // 带有 叹号，表示是Rust宏定义
}
```

Rust是一门面向表达式的语言，也就是说大部分语句都是表达式。`;`表示一个表达式的结束，另一个新表达式的开始。大部分 Rust 代码行以`;`结尾。

### 编译 `hello_world.rs`

```shell
rustc hello_world.rs
```

会生成一个可执行文件 `hello_world`

### 运行

```shell
./hello_world
```

## 设定环境变量获取 backtrace

```shell
export RUST_BACKTRACE=1
# or
RUST_BACKTRACE=1 ./hello_world
RUST_BACKTRACE=1 cargo run
```

# 使用 Cargo

仅仅使用 rustc 编译简单程序是没问题的，不过随着你的项目的增长，你将想要能够控制你项目拥有的所有选项，并易于分享你的代码给别人或别的项目。

Cargo 是 Rust 的构建系统和包管理工具，Cargo 负责三个工作：
- 构建代码
- 下载代码依赖库，我们把你代码需要的库叫做“依赖（dependencies）”因为你的代码依赖他们。
- 编译库

## 查看 Cargo 版本号

```shell
cargo --version
```

## Cargo 包管理

Rust 包管理使用 crate 格式的压缩包存储和发布库，官方有一个集中式的仓库。
我们可以使用国内的仓库镜像地址，使用镜像只需要在项目根目录下新建一个 `.cargo/config` 文件，并在其中加入以下内容：

```config
[registry]
index = "git://crates.mirrors.ustc.edu.cn/index"
```

## 将之前项目转换到 Cargo

让我们将 Hello World 程序迁移至 Cargo，现在需要做三件事：

- 将源文件放到正确的目录
- 创建一个 Cargo 配置文件

### 源文件目录

```shell
mkdir hello_world2/src
cp hello_world.rs hello_world2/src/
```

### 创建配置文件 `Cargo.toml`

```toml
[package]
name = "hello_world"
version = "0.0.1"
authors = [ "dreamsxin <dreamsxin@qq.com>" ]

[[bin]]
name = "hello_world"
path = "src/hello_world.rs
```

第一行，`[package]`，表明下面的语句用来配置一个包。
另外三行设置了 Cargo 编译你的程序所需要知道的三个配置：包的名字，版本，和作者。
如果源文件名为 `main.rs` 则 `[[bin]]` 不需要配置。

### 构建并运行一个 Cargo 项目

当Cargo.toml文件位于项目的根目录时，我们就准备好可以构建并运行 Hello World 程序了！

```shell
cd hello_world2/
cargo build
```

第一次构建会下载依赖库，存放在目录 `~/.cargo/registry/`。
执行成功后，会生成文件 `./target/debug/hello_world`

### 发布构建（Building for Release）

当你的项目准备好发布了，可以使用以下命令来优化编译项目。

```shell
cargo build --release
````

这些优化可以让 Rust 代码运行的更快，不过启用他们会让程序花更长的时间编译。这也是为何这是两种不同的配置，一个为了开发，另一个构建提供给用户的最终程序。

### 运行

```shell
cargo run
```

### 创建新 Cargo 项目的简单方法

```shell
cargo new hello_world --bin
```

这个命令传递了`--bin`参数因为我们的目标是直接创建一个可执行程序，而不是一个库。
Cargo 为我们创建了两个文件和一个目录：一个Cargo.toml和一个包含了 `main.rs` 文件的 `src` 目录。

### 清理

```shell
cargo clean
```

## 使用 Cargo 创建猜数字游戏

```shell
cargo new guessing_game --bin
```

这个命令传递了`--bin`参数因为我们的目标是直接创建一个可执行程序，而不是一个库。
Cargo 为我们生成了一个“Hello, world!”。查看src/main.rs文件：
```rust
fn main() {
    println!("Hello, world!");
}
```

### 引入随机库

在文件 `Cargo.toml` 增加：
```toml
[dependencies]
rand="*"
```

星号 `*` 代表最新版本，我们可以指定版本 rand="0.3"。

### 编写代码

```rust
// 在[dependencies]声明了rand，这里用extern crate来让Rust知道我们正在使用它
extern crate rand;

use std::io;
use std::cmp::Ordering;
use rand::Rng;

fn main() {
    println!("Guess the number!");

    let secret_number = rand::thread_rng().gen_range(1, 101);

    println!("The secret number is: {}", secret_number);

    println!("Please input your guess.");

	// 创建一个地方储存用户输入
    let mut guess = String::new();

    // 终端标准输入句柄获取用户输入
    io::stdin().read_line(&mut guess).expect("failed to read line");

    println!("You guessed: {}", guess);

    match guess.cmp(&secret_number) {
        Ordering::Less    => println!("Too small!"),
        Ordering::Greater => println!("Too big!"),
        Ordering::Equal   => println!("You win!"),
    }
}
```

* 如果无法通过 Cargo 下载包，可以使用代理

```shell
export http_proxy="http://127.0.0.1:8787";
export HTTPS_PROXY="https://127.0.0.1:8787/";
export FTP_PROXY=ftp://127.0.0.1:8787/";
```

或者配置 `.cargo/config`

```conf
[http]
proxy = "127.0.0.1:8787"  # HTTP proxy to use for HTTP requests (defaults to none)
timeout = 60000      # Timeout for each HTTP request, in milliseconds

# or
[http]
proxy = "http://127.0.0.1:8787/"

[https]
proxy = "https://127.0.0.1:8787/"
```

我们可以通过 `cargo build --verbose` 看下最后执行的命令：
```shell
rustc src/main.rs --crate-name guessing_game --crate-type bin -g
	--out-dir /home/zhuzx/work/example/rust/guessing_game/target/debug
	--emit=dep-info,link
	-L dependency=/home/zhuzx/work/example/rust/guessing_game/target/debug
	-L dependency=/home/zhuzx/work/example/rust/guessing_game/target/debug/deps
	--extern rand=/home/zhuzx/work/example/rust/guessing_game/target/debug/deps/librand-49a08859d086fffe.rlib`
```

# 语法语义

## 变量绑定

let 被用来声明一个绑定，他们将一些值绑定到一个名字上，这样可以在之后使用他们。

```rust
fn main() {
    let x = 5;
}
```

### 模式（Patterns）

let 语句的左侧是一个“模式”，而不仅仅是一个变量。这意味着我们可以这样写：

```rust
let (x, y) = (1, 2);
```

在这个语句被计算后，x将会是1，而y将会是2。

### 类型注解（Type annotations）

Rust 是一个静态类型语言，这意味着我们需要先确定我们需要的类型。
Rust 有一个叫做类型推断的功能。如果它能确认这是什么类型，Rust 不需要你明确地指出来。

若你愿意，我们也可以加上类型。类型写在一个冒号`:`后面：

```rust
let x: i32 = 5;
```

可以描述为“x被绑定为i32类型，它的值是5”。
Rust 有许多不同的原生整数类型：
- i 代表有符号整数
- u 代表无符号整数。

可能的整数大小有 8、16、32 和 64 位。

### 可变性

变量默认是不可变（immutable）的。
当变量不可变时，意味着一旦值被绑定上一个名称，你就不能改变这个值。可以通过在变量名之前加 `mut` 来使其可变。
注意和下面变量的覆盖（Shadowing）做区别。

创建文件 `variables.rs`：

```rust
fn main() {
    let x = 5;
    println!("The value of x is: {}", x);
    x = 6;
    println!("The value of x is: {}", x);
}
```

运行将会报错 re-assignment of immutable variable `x`。

这里 `println!` 输出的字符串中包含一对大括号`{}`，Rust将把它解释为插入值的请求。当你只写了大括号的时候，Rust 会尝试检查值的类型来显示一个有意义的值。
更多的选项，查看文档 https://doc.rust-lang.org/std/fmt/。

`println!` 是 Rust 的宏，只要是看到了叹号`!`，就是一个代替普通函数的宏。

```rust
macro_rules! println {
    () => { ... };
    ($fmt:expr) => { ... };
    ($fmt:expr, $($arg:tt)*) => { ... };
}
```

### 初始化绑定

Rust 变量绑定要求在可以使用它之前必须初始化，Rust 是不会让我们使用一个没有经过初始化的值的。

### 作用域（Scope）

变量绑定有一个作用域 - 他们被限制只能在他们被定义的块中存在。一个块是一个被大括号`{`和`}`包围的语句集合。函数定义也是块。

```rust
fn main() {
    let x: i32 = 17;
    {
        let y: i32 = 3;
        println!("The value of x is {} and value of y is {}", x, y);
    }
	// 不能访问 y 值，因为它已不在作用域中
    println!("The value of x is {} and value of y is {}", x, y); // This won't work.
}
```

### 变量的遮盖（Shadowing）

变量可以重复使用 let 关键字进行多次遮盖。这意味着一个后声明的并位于同一作用域的相同名字的变量绑定将会覆盖前一个变量绑定：

```rust
let x: i32 = 8;
{
    println!("{}", x); // Prints "8".
    let x = 12;
    println!("{}", x); // Prints "12".
}
println!("{}", x); // Prints "8".
let x =  42;
println!("{}", x); // Prints "42".
```

隐藏和可变绑定可能表现为同一枚硬币的两面，他们是两个不同的概念，不能互换使用。举个例子，隐藏允许我们将一个名字重绑定为不同的类型。它也可以改变一个绑定的可变性。
注意隐藏并不改变和销毁被绑定的值，这个值会在离开作用域之前继续存在，即便无法通过任何手段访问到它。

```rust
let mut x: i32 = 1;
x = 7;
let x = x; // `x` is now immutable and is bound to `7`

let y = 4;
let y = "I can also be bound to text!"; // `y` is now of a different type
```

### 变量和常量的区别

不允许对常量使用 mut：常量不光默认不能变，它总是不能变。
声明常量使用 const 关键字而不是 let，而且必须注明值的类型。
常量可以在任何作用域声明，包括全局作用域，这在一个值需要被很多部分的代码用到时很有用。
常量只能用于常量表达式，而不能作为函数调用的结果，或任何其他只在运行时计算的值。

一个常量声明的例子，它的名称是 MAX_POINTS，值是 100,000。（常量使用下划线分隔的大写字母命名）：
```rust
const MAX_POINTS: u32 = 100_000;
```

## 数据类型

在 Rust 中，任何值都属于一种明确的类型（type），声明它被指定了何种数据，以便明确其处理方式。我们将分两部分探讨一些内建类型：标量（scalar）和复合（compound）。

### 标量（scalar）

标量类型代表一个单独的值。Rust 有四种基本的标量类型：整型、浮点型、布尔类型和字符类型。

#### 数字类型

Rust有一些分类的大量数字类型：有符号和无符号，定长和变长，浮点和整型。

* 整型

整数是一个没有小数部分的数字。

- i 代表有符号整数，有符号数以二进制补码形式（two’s complement representation）存储。
- u 代表无符号整数。

可能的整数大小有 8、16、32 和 64 位。

每一个有符号的变体可以储存包含从 -(2n - 1) 到 2n - 1 - 1 在内的数字，这里 n 是变体使用的位数。所以 i8 可以储存从 -(27) 到 27 - 1 在内的数字，也就是从 -128 到 127。无符号的变体可以储存从 0 到 2n - 1 的数字，所以 u8 可以储存从 0 到 28 - 1 的数字，也就是从 0 到 255。

* 浮点型

有两个主要的浮点数类型，f32 和 f64，它们对应 IEEE-754 单精度和双精度浮点数。分别占 32 位和 64 位比特。默认类型是 f64，因为它与 f32 速度差不多，然而精度更高。
在 32 位系统上也能够使用 f64，不过比使用 f32 要慢。

```rust
fn main() {
    let x = 2.0; // f64

    let y: f32 = 3.0; // f32
}
```

* 可变大小类型

Rust 也提供了依赖底层机器指针大小的类型。这些类型拥有“size”分类，并有有符号和无符号变体。它有两个类型：isize和usize。

* 布尔型

有两个可能的值：true和false。Rust 中的布尔类型使用bool表示。例如：

```rust
fn main() {
    let t = true;

    let f: bool = false; // with explicit type annotation
}
```

* 字符类型

char类型代表一个单独的 Unicode 字符的值。你可以用单引号（'）创建 `char`，不像其它语言，这意味着Rust的char并不是 1 个字节，而是 4 个。

```rust
let x = 'x';
let two_hearts = '💕';
```

* 字符串类型

Rust 的 str 类型是最原始的字符串类型。作为一个不定长类型，它本身并不是非常有用，不过当它用在引用后是就有用了，例如`&str`。
可以认为 `&str` 是一个字符串片段（string slice）。

```rust
let hello = "Hello, world!";

// with an explicit type annotation
let hello: &'static str = "Hello, world!";
```

#### 复合类型

复合类型可以将多个其他类型的值组合成一个类型。Rust 有两个原生的复合类型：元组（tuple）和数组（array）。

* 数组

一个定长相同类型的元素列表。数组默认是不可变的。
可以用下标（subscript notation）来访问特定的元素，下标从0开始。

```rust
let a = [1, 2, 3]; // a: [i32; 3]
let mut m = [1, 2, 3]; // m: [i32; 3]

println!("a has {} elements", a.len());

let names = ["Graydon", "Brian", "Niko"]; // names: [&str; 3]

println!("The second name is: {}", names[1]);
```

数组的类型是`[T; N]`，N是一个编译时常量，代表数组的长度。

* 切片（Slices）

一个切片（slice）是一个数组的引用（或者“视图”）。它有利于安全，有效的访问数组的一部分而不用进行拷贝。比如，你可能只想要引用读入到内存的文件中的一行。原理上，片段并不是直接创建的，而是引用一个已经存在的变量。片段有预定义的长度，可以是可变也可以是不可变的。

在底层，slice 代表一个指向数据开始的指针和一个长度。

你可以用一个&和[]的组合从多种数据类型创建一个切片。

```rust
let a = [0, 1, 2, 3, 4];
let complete = &a[..]; // A slice containing all of the elements in `a`.
let middle = &a[1..4]; // A slice of `a`: only the elements `1`, `2`, and `3`.
let str_slice: &[&str] = &["one", "two", "three"];
```

* 元组（Tuples）

元组是一个将多个其他类型的值组合进一个复合类型的主要方式，是固定大小的有序列表。
为了从元组中获取单个的值，可以使用模式匹配（pattern matching）来解构（destructure ）元组，也可以使用点号`.`后跟值的索引来直接访问他们，不像数组索引使用`[]`。

```rust
fn main() {
    let tup: (i32, f64, u8) = (500, 6.4, 1);
    let (x, y, z) = tup;
    println!("The value of y is: {}", y);

    let five_hundred = x.0;
    let six_point_four = x.1;
    let one = x.2;
}
```

你可以把一个元组赋值给另一个，如果它们包含相同的类型和数量。当元组有相同的长度时它们有相同的数量。
```rust
let mut x = (1, 2); // x: (i32, i32)
let y = (2, 3); // y: (i32, i32)

x = y;
```

你可以一个逗号来消除一个单元素元组和一个括号中的值的歧义：
```rust
(0,); // single-element tuple
(0); // zero in parentheses
```

### 通用集合类型

Rust 标准库中包含一系列被称为集合（collections）的非常有用的数据结构。

在 Rust 程序中被广泛使用的集合：

- vector 允许我们一个挨着一个地储存一系列数量可变的值
- 字符串（string）是一个字符的集合，不同于上面介绍的 str 类型。
- 哈希 map（hash map）允许我们将值与一个特定的键（key）相关联。这是一个叫做 map 的更通用的数据结构的特定实现。

* 字符串

称作String的类型是由标准库提供的，而没有写进核心语言部分，它是可增长的、可变的、有所有权的、UTF-8 编码的字符串类型。

```rust
// 新建字符串
let s = String::new();

// 使用 to_string 方法
let data = "initial contents";
let s = data.to_string();

// the method also works on a literal directly:
let s = "initial contents".to_string();

let s = String::from("initial contents");

// 更新字符串
let mut s = String::from("foo");
s.push_str("bar");

let mut s1 = String::from("foo");
let s2 = String::from("bar");
s1.push_str(&s2);

// 单个字符
let mut s = String::from("lo");
s.push('l');

// 合并字符串
let s1 = String::from("Hello, ");
let s2 = String::from("world!");
let s3 = s1 + &s2; // Note that s1 has been moved here and can no longer be used

let s1 = String::from("tic");
let s2 = String::from("tac");
let s3 = String::from("toe");

let s = format!("{}-{}-{}", s1, s2, s3);

// 遍历字符串的方法
for c in "नमस्ते".chars() {
    println!("{}", c);
}

// 使用原始字节
for b in "नमस्ते".bytes() {
    println!("{}", b);
}

```

String 是一个 `Vec<u8>` 的封装。


* Vectors

Vector 是一个动态或“可增长”的数组，被实现为标准库类型 `Vec<T>`。
vector 总是在堆上分配数据。vector 与切片就像String与&str一样。你可以使用vec!宏来创建它：

```rust
let v = vec![1, 2, 3, 4, 5]; // v: Vec<i32>
```

对于初始值统一值的初始化方式：

```rust
// 初始化为 0
let v = vec![0; 10]; // ten zeroes
```

使用索引访问时，必须使用 usize 类型的值：

```rust
let v = vec![1, 2, 3, 4, 5];

let i: usize = 0;
let j: i32 = 0;

// Works:
v[i];

// Doesn’t:
v[j];
```

如果你想处理越界错误而不是 panic，你可以使用像get或get_mut这样的方法，他们当给出一个无效的索引时返回 None：

```rust
let v = vec![1, 2, 3];
match v.get(7) {
    Some(x) => println!("Item 7 is {}", x),
    None => println!("Sorry, this vector is too short.")
}
```

可以用for来迭代 vector 的元素。有3个版本：
```rust
let mut v = vec![1, 2, 3, 4, 5];

for i in &v {
    println!("A reference to {}", i);
}

for i in &mut v {
    println!("A mutable reference to {}", i);
}

// 遍历之后 v 不能再使用
for i in v {
    println!("Take ownership of the vector and its element {}", i);
}
```
注意：你不能在使用 vector 的所有权遍历之后再次遍历它。你可以使用它的引用多次遍历 vector。

当我们获取了 vector 的第一个元素的不可变引用并尝试在 vector 末尾增加一个元素的时候，就会发生错误：
```rust
let mut v = vec![1, 2, 3, 4, 5];
let first = &v[0];
v.push(6);
```
在 vector 的结尾增加新元素是，在没有足够空间将所有所有元素依次相邻存放的情况下，可能会要求分配新内存并将老的元素拷贝到新的空间中。这时，第一个元素的引用就指向了被释放的内存。借用规则阻止程序陷入这种状况。

枚举的成员都被定义为相同的枚举类型，所以当需要在 vector 中储存不同类型值时，我们可以定义并使用一个枚举！

如果在编写程序时不能确切无遗的知道运行时会储存进 vector 的所有类型，枚举技术就行不通了。相反，你可以使用 trait 对象。
有些类型的大小不可能在编译时就知道。为此你需要保存一个指向该类型的指针：Box类型正好适合这种情况。

```rust
enum SpreadsheetCell {
    Int(i32),
    Float(f64),
    Text(String),
}

let row = vec![
    SpreadsheetCell::Int(3),
    SpreadsheetCell::Text(String::from("blue")),
    SpreadsheetCell::Float(10.12),
];
```

* 哈希 map

`HashMap<K, V>` 类型储存了一个键类型 K 对应一个值类型 V 的映射。它通过一个哈希函数（hashing function）来实现映射，决定如何将键和值放入内存中。

哈希 map 可以用于需要任何类型作为键来寻找数据的情况，而不是像 vector 那样通过索引。例如，在一个游戏中，你可以将每个团队的分数记录到哈希 map 中，其中键是队伍的名字而值是每个队伍的分数。给出一个队名，就能得到他们的得分。

```rust
// 新建一个哈希 map
use std::collections::HashMap;

let mut scores = HashMap::new();

scores.insert(String::from("Blue"), 10);
scores.insert(String::from("Yellow"), 50);

// 使用 元组的 vector 的 collect 方法
let teams  = vec![String::from("Blue"), String::from("Yellow")];
let initial_scores = vec![10, 50];

let scores: HashMap<_, _> = teams.iter().zip(initial_scores.iter()).collect();

// 所有权
let field_name = String::from("Favorite color");
let field_value = String::from("Blue");

let mut map = HashMap::new();
map.insert(field_name, field_value);
// field_name and field_value are invalid at this point

// 获取值
let team_name = String::from("Blue");
let score = scores.get(&team_name);

for (key, value) in &scores {
    println!("{}: {}", key, value);
}

// 没有对应值时插入
scores.entry(String::from("Yellow")).or_insert(50);
scores.entry(String::from("Blue")).or_insert(50);

// 更新旧值
let text = "hello world wonderful world";

let mut map = HashMap::new();

for word in text.split_whitespace() {
    let count = map.entry(word).or_insert(0);
    *count += 1;
}

println!("{:?}", map);
```

## 函数

fn 表示“这是一个函数”，后面跟着名字，一对括号，括号里是参数，最后是一对大括号代表函数体。
函数参数与let声明非常相似：参数名加上冒号再加上参数类型。
与let不同，你必须为函数参数声明类型。

下面这个函数打印一个数字：

```rust
fn main() {
    print_number(5);
}

fn print_number(x: i32) {
    println!("x is: {}", x);
}
```

### 函数返回值

Rust 函数只能返回一个值，并且你需要在一个“箭头”`->` 后面声明类型。
下面这个函数给一个整数加一：

```rust
fn add_one(x: i32) -> i32 {
    x + 1
}
```

注意这里并没有一个分号。

### 表达式 VS 语句

Rust 主要是一个基于表达式的语言。只有两种语句，其它的一切都是表达式。
Rust 中有两种类型的语句：“声明语句”和“表达式语句”。其余的一切是表达式。
表达式返回一个值，而语句不是。代码块的值是其最后一个表达式的值。

* 声明语句

在 Rust 中，使用let引入一个绑定并不是一个表达式。下面的代码会产生一个编译时错误：

```rust
let x = (let y = 5); // expected identifier, found keyword `let`
```
编译器告诉我们这里它期望看到表达式的开头，而let只能开始一个语句，不是一个表达式。
不像其它语言中赋值语句返回它赋的值，在 Rust 中赋值的值是一个空的元组`()`：

```rust
let mut y = 5;
let x = (y = 6);  // `x` has the value `()`, not `6`.
```

* 表达式语句

它的目的是把任何表达式变为语句。在实践环境中，Rust 语法期望语句后跟其它语句。这意味着你用分号来分隔各个表达式。
函数调用是一个表达式。宏调用是一个表达式。我们用来创新建作用域的大括号`{}`，也是一个表达式：

```rust
fn main() {
    let x = 5;

    let y = {
        let x = 3;
        x + 1
    };

    println!("The value of y is: {}", y);
}
```

在下面代码中：

```rust
fn add_one(x: i32) -> i32 {
    x + 1
}
```

我们的函数声称它返回一个i32，但是如果在`x + 1`后带上个分号，它就变成了语句，最终会返回一个空的元组`()`。

### 提早返回（Early returns）

Rust 有这么一个关键字 `return`：

```rust
fn foo(x: i32) -> i32 {
    return x;

    // we never run this code!
    x + 1
}
```

### 发散函数（Diverging functions）

Rust 有些特殊的语法叫“发散函数”，这些函数并不返回，它的返回值类型设置为`!`代表发散：

```rust
fn diverges() -> ! {
    panic!("This function never returns!");
}
```

`panic!`是一个宏，会导致当前的执行线程崩溃并返回指定的信息。因为这个函数会崩溃，所以它不会返回，所以它拥有一个返回类型`!`，它代表“发散”。

### 函数指针

我们也可以创建指向函数的变量绑定：

```rust
let f: fn(i32) -> i32;
```

f 是一个指向一个获取i32作为参数并返回i32的函数的变量绑定。例如：

```rust
fn plus_one(i: i32) -> i32 {
    i + 1
}

// without type inference
let f: fn(i32) -> i32 = plus_one;

// with type inference
let f = plus_one;

let six = f(5);
```

## 注释

Rust 有两种需要你了解的注释格式：行注释（line comments）和文档注释（doc comments）。

- //    行注释
- ///   用来注释它后面的项，并且内建 Markdown 标记支持
- //!   用来注释包含它的项

## 控制流

Rust 代码中最常见的用来控制执行流的结构是if表达式和循环。

### if表达式

所有if表达式以if关键字开头，它后跟一个条件，条件必须是 bool。
因为if是一个表达式，我们可以在let语句的右侧使用它。if表达式的值是任何被选择的分支的最后一个表达式的值。一个没有 else 的 if 总是返回空元组`()`。
当在let语句的右侧使用它时，整个if表达式的值依赖哪个代码块被执行，这意味着 if的 每个分支的可能的返回值都必须是相同类型

```rust
fn main() {
    let number = 3;

    if number < 5 {
        println!("condition was true");
    } else {
        println!("condition was false");
    }

    let x = 5;

    if x == 5 {
        println!("x is five!");
    } else if x == 6 {
        println!("x is six!");
    } else {
        println!("x is not five or six :(");
    }

    let y = if x == 5 { 10 } else { 15 }; // y: i32
}
```

## 使用循环重复执行

Rust 有三种循环类型：loop、while和for。
可以使用 `break` 关键字来告诉程序何时停止执行循环。

* loop

loop关键字告诉 Rust 一遍又一遍的执行一段代码直到你明确要求停止。

```rust
fn main() {
    loop {
        println!("again!");
    }
}
```

* while 条件循环

```rust
fn main() {
    let mut number = 3;

    while number != 0  {
        println!("{}!", number);

        number = number - 1;
    }

    println!("LIFTOFF!!!");
}
```

* 使用for遍历集合

使用while结构来遍历一个元素集合，比如数组，会使程序更慢，因为编译器增加了运行时代码来对每次循环的每个元素进行条件检查。
可以使用for循环来对一个集合的每个元素执行一些代码，来作为一个更有效率替代。for循环看起来像这样：

```rust
fn main() {
    let a = [10, 20, 30, 40, 50];

    let mut index = 0;

    while index < 5 {
        println!("the value is: {}", a[index]);

        index = index + 1;
    }

    for element in a.iter() {
        println!("the value is: {}", element);
    }

    // 倒计时
    for number in (1..4).rev() {
        println!("{}!", number);
    }
    println!("LIFTOFF!!!");
}
```

## 所有权（ownership）

Rust 的核心功能（之一）是所有权（ownership）。虽然这个功能理解起来很直观，不过它对语言的其余部分有着更深层的含义。

所有程序都必须管理他们运行时使用计算机内存的方式。一些语言中使用垃圾回收在程序运行过程中来时刻寻找不再被使用的内存；在另一些语言中，程序员必须亲自分配和释放内存。Rust 则选择了第三种方式：内存被一个所有权系统管理，它拥有一系列的规则使编译器在编译时进行检查。任何所有权系统的功能都不会导致运行时开销。

### 栈（Stack）与堆（Heap）

栈和堆都是代码在运行时可供使用的内存部分，不过他们以不同的结构组成。栈以放入值的顺序存储并以相反顺序取出值。这也被称作后进先出（last in, first out）。想象一下一叠盘子：当增加更多盘子时，把他们放在盘子堆的顶部，当需要盘子时，也从顶部拿走。不能从中间也不能从底部增加或拿走盘子！增加数据叫做进栈（pushing onto the stack），而移出数据叫做出栈（popping off the stack）。

操作栈是非常快的，因为它访问数据的方式：永远也不需要寻找一个位置放入新数据或者取出数据因为这个位置总是在栈顶。另一个使得栈快速的性质是栈中的所有数据都必须是一个已知的固定的大小。

相反对于在编译时未知大小或大小可能变化的数据，可以把他们储存在堆上。堆是缺乏组织的：当向堆放入数据时，我们请求一定大小的空间。操作系统在堆的某处找到一块足够大的空位，把它标记为已使用，并返回给我们一个它位置的指针。这个过程称作在堆上分配内存（allocating on the heap），并且有时这个过程就简称为“分配”（allocating）。向栈中放入数据并不被认为是分配。因为指针是已知的固定大小的，我们可以将指针储存在栈上，不过当需要实际数据时，必须访问指针。

当调用一个函数，传递给函数的值（包括可能指向堆上数据的指针）和函数的局部变量被压入栈中。当函数结束时，这些值被移出栈。

记录何处的代码在使用堆上的什么数据，最小化堆上的冗余数据的数量以及清理堆上不再使用的数据以致不至于耗尽空间，这些所有的问题正是所有权系统要处理的。一旦理解了所有权，你就不需要经常考虑栈和堆了，不过理解如何管理堆内存可以帮助我们理解所有权为何存在以及为什么以这种方式工作。

### 所有权规则

首先，让我们看一下所有权的规则。请记住它们，我们将讲解一些它们的例子：

- 每一个值都被它的所有者（owner）变量拥有。
- 值在任意时刻只能被一个所有者拥有。
- 当所有者离开作用域，这个值将被丢弃。

### 变量作用域

作用域（scope）。作用域是一个项(原文:item)在程序中有效的范围。假如有一个这样的变量：

```rust
let s = "hello";
```

变量s绑定到了一个字符串字面值，这个字符串值是硬编码进我们程序代码中的。这个变量从声明的点开始直到当前作用域结束时都是有效的。

之前出现的数据类型都是储存在栈上的并且当离开作用域时被移出栈，不过我们需要寻找一个储存在堆上的数据来探索 Rust 如何知道该在何时清理数据的。

我们已经见过字符串字面值了，它被硬编码进程序里。字符串字面值是很方便的，不过他们并不总是适合所有需要使用文本的场景。
原因之一就是他们是不可变的。另一个原因是不是所有字符串的值都能在编写代码时就知道：例如，如果想要获取用户输入并储存该怎么办呢？
为此，Rust 有第二个字符串类型，String。这个类型储存在堆上所以储存在编译时未知大小的文本。

```rust
let s1 = String::from("hello");
let s2 = s1;
```

String 由三部分组成：

- 指向存放字符串内容内存的指针
- 长度，代表当前String的内容使用了多少字节的内存
- 容量，从操作系统总共获取了多少字节的内存

这一组数据储存在栈上。

#### 变量与数据交互：移动

当我们把 s1 赋值给 s2，String的数据被复制了，这意味着我们从栈上拷贝了它的指针、长度和容量。我们并没有复制堆上指针所指向的数据。

之前，我们提到过当变量离开作用域后 Rust 自动调用drop函数并清理变量的堆内存。此时两个数据指针指向了同一位置。
这就有了一个问题：当s2和s1离开作用域，他们都会尝试释放相同的内存，也就是 double free 的错误，也是之前提到过的内存安全性 bug 之一。

为了确保内存安全，与其尝试拷贝被分配的内存，Rust 则认为 s1 不再有效，因此不需要在s1离开作用域后清理任何东西。看看在s2被创建之后尝试使用s1会发生生么：

```rust
let s1 = String::from("hello");
let s2 = s1;

println!("{}", s1);
```

你编译的时候会得到一个类似如下的错误 `error[E0382]: use of moved value: `s1`，因为 Rust 禁止你使用无效的引用。

如果你在其他语言中听说过术语“浅拷贝”（“shallow copy”）和“深拷贝”（“deep copy”），那么拷贝指针、长度和容量而不拷贝数据可能听起来像浅拷贝。
不过因为 Rust 同时使第一个变量无效化了，这个操作被称为移动（move），而不是浅拷贝。上面的例子可以解读为s1被移动到了s2中。

> 这里还隐含了一个设计选择：Rust 永远也不会自动创建数据的“深拷贝”。因此，任何自动的复制可以被认为对运行时性能影响较小。

#### 变量与数据交互：克隆

如果我们确实需要深度复制String中堆上的数据，而不仅仅是栈上的数据，可以使用一个叫做clone的通用函数。

这是一个实际使用clone方法的例子：

```rust
let s1 = String::from("hello");
let s2 = s1.clone();

println!("s1 = {}, s2 = {}", s1, s2);
```

当出现clone调用时，你知道一些特有的代码被执行而且这些代码可能相当消耗资源。所以它作为一个可视化的标识代表了不同的行为。

#### 只在栈上的数据：拷贝

下面代码里的 x 依然有效

```rust
let x = 5;
let y = x;

println!("x = {}, y = {}", x, y);
```

他们似乎与我们刚刚学到的内容相抵触：没有调用clone，不过x依然有效且没有被移动到y中。
原因是像整型这样的在编译时已知大小的类型被整个储存在栈上，所以拷贝其实际的值是快速的。这意味着没有理由在创建变量y后使x无效。

Rust 有一个叫做 `Copy trait` 的特殊注解，可以用在类似整型这样的储存在栈上的类型。如果一个类型拥有Copy trait，一个旧的变量在（重新）赋值后仍然可用。
Rust 不允许自身或其任何部分实现了Drop trait 的类型使用Copy trait。如果我们对其值离开作用域时需要特殊处理的类型使用 Copy 注解，将会出现一个编译时错误。

* Copy 与 Clone

Rust 中有两个常见的 trait，Clone 和 Copy。

Copy 的全名是 `std::marker::Copy`。请大家注意 std::marker 这个模块里面的所有的 trait 都是特殊的 trait。
目前稳定的有四个，它们是 `Copy`、`Send`、`Sized`、`Sync`。

它们的特殊之处在于它们是跟编译器密切绑定的，实现（impl）这些 trait 对编译器的行为有重要影响。
它们的唯一任务是，给类型打一个“标记”，表明它符合某种约定，这些约定会影响编译器的静态检查以及代码生成。

如果一个类型实现了 Copy trait，意味着任何时候，我们可以通过简单的内存拷贝实现该类型的复制（等同于C语言的按位拷贝memcpy）。
一旦一个类型实现了 Copy trait，那么它在变量绑定、函数参数传递、函数返回值传递等场景下，它都是 copy 语义，而不再是默认的 move 语义。

* Copy 的实现条件

Rust规定，对于自定义类型，只有所有的成员都实现了 Copy trait，这个类型才有资格实现 Copy trait。

- 常见的数字类型、bool类型、共享借用指针&，都是具有 Copy 属性的类型。
- Box、Vec、可写借用指针&mut; 等类型都是不具备 Copy 属性的类型。
- 数组类型，如果它内部的元素类型是Copy，那么这个数组也是Copy类型。
- tuple类型，如果它的每一个元素都是Copy类型，那么这个tuple会自动实现Copy trait。
- struct和enum类型，不会自动实现Copy trait。而且只有当struct和enum内部每个元素都是Copy类型的时候，编译器才允许我们针对此类型实现Copy trait。

我们可以认为，Rust中只有 POD 类型（C++语言中的Plain Old Data）才有资格实现 Copy trait。
如果一个类型只包含 POD 数据类型的成员，没有指针类型的成员，并且没有自定义析构函数（实现Drop trait），那它就是POD类型。比如整数、浮点数、只包含POD类型的数组等，都属于POD类型。
而Box、 String、 Vec等，不能按 bit 位拷贝的类型，都不属于POD类型。但是，反过来讲，并不是所有的POD类型都应该实现 Copy trait。

一个通用的规则，任何简单标量值的组合可以是Copy的，任何不需要分配内存或类似形式资源的类型是Copy的，如下是一些Copy的类型：

- 所有整数类型，比如u32。
- 布尔类型，bool，它的值是true和false。
- 所有浮点数类型，比如f64。
- 元组，当且仅当其包含的类型也都是Copy的时候。(i32, i32)是Copy的

* Clone 的含义

Clone 的全名是 `std::clone::Clone`：

```rust
pub trait Clone : Sized {
    fn clone(&self;) -> Self;
    fn clone_from(&mut; self, source: &Self;) {
        *self = source.clone()
    }
}
```

它有两个关联方法 `clone_from` 和 `clone`，
其中 `clone_from` 有默认实现，它依赖于 `clone` 方法的实现。
`clone` 方法没有默认实现，需要我们手动实现。

`clone` 方法一般用于“基于语义的复制”操作。所以，它做什么事情，跟具体类型的作用息息相关。
对于 Box 类型，clone 就是执行的“深拷贝”，
而对于 Rc 类型，clone 做的事情就是把引用计数值加 1。

虽然说，Rust中 clone 方法一般是用来执行复制操作的，但是你如果在自定义的 clone 函数中做点什么别的工作编译器也没法禁止，你可以根据情况在 clone 函数中编写任意的逻辑。但是有一条规则需要注意：对于实现了 Copy 的类型，它的 clone 方法应该跟 Copy 语义相容，等同于按位拷贝。如果违反了这个规则，就是“未定义行为”。

### 所有权与函数

将值传递给函数在语言上与给变量赋值相似。向函数传递值可能会移动或者复制，就像赋值语句一样。

```rust
fn main() {
    let s = String::from("hello");  // s comes into scope.

    takes_ownership(s);             // s's value moves into the function...
                                    // ... and so is no longer valid here.
    let x = 5;                      // x comes into scope.

    makes_copy(x);                  // x would move into the function,
                                    // but i32 is Copy, so it’s okay to still
                                    // use x afterward.

} // Here, x goes out of scope, then s. But since s's value was moved, nothing
  // special happens.

fn takes_ownership(some_string: String) { // some_string comes into scope.
    println!("{}", some_string);
} // Here, some_string goes out of scope and `drop` is called. The backing
  // memory is freed.

fn makes_copy(some_integer: i32) { // some_integer comes into scope.
    println!("{}", some_integer);
} // Here, some_integer goes out of scope. Nothing special happens.
```

### 返回值与作用域

返回值也可以转移作用域。

```rust
fn main() {
    let s1 = gives_ownership();         // gives_ownership moves its return
                                        // value into s1.

    let s2 = String::from("hello");     // s2 comes into scope.

    let s3 = takes_and_gives_back(s2);  // s2 is moved into
                                        // takes_and_gives_back, which also
                                        // moves its return value into s3.
} // Here, s3 goes out of scope and is dropped. s2 goes out of scope but was
  // moved, so nothing happens. s1 goes out of scope and is dropped.

fn gives_ownership() -> String {             // gives_ownership will move its
                                             // return value into the function
                                             // that calls it.

    let some_string = String::from("hello"); // some_string comes into scope.

    some_string                              // some_string is returned and
                                             // moves out to the calling
                                             // function.
}

// takes_and_gives_back will take a String and return one.
fn takes_and_gives_back(a_string: String) -> String { // a_string comes into
                                                      // scope.

    a_string  // a_string is returned and moves out to the calling function.
}
```

* 使用元组返回多个值

变量的所有权总是遵循相同的模式：将值赋值给另一个变量时移动它。当持有堆中数据值的变量离开作用域时，其值将通过drop被清理掉，除非数据被移动为另一个变量所有。

在每一个函数中都获取并接着返回所有权是冗余乏味的。如果我们想要函数使用一个值但不获取所有权改怎么办呢？如果我们还要接着使用它的话，每次都传递出去再传回来就有点烦人了，另外我们也可能想要返回函数体产生的任何（不止一个）数据。

使用元组来返回多个值是可能的，像这样：

```rust
fn main() {
    let s1 = String::from("hello");

    let (s2, len) = calculate_length(s1);

    println!("The length of '{}' is {}.", s2, len);
}

fn calculate_length(s: String) -> (String, usize) {
    let length = s.len(); // len() returns the length of a String.

    (s, length)
}
```

## 引用与借用（References and Borrowing）

下面定义并使用一个 `calculate_length` 函数，它以一个对象的引用作为参数而不是获取值的所有权：

```rust
fn main() {
    let s1 = String::from("hello");

    let len = calculate_length(&s1);

    println!("The length of '{}' is {}.", s1, len);
}

fn calculate_length(s: &String) -> usize { // s is a reference to a String
    s.len()
} // Here, s goes out of scope. But because it does not have ownership of what
  // it refers to, nothing happens.
```
注意我们传递 `&s1` 给 `calculate_length`，同时在函数定义中，我们获取 `&String` 而不是 `String`。

这些 `&` 符号就是引用，他们允许你使用值但不获取它的所有权。
`&s1` 语法允许我们创建一个参考值s1的引用，但是并不拥有它。因为并不拥有这个值，当引用离开作用域它指向的值也不会被丢弃。

变量s有效的作用域与函数参数的作用域一样，不过当引用离开作用域后并不丢弃它指向的数据因为我们没有所有权。函数使用引用而不是实际值作为参数意味着无需返回值来交还所有权，因为就不曾拥有它。

我们将获取引用作为函数参数称为借用（borrowing）。正如现实生活中，如果一个人拥有某样东西，你可以从它哪里借来。当你使用完毕，必须还回去。

正如变量默认是不可变的，引用也一样。不允许修改引用的值。

### 可变引用（Mutable references）

首先，必须将 s 改为 mut。然后必须创建一个可变引用 `&mut s`和接受一个可变引用 `some_string: &mut String`。

```rust
fn main() {
	// 可变
    let mut s = String::from("hello");

    change(&mut s);
}

fn change(some_string: &mut String) {
    some_string.push_str(", world");
}
```

不过可变引用有一个很大的限制：在同一作用域中的同一数据有且只有一个可变引用。
我们也不能在拥有不可变引用的同时拥有可变引用。下面代码会失败：

```rust
let mut s = String::from("hello");

let r1 = &mut s;
let r2 = &mut s;
```

```rust
let mut s = String::from("hello");

let r1 = &s; // no problem
let r2 = &s; // no problem
let r3 = &mut s; // BIG PROBLEM
```

这个限制的好处是 Rust 可以在编译时就避免数据竞争（data races）。

数据竞争是一种特定类型的竞争状态，它可由这三个行为造成：

- 两个或更多指针同时访问同一数据。
- 至少有一个指针被写入。
- 没有同步数据访问的机制。

数据竞争会导致未定义行为，在运行时难以追踪，并且难以诊断和修复；Rust 避免了这种情况，它拒绝编译存在数据竞争的代码！

可以使用大括号来创建一个新的作用域来允许拥有多个可变引用，只是不能同时拥有：

```rust
let mut s = String::from("hello");
{
    let r1 = &mut s;
} // r1 goes out of scope here, so we can make a new reference with no problems.

let r2 = &mut s;
```

### 悬垂引用（dangling pointer）

所谓悬垂指针是其指向的内存可能已经被分配给其它持有者。相比之下，在 Rust 中编译器确保引用永远也不会变成悬垂状态：当我们拥有一些数据的引用，编译器确保数据不会在其引用之前离开作用域。

```rust
fn main() {
    let reference_to_nothing = dangle();
}

fn dangle() -> &String { // dangle returns a reference to a String

    let s = String::from("hello"); // s is a new String

    &s // we return a reference to the String, s
} // Here, s goes out of scope, and is dropped. Its memory goes away.
  // Danger!
```

s是在dangle创建的，当dangle的代码执行完毕后，s将被释放。

```rust
fn no_dangle() -> String {
    let s = String::from("hello");
    s
}
```

这样就可以没有任何错误的运行了。所有权被移动出去，所以没有值被释放掉。

Rust 中的每一个引用都有其生命周期，也就是引用保持有效的作用域。大部分时候生命周期是隐含并可以推断的，正如大部分时候类型也是可以推断的一样。类似于当因为有多种可能类型的时候必须注明类型，也会出现引用的生命周期以多种不同方式相关联的情况，所以 Rust 需要我们使用泛型生命周期参数来注明他们的关系，这样就能确保运行时实际使用的引用绝对是有效的。

编译器的这一部分叫做借用检查器（borrow checker），它比较作用域来确保所有的借用都是有效的：

```rust
{
    let r;         // -------+-- 'a
                   //        |
    {              //        |
        let x = 5; // -+-----+-- 'b
        r = &x;    //  |     |
    }              // -+     |
                   //        |
    println!("r: {}", r); // |
                   //        |
                   // -------+
}
```

## Slices

另一个没有所有权的数据类型是 slice。slice 允许你引用集合中一段连续的元素序列，而不用引用整个集合。

这里有一个小的编程问题：编写一个获取一个字符串并返回它在其中找到的第一个单词的函数。如果函数没有在字符串中找到一个空格，就意味着整个字符串是一个单词，所以整个字符串都应该返回。

让我们看看这个函数的签名：
```rust
fn first_word(s: &String) -> ?
```

`first_word` 这个函数有一个参数 `&String`。因为我们不需要所有权，所以这没有问题。不过应该返回什么呢？我们并没有一个真正获取部分字符串的办法。不过，我们可以返回单词结尾的索引。

```rust
fn first_word(s: &String) -> usize {
	// 因为需要一个元素一个元素的检查String中的值是否是空格，需要用as_bytes方法将String转化为字节数组
    let bytes = s.as_bytes();

	// 使用iter方法在字节数据上创建一个迭代器
	// iter方法返回集合中的每一个元素，而enumerate包装iter的结果并返回一个元组
	// 其中每一个元素是元组的一部分。返回元组的第一个元素是索引，第二个元素是集合中元素的引用。
    for (i, &item) in bytes.iter().enumerate() {
		// 通过字节的字面值来寻找代表空格的字节
        if item == b' ' {
            return i;
        }
    }

    s.len()
}
```

现在有了一个找到字符串中第一个单词结尾索引的方法了，不过这有一个问题。我们返回了单单一个usize，不过它只在&String的上下文中才是一个有意义的数字。换句话说，因为它是一个与String相分离的值，无法保证将来它仍然有效。

幸运的是，Rust 为这个问题提供了一个解决方案：字符串 slice。

### 字符串 slice（string slice）

字符串 slice 是 String 中一部分值的引用，它看起来像这样：

```rust
let s = String::from("hello world");

let hello = &s[0..5];
let world = &s[6..11];
```

这类似于获取整个String的引用不过带有额外的[0..5]部分。不同于整个String的引用，这是一个包含String内部的一个位置和所需元素数量的引用。

我们使用一个 `range [starting_index..ending_index]` 来创建 slice，不过 slice 的数据结构实际上储存了开始位置和 slice 的长度。所以就`let world = &s[6..11];`来说，`world` 将是一个包含指向s第 6 个字节的指针和长度值 5 的 slice。

对于 Rust 的.. range 语法，如果想要从第一个索引（0）开始，可以不写两个点号之前的值。由此类推，如果 slice 包含String的最后一个字节，也可以舍弃尾部的数字。

在记住所有这些知识后，让我们重写first_word来返回一个 slice。“字符串 slice”的签名写作`&str`：

```rust
fn first_word(s: &String) -> &str {
    let bytes = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return &s[0..i];
        }
    }

    &s[..]
}
```

现在我们有了一个不易混杂的直观的 API 了，因为编译器会确保指向String的引用保持有效。
使用 slice 后，当清除字符串时会抛出一个编译时错误：

```rust
fn main() {
    let mut s = String::from("hello world");

    let word = first_word(&s);

    s.clear(); // Error!
}
```

回忆一下借用规则，当拥有某值的不可变引用时。不能再获取一个可变引用。因为clear需要清空String，它尝试获取一个可变引用，它失败了。Rust 不仅使得我们的 API 简单易用，也在编译时就消除了一整个错误类型！

* 字符串字面值就是 slice

还记得我们讲到过字符串字面值被储存在二进制文件中吗。现在知道 slice 了，我们就可以正确的理解字符串字面值了：

```rust
let s = "Hello, world!";
```

这里s的类型是 `&str`：它是一个指向二进制程序特定位置的 slice。
这也就是为什么字符串字面值是不可变的；`&str`是一个不可变引用。

* 字符串 slice 作为参数

```rust
fn first_word(s: &str) -> &str {
```

* 其他 slice

就跟我们想要获取字符串的一部分那样，我们也会想要引用数组的一部分，而我们可以这样做：

```rust
let a = [1, 2, 3, 4, 5];
let slice = &a[1..3];
```

这个 slice 的类型是&[i32]。它跟以跟字符串 slice 一样的方式工作，通过储存第一个元素的引用和一个长度。

所有权、借用和 slice 这些概念是 Rust 何以在编译时保障内存安全的关键所在。Rust 像其他系统编程语言那样给予你对内存使用的控制，但拥有数据所有者在离开作用域后自动清除其数据的功能意味着你无须额外编写和调试相关的控制代码。

## 结构体（struct）

struct，是 structure 的缩写，是一个允许我们命名并将多个相关值包装进一个有意义的组合的自定义类型。
为了定义结构体，通过struct关键字并为整个结构体提供一个名字。结构体的名字需要描述它所组合的数据的意义。
接着，在大括号中，定义每一部分数据的名字，他们被称作字段（field），并定义字段类型。例如，下面代码展示了一个储存用户账号信息的结构体：

```rust
struct User {
    username: String,
    email: String,
    sign_in_count: u64,
    active: bool,
}
```

一旦定义了结构体后为了使用它，通过为每个字段指定具体值来创建这个结构体的实例：

```rust
let user1 = User {
    email: String::from("someone@example.com"),
    username: String::from("someusername123"),
    active: true,
    sign_in_count: 1,
};
```

为了从结构体中获取某个值，可以使用点号。如果我们只想要用户的邮箱地址，可以用 `user1.email`。

### 结构体数据的所有权

在 User 结构体的定义中，我们使用了自身拥有所有权的String类型而不是&str字符串 slice 类型。这是一个有意而为之的选择，因为我们想要这个结构体拥有它所有的数据，为此只要整个结构体是有效的话其数据也应该是有效的。

可以使结构体储存被其他对象拥有的数据的引用，不过这么做的话需要用上生命周期（lifetimes），这是第十章会讨论的一个 Rust 的功能。生命周期确保结构体引用的数据有效性跟结构体本身保持一致。

如果你尝试在结构体中储存一个引用而不指定生命周期，编译时将会报错 `error[E0106]: missing lifetime specifier`，比如这样：

```rust
struct User {
    username: &str,
    email: &str,
    sign_in_count: u64,
    active: bool,
}

fn main() {
    let user1 = User {
        email: "someone@example.com",
        username: "someusername123",
        active: true,
        sign_in_count: 1,
    };
}
```

我可以加上生命周期说明符：

```rust
struct User<'a> {
    username: &'a str,
    email: &'a str,
    sign_in_count: u64,
    active: bool,
}

fn main() {
    let user1 = User {
        email: "someone@example.com",
        username: "someusername123",
        active: true,
        sign_in_count: 1,
    };
}
```

### 通过衍生 trait 增加实用功能

编译下面的代码，会出现带有如下核心信息的错误：

`error[E0277]: the trait bound `Rectangle: std::fmt::Display` is not satisfied`

```rust
struct Rectangle {
    length: u32,
    width: u32,
}

fn main() {
    let rect1 = Rectangle { length: 50, width: 30 };

    println!("rect1 is {}", rect1);
}
```

`println!` `宏能处理很多类型的格式，不过由于 `{}` 使用称为 `Display` 的格式：直接提供给终端用户查看的输出。
目前为止见过的基本类型都默认实现了 `Display`，所以它就是向用户展示1或其他任何基本类型的唯一方式。
对于结构体，应该用来输出的格式是不明确的，因为这有更多显示的可能性：是否需要逗号？需要打印出结构体的{}吗？所有字段都应该显示吗？
因为这种不确定性，Rust 不尝试猜测我们的意图所以结构体并没有提供一个 Display 的实现。

我可以使用 `{:?}` 告诉 `println!` `我们想要使用叫做Debug的输出格式。Debug 是一个 trait，它允许我们在调试代码时以一种对开发者有帮助的方式打印出结构体。
不过我们必须为结构体显式选择这个功能，在结构体定义之前加上#[derive(Debug)]注解。

```rust
#[derive(Debug)]
struct Rectangle {
    length: u32,
    width: u32,
}

fn main() {
    let rect1 = Rectangle { length: 50, width: 30 };

    println!("rect1 is {:?}", rect1);
}
```

如果想要输出再好看和易读一点，这对更大的结构体会有帮助，可以将`println!``的字符串中的`{:?}`替换为`{:#?}`。

### 定义方法

让我们将获取一个 `Rectangle` 实例作为参数的 `area` 函数改写成一个定义于 `Rectangle` 结构体上的 `area` 方法：

```rust
#[derive(Debug)]
struct Rectangle {
    length: u32,
    width: u32,
}

impl Rectangle {
    fn area(&self) -> u32 {
        self.length * self.width
    }
}

fn main() {
    let rect1 = Rectangle { length: 50, width: 30 };

    println!(
        "The area of the rectangle is {} square pixels.",
        rect1.area()
    );
}
```

为了使函数定义于Rectangle的上下文中，我们开始了一个impl块（impl是 implementation 的缩写）。接着将函数移动到impl大括号中，并将签名中的第一个（在这里也是唯一一个）参数和函数体中其他地方的对应参数改成self。然后在main中将我们调用area方法并传递rect1作为参数的地方，改成使用方法语法在Rectangle实例上调用area方法。方法语法获取一个实例并加上一个点号后跟方法名、括号以及任何参数。

这里选择&self跟在函数版本中使用&Rectangle出于同样的理由：我们并不想获取所有权，只希望能够读取结构体中的数据，而不是写入。
