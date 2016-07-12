初学者可以看中文文档：https://github.com/KaiserY/rust-book-chinese

# 下载安装

```shell
wget https://static.rust-lang.org/dist/rust-1.10.0-x86_64-unknown-linux-gnu.tar.gz
tar zxvf rust-1.10.0-x86_64-unknown-linux-gnu.tar.gz
cd rust-1.10.0-x86_64-unknown-linux-gnu
sudo ./install

# or

curl -sSf https://static.rust-lang.org/rustup.sh | sh
```

## 卸载

```shell
sudo /usr/local/lib/rustlib/uninstall.sh
```

## 显示版本号

```shell
rustc --version
```

# 创建项目

```shell
mkdir hello_world
```

## 创建文件 `hello_world.rs`

```rust
fn main() {
    println!("Hello, world!"); // 带有 叹号，表示是Rust宏定义
}
```

## 编译运行

```shell
rustc hello_world.rs
./hello_world
```

# 使用 Cargo

仅仅使用rustc编译简单程序是没问题的，不过随着你的项目的增长，你将想要能够控制你项目拥有的所有选项，并易于分享你的代码给别人或别的项目。

Cargo 是 Rust 的构建系统和包管理工具，Cargo 负责三个工作：
- 构建代码
- 下载代码依赖库
- 编译库

## 查看 Cargo 版本号

```shell
cargo --version
```

## Cargo 包管理

Rust 包管理使用 crate 格式的压缩包存储和发布库，官方有一个集中式的仓库，由于其使用了动态网站技术，给镜像增加了难度，不过在 Rust 社区努力下，中科大也加入了对 crates.io 的支持，实现方式见这个讨论。

使用镜像只需要在项目根目录下新建一个 `.cargo/config` 文件，并在其中加入以下内容：

```config
[registry]
index = "git://crates.mirrors.ustc.edu.cn/index"
```

## 将之前项目转换到 Cargo

让我们将 Hello World 程序迁移至 Cargo，现在需要做三件事：
- 将源文件放到正确的目录
- 删除旧的可执行文件并生成一个新的。
- 创建一个 Cargo 配置文件

### 源文件目录

```shell
rm hello_world
mkdir src
mv hello_world.rs src/
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
cargo build
```

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

// 指定版本 rand="0.3"
// 最新版本
rand="*"
```

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