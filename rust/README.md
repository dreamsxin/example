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
表达式返回一个值，而语句不是。

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
