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

### 发布配置

Cargo 支持四种配置：

* `dev`: used for `cargo build`
* `release` used for `cargo build --release`
* `test` used for `cargo test`
* `doc` used for `cargo doc`

可以通过自定义Cargo.toml文件中的[profile.*]部分来调整这些配置的编译器参数。例如，这里是dev和release配置的默认参数：

```toml
[profile.dev]
opt-level = 0

[profile.release]
opt-level = 3
```

`opt-level` 设置控制 Rust 会进行何种程度的优化。这个配置的值从 0 到 3。越高的优化级别需要更多的时间。

更多查看文档 http://doc.crates.io/

### 将 crate 发布到 Crates.io

我们创建了一个库art，其包含一个kinds模块，模块中包含枚举Color和包含函数mix的模块utils：
```rust
//! # Art
//!
//! A library for modeling artistic concepts.

pub mod kinds {
    /// The primary colors according to the RYB color model.
    pub enum PrimaryColor {
        Red,
        Yellow,
        Blue,
    }

    /// The secondary colors according to the RYB color model.
    pub enum SecondaryColor {
        Orange,
        Green,
        Purple,
    }
}

pub mod utils {
    use kinds::*;

    /// Combines two primary colors in equal amounts to create
    /// a secondary color.
    pub fn mix(c1: PrimaryColor, c2: PrimaryColor) -> SecondaryColor {
        // ...snip...
#         SecondaryColor::Green
    }
}
```

为了使用这个库，另一个 crate 中使用了use语句：

```rust
extern crate art;

use art::kinds::PrimaryColor;
use art::utils::mix;

fn main() {
    let red = PrimaryColor::Red;
    let yellow = PrimaryColor::Yellow;
    mix(red, yellow);
}
```

* 使用pub use来导出合适的公有 API

增加`pub use`语句来将这些类型重新导出到顶级结构：
```rust,ignore
//! # Art
//!
//! A library for modeling artistic concepts.

pub use kinds::PrimaryColor;
pub use kinds::SecondaryColor;
pub use utils::mix;

pub mod kinds {
    // ...snip...
```

重导出的项将会被连接和排列在 crate API 文档的头版。
这样用户就可以如下使用：
```rust
extern crate art;

use art::PrimaryColor;
use art::mix;

fn main() {
    // ...snip...
}
```

使用 GitHub 账号登陆 https://crates.io/，查看`Account Settings`页面并使用其中指定的 API key 运行cargo login命令，这看起来像这样：

```shell
cargo login abcdefghijklmnopqrstuvwxyz012345
```
这个命令会通知 Cargo 你的 API token 并将其储存在本地的 ~/.cargo/config 文件中。

crate 必须有一个位移的名称。虽然在本地开发 crate 时，可以使用任何你喜欢的名字，不过crates.io上的 crate 名称遵守先到先得的原则分配。一旦一个 crate 名被使用，就不能被另一个 crate 所使用，所以请确认你喜欢的名字在网站上是可用的。

发布命令：
```shell
cargo publish
```
在包的 Cargo.toml 文件中包含更多的信息。其中一些字段是可选的，不过描述和 license 是发布所必须的，因为这样人们才能知道 crate 是干什么的已经在什么样的条款下可以使用他们。

所以一个准备好发布的项目的 Cargo.toml 文件看起来像这样：
```toml
[package]
name = "guessing_game"
version = "0.1.0"
authors = ["Your Name <you@example.com>"]
description = "A fun game where you guess what number the computer has chosen."
license = "MIT/Apache-2.0"

[dependencies]
```

当你修改了 crate 并准备好发布新版本时，改变 Cargo.toml 中version所指定的值。

* 使用cargo yank从 Crates.io 删除版本

```shell
cargo yank --vers 1.0.1
cargo yank --vers 1.0.1 --undo
```

### 依赖 dependencies


使用 Cargo 工具的最大优势就在于，能够对该项目进行方便、统一和灵活的管理。常用的依赖描述有以下几种：

- 基于 rust 官方仓库 crates.io，通过版本说明来描述。
- 基于项目源码的 git 仓库地址，通过 URL 来描述。
- 基于本地项目的绝对路径或相对路径来描述。

```toml
[dependencies]
rand = "0.3"
time = "0.1.35"
log = { version = "0.3" }
regex = { git = "https://github.com/rust-lang-nursery/regex" }
trust = { path = "cratex/trust" }
```

### 工作空间（workspaces）

Cargo 提供了一个叫工作空间（workspaces）的功能，它可以帮助我们管理多个相关的并行开发的包。

让我们为这个二进制项目创建一个新 crate 作为开始：

```text
$ cargo new --bin adder
     Created binary (application) `adder` project
$ cd adder
```

修改二进制包的 Cargo.toml 来告诉 Cargo 包`adder`是一个工作空间。在 Cargo.toml 文件末尾增加如下：
```toml
[workspace]
```

工作空间支持配置惯例：只要遵循这些惯例就无需再增加任何配置了。这个惯例是任何作为子目录依赖的 crate 将是工作空间的一部分。
让我们像这样在 Cargo.toml 中增加一个adder crate 的路径依赖：
```toml
[dependencies]
add-one = { path = "add-one" }
```
如果增加依赖但没有指定path，这将是一个基于 rust 官方仓库 crates.io 依赖。

接下来，在adder目录中生成add-one crate：

```text
$ cargo new add-one
     Created library `add-one` project
```
现在`adder`目录应该有如下目录和文件：
```text
├── Cargo.toml
├── add-one
│   ├── Cargo.toml
│   └── src
│       └── lib.rs
└── src
    └── main.rs
```

在 add-one/src/lib.rs 中增加add_one函数的实现：

```rust
pub fn add_one(x: i32) -> i32 {
    x + 1
}
```
打开adder的 src/main.rs 并增加一行extern crate将新的add-one库引入作用域，并修改main函数来使用add_one函数：
```rust
extern crate add_one;

fn main() {
    let num = 10;
    println!("Hello, world! {} plus one is {}!", num, add_one::add_one(num));
}
```
假如我们想要在add-one crate 中使用rand crate。一如既往在 add-one/Cargo.toml 的增加这个 crate：

```toml
[dependencies]

rand = "0.3.14"
```

为 crate 中的add_one::add_one函数增加一个测试：

```rust
pub fn add_one(x: i32) -> i32 {
    x + 1
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        assert_eq!(3, add_one(2));
    }
}
```

运行测试，需要指定包：
```shell
cargo test -p add-one
```

如果选择将工作空间发布到 crates.io，其中的每一个包都需要单独发布。

### 安装包 cargo install

只有有二进制目标文件的包能够安装，而且所有二进制文件都被安装到 Rust 安装根目录的 bin 文件夹中。如果你使用 rustup.rs 安装的 Rust 且没有自定义任何配置，这将是$HOME/.cargo/bin。将这个目录添加到$PATH环境变量中就能够运行通过cargo install安装的程序了。

```text
$ cargo install ripgrep
Updating registry `https://github.com/rust-lang/crates.io-index`
 Downloading ripgrep v0.3.2
 ...snip...
   Compiling ripgrep v0.3.2
    Finished release [optimized + debuginfo] target(s) in 97.91 secs
  Installing ~/.cargo/bin/rg
```

### 自定义 Cargo 扩展命令

如果$PATH中有类似cargo-something的二进制文件，就可以通过cargo something来像 Cargo 子命令一样运行它。像这样的自定义命令也可以运行cargo --list来展示出来，通过cargo install向 Cargo 安装扩展并可以如内建 Cargo 工具那样运行他们是很方便的！

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

### 过程宏（和自定义导出）

在上面你看到 Rust 提供了一个叫做“导出（derive）”的机制来轻松的实现 trait，他的工作机制：
```rust
#[derive(Debug)]
struct Point {
    x: i32,
    y: i32,
}
```
上面代码，类似生成下面的代码：
```rust
struct Point {
    x: i32,
    y: i32,
}

use std::fmt;

impl fmt::Debug for Point {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "Point {{ x: {}, y: {} }}", self.x, self.y)
    }
}
```

Rust 包含很多可以导出的 trait，不过也允许定义你自己的 trait。我们可以通过一个叫做“过程宏”的 Rust 功能来实现这个效果。最终，过程宏将会允许 Rust 所有类型的高级元编程。

### 实现自己的过程宏

我们引入了两个新的 crate：syn和quote。用 syn 解析我们定义的hello_world代码，（使用quote）构建hello_world的实现，接着把它传递回给 Rust 编译器。

创建项目
```shell
cargo new --bin hello-world
```

然后编写调用代码：
```rust
#[derive(HelloWorld)]
struct Pancakes;

fn main() {
    Pancakes::hello_world();
}
```
继续并从用户的角度编写我们的宏。在src/main.rs中：
```rust
#[macro_use]
extern crate hello_world_derive;

trait HelloWorld {
    fn hello_world();
}

#[derive(HelloWorld)]
struct FrenchToast;

#[derive(HelloWorld)]
struct Waffles;

fn main() {
    FrenchToast::hello_world();
    Waffles::hello_world();
}
```

接下来来编写我们的过程宏：

让我们在`hello-world`项目中新建一个叫做`hello-world-derive`的 crate。
```shell
cargo new hello-world-derive
```

我们需要在hello-world-derive crate 的cargo.toml中添加syn和quote的依赖。

```toml
[dependencies]
syn = "0.10.5"
quote = "0.3.10"
```

为了确保 `hello-world` 能够找到这个新创建的 `hello-world-derive` 我们把它加入到项目 toml 文件中：
```toml
[dependencies]
hello-world-derive = { path = "hello-world-derive" }
```

编写 lib.rs：
```rust
extern crate proc_macro;
extern crate syn;
#[macro_use]
extern crate quote;

use proc_macro::TokenStream;

#[proc_macro_derive(HelloWorld)]
pub fn hello_world(input: TokenStream) -> TokenStream {
    // Construct a string representation of the type definition
    let s = input.to_string();

    // Parse the string representation
    let ast = syn::parse_derive_input(&s).unwrap();

    // Build the impl
    let gen = impl_hello_world(&ast);

    // Return the generated impl
    gen.parse().unwrap()
}

