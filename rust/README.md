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
# 调试信息
rustc -g hello_world.rs
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

这里选择 `&self` 跟在函数版本中使用 `&Rectangle` 出于同样的理由：我们并不想获取所有权，只希望能够读取结构体中的数据，而不是写入。

Rust 并没有一个与`->`等效的运算符；相反，Rust 有一个叫自动引用和解引用（automatic referencing and dereferencing）的功能。
方法调用是 Rust 中少数几个拥有这种行为的地方。

在给出接收者和方法名的前提下，Rust 可以明确的计算出方法是仅仅读取（所以需要&self），做出修改（所以是&mut self）或者是获取所有权（所以是self）。
Rust 这种使得借用对方法接收者来说是隐式的做法是其所有权系统程序员友好性实现的一大部分。

### 带有更多参数的方法

方法可以在self后增加多个参数，而且这些参数就像函数中的参数一样工作。

```rust
impl Rectangle {
    fn area(&self) -> u32 {
        self.length * self.width
    }

    fn can_hold(&self, other: &Rectangle) -> bool {
        self.length > other.length && self.width > other.width
    }
}

fn main() {
    let rect1 = Rectangle { length: 50, width: 30 };
    let rect2 = Rectangle { length: 40, width: 10 };
    let rect3 = Rectangle { length: 45, width: 60 };

    println!("Can rect1 hold rect2? {}", rect1.can_hold(&rect2));
    println!("Can rect1 hold rect3? {}", rect1.can_hold(&rect3));
}
```

### 关联函数

impl块的另一个好用的功能是：允许在impl块中定义不以self作为参数的函数。这被称为关联函数（associated functions），因为他们与结构体相关联。
即便如此他们也是函数而不是方法，因为他们并不作用于一个结构体的实例。你已经使用过一个关联函数了：String::from。

我们可以提供一个关联函数，它接受一个维度参数并且用来作为长和宽，这样可以更轻松的创建一个正方形Rectangle而不必指定两次同样的值：

```rust
impl Rectangle {
    fn square(size: u32) -> Rectangle {
        Rectangle { length: size, width: size }
    }
}
```

使用结构体名和::语法来调用这个关联函数：比如 `let sq = Rectangle::square(3);` 这个方法位于结构体的命名空间中。

## 枚举和模式匹配

枚举，也被称作 enums。枚举允许你通过列举可能的值来定义一个类型。
Rust 的枚举与像F#、OCaml 和 Haskell这样的函数式编程语言中的代数数据类型（algebraic data types）最为相似。

可以通过在代码中定义一个IpAddrKind枚举来表现这个概念并列出可能的 IP 地址类型，V4和V6。这被称为枚举的成员（variants）：

```rust
enum IpAddrKind {
    V4,
    V6,
}
```

可以像这样创建IpAddrKind两个不同成员的实例：

```rust
let four = IpAddrKind::V4;
let six = IpAddrKind::V6;
```

我们先用结构体储存实际 IP 地址数据：
```rust
enum IpAddrKind {
    V4,
    V6,
}

struct IpAddr {
    kind: IpAddrKind,
    address: String,
}

let home = IpAddr {
    kind: IpAddrKind::V4,
    address: String::from("127.0.0.1"),
};

let loopback = IpAddr {
    kind: IpAddrKind::V6,
    address: String::from("::1"),
};
```

我们可以使用一种更简洁的方式来表达相同的概念，仅仅使用枚举并将数据直接放进每一个枚举成员而不是将枚举作为结构体的一部分。
IpAddr枚举的新定义表明了V4和V6成员都关联了String值：

```rust
enum IpAddr {
    V4(String),
    V6(String),
}

let home = IpAddr::V4(String::from("127.0.0.1"));

let loopback = IpAddr::V6(String::from("::1"));
```

我们直接将数据附加到枚举的每个成员上，这样就不需要一个额外的结构体了。
使用枚举而不是结构体还有另外一个优势：每个成员可以处理不同类型和数量的数据。

```rust
enum IpAddr {
    V4(u8, u8, u8, u8),
    V6(String),
}

let home = IpAddr::V4(127, 0, 0, 1);

let loopback = IpAddr::V6(String::from("::1"));
```

标准库里的定义：
```rust
struct Ipv4Addr {
    // details elided
}

struct Ipv6Addr {
    // details elided
}

enum IpAddr {
    V4(Ipv4Addr),
    V6(Ipv6Addr),
}
```

### Option枚举和其相对空值的优势

Rust 并没有空值，不过它确实拥有一个可以编码存在或不存在概念的枚举。
这个枚举是Option<T>，而且它定义于标准库中，如下:

```rust
enum Option<T> {
    Some(T),
    None,
}
```

空值（Null ）是一个值，它代表没有值。在有空值的语言中，变量总是这两种状态之一：空值和非空值。

空值的问题在于当你尝试像一个非空值那样使用一个空值，会出现某种形式的错误。因为空和非空的属性是无处不在的，非常容易出现这类错误。

然而，空值尝试表达的概念仍然是有意义的：空值是一个因为某种原因目前无效或缺失的值。

Option<T>是如此有用以至于它甚至被包含在了 prelude 之中：不需要显式导入它。另外，它的成员也是如此：可以不需要Option::前缀来直接使用Some和None。
即便如此Option<T>也仍是常规的枚举，Some(T)和None仍是Option<T>的成员。

## match控制流运算符

它允许我们将一个值与一系列的模式相比较并根据匹配的模式执行代码。模式可由字面值、变量、通配符和许多其他内容构成。
match的力量来源于模式的表现力以及编译器检查，它确保了所有可能的情况都得到处理（必须匹配所有可能的值，可以使用通配符 `_`）。

把match表达式想象成某种硬币分类器：硬币滑入有着不同大小孔洞的轨道，每一个硬币都会掉入符合它大小的孔洞。同样地，值也会检查match的每一个模式，并且在遇到第一个“符合”的模式时，值会进入相关联的代码块并在执行中被使用。

我们写一个函数：

