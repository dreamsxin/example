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

### 运行

```shell
cargo run
```

## 使用 Cargo 创建新项目

```shell
cargo new hello_world2 --bin
```