fn impl_hello_world(ast: &syn::MacroInput) -> quote::Tokens {
    let name = &ast.ident;
    quote! {
        impl HelloWorld for #name {
            fn hello_world() {
                println!("Hello, World! My name is {}", stringify!(#name));
            }
        }
    }
}
```

我们可以通过ast.ident获取类型的信息。quote!宏允许我们编写想要返回的 Rust 代码并把它转换为Tokens。quote!让我们可以使用一些炫酷的模板机制；简单的使用#name，quote!就会把它替换为叫做name的变量。你甚至可以类似常规宏那样进行一些重复。

更多查看文档：https://docs.rs/quote/0.3.15/quote/

看来我们需要把hello-world-derive crate 声明为proc-macro类型。怎么做呢？像这样：

```toml
[lib]
proc-macro = true
```

### 自定义属性（Custom Attributes）

用户可能想要重载hello_world()方法打印出的名字的值。

这可以通过自定义 attribute 来实现：

```rust
#[derive(HelloWorld)]
#[HelloWorldName = "the best Pancakes"]
struct Pancakes;

fn main() {
    Pancakes::hello_world();
}
```

在hello-world-derive crate 中对proc_macro_derive attribute 增加attributes来实现：

```rust
#[proc_macro_derive(HelloWorld, attributes(HelloWorldName))]
pub fn hello_world(input: TokenStream) -> TokenStream
```

### 过程宏限制类型

假设我们并不希望在我们的自定义导出方法中接受枚举作为输入。
这个条件可以通过syn轻松的进行检查，在过程宏中报告错误的传统做法是 panic：
```rust
fn impl_hello_world(ast: &syn::MacroInput) -> quote::Tokens {
    let name = &ast.ident;
    // Check if derive(HelloWorld) was specified for a struct
    if let syn::Body::Struct(_) = ast.body {
        // Yes, this is a struct
        quote! {
            impl HelloWorld for #name {
                fn hello_world() {
                    println!("Hello, World! My name is {}", stringify!(#name));
                }
            }
        }
    } else {
        //Nope. This is an Enum. We cannot handle these!
       panic!("#[derive(HelloWorld)] is only defined for structs, not for enums!");
    }
}
```

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

	// 将整个 vector 传递给parse_config函数
    let (query, filename) = parse_config(&args);

    // ...snip...
}

fn parse_config(args: &[String]) -> (&str, &str) {
    let query = &args[1];
    let filename = &args[2];

    (query, filename)
}
```

### 组合配置值

定义新的结构体Config，它有字段query和filename。我们也改变了parse_config函数来返回一个Config结构体的实例，并更新main来使用结构体字段而不是单独的变量：

```rust


struct Config {
    query: String,
    filename: String,
}

fn parse_config(args: &[String]) -> Config {
	// ain中的args变量是参数值的所有者并只允许parse_config函数借用他们，所以用了 clone，我们也可以使用 match 直接从 std::env::Args 提取值
    let query = args[1].clone();
    let filename = args[2].clone();

    Config {
        query: query,
        filename: filename,
    }
}
```

### 创建一个Config构造函数

现在`parse_config`函数的目的是创建一个Config实例，我们可以将parse_config从一个普通函数变为一个叫做new的与结构体关联的函数。做出这个改变使得代码更符合习惯：可以像标准库中的String调用String::new来创建一个该类型的实例那样，将parse_config变为一个与Config关联的new函数。

```rust
struct Config {
    query: String,
    filename: String,
}

impl Config {
    fn new(args: &[String]) -> Config {
        let query = args[1].clone();
        let filename = args[2].clone();

        Config {
            query: query,
            filename: filename,
        }
    }
}
```

### 修复错误处理

现在我们开始修复错误处理。回忆一下之前提到过如果args vector 包含少于 3 个项并尝试访问 vector 中索引 1 或 索引 2 的值会造成程序 panic：

```text
   Compiling greprs v0.1.0 (file:///home/develop/workspace/example/rust/code/greprs)
    Finished dev [unoptimized + debuginfo] target(s) in 2.28 secs
     Running `target/debug/greprs`
thread 'main' panicked at 'index out of bounds: the len is 1 but the index is 1', /checkout/src/libcollections/vec.rs:1422
note: Run with `RUST_BACKTRACE=1` for a backtrace.
```

改进下错误提示：
```rust
fn new(args: &[String]) -> Config {
    if args.len() < 3 {
        panic!("not enough arguments");
    }

	let query = args[1].clone();
	let filename = args[2].clone();

	Config {
		query: query,
		filename: filename,
	}
}
```

但是 `panic!` 的调用更适合程序问题而不是使用问题，我们可以返回一个可以表明成功或错误的Result：

```rust
impl Config {
    fn new(args: &[String]) -> Result<Config, &'static str> {
        if args.len() < 3 {
            return Err("not enough arguments");
        }

        let query = args[1].clone();
        let filename = args[2].clone();

        Ok(Config {
            query: query,
            filename: filename,
        })
    }
}
```

现在new函数返回一个Result，在成功时带有一个Config实例而在出现错误时带有一个`&'static str`。

### 处理返回的错误信息

从标准库中导入 `process`。process::exit会立即停止程序并将传递给它的数字作为返回状态码。
使用`unwrap_or_else`可以进行一些自定义的非 `panic!` 的错误处理。
当`Result`是`Ok`时，这个方法的行为类似于`unwrap`：它返回Ok内部封装的值。
当`Result`是`Err`时，它调用一个闭包（closure），也就是一个我们定义的作为参数传递给unwrap_or_else的匿名函数。

```rust
use std::process;

fn main() {
    let args: Vec<String> = env::args().collect();

    let config = Config::new(&args).unwrap_or_else(|err| {
        println!("Problem parsing arguments: {}", err);
        process::exit(1);
    });

    // ...snip...
```

### 提取`run`函数

现在我们完成了配置解析的重构：让我们转向程序的逻辑。

```rust
fn main() {
    // ...snip...

    println!("Searching for {}", config.query);
    println!("In file {}", config.filename);

    run(config);
}

fn run(config: Config) {
    let mut f = File::open(config.filename).expect("file not found");

    let mut contents = String::new();
    f.read_to_string(&mut contents).expect("something went wrong reading the file");

    println!("With text:\n{}", contents);
}

// ...snip...
```

现在run函数包含了main中从读取文件开始的剩余的所有逻辑。run函数获取一个Config实例作为参数。

### 从run函数中返回错误

返回实现了`Error trait`的类型，不过无需指定具体将会返回的值的类型。这提供了在不同的错误场景可能有不同类型的错误返回值的灵活性。
我们使用`if let`来检查`run`是否返回一个`Err`值，因为run在成功时返回()，而我们只关心发现一个错误，所以并不需要`unwrap_or_else`来返回未封装的值，因为它只会是`()`。
```rust
use std::error::Error;

// ...snip...

fn main() {
    // ...snip...

    println!("Searching for {}", config.query);
    println!("In file {}", config.filename);

    if let Err(e) = run(config) {
        println!("Application error: {}", e);

        process::exit(1);
    }
}

fn run(config: Config) -> Result<(), Box<Error>> {
    let mut f = File::open(config.filename)?;

    let mut contents = String::new();
    f.read_to_string(&mut contents)?;

    println!("With text:\n{}", contents);

    Ok(())
}
```

### 将代码拆分到库 crate

让我们将如下代码片段从 src/main.rs 移动到新文件 src/lib.rs 中：

- run函数定义
- 相关的use语句
- Config的定义
- Config::new函数定义

文件`src/lib.rs`内容如下：
```
use std::error::Error;
use std::fs::File;
use std::io::prelude::*;

pub struct Config {
    pub query: String,
    pub filename: String,
}

impl Config {
    pub fn new(args: &[String]) -> Result<Config, &'static str> {
        if args.len() < 3 {
            return Err("not enough arguments");
        }

        let query = args[1].clone();
        let filename = args[2].clone();

        Ok(Config {
            query: query,
            filename: filename,
        })
    }
}

pub fn run(config: Config) -> Result<(), Box<Error>>{
    let mut f = File::open(config.filename)?;

    let mut contents = String::new();
    f.read_to_string(&mut contents)?;

    println!("With text:\n{}", contents);

    Ok(())
}
```

这里使用了公有的pub：在Config、其字段和其new方法，以及run函数上。现在我们有了一个拥有可以测试的公有 API 的库 crate 了。

### 从二进制 crate 中调用库 crate

现在需要在 src/main.rs 中使用extern crate greprs将移动到 src/lib.rs 的代码引入二进制 crate 的作用域。接着我们将增加一个use greprs::Config行将Config类型引入作用域，并使用库 crate 的名称作为run函数的前缀：

```rust
extern crate greprs;

use std::env;
use std::process;

use greprs::Config;

fn main() {
    let args: Vec<String> = env::args().collect();

    let config = Config::new(&args).unwrap_or_else(|err| {
        println!("Problem parsing arguments: {}", err);
        process::exit(1);
    });

    println!("Searching for {}", config.query);
    println!("In file {}", config.filename);

    if let Err(e) = greprs::run(config) {
        println!("Application error: {}", e);

        process::exit(1);
    }
}
```

### 测试库的功能

我们将遵循测试驱动开发（Test Driven Development, TTD）的模式。这是一个软件开发技术，它遵循如下步骤：

- 编写一个会失败的测试，并运行它以确保其因为你期望的原因失败。
- 编写或修改刚好足够的代码来使得新的测试通过。
- 重构刚刚增加或修改的代码，并确保测试仍然能通过。
- 重复上述步骤！

去掉 `src/lib.rs` 和 `src/main.rs` 中的`println!`语句，因为不再真的需要他们了。
在 lib.rs 增加一个test模块和一个测试函数。测试函数指定了我们希望search函数拥有的行为：它会获取一个需要查询的字符串和用来查询的文本。

```rust
fn search<'a>(query: &str, contents: &'a str) -> Vec<&'a str> {
     vec![]
}

// ...snip...

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn one_result() {
        let query = "duct";
        let contents = "\
Rust:
safe, fast, productive.
Pick three.";

        assert_eq!(
            vec!["safe, fast, productive."],
            search(query, contents)
        );
    }
}
```

这里选择使用 "duct" 作为这个测试中需要搜索的字符串。用来搜索的文本有三行，其中只有一行包含 "duct"。我们断言search函数的返回值只包含期望的那一行。
在search的签名中显式定义一个显式生命周期'a并用于contents参数和返回值。
我们告诉 Rust 函数search返回的数据将与search函数中的参数contents的数据存在的一样久。

现在我们尝试运行`cargo test`将会是我们期望的失败结果。

### 编写 search 让测试通过

目前测试之所以会失败是因为我们总是返回一个空的 vector。为了修复并实现search，我们的程序需要遵循如下步骤：

1. 遍历每一行文本。
2. 查看这一行是否包含要搜索的字符串。
	* 如果有，将这一行加入返回列表中。
	* 如果没有，什么也不做。
3. 返回匹配到的列表

让我们一步一步的来，从遍历每行开始。

* 使用lines方法遍历每一行

```rust
fn search<'a>(query: &str, contents: &'a str) -> Vec<&'a str> {
    for line in contents.lines() {
        // do something with line
    }
}
```

* 用`contains`查询字符串搜索每一行

```rust
fn search<'a>(query: &str, contents: &'a str) -> Vec<&'a str> {
    for line in contents.lines() {
        if line.contains(query) {
            // do something with line
        }
    }
}
```

* 存储匹配的行

```rust
fn search<'a>(query: &str, contents: &'a str) -> Vec<&'a str> {
    let mut results = Vec::new();

    for line in contents.lines() {
        if line.contains(query) {
            results.push(line);
        }
    }

    results
}
```

我们可以利用迭代器改进下代码：

```rust
fn search<'a>(query: &str, contents: &'a str) -> Vec<&'a str> {
    contents.lines()
        .filter(|line| line.contains(query))
        .collect()
}
```

这里使用了filter适配器来只保留line.contains(query)为真的那些行。接着使用collect将他们放入另一个 vector 中。这就简单多了！

### 在run函数中使用search函数

```rust
pub fn run(config: Config) -> Result<(), Box<Error>> {
    let mut f = File::open(config.filename)?;

    let mut contents = String::new();
    f.read_to_string(&mut contents)?;

    for line in search(&config.query, &contents) {
        println!("{}", line);
    }

    Ok(())
}
```

### 大小写不敏感`search`函数

将老的测试`one_result`改名为`case_sensitive`，编写新的测试`case_insensitive`：

```rust
#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn case_sensitive() {
        let query = "duct";
        let contents = "\
Rust:
safe, fast, productive.
Pick three.
Duct tape.";

        assert_eq!(
            vec!["safe, fast, productive."],
            search(query, contents)
        );
    }

    #[test]
    fn case_insensitive() {
        let query = "rUsT";
        let contents = "\
Rust:
safe, fast, productive.
Pick three.
Trust me.";

        assert_eq!(
            vec!["Rust:", "Trust me."],
            search_case_insensitive(query, contents)
        );
    }
}
```

### 实现`search_case_insensitive`函数

它会将query变量和每一line都变为小写，这样不管输入参数是大写还是小写，在检查该行是否包含查询字符串时都会是小写。

```rust
fn search_case_insensitive<'a>(query: &str, contents: &'a str) -> Vec<&'a str> {
	// 将query字符串转换为小写，并将其储存（覆盖）到同名的变量中。
    let query = query.to_lowercase();
    let mut results = Vec::new();

    for line in contents.lines() {
		// 因为 query 现在是一个String，当我们将query作为一个参数传递给contains方法时，需要增加一个 & 因为contains的签名被定义为获取一个字符串 slice。
        if line.to_lowercase().contains(&query) {
            results.push(line);
        }
    }

    results
}
```

现在，让我们在run函数中调用真正的新`search_case_insensitive`函数。首先，我们将在 Config 结构体中增加一个配置项来切换大小写是否敏感。

```rust
pub struct Config {
    pub query: String,
    pub filename: String,
    pub case_sensitive: bool,
}
```

`run`函数检查`case_sensitive`字段的值并使用它来决定是否调用`search`函数或`search_case_insensitive`函数：

```rust
pub fn run(config: Config) -> Result<(), Box<Error>>{
    let mut f = File::open(config.filename)?;

    let mut contents = String::new();
    f.read_to_string(&mut contents)?;

    let results = if config.case_sensitive {
        search(&config.query, &contents)
    } else {
        search_case_insensitive(&config.query, &contents)
    };

    for line in results {
        println!("{}", line);
    }

    Ok(())
}
```

### 检测`CASE_INSENSITIVE`环境变量

处理环境变量的函数位于标准库的`env`模块中，所以我们需要在 `src/lib.rs` 的开头增加一个`use std::env;`行将这个模块引入作用域中。

```rust
use std::env;

// ...snip...

impl Config {
    pub fn new(args: &[String]) -> Result<Config, &'static str> {
        if args.len() < 3 {
            return Err("not enough arguments");
        }

        let query = args[1].clone();
        let filename = args[2].clone();

        let case_sensitive = env::var("CASE_INSENSITIVE").is_err();

        Ok(Config {
            query: query,
            filename: filename,
            case_sensitive: case_sensitive,
        })
    }
}
```

### 输出到`stderr`而不是`stdout`

我们准备将标准输出重定向到一个文件中，而不包括错误信息，这时就需要将错误信息输出到标准错误`stderr`。

```rust
extern crate greprs;

use std::env;
use std::process;
use std::io::prelude::*;

use greprs::Config;

fn main() {
    let args: Vec<String> = env::args().collect();
    let mut stderr = std::io::stderr();

    let config = Config::new(&args).unwrap_or_else(|err| {
        writeln!(
            &mut stderr,
            "Problem parsing arguments: {}",
            err
        ).expect("Could not write to stderr");
        process::exit(1);
    });

    if let Err(e) = greprs::run(config) {
        writeln!(
            &mut stderr,
            "Application error: {}",
            e
        ).expect("Could not write to stderr");

        process::exit(1);
    }
}
```

Rust 并没有类似println!这样的方便写入标准错误的函数。相反，我们使用writeln!宏，它有点像println!，不过它获取一个额外的参数。第一个参数是希望写入内容的位置。可以通过std::io::stderr函数获取一个标准错误的句柄。我们将一个stderr的可变引用传递给writeln!；它需要是可变的因为这样才能写入信息！第二个和第三个参数就像println!的第一个和第二参数：一个格式化字符串和任何需要插入的变量。

## Rust 中的函数式语言功能 —— 迭代器和闭包


- 闭包（Closures），一个可以储存在变量里的类似函数的结构
- 迭代器（Iterators），一种处理元素序列的方式。。
- 如何使用这些功能来改进上一章的项目
- 这些功能的性能。**剧透高能：**他们的速度超乎想象！

这并不是一个 Rust 受函数式风格影响的完整功能列表：还有模式匹配、枚举和很多其他功能。

### 闭包

Rust 提供了定义闭包的能力，它类似于函数。让我们先不从技术上的定义开始，而是看看闭包语句结构，然后再返回他们的定义。

```rust
fn main() {
	// 赋值给变量 add_one 的小的闭包定义
    let add_one = |x| x + 1;
	// 调用闭包
    let five = add_one(4);

    assert_eq!(5, five);
}
```

这是一个很小的闭包，它只包含一个表达式。
一个稍微复杂一点的闭包：

```rust
Filename: src/main.rs

fn main() {
    let calculate = |a, b| {
        let mut result = a * 2;

        result += b;

        result
    };

    assert_eq!(7, calculate(2, 3)); // 2 * 2 + 3 == 7
    assert_eq!(13, calculate(4, 5)); // 4 * 2 + 5 == 13
}
```

你会注意到一些闭包不同于fn关键字定义的函数的地方。
第一个不同是并不需要声明闭包的参数和返回值的类型。
也可以选择加上类型注解：

```rust
fn main() {
    let add_one = |x: i32| -> i32 { x + 1 };

    assert_eq!(2, add_one(1));
}
```

不过闭包的定义会推断每一个参数和返回值的类型。例如，如果用`i8`调用没有类型注解的闭包，接着用`i32`调用同一闭包则会得到一个错误：

```rust
let add_one = |x| x + 1;

let five = add_one(4i8);
assert_eq!(5i8, five);

let three = add_one(2i32);
```

### 闭包可以引用其环境

函数只能使用其作用域内的变量，或者要么是const的要么是被声明为参数的。闭包则可以做的更多：闭包允许使用包含他们的作用域的变量。

```rust
fn main() {
    let x = 4;

    let equal_to_x = |z| z == x;

    let y = 4;

    assert!(equal_to_x(y));
}
```

即便x并不是equal_to_x的一个参数，equal_to_x闭包也被允许使用它，因为变量x定义于同样定义equal_to_x的作用域中。

获取他们环境中值的闭包主要用于开始新线程的场景。我们也可以定义以闭包作为参数的函数，通过使用Fn trait。这里是一个函数call_with_one的例子，它的签名有一个闭包参数：
```rust
fn call_with_one<F>(some_closure: F) -> i32
    where F: Fn(i32) -> i32 {

    some_closure(1)
}

let answer = call_with_one(|x| x + 2);

assert_eq!(3, answer);
```

### 迭代器

之前我们讲过 trait 类似于其他语言中的常被称为接口（interfaces）的功能。
迭代器是 Rust 中的一个模式，它允许你对一个项的序列进行某些处理。

```rust
let v1 = vec![1, 2, 3];

let v2: Vec<i32> = v1.iter()	// `vector` 的`iter`方法从 vector 创建一个迭代器（iterator）
	.map(|x| x + 1)				// 迭代器上的map方法调用允许我们处理每一个元素
	.collect();					// collect 方法消费了迭代器并将其元素存放到一个新的数据结构中

assert_eq!(v2, [2, 3, 4]);
```

map是最基本的与比较交互的方法之一，因为依次处理每一个元素是非常有用的！

像map这样的迭代器方法有时被称为迭代器适配器（iterator adaptors），因为他们获取一个迭代器并产生一个新的迭代器。也就是说，map在之前迭代器的基础上通过调用传递给它的闭包来创建了一个新的值序列的迭代器。

概括一下，这行代码进行了如下工作：

- 从 vector 中创建了一个迭代器。
- 使用map适配器和一个闭包参数对每一个元素加一。
- 使用collect适配器来消费迭代器并生成了一个新的 vector。

* 迭代器是惰性的

我们说map适配（adapts）了一个迭代器，而collect消费（consumes）了一个迭代器。这是有意为之的。
单独的迭代器并不会做任何工作；他们是惰性的。
他们需要一些其他方法来触发迭代器链的计算。我们称之为消费适配器（consuming adaptors），而`collect`就是其中之一。

那么如何知道迭代器方法是否消费了迭代器呢？还有哪些适配器是可用的呢？为此，让我们看看Iterator trait。
其定义看起来像这样：

```rust
trait Iterator {
    type Item;

    fn next(&mut self) -> Option<Self::Item>;
}
```

这里有一些还未讲到的新语法：`type Item`和`Self::Item`定义了这个 trait 的关联类型（associated type），要求你也定义一个 Item 类型，而这个 Item 类型用作next方法的返回值。换句话说，Item 类型将是迭代器返回的元素的类型。

### 实现迭代器 Counter

首先，需要创建一个结构体来存放迭代器的当前状态，它有一个u32的字段count。我们也定义了一个new方法，当然这并不是必须的：

```rust
struct Counter {
    count: u32,
}

impl Counter {
    fn new() -> Counter {
        Counter { count: 0 }
    }
}
```

因为我们希望 Counter 能从一数到五，所以它总是从零开始。
通过定义 `next` 方法来为 `Counter` 类型实现 `Iterator trait`。

```rust
impl Iterator for Counter {
    // Our iterator will produce u32s
    type Item = u32;

    fn next(&mut self) -> Option<Self::Item> {
        // increment our count. This is why we started at zero.
        self.count += 1;

        // check to see if we've finished counting or not.
        if self.count < 6 {
            Some(self.count)
        } else {
            None
        }
    }
}
```

我们希望迭代器的工作方式是对当前状态加一（这就是为什么将count初始化为零，这样迭代器首先就会返回一）。如果count仍然小于六，将返回当前状态，不过如果count大于等于六，迭代器将返回None。

next方法是迭代器的主要接口，它返回一个Option。如果它是Some(value)，相当于可以迭代器中获取另一个值。如果它是None，迭代器就结束了。在next方法中可以进行任何迭代器需要的计算。

### 各种Iterator适配器

Counter如何才能得到像map和collect这样的方法呢？

当讲到Iterator的定义时，我们故意省略一个小的细节。Iterator 定义了一系列默认实现，他们会调用next方法。
因为next是唯一一个Iterator trait 没有默认实现的方法，一旦实现之后，Iterator的所有其他的适配器就都可用了。这些适配器可不少！

处于某种原因我们希望获取一个Counter实例产生的值，与另一个Counter实例忽略第一个值之后的值相组合，将每组数相乘，并只保留能被三整除的相乘结果，最后将所有保留的结果相加，我们可以这么做：

```rust
let sum: u32 = Counter::new().zip(Counter::new().skip(1))
                             .map(|(a, b)| a * b)
                             .filter(|x| x % 3 == 0)
                             .sum();
assert_eq!(18, sum);
```

* 迭代器性能

迭代器是 Rust 的零成本抽象（zero-cost abstractions）之一，它意味着抽象并不会强加运行时开销，它与本贾尼·斯特劳斯特卢普，C++ 的设计和实现者所定义的零开销（zero-overhead）如出一辙：

> In general, C++ implementations obey the zero-overhead principle: What you
> don’t use, you don’t pay for. And further: What you do use, you couldn’t hand
> code any better.
>
> - Bjarne Stroustrup "Foundations of C++"

作为另一个例子，这里有一些来自于音频解码器的代码。
这些代码使用迭代器链来对作用域中的三个变量进行了某种数学计算：
- 一个叫buffer的数据 slice
- 一个有12个元素的数组coefficients
- 一个代表移位位数的qlp_shift。

来展示 Rust 如何将高级概念转换为底层代码：

```rust
let buffer: &mut [i32];
let coefficients: [i64; 12];
let qlp_shift: i16;

for i in 12..buffer.len() {
    let prediction = coefficients.iter()
                                 .zip(&buffer[i - 12..i])
                                 .map(|(&c, &s)| c * s as i64)
                                 .sum::<i64>() >> qlp_shift;
    let delta = buffer[i];
    buffer[i] = prediction as i32 + delta;
}
```

为了计算prediction的值，这些代码遍历了coefficients中的 12 个值，使用zip方法将系数与buffer的前 12 个值组合在一起。接着将每一对值相乘，再将所有结果相加，然后将总和右移qlp_shift位。

遍历coefficients的值完全用不到循环：Rust 知道这里会迭代 12 次，所以它“展开”了循环。所有的系数都被储存在了寄存器中（这意味着访问他们非常快）。也没有数组访问边界检查。这是极端有效率的。

## 智能指针

指针是一个常见的编程概念，它代表一个指向储存其他数据的位置。之前学习了 Rust 的引用；他们是一类很平常的指针，以`&`符号为标志并借用了他们所指向的值。
智能指针（Smart pointers）是一类数据结构，他们的表现类似指针，但是也拥有额外的元数据和能力，比如说引用计数。智能指针模式起源于 C++。
在 Rust 中，普通引用和智能指针的一个额外的区别是引用是一类只借用数据的指针；相反大部分情况，智能指针拥有他们指向的数据。

`String`和`Vec<T>`都是智能指针。他们拥有一些数据并允许你修改他们，并带有元数据（比如他们的容量）和额外的功能或保证（String的数据总是有效的 UTF-8 编码）。智能指针区别于常规结构体的特性在于他们实现了`Deref`和`Drop trait`，而本章会讨论这些 trait 以及为什么对于智能指针来说他们很重要。

很多库都有自己的智能指针而你也可以编写属于你自己的。这里将会讲到的是来自标准库中最常用的一些：

- Box<T>，用于在堆上分配值
- Rc<T>，一个引用计数类型，其数据可以有多个所有者
- RefCell<T>，其本身并不是智能指针，不过它管理智能指针Ref和RefMut的访问，在运行时而不是在编译时执行借用规则。

同时我们还将涉及：

- 内部可变性（interior mutability）模式，当一个不可变类型暴露出改变其内部值的 API，这时借用规则适用于运行时而不是编译时。
- 引用循环，它如何会泄露内存，以及如何避免他们

### Box<T>

最简单直接的智能指针是 box，它的类型是Box<T>。 box 允许你将一个值放在堆上：
```rust
fn main() {
    let b = Box::new(5);
    println!("b = {}", b);
}
```
一个 box 的实用场景是当你希望确保类型有一个已知大小的时候。
```rust
enum List {
    Cons(i32, List),
    Nil,
}
```
它是一个用于 cons list 的枚举定义，cons list 是一个来源于 Lisp 编程语言及其方言的数据结构。在 Lisp 中，cons函数（"construct function"的缩写）利用两个参数来构造一个新的列表，他们通常是一个单独的值和另一个列表。cons 函数的概念涉及到更通用的函数式编程术语；“将 x 与 y 连接”通常意味着构建一个新的容器而将 x 的元素放在新容器的开头，其后则是容器 y 的元素。

cons list 在 Rust 中并不常见；通常Vec<T>是一个更好的选择。实现这个数据结构是Box<T>实用性的一个好的例子。让我们看看为什么！

使用 cons list 来储存列表1, 2, 3将看起来像这样：
```rust
use List::{Cons, Nil};

fn main() {
    let list = Cons(1, Cons(2, Cons(3, Nil)));
}
```

如果尝试编译上面的代码，会得到错误：`error[E0072]: recursive type `List` has infinite size`。因为List的一个成员被定义为递归的：它存放了另一个相同类型的值。这意味着 Rust 无法计算为了存放List值到底需要多少空间。

因为Box<T>是一个指针，我们总是知道它需要多少空间：
```rust
enum List {
    Cons(i32, Box<List>),
    Nil,
}

use List::{Cons, Nil};

fn main() {
    let list = Cons(1,
        Box::new(Cons(2,
            Box::new(Cons(3,
                Box::new(Nil))))));
}
```
这就是 box 主要应用场景：打破无限循环的数据结构以便编译器可以知道其大小。

### Deref Trait 允许通过引用访问数据

第一个智能指针相关的重要 trait 是`Deref`，它允许我们重载*，解引用运算符（不同于乘法运算符和全局引用运算符）。

这是一个使用i32值引用的例子：
```rust
let mut x = 5;
{
    let y = &mut x;

    *y += 1
}

assert_eq!(6, x);
```
我们使用`*y`来访问可变引用y所指向的数据，而不是可变引用本身。接着可以修改它的数据，在这里对其加一。
当解引用一个智能指针时，我们只想要数据，而不需要元数据。我们希望能在使用常规引用的地方也能使用智能指针。为此，可以通过实现`Deref trait`来重载`*`运算符的行为。

一个定义为储存 mp3 数据和元数据的结构体通过Deref trait 来重载*的例子：
```rust
use std::ops::Deref;

struct Mp3 {
    audio: Vec<u8>,
    artist: Option<String>,
    title: Option<String>,
}

impl Deref for Mp3 {
    type Target = Vec<u8>;

    fn deref(&self) -> &Vec<u8> {
        &self.audio
    }
}

fn main() {
    let my_favorite_song = Mp3 {
        // we would read the actual audio data from an mp3 file
        audio: vec![1, 2, 3],
        artist: Some(String::from("Nirvana")),
        title: Some(String::from("Smells Like Teen Spirit")),
    };

    assert_eq!(vec![1, 2, 3], *my_favorite_song);
}
```

我们希望能够方便的访问音频数据而不是元数据，所以需要实现`Deref trait`来返回音频数据。
所以为了得到一个*可以解引用的&引用，编译器将`*my_favorite_song`展开为：
`*(my_favorite_song.deref())`。

### 函数和方法的隐式解引用强制多态

Rust 倾向于偏爱明确而不是隐晦，不过一个情况下这并不成立，就是函数和方法的参数的解引用强制多态（deref coercions）。

```rust
fn compress_mp3(audio: &[u8]) -> Vec<u8> {
    // the actual implementation would go here
}

let result = compress_mp3(&my_favorite_song);
```

如果 Rust 没有解引用强制多态，为了使用my_favorite_song中的音频数据调用此函数，必须写成：

```rust
compress_mp3(my_favorite_song.audio.as_slice())
```

也就是说，必须明确表用需要`my_favorite_song`中的audio字段而且我们希望有一个 slice 来引用这整个`Vec<u8>`。

### Drop Trait 运行清理代码

对于智能指针模式来说另一个重要的 trait 是Drop。Drop运行我们在值要离开作用域时执行一些代码。智能指针在被丢弃时会执行一些重要的清理工作，比如释放内存或减少引用计数。更一般的来讲，数据类型可以管理多于内存的资源，比如文件或网络连接，而使用Drop在代码处理完他们之后释放这些资源。我们在智能指针上下文中讨论Drop是因为其功能几乎总是用于实现智能指针。

```rust
struct CustomSmartPointer {
    data: String,
}

impl Drop for CustomSmartPointer {
    fn drop(&mut self) {
        println!("Dropping CustomSmartPointer!");
    }
}

fn main() {
    let c = CustomSmartPointer { data: String::from("some data") };
    println!("CustomSmartPointer created.");
    println!("Wait for it...");
}
```

我们也可以使用`std::mem::drop`函数提前丢弃它，它位于 prelude 中所以可以直接调用drop：
```rust
fn main() {
    let c = CustomSmartPointer { data: String::from("some data") };
    println!("CustomSmartPointer created.");
    drop(c);
    println!("Wait for it...");
}
```
std::mem::drop 的定义是：
```rust
pub mod std {
    pub mod mem {
        pub fn drop<T>(x: T) { }
    }
}
```
这个函数对于T是泛型的，所以可以传递任何值。这个函数的函数体并没有任何实际内容，所以它也不会利用其参数。这个空函数的作用在于drop获取其参数的所有权，它意味着在这个函数结尾x离开作用域时x会被丢弃。

### Rc<T> 引用计数智能指针

大部分情况下所有权是非常明确的：可以准确的知道哪个变量拥有某个值。然而并不总是如此；有时确实可能需要多个所有者。为此，Rust 有一个叫做Rc<T>的类型。它的名字是引用计数（reference counting）的缩写。引用计数意味着它记录一个值引用的数量来知晓这个值是否仍在被使用。如果这个值有零个引用，就知道可以在没有有效引用的前提下清理这个值。

注意Rc<T>只能用于单线程场景。

Rc<T>被实现为用于单线程场景，这时不需要为拥有线程安全的引用计数而付出性能代价。

因为 Rc<T> 没有标记为 Send，Rust 的类型系统和 trait bound 会确保我们不会错误的把一个 Rc<T> 值不安全的在线程间传递。

> Mutex<T>是Sync的，正如上一部分所讲的它可以被用来在多线程中共享访问。

### 使用`Rc<T>`分享数据

让我们回到 cons list 例子，例子中尝试使用Box<T>定义的List。
下面的代码并不能工作：
 ```rust
 enum List {
    Cons(i32, Box<List>),
    Nil,
}

use List::{Cons, Nil};

fn main() {
    let a = Cons(5,
        Box::new(Cons(10,
            Box::new(Nil))));
    let b = Cons(3, Box::new(a));
    let c = Cons(4, Box::new(a));
}
```

Cons 成员拥有其储存的数据，所以当创建b列表时将`a`的所有权移动到了`b`。接着当再次尝使用`a`创建`c`时，这不被允许因为`a`的所有权已经被移动。
相反可以改变Cons的定义来存放一个引用，不过接着必须指定生命周期参数，而且在构造列表时，也必须使列表中的每一个元素都至少与列表本身存在的一样久。否则借用检查器甚至都不会允许我们编译代码。

可以将List的定义从Box<T>改为Rc<T>：
```rust
enum List {
    Cons(i32, Rc<List>),
    Nil,
}

use List::{Cons, Nil};
use std::rc::Rc;

fn main() {
    let a = Rc::new(Cons(5, Rc::new(Cons(10, Rc::new(Nil)))));
    let b = Cons(3, a.clone());
    let c = Cons(4, a.clone());
}
```

注意必须为Rc增加use语句因为它不在 prelude 中。在main中创建了存放 5 和 10 的列表并将其存放在一个叫做a的新的Rc中。接着当创建b和c时，我们对a调用了clone方法。

#### 克隆Rc<T>会增加引用计数

之前我们见过clone方法，当时使用它来创建某些数据的完整拷贝。但是对于Rc<T>来说，它并不创建一个完整的拷贝。Rc<T>存放了引用计数，也就是说，一个存在多少个克隆的计数器。

打印出关联函数Rc::strong_count的结果。

```rust
fn main() {
    let a = Rc::new(Cons(5, Rc::new(Cons(10, Rc::new(Nil)))));
    println!("rc = {}", Rc::strong_count(&a));
    let b = Cons(3, a.clone());
    println!("rc after creating b = {}", Rc::strong_count(&a));
    {
        let c = Cons(4, a.clone());
        println!("rc after creating c = {}", Rc::strong_count(&a));
    }
    println!("rc after c goes out of scope = {}", Rc::strong_count(&a));
}
```

这会打印出：
```text
rc = 1
rc after creating b = 2
rc after creating c = 3
rc after c goes out of scope = 2
```

我们说Rc<T>只允许程序的多个部分读取Rc<T>中T的不可变引用。

### RefCell<T>和内部可变性模式

内部可变性（Interior mutability）是 Rust 中的一个设计模式，它允许你即使在有不可变引用时改变数据，这通常是借用规则所不允许。内部可变性模式涉及到在数据结构中使用unsafe代码来模糊 Rust 通常的可变性和借用规则。

内部可变性模式用于当你可以确保代码在运行时也会遵守借用规则，哪怕编译器也不能保证的情况。引入的unsafe代码将被封装进安全的 API 中，而外部类型仍然是不可变的。

* RefCell<T>拥有内部可变性

不同于Rc<T>，RefCell<T>代表其数据的唯一的所有权。

回忆一下借用规则：

1. 在任意给定时间，只能拥有如下中的一个：
	* 一个可变引用。
	* 任意属性的不可变引用。
2. 引用必须总是有效的。

对于引用和Box<T>，借用规则的不可变性作用于编译时。对于RefCell<T>，这些不可变性作用于运行时。对于引用，如果违反这些规则，会得到一个编译错误。而对于RefCell<T>，违反这些规则会panic!。

Rust 编译器执行的静态分析天生是保守的。RefCell<T>正是用于当你知道代码遵守借用规则，而编译器不能理解的时候。
类似于Rc<T>，RefCell<T>只能用于单线程场景。

对于引用，可以使用&和&mut语法来分别创建不可变和可变的引用。不过对于RefCell<T>，我们使用borrow和borrow_mut方法，它是RefCell<T>拥有的安全 API 的一部分。borrow返回Ref类型的智能指针，而borrow_mut返回RefMut类型的智能指针。这两个类型实现了Deref所以可以被当作常规引用处理。Ref和RefMut动态的借用所有权，而他们的Drop实现也动态的释放借用。

使用RefCell<T>来使函数不可变的和可变的借用它的参数。注意data变量使用let data而不是let mut data来声明为不可变的，而a_fn_that_mutably_borrows则允许可变的借用数据并修改它：
```rust
use std::cell::RefCell;

fn a_fn_that_immutably_borrows(a: &i32) {
    println!("a is {}", a);
}

fn a_fn_that_mutably_borrows(b: &mut i32) {
    *b += 1;
}

fn demo(r: &RefCell<i32>) {
    a_fn_that_immutably_borrows(&r.borrow());
    a_fn_that_mutably_borrows(&mut r.borrow_mut());
    a_fn_that_immutably_borrows(&r.borrow());
}

fn main() {
    let data = RefCell::new(5);
    demo(&data);
}
```

在main函数中，我们新声明了一个包含值 5 的RefCell<T>，并储存在变量data中，声明时并没有使用mut关键字。接着使用data的一个不可变引用来调用demo函数：对于main函数而言data是不可变的！

#### RefCell<T>在运行时检查借用规则

尝试使用常规引用在同一作用域中创建两个可变引用的代码无法编译：
```rust
let mut s = String::from("hello");

let r1 = &mut s;
let r2 = &mut s;
```

与此相反，使用RefCell<T>并在同一作用域调用两次borrow_mut的代码是可以编译的，不过它会在运行时 panic。如下代码：
```rust
use std::cell::RefCell;

fn main() {
    let s = RefCell::new(String::from("hello"));

    let r1 = s.borrow_mut();
    let r2 = s.borrow_mut();
}
```

这个运行时BorrowMutError类似于编译错误：它表明我们已经可变得借用过一次s了，所以不允许再次借用它。我们并没有绕过借用规则，只是选择让 Rust 在运行时而不是编译时执行他们。

考虑到RefCell<T>是不可变的，但是拥有内部可变性，可以将Rc<T>与RefCell<T>结合来创造一个既有引用计数又可变的类型。

```rust
#[derive(Debug)]
enum List {
    Cons(Rc<RefCell<i32>>, Rc<List>),
    Nil,
}

use List::{Cons, Nil};
use std::rc::Rc;
use std::cell::RefCell;

fn main() {
    let value = Rc::new(RefCell::new(5));

    let a = Cons(value.clone(), Rc::new(Nil));
    let shared_list = Rc::new(a);

    let b = Cons(Rc::new(RefCell::new(6)), shared_list.clone());
    let c = Cons(Rc::new(RefCell::new(10)), shared_list.clone());

    *value.borrow_mut() += 10;

    println!("shared_list after = {:?}", shared_list);
    println!("b after = {:?}", b);
    println!("c after = {:?}", c);
}
```

RefCell<T>并不是标准库中唯一提供内部可变性的类型。Cell<T>有点类似，不过不同于RefCell<T>那样提供内部值的引用，其值被拷贝进和拷贝出Cell<T>。Mutex<T>提供线程间安全的内部可变性。

### 引用循环和内存泄漏是安全的

在使用Rc<T>和RefCell<T>时，有可能创建循环引用，这时各个项相互引用并形成环。这是不好的因为每一项的引用计数将永远也到不了 0，其值也永远也不会被丢弃。

```rust
use List::{Cons, Nil};
use std::rc::Rc;
use std::cell::RefCell;

#[derive(Debug)]
enum List {
    Cons(i32, RefCell<Rc<List>>),
    Nil,
}

impl List {
    fn tail(&self) -> Option<&RefCell<Rc<List>>> {
        match *self {
            Cons(_, ref item) => Some(item),
            Nil => None,
        }
    }
}

fn main() {

    let a = Rc::new(Cons(5, RefCell::new(Rc::new(Nil))));

    println!("a initial rc count = {}", Rc::strong_count(&a));
    println!("a next item = {:?}", a.tail());

    let b = Rc::new(Cons(10, RefCell::new(a.clone())));

    println!("a rc count after b creation = {}", Rc::strong_count(&a));
    println!("b initial rc count = {}", Rc::strong_count(&b));
    println!("b next item = {:?}", b.tail());

    if let Some(ref link) = a.tail() {
        *link.borrow_mut() = b.clone();
    }

    println!("b rc count after changing a = {}", Rc::strong_count(&b));
    println!("a rc count after changing a = {}", Rc::strong_count(&a));

    // Uncomment the next line to see that we have a cycle; it will
    // overflow the stack
    // println!("a next item = {:?}", a.tail());
}
```

使用tail方法来获取a中RefCell的引用，并将其放入变量link中。接着对RefCell使用borrow_mut方法将其中的值从存放Nil值的Rc改为b中的Rc。

#### 避免引用循环：将Rc<T>变为Weak<T>

# 无畏并发（fearless concurrency）

确保内存安全并不是 Rust 的唯一目标：更好的处理并发和并行编程一直是 Rust 的另一个主要目标。 并发编程（concurrent programming）代表程序的不同部分相互独立的执行，而并行编程代表程序不同部分同时执行，这两个概念在计算机拥有更多处理器可供程序利用时变得更加重要。由于历史的原因，在此类上下文中编程一直是困难且容易出错的：Rust 希望能改变这一点。

## 使用线程同时运行代码

在今天使用的大部分操作系统中，当程序执行时，操作系统运行代码的上下文称为进程（process）。操作系统可以运行很多进程，而操作系统也管理这些进程使得多个程序可以在电脑上同时运行。

我们可以将每个进程运行一个程序的概念再往下抽象一层：程序也可以在其上下文中同时运行独立的部分。这个功能叫做线程（thread）。

编程语言提供的线程有时被称作轻量级（lightweight）或绿色（green）线程。这些语言将一系列绿色线程放入不同数量的操作系统线程中执行。因为这个原因，语言调用操作系统 API 创建线程的模型有时被称为 1:1，一个 OS 线程对应一个语言线程。绿色线程模型被称为 M:N 模型，M个绿色线程对应N个 OS 线程，这里M和N不必相同。

绿色线程模型功能要求更大的运行时来管理这些线程。为此，Rust 标准库只提供了 1:1 线程模型实现。因为 Rust 是这么一个底层语言，所以有相应的 crate 实现了 M:N 线程模型，如果你宁愿牺牲性能来换取例如更好的线程运行控制和更低的上下文切换成本。

系统线程实现请看：https://github.com/rust-lang/rust/blob/master/src/libstd/sys/unix/thread.rs

### 使用spawn创建新线程

大部分时候不能保证新建线程执行完毕，甚至不能实际保证新建线程会被执行！可以通过保存thread::spawn的返回值来解决这个问题，这是一个JoinHandle。

```rust
use std::thread;

fn main() {
    thread::spawn(|| {
        for i in 1..10 {
            println!("hi number {} from the spawned thread!", i);
        }
    });

    for i in 1..5 {
        println!("hi number {} from the main thread!", i);
    }
	handle.join();
}
```

接下来我们使用第二种方式创建线程，它比第一种方式稍微复杂一点，因为功能强大一点，可以在创建之前设置线程的名称和堆栈大小，参见下面的代码：
```rust
use std::thread;

fn main() {
    // 创建一个线程，线程名称为 thread1, 堆栈大小为4k
    let new_thread_result = thread::Builder::new()
                            .name("thread1".to_string())
                            .stack_size(4*1024*1024).spawn(move || {
        println!("I am thread1.");
    });
    // 等待新创建的线程执行完成
    new_thread_result.unwrap().join().unwrap();
}
```

### 线程和move闭包

当在闭包环境中获取某些值时，Rust 会尝试推断如何获取它。println!只需要v的一个引用，所以闭包尝试借用v。但是这有一个问题：我们并不知道新建线程会运行多久，所以无法知道v是否一直时有效的。

```rust
use std::thread;

fn main() {
    let v = vec![1, 2, 3];

    let handle = thread::spawn(|| {
        println!("Here's a vector: {:?}", v);
    });

    drop(v); // oh no!

    handle.join();
}
```

通过在闭包之前增加move关键字，我们强制闭包获取它使用的值的所有权，而不是引用借用。

```rust
use std::thread;

fn main() {
    let v = vec![1, 2, 3];

    let handle = thread::spawn(move || {
        println!("Here's a vector: {:?}", v);
    });

    handle.join();
}
```
如果在闭包上增加了move，就将v移动到了闭包的环境中，我们将不能对其调用drop了。

### 使用消息传递在线程间传送数据

最近人气正在上升的一个并发方式是消息传递（message passing），这里线程或 actor 通过发送包含数据的消息来沟通。这个思想来源于Effective Go 口号：

> 不要共享内存来通讯；而是要通讯来共享内存。

参考文档：http://golang.org/doc/effective_go.html

实现这个目标的主要工具是通道（channel）。通道有两部分组成，一个发送者（transmitter）和一个接收者（receiver）。代码的一部分可以调用发送者和想要发送的数据，而另一部分代码可以在接收的那一端收取消息。

下面创建一个通道：

```rust
use std::sync::mpsc;

fn main() {
    let (tx, rx) = mpsc::channel();
}
```

`mpsc::channel`函数创建一个新的通道。mpsc是多个生产者，单个消费者（multiple producer, single consumer）的缩写。简而言之，可以有多个产生值的发送端，但只能有一个消费这些值的接收端。现在我们以一个单独的生产者开始，不过一旦例子可以工作了就会增加多个生产者。

`mpsc::channel`返回一个元组：第一个元素是发送端，而第二个元素是接收端。由于历史原因，很多人使用tx和rx作为发送者和接收者的缩写，所以这就是我们将用来绑定这两端变量的名字。这里使用了一个 let 语句和模式来解构了元组。

让我们将发送端移动到一个新建线程中并发送一个字符串：

```rust
use std::thread;
use std::sync::mpsc;

fn main() {
    let (tx, rx) = mpsc::channel();

    thread::spawn(move || {
        let val = String::from("hi");
        tx.send(val).unwrap(); // 忽略错误
    });
}
```

正如上一部分那样使用thread::spawn来创建一个新线程。并使用一个move闭包来将tx移动进闭包这样新建线程就是其所有者。

通道的发送端有一个send方法用来获取需要放入通道的值。send方法返回一个Result<T, E>类型，因为如果接收端被丢弃了，将没有发送值的目标，所以发送操作会出错。

让我们在主线程中从通道的接收端获取值：
```rust
use std::thread;
use std::sync::mpsc;

fn main() {
    let (tx, rx) = mpsc::channel();

    thread::spawn(move || {
        let val = String::from("hi");
        tx.send(val).unwrap();
    });

    let received = rx.recv().unwrap();
    println!("Got: {}", received);
}
```

通道的接收端有两个有用的方法：`recv`和`try_recv`。这里，我们使用了recv，它是 receive 的缩写。
这个方法会阻塞执行直到从通道中接收一个值。
一旦发送了一个值，recv会在一个`Result<T, E>`中返回它。
当通道发送端关闭，`recv`会返回一个错误。
`try_recv`不会阻塞；相反它立刻返回一个`Result<T, E>`。

一旦将值发送到另一个线程后，那个线程可能会在我们在此使用它之前就修改或者丢弃它。
send获取其参数的所有权并移动这个值归接收者所有。这个意味着不可能意外的在发送后再次使用这个值；所有权系统检查一切是否合乎规则。

### 发送多个值并观察接收者的等待

```rust
use std::thread;
use std::sync::mpsc;
use std::time::Duration;

fn main() {
    let (tx, rx) = mpsc::channel();

    thread::spawn(move || {
        let vals = vec![
            String::from("hi"),
            String::from("from"),
            String::from("the"),
            String::from("thread"),
        ];

        for val in vals {
            tx.send(val).unwrap();
            thread::sleep(Duration::new(1, 0));
        }
    });

    for received in rx {
        println!("Got: {}", received);
    }
}
```

这一次，在新建线程中有一个字符串 vector 希望发送到主线程。我们遍历他们，单独的发送每一个字符串并通过一个Duration值调用thread::sleep函数来暂停一秒。

在主线程中，不再显式的调用recv函数：而是将rx当作一个迭代器。对于每一个接收到的值，我们将其打印出来。当通道被关闭时，迭代器也将结束。

### 通过克隆发送者来创建多个生产者

创建都向同一接收者发送值的多个线程。这可以通过克隆通道的发送端在来做到。

```rust
// ...snip...
let (tx, rx) = mpsc::channel();

let tx1 = tx.clone();
thread::spawn(move || {
    let vals = vec![
        String::from("hi"),
        String::from("from"),
        String::from("the"),
        String::from("thread"),
    ];

    for val in vals {
        tx1.send(val).unwrap();
        thread::sleep(Duration::new(1, 0));
    }
});

thread::spawn(move || {
    let vals = vec![
        String::from("more"),
        String::from("messages"),
        String::from("for"),
        String::from("you"),
    ];

    for val in vals {
        tx.send(val).unwrap();
        thread::sleep(Duration::new(1, 0));
    }
});
```

这一次，在创建新线程之前，我们对通道的发送端调用了clone方法。这会给我们一个可以传递给第一个新建线程的发送端句柄。我们会将原始的通道发送端传递给第二个新建线程，这样每个线程将向通道的接收端发送不同的消息。

## 共享状态并发

虽然消息传递是一个很好的处理并发的方式，但并不是唯一的一个。再次考虑一下它的口号：

> 不要共享内存来通讯；而是要通讯来共享内存。

那么“共享内存来通讯”是怎样的呢？共享内存并发有点像多所有权：多个线程可以同时访问相同的内存位置。

### 互斥器一次只允许一个线程访问数据

互斥器（mutex）是一种用于共享内存的并发原语。它是“mutual exclusion”的缩写，也就是说，任意时间，它只允许一个线程访问某些数据。互斥器以难以使用著称，因为你不得不记住：

1. 在使用数据之前尝试获取锁。
2. 处理完被互斥器所保护的数据之后，必须解锁数据，这样其他线程才能够获取锁。

正确的管理互斥器异常复杂，这也是许多人之所以热衷于通道的原因。然而，在 Rust 中，得益于类型系统和所有权，我们不会在锁和解锁上出错。

* Mutex<T>的 API

```rust
use std::sync::Mutex;

fn main() {
	// 使用关联函数 new 来创建
    let m = Mutex::new(5);

    {
		// 使用lock方法获取锁
        let mut num = m.lock().unwrap();
        *num = 6;
    }

    println!("m = {:?}", m);
}
```

如果另一个线程拥有锁，并且那个线程 `panic` 了，则这个调用会失败，我们暂时使用 `unwrap()` 进行错误处理。
`Mutex<i32>` 并不是一个i32，所以必须获取锁才能使用这个i32值。我们是不会忘记这么做的，因为类型系统不允许。
一旦获取了锁，就可以将返回值（在这里是num）作为一个数据的可变引用使用了。
`lock` 调用返回一个叫做`MutexGuard`的智能指针，它实现了Deref来指向其内部数据。
另外`MutexGuard`有一个用来释放锁的`Drop`实现。这样就不会忘记释放锁了，这在MutexGuard离开作用域时会自动发生。

### 在线程间共享 `Mutex<T>`

我们将启动十个线程，并在各个线程中对同一个计数器值加一，这样计数器将从 0 变为 10：

```rust
use std::sync::Mutex;
use std::thread;

fn main() {
	// 创建了一个 counter 变量来存放内含 i32 的 Mutex<T>
    let counter = Mutex::new(0);
    let mut handles = vec![];

	// 使用 range 创建了 10 个线程
    for _ in 0..10 {
        let handle = thread::spawn(|| {
            let mut num = counter.lock().unwrap();

            *num += 1;
        });
        handles.push(handle);
    }

	// 调用 join 方法来确保所有线程都会结束
    for handle in handles {
        handle.join().unwrap();
    }

    println!("Result: {}", *counter.lock().unwrap());
}
```

Rust 无法知道这些线程会运行多久，而在每一个线程尝试借用 `counter` 时它是否仍然有效。
所以这里要使用 `move` 来给予每个线程其所有权：

```rust,ignore
thread::spawn(move || {
```

但是 counter 被移动进了 handle 所代表线程的闭包中。因此我们无法在第二个线程中对其调用 lock，所以 Rust 会告诉我们不能将 counter 的所有权移动到多个线程中。
所以这里我们就需要引入类似智能指针 `Rc<T>` 来创建引用计数的值，以便拥有多所有权。但是 Rc<T> 只能在单线程环境中使用。

### 多线程和多所有权

* 原子引用计数 `Arc<T>`

字母“a”代表原子性（atomic），所以这是一个原子引用计数（atomically reference counted）类型。
原子性类型工作起来类似原始类型，不过可以安全的在线程间共享。

`Arc<T>`和`Rc<T>`除了Arc<T>内部的原子性之外没有区别，其 API 也相同：

```rust
use std::sync::{Mutex, Arc};
use std::thread;

fn main() {
    let counter = Arc::new(Mutex::new(0));
    let mut handles = vec![];

    for _ in 0..10 {
        let counter = counter.clone();
        let handle = thread::spawn(move || {
            let mut num = counter.lock().unwrap();

            *num += 1;
        });
        handles.push(handle);
    }

    for handle in handles {
        handle.join().unwrap();
    }

    println!("Result: {}", *counter.lock().unwrap());
}
```

## 使用Sync和Send trait 的可扩展并发

Rust 的并发模型中一个有趣的方面是：语言本身对并发知之甚少。我们之前讨论的几乎所有内容，都属于标准库，而不是语言本身的内容。
由于不需要语言提供并发相关的基础设施，并发方案不受标准库或语言所限：我们可以编写自己的或使用别人编写的。

属于语言本身的有两个 `trait`，都位于`std::marker`： `Sync`和`Send`。

### `Send` 用于表明所有权可能被传送给其他线程

`send` 标记 `trait` 表明类型的所有权可能被在线程间传递。
因为 `Rc<T>` 没有标记为 `Send`，`Rust` 的类型系统和 `trait bound` 会确保我们不会错误的把一个 `Rc<T>` 值不安全的在线程间传递。

任何完全由 Send 的类型组成的类型也会自动被标记为 Send：几乎所有基本类型都是 Send 的，大部分标准库类型是 Send 的，除`了Rc<T>`，以及裸指针（raw pointer）。

### `Sync` 表明多线程访问是安全的

Sync 标记 trait 表明一个类型可以安全的在多个线程中拥有其值的引用。
换一种方式来说，对于任意类型 `T`，如果`&T`（T的引用）是Send的话T就是Sync的，这样其引用就可以安全的发送到另一个线程。
类似于 Send 的情况，基本类型是 Sync 的，完全由 Sync 的类型组成的类型也是 Sync 的。

# 不使用标准库开发 Rust

有些系统不支持：线程，网络，堆分配等功能。为了 Rust 也能在这些系统上工作。为此，我们可以通过一个属性来告诉 Rust 我们不想使用标准库：`#![no_std]`。

在 crate 的根文件上加入：
```toml
#![no_std]

fn plus_one(x: i32) -> i32 {
    x + 1
}
```

## 使用外部函数接口

Rust 无需运行时（ runtime ）的优势使得 Rust 与其他语言的相互调用变得简单而高效。

由于 Rust 致力于成为系统级编程语言，因而它并没有像其他语言一样的运行时环境（ runtime ），这也给其他语言与之结合提供了便利。

### 链接

extern 块中的 link 属性提供给 rustc 基本构建块，告诉它如何链接到本地库。有两种可接受的 link 编写形式：

- #[link(name = "foo")]
- #[link(name = "foo", kind = "bar")]

在这两种情况下，foo 是它要连接到本地库的名称，而在第二种情况中 bar 是编译期连接到本地库的类型。目前有三个已知的本地库类型：

- 动态 - #[link(name = "readline")]
- 静态 - #[link(name = "my_build_dependency", kind = "static")]
- 框架 - #[link(name = "CoreFoundation", kind = "framework")]

注意，框架类型仅仅对 OSX 目标平台可用。

从连接的角度来看，Rust 编译器创建两种构件：部分(rlib/staticlib)和最终(dylib/binary)。本地动态库和框架属于最终构件范围，而静态库不属于。

本指南将使用 snappy 压缩/解压库作为引言来介绍编写绑定外部代码。Rust 目前无法直接调用 c++ 库，但是 snappy 包括 C 的接口(头文件 snappy-c.h)。

```rust
extern crate libc;
use libc::size_t;

\#[link(name = "snappy")]
extern {
    fn snappy_max_compressed_length(source_length: size_t) -> size_t;
}

fn main() {
    let x = unsafe { snappy_max_compressed_length(100) };
    println!("max compressed length of a 100 byte buffer: {}", x);
}
```

extern 语句块中包含的是外部库中函数签名列表，在这个例子中调用的是平台的 C ABI。#[link(...)] 属性用于指示链接器对 snappy 库进行连接，从而保证库中的符号能够被解析。

外部函数被假定为不安全的，所以当调用他们时，需要利用 unsafe{ } 进行封装，进而告诉编译器被调用的函数中包含的代码是安全的。C 库经常暴露不是线程安全的接口给外部调用，而且几乎任何携带指针参数的函数对于所有的输入都不是有效的，因为这些指可能悬空，并且未经处理的指针可能指向 Rust 内存安全模型之外的区域。

当声明外部函数的参数类型时，Rust 编译器不会检查声明是正确的，所以正确地指定它是在运行时能够正确的绑定的一部分。

extern 块可以扩展到覆盖整个 snappy API:
```rust
extern crate libc;
use libc::{c_int, size_t};

\#[link(name = "snappy")]
extern {
    fn snappy_compress(input: *const u8, input_length: size_t, compressed: *mut u8, compressed_length: *mut size_t) -> c_int;
    fn snappy_uncompress(compressed: *const u8, compressed_length: size_t, uncompressed: *mut u8, uncompressed_length: *mut size_t) -> c_int;
    fn snappy_max_compressed_length(source_length: size_t) -> size_t;
    fn snappy_uncompressed_length(compressed: *const u8, compressed_length: size_t, result: *mut size_t) -> c_int;
    fn snappy_validate_compressed_buffer(compressed: *const u8, compressed_length: size_t) -> c_int;
}
```

### 创建一个安全接口

原始 C API 需要经过封装之后提供内存安全性，并且才可以使用更高级的概念类似向量。库可以选择只暴露安全、高级接口而隐藏不安全的内部细节。

```rust
pub fn validate_compressed_buffer(src: &[u8]) -> bool {
    unsafe {
        snappy_validate_compressed_buffer(src.as_ptr(), src.len() as size_t) == 0
    }
}
```
上述 validate_compressed_buffer 封装器使用了不安全的语句块，但它保证对于所有的输入在离开那个 unsafe 函数签名的时候是安全的。
通过指定最大所需容量用来分配向量空间，接着用该向量来保存输出：
```rust
pub fn compress(src: &[u8]) -> Vec<u8> {
    unsafe {
        let srclen = src.len() as size_t;
        let psrc = src.as_ptr();

        let mut dstlen = snappy_max_compressed_length(srclen);
        let mut dst = Vec::with_capacity(dstlen as usize);
        let pdst = dst.as_mut_ptr();

        snappy_compress(psrc, srclen, pdst, &mut dstlen);
        dst.set_len(dstlen as usize);
        dst
    }
}

pub fn uncompress(src: &[u8]) -> Option<Vec<u8>> {
    unsafe {
        let srclen = src.len() as size_t;
        let psrc = src.as_ptr();

        let mut dstlen: size_t = 0;
        snappy_uncompressed_length(psrc, srclen, &mut dstlen);

        let mut dst = Vec::with_capacity(dstlen as usize);
        let pdst = dst.as_mut_ptr();

        if snappy_uncompress(psrc, srclen, pdst, &mut dstlen) == 0 {
            dst.set_len(dstlen as usize);
            Some(dst)
        } else {
            None // SNAPPY_INVALID_INPUT
        }
    }
}
```

* 析构函数

外部库经常更换被调用代码资源的所有权。当这种情况发生时，我们必须使用 Rust 提供的析构函数来提供安全保证的释放这些资源。

### Rust 函数调用 C 代码进行回调

可以通过 Rust 中定义的传递函数与外部库进行通信。当调用 C 代码时，要求回调函数必须使用 extern 标记。

Rust 代码：
```rust
extern fn callback(a: i32) {
    println!("I'm called from C with value {0}", a);
}

\#[link(name = "extlib")]
extern {
   fn register_callback(cb: extern fn(i32)) -> i32;
   fn trigger_callback();
}

fn main() {
    unsafe {
        register_callback(callback);
        trigger_callback(); // Triggers the callback
    }
}
```

C 代码：
```c
typedef void (*rust_callback)(int32_t);
rust_callback cb;

int32_t register_callback(rust_callback callback) {
    cb = callback;
    return 1;
}

void trigger_callback() {
  cb(7); // Will call callback(7) in Rust
}
```

### 针对 Rust 对象的回调

前面的例子显示了如何在 C 代码中如何回调一个全局函数。然而通常这个回调是针对于 Rust 中某个特定的对象。这个对象可能相应的由 C 对象封装之后的对象。

这个可以通过利用传递一个不安全的指针给 C 库来实现。接着 C 库能够在通知中包含 Rust 对象的指针。此时，允许不安全的访问 Rust 索引对象。
```rust
\#[repr(C)]
struct RustObject {
    a: i32,
    // other members
}

extern "C" fn callback(target: *mut RustObject, a: i32) {
    println!("I'm called from C with value {0}", a);
    unsafe {
        // Update the value in RustObject with the value received from the callback
        (*target).a = a;
    }
}

\#[link(name = "extlib")]
extern {
   fn register_callback(target: *mut RustObject, cb: extern fn(*mut RustObject, i32)) -> i32;
   fn trigger_callback();
}

fn main() {
    // Create the object that will be referenced in the callback
    let mut rust_object = Box::new(RustObject { a: 5 });

    unsafe {
        register_callback(&mut *rust_object, callback);
        trigger_callback();
    }
}
```

C 代码：
```c
typedef void (*rust_callback)(void*, int32_t);
void* cb_target;
rust_callback cb;

int32_t register_callback(void* callback_target, rust_callback callback) {
    cb_target = callback_target;
    cb = callback;
    return 1;
}

void trigger_callback() {
  cb(cb_target, 7); // Will call callback(&rustObject, 7) in Rust
}
```

### 异步回调

当外部库生成自己的线程，并调用回调时情况就变得更加的复杂。在这些情况下，在回调函数内使用 Rust 中的数据结构是特别不安全的，而且必须使用适当的同步机制。除了经典的同步机制，例如互斥，Rust 中提供了一种可行的方式是使用管道(std::comm)，它会将数据从调用回调的 C 线程中转发到 Rust 中的线程。

文档：https://doc.rust-lang.org/0.11.0/std/comm/

### 访问外部全局变量

外部 API 经常导出全局变量，这样可以做一些类似于跟踪全局状态的事情。为了访问这些变量，你在 extern 语句块中声明他们时要使用关键字 static：
```rust
extern crate libc;

\#[link(name = "readline")]
extern {
    static rl_readline_version: libc::c_int;
}

fn main() {
    println!("You have readline version {} installed.",
             rl_readline_version as i32);
}
```
或者，您可能需要使用外部接口来改变全局状态。为了做到这一点，在声明他们时使用 mut，这样就可以修改他们了。
```rust
extern crate libc;

use std::ffi::CString;
use std::ptr;

\#[link(name = "readline")]
extern {
    static mut rl_prompt: *const libc::c_char;
}

fn main() {
    let prompt = CString::new("[my-awesome-shell] $").unwrap();
    unsafe {
        rl_prompt = prompt.as_ptr();

        println!("{:?}", rl_prompt);

        rl_prompt = ptr::null();
    }
}
```
### 外部调用约定

大多数外部代码暴露了 C ABI，并且 Rust 默认情况下调用外部函数时使用的是 C 平台调用约束。一些外部函数，尤其是 Windows API，使用的是其他调用约定。Rust 提供了一种方法来告诉编译器它使用的是哪个约定：
```rust
extern crate libc;

\#[cfg(all(target_os = "win32", target_arch = "x86"))]
\#[link(name = "kernel32")]
\#[allow(non_snake_case)]
extern "stdcall" {
    fn SetEnvironmentVariableA(n: *const u8, v: *const u8) -> libc::c_int;
}
```
下面的适用于整个 extern 块。Rust 中支持的ABI 约束列表如下：

- stdcall
- aapcs
- cdecl
- fastcall
- Rust
- rust-intrinsic
- system
- C
- win64

上面列表中的大部分 abis 是不需要解释的，但 system 这个 abi 可能看起来有点奇怪。这个约束的意思是选择与任何与目标库合适的 ABI 进行交互。例如，在 win32 x86 体系结构中，这意味着 abi 将会选择 stdcall。然而在 x86_64 中，windows 使用 C 调用协定，因此将会使用 C 的标准。也就是说，在前面的例子中，我们可以在 extern 中使用 “system”{...} 来定义 所有 windows 系统中的块，而不仅仅是 x86 的。
与外部代码的交互

* `#[repr(C)]`

只要 `#[repr(C)]` 这个属性应用在代码中，Rust 保证的 struct 的结构与平台的表示形式是兼容的。`#[repr(C、包装)]` 可以用来布局 sturct 的成员没而不需要有填充元素。`#[repr(C)]` 也适用于枚举类型。

### 可空指针优化

某些类型的定义不为空。这包括引用类型(&T、&mut T)，boxes(Box<T>)，和函数指针(extern "abi" fn())。当与 C 交互时，经常使用的指针可能为空。
`Option<extern "C" fn(c_int) -> c_int>` 展示了一个表示空函数指针是如何使用 C ABI。

### C 语言中调用 Rust 代码

你可能想要在 C 中调用 Rust 代码，并且编译。这也好似相当容易，但是需要几件事：
```rust
\#[no_mangle]
pub extern fn hello_rust() -> *const u8 {
    "Hello, world!\0".as_ptr()
}
```
`extern` 让这个函数符合 C 调用函数的约束，就如上面说的“外部函数调用约束”。no_mangle 属性关闭了 Rust 的名称纠正，因此这里是很容易的进行连接的。

 ### 使用 Rust FFI
 
 Hashcash 是用来验证计算机计算能力的一种算法，被用于比特币挖矿以及部分反垃圾邮件的系统中，算法简明而有效。此算法作者的实现用的是 C 语言，下面就通过 Rust 的外部函数接口（ Foreign Function Interface ）来调用他实现的 Hashcash C 库接口，以快速实现 Hashcash 算法的 Rust 版本。
 
1. 下载 Hashcash

```shell
git clone https://github.com/jbboehr/hashcash.git
```

2. 编译安装

```shell
sudo apt-get install autoconf automake libtool
autoreconf -i
./configure
sudo make install
```

3. 验证安装

我们可以通过运行下面这个简单的测试程序来验证 Hashcash C 库是否可用。

编写 test.c 文件：
```c
#include <stdio.h>
#include <hashcash.h>
int main() {
	const char *version = hashcash_version();
	printf("hashcash version: %s\n", version);
	return 0;
}
```
然后执行如下命令编译运行：
```shell
gcc -o test test.c -lhashcash
./test
```
如果看到如下输出就说明 Hashcash C 库是可以正常使用的了。
```text
hashcash version: 1.22
```

4. 使用 rust 查看 Hashcash 版本号

```rust
#![feature(libc)] // 文档注释

extern crate libc;
use std::str;
use std::ffi::CStr;
use libc::c_char;

#[link(name = "hashcash")]
extern {
	fn hashcash_version() -> *const c_char;
}

fn main() {
	let version: &CStr = unsafe {
		let c_buf: *const c_char = hashcash_version();
		CStr::from_ptr(c_buf)
	};
	println!("hashcash version: {}", str::from_utf8(version.to_bytes()).unwrap());
}
```

5. 产生 stamp

Hashcash 算法的核心是一串叫做 stamp 的字符串，如下所示：
```text
1:20:150811:my_test::Kw6sW7wzgMGBNzSV:00000000001E3c
```

stamp 总共有7个域，相互之间像 IPv6 地址一样用冒号隔开。7个域的作用分别如下：

- 版本号 现在都是1
- 比特数 这个 stamp 保证其 SHA 后能有几比特前导0
- 日期 产生 stamp 的时间
- 资源 stamp 所承载的信息
- 扩展 暂时保留
- 随机串 依据 a-zA-Z0-9+/= 这些字符随机生成的字符串
- 计数器 产生 stamp 总共尝试了多少次

Hashcash 是一种验证计算能力的算法，那么如何验证产生一个 stamp 的计算机的计算能力如何呢？只需要看比特数和日期这两个域即可。日期大概可以猜到是为了保证 stamp 的时效性，比特数是验证的关键，可以通过如下指令进行验证：

```shell
echo -n 1:20:150811:my_test::Kw6sW7wzgMGBNzSV:00000000001E3c | shasum
```
输出
```text
00000af7f14703c7b9168aaa468fc7cb3dfcd6bd
```
可以看到 shasum 输出的字符串前面有5个0，即20比特0，与比特数域所示一致，故该 stamp 有效。

一个 stamp 进行 shasum 后出现前导几个数字都为0是一个极小概率的事件，而且随着0的个数的增加概率也随之降低， Hashcash 便是通过这样的方法验证计算能力的。下面就使用 Rust 编写一段程序来验证一下计算机的计算能力，代码如下：
```rust
#![feature(libc)]
extern crate libc;
use std::str;
use std::ptr;
use std::ffi::{CStr, CString};
use libc::{c_int, c_long, c_char, c_void, size_t};
#[link(name = "hashcash")]
extern {
    fn hashcash_simple_mint(resource: *const c_char,
        bits: size_t,
        anon_period: c_long,
        ext: *mut c_char,
        compress: c_int) -> *mut c_char;
    fn hashcash_free(ptr: *mut c_void);
}
fn main() {
    let (resource, bits) = (CString::new("my_test").unwrap(), 28);
    let stamp: &CStr = unsafe {
        let c_buf: *mut c_char = hashcash_simple_mint(resource.as_ptr(), bits, 0, ptr::null_mut(), 0);
        CStr::from_ptr(c_buf)
    };
    println!("hashcash stamp: {}", str::from_utf8(stamp.to_bytes()).unwrap());
    unsafe {
        hashcash_free(stamp.as_ptr() as *mut c_void);
    };
}
```
编译运行：
```text
$ rustc test.rs 
time ./test
hashcash stamp: 1:28:150811:my_test::ZIkGrcoTk59SGw23:000000000JunhG
real    0m46.534s
user    0m46.289s
sys     0m0.122s
```
从上面可以看到我的电脑整整花了46秒的时间才找到符合要求的 stamp ，有兴趣你也可以试下哦！

### C 回调 Rust 函数

上面的例子只是简单地使用 Rust 调用 C 的接口，其实反过来也一样简单，下面是完成同样功能的一段代码，不过它实现了一个函数以供 Hashcash C 库回调。

```c
#![feature(libc)]
extern crate libc;
extern crate time;
use std::str;
use std::ptr;
use std::ffi::{CStr, CString};
use libc::{c_int, c_long, c_ulong, c_char, c_double, c_void, size_t};
use time::now_utc;
#[link(name = "hashcash")]
extern {
    fn hashcash_free(ptr: *mut c_void);
    fn hashcash_mint(now_time: c_ulong,
        time_width: c_int, 
        resource: *const c_char,
        bits: size_t,
        anon_period: c_long,
        stamp: *const *mut c_char,
        anon_random: *mut c_long,
        tries_taken: *mut c_double,
        ext: *mut c_char,
        compress: c_int,
        cb: extern fn(percent: c_int,
            largest: c_int,
            target: c_int,
            count: c_double,
            expected: c_double,
            user: *const c_void) -> c_int,
        user_arg: *const c_void) -> c_int;
}
extern "C" fn callback(_percent: c_int, _largest: c_int,
            _target: c_int, count: c_double,
            _expected: c_double, _user: *const c_void) -> c_int {
    println!("mint count: {}", count);
    return 1;
}
fn main() {
    let (resource, bits) = (CString::new("my_test").unwrap(), 28);
    let now_time = now_utc().to_timespec().sec as u64;
    let stamp2: &CStr = unsafe {
        let c_buf: *mut c_char = ptr::null_mut();
        let user_arg: *const c_void = ptr::null();
        hashcash_mint(now_time, 6, resource.as_ptr(), bits, 0, &c_buf as *const *mut c_char, ptr::null_mut(), ptr::null_mut(), ptr::null_mut(), 0, callback, user_arg);
        CStr::from_ptr(c_buf)
    };
    println!("hashcash stamp: {}", str::from_utf8(stamp2.to_bytes()).unwrap());
    unsafe {
        hashcash_free(stamp2.as_ptr() as *mut c_void);
    };
}
```

## Rust 生成 C 语言库

Rust 不会自动生活 C 语言的头文件，需由使用者自行创建，或是使用 rusty-cheddar 生成。

### 从 Rust 导出常量

```rust
#[no_mangle]
pub static VAR: i32 = 42;
```
修改 Cargo.toml：
```toml
[lib]
name = "const"
crate_type = ["dylib"]
```
在本例中，我们是编译为动态库，若要编译为静态库，`dylib` 改为 `staticlib` 即可。

然后编译构建：

```shell
cargo build --release
```

最后在 C 中使用：

```c
#include <stdio.h>

extern const int VAR;

int main() {
    int x = VAR;
    printf("%d\n", x);
}
```

使用 gcc 编译：
```shell
gcc -o const main.c -Ltarget/release -lconst
```

### 从 Rust 导出函数

```rust
use std::ffi::{CStr, CString};
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn double_int(x: i32) -> i32 {
    x * 2
}

#[no_mangle]
pub extern "C" fn double_float(x: f64) -> f64 {
    x * 2.0
}

#[no_mangle]
pub extern "C" fn double_str(x: *const c_char) -> *const c_char {
    let string = unsafe { CStr::from_ptr(x).to_str().unwrap() };
    let output = format!("{}{}", string, string);
    CString::new(output).unwrap().into_raw()
}

#[no_mangle]
pub extern "C" fn str_free(x: *mut c_char) {
    if x.is_null() {
        return
    }

    unsafe { Box::from_raw(x); }
}
```
`extern "C"` 告诉 Rust 该函数要导出给 C。`#[no_mangle]` 避免 Rust 在编译时修改函数名称。
我们可以创建 C 头文件：
```c


#ifndef __DOUBLER_H__
#define __DOUBLER_H__

#ifdef __cplusplus
extern "C" {
#endif

  int double_int(int);
  double double_float(double);
  char* double_str(char*);
  void str_free(char*);

#ifdef __cplusplus
}
#endif

#endif  // __DOUBLER_H__
```

C 调用代码：
```c
#include <stdio.h>
#include "doubler.h"

int main() {
    printf("%d\n", double_int(2));
    printf("%lf\n", double_float(1.3));
    char* str = double_str("Hi");
    printf("%s\n", str);
    str_free(str);

    return 0;
}
```

### 从 Rust 导出复杂数据

```rust
#[repr(C)]
pub struct Matrix {
    m: Vec<Vec<f64>>,
}

#[no_mangle]
pub extern "C" fn matrix_new(nrow: usize, ncol: usize) -> *mut Matrix {
    let mut m = Vec::new();
    for _ in 0..(nrow) {
        let mut n = Vec::new();

        for _ in 0..(ncol) {
            n.push(0.0);
        }

        m.push(n);
    }

    Box::into_raw(Box::new(Matrix { m: m }))
}

#[no_mangle]
pub extern "C" fn matrix_get(matrix: *const Matrix, row: usize, col: usize)
                                -> f64 {
    unsafe {
        (*matrix).m[row][col]
    }
}

#[no_mangle]
pub extern "C" fn matrix_set(matrix: *mut Matrix,
                             row: usize, col: usize, value: f64) {
    unsafe {
        (*matrix).m[row][col] = value;
    }
}

#[no_mangle]
pub extern "C" fn matrix_free(matrix: *mut Matrix) {
    if matrix.is_null() {
        return
    }

    unsafe { Box::from_raw(matrix); }
}
```

C 头文件：
```c
#ifndef __MATRIX_H__
#define __MATRIX_H__

#ifdef __cplusplus
extern "C" {
#endif

  void* matrix_new(size_t, size_t);
  double matrix_get(void*, size_t, size_t);
  void matrix_set(void*, size_t, size_t, double);
  void matrix_free(void*);

#ifdef __cplusplus
}
#endif

#endif  // __MATRIX_H__
```
C 源码：
```c
#include <stdio.h>
#include "matrix.h"

int main() {
    void* m = matrix_new(3, 3);

    printf("(1, 1) = %lf\n", matrix_get(m, 1, 1));

    matrix_set(m, 1, 1, 99);
    printf("(1, 1) = %lf\n", matrix_get(m, 1, 1));

    matrix_free(m);

    return 0;
}
```