```rust
enum Coin {
    Penny,
    Nickel,
    Dime,
    Quarter,
}

fn value_in_cents(coin: Coin) -> i32 {
    match coin {
        Coin::Penny => 1,
        Coin::Nickel => 5,
        Coin::Dime => 10,
        Coin::Quarter => 25,
    }
}

fn main() {
    println!("Penny is {:?}", value_in_cents(Coin::Penny)); // Prints "1".
    println!("Nickel is {:?}", value_in_cents(Coin::Nickel)); // Prints "5".
}
```

match的一个分支有两个部分：一个模式和一些代码。
第一个分支的模式是值 `Coin::Penny` 而之后的=>运算符将模式和将要运行的代码分开。
这里的代码就仅仅是值 1。每一个分支之间使用逗号分隔。

当match表达式执行时，它将结果值按顺序与每一个分支的模式相比较，如果模式匹配了这个值，这个模式相关联的代码将被执行。

如果想要在分支中运行多行代码，可以使用大括号。

```rust
fn value_in_cents(coin: Coin) -> i32 {
    match coin {
        Coin::Penny => {
            println!("Lucky penny!");
            1
        },
        Coin::Nickel => 5,
        Coin::Dime => 10,
        Coin::Quarter => 25,
    }
}
```

* 使用绑定的值

```rust
enum UsState {
    Alabama,
    Alaska,
    // ... etc
}

enum Coin {
    Penny,
    Nickel,
    Dime,
    Quarter(UsState),
}

fn value_in_cents(coin: Coin) -> i32 {
    match coin {
        Coin::Penny => 1,
        Coin::Nickel => 5,
        Coin::Dime => 10,
        Coin::Quarter(state) => {
            println!("State quarter from {:?}!", state);
            25
        },
    }
}

fn main() {
    println!("Quarter is {:?}", value_in_cents(Coin::Quarter(UsState::Alaska))); // Prints "25".
}
```

* 匹配 Option<T>

比如我们想要编写一个函数，它获取一个Option<i32>并且如果其中有一个值，将其加一。如果其中没有值，函数应该返回None值并不尝试执行任何操作。

```rust
fn plus_one(x: Option<i32>) -> Option<i32> {
    match x {
        None => None,
        Some(i) => Some(i + 1),
    }
}

let five = Some(5);
let six = plus_one(five);
let none = plus_one(None);
```
Rust 知道我们没有覆盖所有可能的情况甚至知道那些模式被忘记了！Rust 中的匹配是穷尽的（*exhaustive）：必须穷举到最后的可能性来使代码有效。特别的在这个Option<T>的例子中，Rust 防止我们忘记明确的处理None的情况，这使我们免于假设拥有一个实际上为空的值，这造成了之前提到过的价值亿万的错误。

* 通配符 `_`

Rust 也提供了一个模式用于不想列举出所有可能值的场景。

u8可以拥有 0 到 255 的有效的值，如果我们只关心 1、3、5 和 7 这几个值：

```rust
let some_u8_value = 0u8;
match some_u8_value {
    1 => println!("one"),
    3 => println!("three"),
    5 => println!("five"),
    7 => println!("seven"),
    _ => (),
}
```

match在只关心一个情况的场景中可能就有点啰嗦了。为此 Rust 提供了 `if let`。

### if let简单控制流

我们想要对`Some(3)`匹配进行操作不过不想处理任何其他Some<u8>值或None值。为了满足match表达式（穷尽性）的要求，必须在处理完这唯一的成员后加上_ => ()，这样也要增加很多样板代码。
不过我们可以使用if let这种更短的方式编写：

```rust
if let Some(3) = some_u8_value {
    println!("three");
}
```

## 模块

在你刚开始编写 Rust 程序时，代码可能仅仅位于main函数里。随着代码数量的增长，最终你会将功能移动到其他函数中，为了复用也为了更好的组织。通过将代码分隔成更小的块，每一个块代码自身就更易于理解。不过当你发现自己有太多的函数了该怎么办呢？Rust 有一个模块系统来处理编写可复用代码同时保持代码组织度的问题。

模块（module）是一个包含函数或类型定义的命名空间，你可以选择这些定义是能（公有）还是不能（私有）在其模块外可见。这是一个模块如何工作的概括：

- 使用mod关键字声明模块
- 默认所有内容都是私有的（包括模块自身）。可以使用pub关键字将其变成公有并在其命名空间外可见。
- use关键字允许引入模块、或模块中的定义到作用域中以便于引用他们。

### mod和文件系统

#### 创建一个项目

这个项目叫做communicator。Cargo 默认会创建一个库（crate） 除非指定其他项目类型（加--bin参数则项目将会是一个执行程序）：

```shell
cargo new communicator
cd communicator
```

Cargo 生成了 `src/lib.rs` 而不是 `src/main.rs`。在 `src/lib.rs` 中我们会看到：
```rust
#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
    }
}
```

#### 模块定义

Rust 中所有模块的定义以关键字 mod 开始。对于communicator网络库，首先要定义一个叫做network的模块，它包含一个叫做connect的函数定义。
在 src/lib.rs 文件的开头在测试代码的上面增加这些代码：
```rust
mod network { // 模块的名字 network
    fn connect() { // connect
    }
}
```
也可以在 src/lib.rs 文件中同时存在多个模块。例如，再拥有一个client模块，它也有一个叫做connect的函数：
```rust
mod network {
    fn connect() {
    }
}

mod client {
    fn connect() {
    }
}
```
也可以在 src/main.rs 中使用子模块：
```rust
mod network {
    fn connect() {
    }

    mod client {
        fn connect() {
        }
    }
}
```
现在我们有了 `network::connect和network::client::connect` 函数。

#### 将模块移动到其他文件

我们可以利用 Rust 的模块系统连同多个文件一起分解 Rust 项目，这样就不是所有的内容都落到 src/lib.rs 中了。作为例子，我们将下面代码分解：

```rust
mod client {
    fn connect() {
    }
}

mod network {
    fn connect() {
    }

    mod server {
        fn connect() {
        }
    }
}
```

让我们开始把client模块提取到另一个文件中。首先，将 src/lib.rs 中的client模块代码替换为如下：

```rust
mod client;

mod network {
    fn connect() {
    }

    mod server {
        fn connect() {
        }
    }
}
```

这里我们仍然定义了client模块，不过去掉了大括号和client模块中的定义并替换为一个分号，这使得 Rust 知道去其他地方寻找模块中定义的代码。

在 src/ 目录创建一个 `client.rs` 文件，接着打开它并输入如下内容，它是上一步client模块中被去掉的connect函数：
```rust
fn connect() {
}
```

注意这个文件中并不需要一个mod声明。

这时我们可以使用 `cargo build` 编译项目。

使用相同的方法将 network 模块提取到它自己的文件中。

如果想将 network 中的 server 再提取出来，那么我们的目录结构如下：

```text
├── src
│   ├── client.rs
│   ├── lib.rs
│   └── network
│       ├── mod.rs
│       └── server.rs
```

`network.rs` 改成了 `mod.rs`。

### 模块文件系统的规则

与文件系统相关的模块规则总结如下：

- 如果一个叫做foo的模块没有子模块，应该将foo的声明放入叫做 foo.rs 的文件中。
- 如果一个叫做foo的模块有子模块，应该将foo的声明放入叫做 foo/mod.rs 的文件中。

## 使用pub控制可见性

Rust 所有代码的默认状态是私有的：除了自己之外别人不允许使用这些代码。如果不在自己的项目中使用一个私有函数，因为程序自身是唯一允许使用这个函数的代码，Rust 会警告说函数未被使用。

我们在原有的项目创建 `src/main.rs`：
```rust
extern crate communicator;

fn main() {
    communicator::client::connect();
}
```

使用`extern crate`指令将`communicator`库 crate 引入到作用域，因为事实上我们的包包含两个 crate。Cargo 认为 src/main.rs 是一个二进制 crate 的根文件，与现存的以 src/lib.rs 为根文件的库 crate 相区分。
Rust 中的独立编译单位是crate：rustc每次编译一个crate，生成一个库或者可执行文件。
当编译单个`.rs`文件时，文件自身充当 crate。编译时可以通过开关 --lib 来创建共享库，或者没有用--lib开关，若文件中包含fn main函数，则生成可执行文件。

### 标记函数为公有

为了告诉 Rust 某项为公有，在想要标记为公有的项的声明开头加上pub关键字。修改 `src/lib.rs` 使client模块公有：

```rust
pub mod client;
mod network;
```

让我们修改 `src/client.rs` 将 `client::connect`也设为公有：

```rust
pub fn connect() {
}
```

* 私有性规则

总的来说，有如下项的可见性规则：

- 如果一个项是公有的，它能被任何父模块访问
- 如果一个项是私有的，它只能被当前模块或其子模块访问

编译项目时，对私有的函数会输出警告，我们可以通过关闭默认的 `#[warn(dead_code)]` 选项，来关闭警告，有下面几种方式：

* 在警告的对象上加上 `#[allow(dead_code)]` :
```rust
mod server {
	#[allow(dead_code)]
    pub fn connect() {
    }
}
```

* 或者在 main crate 加上 `#![allow(dead_code)]`

* 或者编译的时候加上选项：
```shell
rustc -A dead_code main.rs
# or
RUSTFLAGS="$RUSTFLAGS -A dead_code" cargo build
```

### 使用 use

use关键字只将指定的模块引入作用域；它并不会将其子模块也引入。

```rust
pub mod a {
    pub mod series {
        pub mod of {
            pub fn nested_modules() {}
        }
    }
}

use a::series::of::nested_modules;

fn main() {
    nested_modules();
}
```
`use a::series::of;`这一行的意思是每当想要引用of模块时，不用使用完整的a::series::of路径，可以直接使用of。
也可以将函数本身引入到作用域中，通过如下在use中指定函数的方式：

```rust
pub mod a {
    pub mod series {
        pub mod of {
            pub fn nested_modules() {}
        }
    }
}

use a::series::of::nested_modules;

fn main() {
    nested_modules();
}
```

因为枚举也像模块一样组成了某种命名空间，也可以使用use来导入枚举的成员。对于任何类型的use语句，如果从一个命名空间导入多个项，可以使用大括号和逗号来列举他们，像这样：
```rust
enum TrafficLight {
    Red,
    Yellow,
    Green,
}

use TrafficLight::{Red, Yellow};

fn main() {
    let red = Red;
    let yellow = Yellow;
    let green = TrafficLight::Green; // because we didn’t `use` TrafficLight::Green
}
```

* 使用通配符`*`的全局引用导入（glob）

为了一次导入某个命名空间的所有项，可以使用*语法。它会导入命名空间中所有可见的项。全局导入应该保守的使用：他们是方便的，但是也可能会引入多于你预期的内容从而导致命名冲突。
例如：

```rust
enum TrafficLight {
    Red,
    Yellow,
    Green,
}

use TrafficLight::*;

fn main() {
    let red = Red;
    let yellow = Yellow;
    let green = Green;
}
```

### 使用super访问父模块

正如我们已经知道的，当创建一个库 crate 时，Cargo 会生成一个tests模块。现在让我们来深入了解一下。在communicator项目中，打开 src/lib.rs。

```rust
pub mod client;

pub mod network;

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
    }
}
```

有一个叫做 tests 的模块紧邻其他模块，同时包含一个叫做it_works的函数。
这个模块是为了检验库中的代码而存在的，所以让我们尝试在`it_works`函数中调用`client::connect`函数，即便现在不准备测试任何功能。
在tests模块中，要么可以在开头使用双冒号来让 Rust 知道我们想要从根模块开始并列出整个路径：

```rust
::client::connect();
```
要么可以使用super在层级中获取当前模块的上一级模块：

```rust
super::client::connect();
```
最终代码如下：
```rust
#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        ::client::connect();
    }
}
```

使用cargo test命令运行测试：
```rust
cargo test
```

如果写很多 super 也会很烦人，我们可以使用 use：
```rust
#[cfg(test)]
mod tests {
    use super::client;

    #[test]
    fn it_works() {
        client::connect();
    }
}
```

## 错误处理

Rust 将错误组合成两个主要类别：可恢复错误（recoverable）和不可恢复错误（unrecoverable）。可恢复错误通常代表向用户报告错误和重试操作是合理的情况，比如未找到文件。不可恢复错误通常是 bug 的同义词，比如尝试访问超过数组结尾的位置。

Rust 并没有异常。可恢复错误有`Result<T, E>`值，不可恢复错误，例如`panic!`，它在遇到不可恢复错误时停止程序执行。

### panic!与不可恢复的错误

当出现`panic!`时，程序默认会开始展开（unwinding），这意味着 Rust 会回溯栈并清理它遇到的每一个函数的数据，不过这个回溯并清理的过程有很多工作。
另一种选择是直接终止（abort），这会不清理数据就退出程序。那么程序所使用的内存需要由操作系统来清理。
如果你需要项目的最终二进制文件越小越好，可以由 panic 时展开切换为终止，通过在 Cargo.toml 的`[profile]`部分增加`panic = 'abort'`。
例如，如果你想要在发布模式中 panic 时直接终止：
```toml
[profile.release]
panic = 'abort'
```

我们主动调用下`panic!`：
```rust
fn main() {
    panic!("crash and burn");
}
```

直接运行时候可以看到错误所在文件和行数，如下代码，就要使用回溯功能，才能看到错误所在的文件和行数：
```rust
fn main() {
    let v = vec![1, 2, 3];

    v[100];
}
```
编译运行：
```shell
rustc -g panic.ts
RUST_BACKTRACE=1 ./panic
```

### Result与可恢复的错误

大部分错误并没有严重到需要程序完全停止执行。有时，一个函数会因为一个容易理解并做出反映的原因失败。例如，如果尝试打开一个文件不过由于文件并不存在而操作就失败，这是我们可能想要创建这个文件而不是终止进程。

Result枚举，它定义有如下两个成员，Ok和Err：

```rust
enum Result<T, E> {
    Ok(T),
    Err(E),
}
```

因为Result有这些泛型类型参数，我们可以将Result类型和标准库中为其定义的函数用于很多不同的场景，这些情况中需要返回的成功值和失败值可能会各不相同。

让我们调用一个返回Result的函数，因为它可能会失败：
```rust
use std::fs::File;

fn main() {
    let f = File::open("hello.txt");
}
```

`File::open` 函数的返回值类型是 `Result<T, E>`。这里泛型参数T放入了成功值的类型`std::fs::File`，它是一个文件句柄。E被用在失败值上其类型是`std::io::Error`。

根据File::open返回值进行不同处理的逻辑：
```rust
use std::fs::File;

fn main() {
    let f = File::open("hello.txt");

    let f = match f {
        Ok(file) => file,
        Err(error) => {
            panic!("There was a problem opening the file: {:?}", error)
        },
    };
}
```
注意与Option枚举一样，Result枚举和其成员也被导入到了 prelude 中，所以就不需要在match分支中的Ok和Err之前指定Result::。

这里我们告诉 Rust 当结果是Ok，返回Ok成员中的file值，然后将这个文件句柄赋值给变量f。match之后，我们可以利用这个文件句柄来进行读写。

我们还可以增加macth guard：
```rust
use std::fs::File;
use std::io::ErrorKind;

fn main() {
    let f = File::open("hello.txt");

    let f = match f {
        Ok(file) => file,
        Err(ref error) if error.kind() == ErrorKind::NotFound => {
            match File::create("hello.txt") {
                Ok(fc) => fc,
                Err(e) => {
                    panic!(
                        "Tried to create file but there was a problem: {:?}",
                        e
                    )
                },
            }
        },
        Err(error) => {
            panic!(
                "There was a problem opening the file: {:?}",
                error
            )
        },
    };
}
```

模式中的ref是必须的，这样error就不会被移动到 guard 条件中而只是仅仅引用它。简而言之，在模式的上下文中，`&`匹配一个引用并返回它的值，而`ref`匹配一个值并返回一个引用。

### 失败时 panic 的捷径：unwrap和expect

`Result<T, E>` 类型定义了很多辅助方法来处理各种情况。其中之一叫做`unwrap`，如果`Result`值是成员`Ok`，`unwrap`会返回`Ok`中的值。如果`Result`是成员`Err`，`unwrap`会为我们调用`panic!`。
```rust
use std::fs::File;

fn main() {
    let f = File::open("hello.txt").unwrap();
}
```

还有另一个类似于unwrap的方法它还允许我们设置`panic!``的错误信息：`expect`：

```rust
use std::fs::File;

fn main() {
    let f = File::open("hello.txt").expect("Failed to open hello.txt");
}
```

### 传播错误

当编写一个其实现会调用一些可能会失败的操作的函数时，除了在这个函数中处理错误外，还可以选择让调用者知道这个错误并决定该如何处理。这被称为传播（propagating）错误，这样能更好的控制代码调用，因为比起你代码所拥有的上下文，调用者可能拥有更多信息或逻辑来决定应该如何处理错误。

```rust
use std::io;
use std::io::Read;
use std::fs::File;

fn read_username_from_file() -> Result<String, io::Error> {
    let f = File::open("hello.txt");

    let mut f = match f {
        Ok(file) => file,
        Err(e) => return Err(e),
    };

    let mut s = String::new();

    match f.read_to_string(&mut s) {
        Ok(_) => Ok(s),
        Err(e) => Err(e),
    }
}
```
这种传播错误的模式在 Rust 是如此的常见，以至于有一个更简便的专用语法：`?`。
```rust
use std::io;
use std::io::Read;
use std::fs::File;

fn read_username_from_file() -> Result<String, io::Error> {
    let mut f = File::open("hello.txt")?;
    let mut s = String::new();
    f.read_to_string(&mut s)?;
    Ok(s)
}
```
我们甚至可以在`?`之后直接使用链式方法调用来进一步缩短代码：
```rust
use std::io;
use std::io::Read;
use std::fs::File;

fn read_username_from_file() -> Result<String, io::Error> {
    let mut s = String::new();

    File::open("hello.txt")?.read_to_string(&mut s)?;

    Ok(s)
}
```
`?`只能被用于返回Result的函数

## 泛型、trait 和生命周期

在不同的场景使用不同的方式泛型也可以利用相同的步骤来减少重复代码。与函数体中现在作用于一个抽象的list而不是具体值一样，使用泛型的代码也作用于抽象类型。支持泛型背后的概念与你已经了解的支持函数的概念是一样的，不过是实现方式不同。

```rust
fn largest(list: &[i32]) -> i32 {
    let mut largest = list[0];

    for &item in list.iter() {
        if item > largest {
            largest = item;
        }
    }

    largest
}

fn main() {
    let numbers = vec![34, 50, 25, 100, 65];

    let result = largest(&numbers);
    println!("The largest number is {}", result);

    let numbers = vec![102, 34, 6000, 89, 54, 2, 43, 8];

    let result = largest(&numbers);
    println!("The largest number is {}", result);
}
```

如果我们有两个函数，一个寻找一个i32值的 slice 中的最大项而另一个寻找char值的 slice 中的最大项该怎么办？该如何消除重复呢？

## 泛型

泛型用于通常我们放置类型的位置，比如函数签名或结构体，允许我们创建可以代替许多具体数据类型的结构体定义。

### 在函数中使用泛型

定义函数时可以在函数签名的参数数据类型和返回值中使用泛型。以这种方式编写的代码将更灵活并能向函数调用者提供更多功能，同时不引入重复代码。

```rust
fn largest_i32(list: &[i32]) -> i32 {
    let mut largest = list[0];

    for &item in list.iter() {
        if item > largest {
            largest = item;
        }
    }

    largest
}

fn largest_char(list: &[char]) -> char {
    let mut largest = list[0];

    for &item in list.iter() {
        if item > largest {
            largest = item;
        }
    }

    largest
}

fn main() {
    let numbers = vec![34, 50, 25, 100, 65];

    let result = largest_i32(&numbers);
    println!("The largest number is {}", result);

    let chars = vec!['y', 'm', 'a', 'q'];

    let result = largest_char(&chars);
    println!("The largest char is {}", result);
}
```

这里largest_i32和largest_char有着完全相同的函数体，所以能够将这两个函数变成一个来减少重复就太好了。所幸通过引入一个泛型参数就能实现！

我们将要定义的泛型版本的largest函数的签名看起来像这样：
```rust
fn largest<T>(list: &[T]) -> T {
```
这可以理解为：函数`largest`有泛型类型`T`。它有一个参数`list`，它的类型是一个`T`值的 `slice`。`largest`函数将会返回一个与`T`相同类型的值。

```rust
fn largest<T>(list: &[T]) -> T {
    let mut largest = list[0];

    for &item in list.iter() {
        // 注意这里，泛型是不能直接比较的
        if item > largest {
            largest = item;
        }
    }

    largest
}

fn main() {
    let numbers = vec![34, 50, 25, 100, 65];

    let result = largest(&numbers);
    println!("The largest number is {}", result);

    let chars = vec!['y', 'm', 'a', 'q'];

    let result = largest(&chars);
    println!("The largest char is {}", result);
}
```
如果现在就尝试编译这些代码，会出现如下错误：
```text
error[E0369]: binary operation `>` cannot be applied to type `T`
  |
5 |         if item > largest {
  |            ^^^^
  |
note: an implementation of `std::cmp::PartialOrd` might be missing for `T`
```

标准库中定义的 `std::cmp::PartialOrd trait` 可以实现类型的排序功能。

```shell
use std::cmp::PartialOrd;

fn largest<T: PartialOrd + Copy>(list: &[T]) -> T {
    let mut largest = list[0];

    for &item in list.iter() {
        if item > largest {
            largest = item;
        }
    }

    largest
}

fn main() {
    let numbers = vec![34, 50, 25, 100, 65];

    let result = largest(&numbers);
    println!("The largest number is {}", result);

    let chars = vec!['y', 'm', 'a', 'q'];

    let result = largest(&chars);
    println!("The largest char is {}", result);
}
```

# 结构体定义中的泛型

同样也可以使用<>语法来定义拥有一个或多个泛型参数类型字段的结构体。
```rust
struct Point<T> {
    x: T,
    y: T,
}

fn main() {
    let integer = Point { x: 5, y: 10 };
    let float = Point { x: 1.0, y: 4.0 };
}
```

如果想要一个x和y可以有不同类型且仍然是泛型的Point结构体，我们可以使用多个泛型类型参数。

```rust
struct Point<T, U> {
    x: T,
    y: U,
}

fn main() {
    let both_integer = Point { x: 5, y: 10 };
    let both_float = Point { x: 1.0, y: 4.0 };
    let integer_and_float = Point { x: 5, y: 4.0 };
}
```

### 枚举定义中的泛型数据类型

类似于结构体，枚举也可以在其成员中存放泛型数据类型。

```rust
enum Option<T> {
    Some(T),
    None,
}
```
换句话说Option<T>是一个拥有泛型T的枚举。它有两个成员：Some，它存放了一个类型T的值，和不存在任何值的None。

枚举也可以拥有多个泛型类型。使用过的Result枚举定义就是一个这样的例子：

```rust
enum Result<T, E> {
    Ok(T),
    Err(E),
}
```

### 为带有泛型数据的枚举实现方法

在Point<T>上定义了一个叫做x的方法来返回字段x中数据的引用：
```rust
struct Point<T> {
    x: T,
    y: T,
}

impl<T> Point<T> {
    fn x(&self) -> &T {
        &self.x
    }
}

fn main() {
    let p = Point { x: 5, y: 10 };

    println!("p.x = {}", p.x());
}
```

注意必须在impl后面声明T，这样就可以在Point<T>上实现的方法中使用它了。
结构体定义中的泛型类型参数并不总是与结构体方法签名中使用的泛型是同一类型。

```rust
struct Point<T, U> {
    x: T,
    y: U,
}

impl<T, U> Point<T, U> {
    fn mixup<V, W>(self, other: Point<V, W>) -> Point<T, W> {
        Point {
            x: self.x,
            y: other.y,
        }
    }
}

fn main() {
    let p1 = Point { x: 5, y: 10.4 };
    let p2 = Point { x: "Hello", y: 'c'};

    let p3 = p1.mixup(p2);

    println!("p3.x = {}, p3.y = {}", p3.x, p3.y);
}
```

注意泛型参数T和U声明于impl之后，因为他们于结构体定义相对应。而泛型参数V和W声明于fn mixup之后，因为他们只是相对于方法本身的。

### 泛型代码的性能

在阅读本部分的内容的同时你可能会好奇使用泛型类型参数是否会有运行时消耗。好消息是：Rust 实现泛型泛型的方式意味着你的代码使用泛型类型参数相比指定具体类型并没有任何速度上的损失。

Rust 通过在编译时进行泛型代码的单态化（monomorphization）来保证效率。单态化是一个将泛型代码转变为实际放入的具体类型的特定代码的过程。

编译器生成的单态化版本的代码看起来像这样，并包含将泛型Option替换为编译器创建的具体定义后的用例代码：

```rust
enum Option_i32 {
    Some(i32),
    None,
}

enum Option_f64 {
    Some(f64),
    None,
}

fn main() {
    let integer = Option_i32::Some(5);
    let float = Option_f64::Some(5.0);
}
```

## trait：定义共享的行为

trait 允许我们进行另一种抽象：他们让我们可以抽象类型所通用的行为。trait 告诉 Rust 编译器某个特定类型拥有可能与其他类型共享的功能。

> 注意：trait 类似于其他语言中的常被称为接口（interfaces）的功能，虽然有一些不同。

### 定义 trait

一个类型的行为由其可供调用的方法构成。如果可以对不同类型调用相同的方法的话，这些类型就可以共享相同的行为了。
trait 定义是一种将方法签名组合起来的方法，目的是定义一个实现某些目的所必需的行为的集合。

下面的代码展示了一个表现这个概念的Summarizable trait 的定义：

```rust
pub trait Summarizable {
    fn summary(&self) -> String;
}
```

使用trait关键字来定义一个 trait，后面是 trait 的名字，在这个例子中是Summarizable。在大括号中声明描述实现这个 trait 的类型所需要的行为的方法签名，在这个例子中是是fn summary(&self) -> String。在方法签名后跟分号而不是在大括号中提供其实现。接着每一个实现这个 trait 的类型都需要提供其自定义行为的方法体，编译器也会确保任何实现Summarizable trait 的类型都拥有与这个签名的定义完全一致的summary方法。

trait 体中可以有多个方法，一行一个方法签名且都以分号结尾。

### 为类型实现 trait

现在我们定义了Summarizable trait，接着就可以在多媒体聚合库中需要拥有这个行为的类型上实现它了。

```rust
pub struct NewsArticle {
    pub headline: String,
    pub location: String,
    pub author: String,
    pub content: String,
}

impl Summarizable for NewsArticle {
    fn summary(&self) -> String {
        format!("{}, by {} ({})", self.headline, self.author, self.location)
    }
}

pub struct Tweet {
    pub username: String,
    pub content: String,
    pub reply: bool,
    pub retweet: bool,
}

impl Summarizable for Tweet {
    fn summary(&self) -> String {
        format!("{}: {}", self.username, self.content)
    }
}
```

与为类型实现方法区别在于impl关键字之后，我们提供需要实现 trait 的名称，接着是for和需要实现 trait 的类型的名称。

```rust
let tweet = Tweet {
    username: String::from("horse_ebooks"),
    content: String::from("of course, as you probably already know, people"),
    reply: false,
    retweet: false,
};

println!("1 new tweet: {}", tweet.summary());
```

trait 实现的一个需要注意的限制是：只能在 trait 或对应类型位于我们 crate 本地的时候为其实现 trait。
没有这条规则的话，两个 crate 可以分别对相同类型是实现相同的 trait，因而这两个实现会相互冲突：Rust 将无从得知应该使用哪一个。
因为 Rust 强制执行 orphan rule，其他人编写的代码不会破坏你代码，反之亦是如此。

### 默认实现

有时直接在 trait 中的实现某些或全部默认的行为，而不是在每个类型的实现定义的行为。
为Summarize trait 的summary方法指定一个默认的字符串值：
```rust
    fn summary(&self) -> String {
        String::from("(Read more...)")
    }
}
```

现在想要对 `NewsArticle` 实例使用这个默认实现，而不是定义一个自己的实现，则可以指定一个空的impl块：

```rust
impl Summarizable for NewsArticle {}
```
重载一个默认实现的语法与实现没有默认实现的 trait 方法时完全一样的。

> 注意在重载过的实现中调用默认实现是不可能的。

### trait bounds

可以对泛型类型参数使用 trait。

我们可以定义一个函数notify来调用summary方法，它拥有一个泛型类型T的参数item。
为了能够在item上调用summary而不出现错误，我们可以在T上使用 trait bounds 来指定item必须是实现了Summarizable trait 的类型：

```rust
pub fn notify<T: Summarizable>(item: T) {
    println!("Breaking news! {}", item.summary());
}
```
可以通过+来为泛型指定多个 `trait bounds`。如果我们需要能够在函数中使用T类型的显示格式的同时也能使用summary方法，则可以使用 `trait bounds T: Summarizable + Display`。这意味着T可以是任何实现了`Summarizable`和`Display`的类型。

对于拥有多个泛型类型参数的函数，每一个泛型都可以有其自己的 trait bounds，这样写：

```rust,ignore
fn some_function<T: Display + Clone, U: Clone + Debug>(t: T, u: U) -> i32 {
```
我们也可以使用where从句：
```rust,ignore
fn some_function<T, U>(t: T, u: U) -> i32
    where T: Display + Clone,
          U: Clone + Debug
{
```

## 测试

当使用 Cargo 新建一个库项目时，它会自动为我们生成一个测试模块和一个测试函数。这有助于我们开始编写测试，因为这样每次开始新项目时不必去查找测试函数的具体结构和语法了。同时可以额外增加任意多的测试函数以及测试模块！

让我们创建一个新的库项目adder：

```shell
cargo new adder
cd adder
```
adder 库中`src/lib.rs`的内容应该看起来像这样：

```rust
#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
    }
}
```
注意fn行之前的`#[test]`：这个属性表明这是一个测试函数，这样测试执行者就知道将其作为测试处理。也可以在tests模块中拥有非测试的函数来帮助我们建立通用场景或进行常见操作，所以需要使用`#[test]`属性标明哪些函数是测试。

`cargo test`命令会运行项目中所有的测试。

```text
$ cargo test
   Compiling adder v0.1.0 (file:///projects/adder)
    Finished debug [unoptimized + debuginfo] target(s) in 0.22 secs
     Running target/debug/deps/adder-ce99bcc2479f4607

running 1 test
test tests::it_works ... ok

test result: ok. 1 passed; 0 failed; 0 ignored; 0 measured

   Doc-tests adder

running 0 tests

test result: ok. 0 passed; 0 failed; 0 ignored; 0 measured
```

Cargo 编译并运行了测试。在`Compiling`、`Finished`和`Running`这几行之后，可以看到`running 1 test`这一行。
下一行显示了生成的测试函数的名称，它是`it_works`，以及测试的运行结果，ok。
接着可以看到全体测试运行结果的总结：`test result: ok`.意味着所有测试都通过了。
`1 passed; 0 failed`表示通过或失败的测试数量。

给it_works函数起个不同的名字，比如exploration，像这样：

```rust
#[cfg(test)]
mod tests {
    #[test]
    fn exploration() {
    }
}
```
并再次运行cargo test。现在输出中将出现`exploration`而不是`it_works`。
让我们增加另一个测试，不过这一次是一个会失败的测试！当测试函数中出现 panic 时测试就失败了。
```rust
#[cfg(test)]
mod tests {
    #[test]
    fn exploration() {
    }

    #[test]
    fn another() {
        panic!("Make this test fail");
    }
}
```

### 使用assert!宏来检查结果

`assert!`宏由标准库提供，在希望确保测试中一些条件为true时非常有用。需要向`assert!`宏提供一个计算为布尔值的参数。如果值是`true`，`assert!`什么也不做同时测试会通过。如果值为`false`，`assert!`调用`panic!`宏，这会导致测试失败。这是一个帮助我们检查代码是否以期望的方式运行的宏。

```rust
#[derive(Debug)]
pub struct Rectangle {
    length: u32,
    width: u32,
}

impl Rectangle {
    pub fn can_hold(&self, other: &Rectangle) -> bool {
        self.length > other.length && self.width > other.width
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn larger_can_hold_smaller() {
        let larger = Rectangle { length: 8, width: 7 };
        let smaller = Rectangle { length: 5, width: 1 };

        assert!(larger.can_hold(&smaller));
    }

    #[test]
    fn smaller_can_hold_larger() {
        let larger = Rectangle { length: 8, width: 7 };
        let smaller = Rectangle { length: 5, width: 1 };

        assert!(!smaller.can_hold(&larger));
    }
}
```

### 使用assert_eq!和assert_ne!宏来测试相等

```rust
pub fn add_two(a: i32) -> i32 {
    a + 2
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_adds_two() {
        assert_eq!(4, add_two(2));
    }
}
```

### 自定义错误信息

也可以向assert!、assert_eq!和assert_ne!宏传递一个可选的参数来增加用于打印的自定义错误信息。

```rust
#[test]
fn greeting_contains_name() {
    let result = greeting("Carol");
    assert!(
        result.contains("Carol"),
        "Greeting did not contain name, value was `{}`", result
    );
}
```

### 使用should_panic检查 panic

```rust
struct Guess {
    value: u32,
}

impl Guess {
    pub fn new(value: u32) -> Guess {
        if value < 1 || value > 100 {
            panic!("Guess value must be between 1 and 100, got {}.", value);
        }

        Guess {
            value: value,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    #[should_panic]
    fn greater_than_100() {
        Guess::new(200);
    }
}
```
为了使should_panic测试更精确，可以给should_panic属性增加一个可选的expected参数。测试工具会确保错误信息中包含其提供的文本。

```rust
struct Guess {
    value: u32,
}

impl Guess {
    pub fn new(value: u32) -> Guess {
        if value < 1 {
            panic!("Guess value must be greater than or equal to 1, got {}.",
                   value);
        } else if value > 100 {
            panic!("Guess value must be less than or equal to 100, got {}.",
                   value);
        }

        Guess {
            value: value,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    #[should_panic(expected = "Guess value must be less than or equal to 100")]
    fn greater_than_100() {
        Guess::new(200);
    }
}
```

## 运行测试

`cargo test`在测试模式下编译代码并运行生成的测试二进制文件。这里有一些选项可以用来改变cargo test的默认行为。
例如，cargo test生成的二进制文件的默认行为是并行的运行所有测试，并捕获测试运行过程中产生的输出避免他们被显示出来使得阅读测试结果相关的内容变得更容易。
可以指定命令行参数来改变这些默认行为。

运行`cargo test --help`会告诉你`cargo test`的相关参数。
运行`cargo test -- --help`则会告诉你位于分隔符--之后的相关参数。

### 并行或连续的运行测试

当运行多个测试时，他们默认使用线程来并行的运行。因为测试是在同时运行的，你应该小心测试不能相互依赖或任何共享状态，包括类似于当前工作目录或者环境变量这样的共享环境。

* 控制使用线程的数量

如果你不希望测试并行运行，或者想要更加精确的控制使用线程的数量，可以传递`--test-threads`参数和希望使用线程的数量给测试二进制文件。例如：

```shell
cargo test -- --test-threads=1
```
* 显示测试输出

如果你希望也能看到通过的测试中打印的值，捕获输出的行为可以通过`--nocapture`参数来禁用：

```shell
cargo test -- --nocapture
```

* 通过名称来运行测试的子集

可以向cargo test传递希望运行的测试的（部分）名称作为参数来选择运行哪些测试。

```rust
pub fn add_two(a: i32) -> i32 {
    a + 2
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn add_two_and_two() {
        assert_eq!(4, add_two(2));
    }

    #[test]
    fn add_three_and_two() {
        assert_eq!(5, add_two(3));
    }

    #[test]
    fn one_hundred() {
        assert_eq!(102, add_two(100));
    }
}
```

只测试 `one_hundred`：
```shell
cargo test one_hundred
```

过滤运行多个测试，会运行所有前缀包含add的测试：
```shell
cargo test add
```

* 忽略某些测试，除非指定

这里 `expensive_test` 测试设置了忽略：
```rust
#[test]
fn it_works() {
    assert!(true);
}

#[test]
#[ignore]
fn expensive_test() {
    // code that takes an hour to run
}
```

### 集成测试

为了编写集成测试，需要在项目根目录创建一个 tests 目录，与 src 同级。Cargo 知道如何去寻找这个目录中的集成测试文件。接着可以随意在这个文件夹中创建任意多的测试文件，Cargo 会将每一个文件当作单独的 crate 来编译。

创建一个 tests 目录，新建一个文件 tests/integration_test.rs：
```rust
extern crate adder;

#[test]
fn it_adds_two() {
    assert_eq!(4, adder::add_two(2));
}
```

并不需要将 tests/integration_test.rs 中的任何代码标注为#[cfg(test)]。Cargo 对tests文件夹特殊处理并只会在运行cargo test时编译这个目录中的文件。
tests 目录中的子目录不会被作为单独的 crate 编译或作为一部分出现在测试输出中。

## 构建一个小巧的 grep

Rust 的运行速度、安全性、“单二进制文件”输出和跨平台支持使其成为创建命令行程序的绝佳选择，所以我们的项目将创建一个我们自己版本的经典命令行工具：grep。grep 是“Globally search a Regular Expression and Print.”的首字母缩写。grep最简单的使用场景是使用如下步骤在特定文件中搜索指定字符串：

- 获取一个文件和一个字符串作为参数。
- 读取文件
- 寻找文件中包含字符串参数的行
- 打印出这些行

```shell
cargo new --bin greprs
cd greprs
```

### 接受命令行参数

第一个任务是让greprs能够接受两个命令行参数：文件名和要搜索的字符串。
为了能够获取传递给程序的命令行参数的值，我们需要调用一个 Rust 标准库提供的函数：`std::env::args`。
这个函数返回一个传递给程序的命令行参数的迭代器（iterator）。

- 迭代器生成一系列的值。
- 在迭代器上调用collect方法可以将其生成的元素转换为一个 vector。

读取任何传递给greprs的命令行参数并将其收集到一个 vector 中：
```rust
use std::env;

fn main() {
	// collect 可以被用来创建很多类型的集合，这里显式注明的args类型来指定我们需要一个字符串 vector。
    let args: Vec<String> = env::args().collect();
    println!("{:?}", args);
}
```

> 注意：std::env::args在其任何参数包含无效 Unicode 字符时会 panic。如果你需要接受包含无效 Unicode 字符的参数，使用std::env::args_os代替。这个函数返回OsString值而不是String值。OsString 值每个平台都不一样而且比String值处理起来更复杂。

### 将参数值保存进变量

```rust
use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();

    let query = &args[1];
    let filename = &args[2];

    println!("Searching for {}", query);
    println!("In file {}", filename);
}
```

程序的名称占据了 vector 的第一个值args[0]，所以我们从索引1开始。第一个参数greprs是需要搜索的字符串，所以将其将第一个参数的引用存放在变量query中。第二个参数将是文件名，所以将第二个参数的引用放入变量filename中。

### 读取文件

我们读取文件 `poem.txt`，需要std::fs::File来处理文件，而std::io::prelude::*则包含许多对于 I/O 包括文件 I/O 有帮助的 trait。类似于 Rust 有一个通用的 prelude 来自动引入特定内容，std::io也有其自己的 prelude 来引入处理 I/O 时所需的通用内容。不同于默认的 prelude，必须显式use位于std::io中的 prelude ：
```rust
use std::env;
use std::fs::File;
use std::io::prelude::*;

fn main() {
    let args: Vec<String> = env::args().collect();

    let query = &args[1];
    let filename = &args[2];

    println!("Searching for {}", query);
    println!("In file {}", filename);

    let mut f = File::open(filename).expect("file not found");

    let mut contents = String::new();
    f.read_to_string(&mut contents).expect("something went wrong reading the file");

    println!("With text:\n{}", contents);
}
```

运行 `cargo run the poem.txt` 将看到文件的内容。

### 二进制项目的关注分离

过程有如下步骤：

1. 将程序拆分成 main.rs 和 lib.rs 并将程序的逻辑放入 lib.rs 中。
2. 当命令行解析逻辑比较小时，可以保留在 main.rs 中。
3. 当命令行解析开始变得复杂时，也同样将其从 main.rs 提取到 lib.rs中。
4. 经过这些过程之后保留在main函数中的责任是：
	* 使用参数值调用命令行解析逻辑
	* 设置任何其他的配置
	* 调用 lib.rs 中的run函数
	* 如果run返回错误，则处理这个错误

这个模式的一切就是为了关注分离：main.rs 处理程序运行，而 lib.rs 处理所有的真正的任务逻辑。因为不能直接测试main函数，这个结构通过将所有的程序逻辑移动到 lib.rs 的函数中使得我们可以测试他们。仅仅保留在 main.rs 中的代码将足够小以便阅读就可以验证其正确性。

### 提取参数解析器

定义新函数`parse_config`：

```rust
fn main() {
    let args: Vec<String> = env::args().collect();

    let (query, filename) = parse_config(&args);

    // ...snip...
}

fn parse_config(args: &[String]) -> (&str, &str) {
    let query = &args[1];
    let filename = &args[2];

    (query, filename)
}
```