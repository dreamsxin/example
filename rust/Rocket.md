# Rocket 使用

Rocket is web framework for Rust (nightly) with a focus on ease-of-use, expressibility, and speed.

## Overview

### 安装 Rust

需要最新的 Rust 支持。

```shell
curl https://sh.rustup.rs -sSf | sh
rustup default nightly
# or
rustup override set nightly
```

### 升级 Rust

```shell
rustup update && cargo update
```

### 运行自带例子 hello_world

```shell
git clone https://github.com/SergioBenitez/rocket
cd rocket/examples/hello_world
cargo run
```

运行成功后，访问：http://localhost:8000/

### 自己编写 hello, world

* 创建项目

```shell
cargo new hello-rocket --bin
cd hello-rocket
```

* 配置 Cargo.toml

```toml
[package]
name = "hello-rocket"
version = "0.1.0"
authors = ["dreamsxin <dreamsxin@126.com>"]

[dependencies]
rocket = "0.2.8"
rocket_codegen = "0.2.8"
```

* 编写 main.rs

```rust
#![feature(plugin)]
#![plugin(rocket_codegen)]

extern crate rocket;

#[get("/")]
fn index() -> &'static str {
    "Hello, world!"
}

fn main() {
    rocket::ignite().mount("/", routes![index]).launch();
}
```

### 生命周期 Lifecycle

Rocket 的主要任务是监听 web 请求，然后将请求交由应用程序处理以及输出响应给客户端。从请求到响应这个过程成为一个生命周期。
生命周期分为以下几个步骤：

1. 路由 Routing
	将 HTTP 请求解析为本地数据结构，然后根据声明的路由属性进行匹配，来决定调用哪个请求处理程序。

2. 验证 Validation
	验证 HTTP 请求类型，以及请求数据。如果验证失败，则将请求转发到下一个匹配路由或调用错误处理程序。

3. 处理 Processing
	调用与路径关联的处理程序。这是应用程序的主要业务逻辑。处理完成后返回 `Response`。

4. 响应 Response
	处理返回的`Response`。生成适当的 HTTP 响应并将其发送给客户端。这就完成了一个生命周期。继续侦听下一个请求，重新开启每个传入的请求。

### 路由 Routing

Rocket 应用程序以路由 routers 和处理程序 handlers 为中心。
处理程序 handler 只是一个具有任意数量参数并返回任意类型的函数。路由 route 是一种结合：
- 匹配请求参数。
- 调用处理程序以及返回响应

匹配的参数包括静态路径、动态路径、路径片段、表单、请求的字符串、请求的格式说明符和请求的 body 数据。
Rocket 使用属性以及定义一个匿名函数（即处理程序）来声明路由，并使用参数集来匹配。完整的路线声明如下:

```rust
#[get("/world")]              // <- route attribute
fn world() -> &'static str {  // <- request handler
    "Hello, world!"
}
```

### 绑定 Mounting

通过 Rocket 实例上的 `mount` 方法来绑定路由。

```rust
rocket::ignite() // 创建实例
	.mount("/hello", routes![world]);
```
请求 `/hello/world` 将会定向到 `world` 函数。

`mount` 使用了两个参数：
- 参数1，路径
- 参数2，路由处理程序列表，通过宏 `routes!` 将 Rocket 的代码生成与应用程序联系起来。

### 命名空间

当在根之外的模块中声明路径时，会发生错误:
```rust
mod other {
    #[get("/world")]
    pub fn world() -> &'static str {
        "Hello, world!"
    }
}

use other::world;

fn main() {
  // error[E0425]: cannot find value `static_rocket_route_info_for_world` in this scope
  rocket::ignite().mount("/hello", routes![world])
}
```
这是因为 `routes!` 宏隐式地将路由的名称转换为由 Rocket 代码生成所生成的结构名称，解决方案是用模块路径命名:
```rust
rocket::ignite().mount("/hello", routes![other::world])
```

### Launching

Rocket 知道了路由之后，就可以通过 launch 方法启动接受请求了。该方法启动服务器并等待请求的传入。
当请求到达时，Rocket 将会找到匹配的路径，并将请求分派给路由的处理程序。

```rust
#![feature(plugin)]
#![plugin(rocket_codegen)]

extern crate rocket;

#[get("/world")]
fn world() -> &'static str {
    "Hello, world!"
}

fn main() {
    rocket::ignite()
		.mount("/hello", routes![world])
		.launch();
}
```

> 请注意！这里增加了 `#![feature(plugin)]` 和 `#![plugin(rocket_codegen)]`，告诉 Rust 我们将使用 Rocket 的代码生成插件。

启动：
```shell
cargo run
```
输出：
```text
   Compiling hello-rocket v0.1.0 (file:///home/develop/workspace/example/rust/code/hello-rocket)
    Finished dev [unoptimized + debuginfo] target(s) in 5.44 secs
     Running `target/debug/hello-rocket`
🔧  Configured for development.
    => address: localhost
    => port: 8000
    => log: normal
    => workers: 4
🛰  Mounting '/hello':
    => GET /hello/world
🚀  Rocket has launched from http://localhost:8000...
```

### 配置 Configuration

配置环境：

- development (short: dev)
- staging (short: stage)
- production (short: prod)

运行时加上：

```shell
ROCKET_ENV=stage cargo run
```

也可以写入配置文件 `Rocket.toml`：

```toml
[development]
address = "localhost"
port = 8000
workers = max(number_of_cpus, 2)
log = "normal"

[staging]
address = "0.0.0.0"
port = 80
workers = max(number_of_cpus, 2)
log = "normal"

[production]
address = "0.0.0.0"
port = 80
workers = max(number_of_cpus, 2)
log = "critical"
```

我们可以使用 `global` 全局覆盖配置：
```toml
[global]
address = "1.2.3.4"

[development]
address = "localhost"

[production]
address = "0.0.0.0"
```

* Extras

```toml
[development]
template_dir = "dev_templates/"

[production]
template_dir = "prod_templates/"
```

* 环境变量 Environment Variables

所有的配置参数，包括 Extras 都会被环境变量覆盖，配置参数为`{NAME}`，使用环境变量`ROCKET_{PARAM}`进行覆盖：
```shell
ROCKET_PORT=3721 cargo run
```

## 请求 Requests

Rocket 有很多可选项用来处理请求信息。

### Methods

匹配 HTTP 方法，可以使用的属性有`get`, `put`, `post`, `delete`, `head`, `patch`, or `options`：
```rust
#[post("/")]
```
可以查看文档 https://api.rocket.rs/rocket_codegen/

如果 POST 请求的 `Content-Type` 为 `application/x-www-form-urlencoded` 将会检测表单第一个字段是否为 `_method`，如果存在将会使用 `_method` 这个值来验证 HTTP 方法。

### Format

定义路由属性 `format` 匹配验证 `Content-Type`：
```rust
#[post("/user", format = "application/json", data = "<user>")]
fn new_user(user: JSON<User>) -> T { ... }
```

> 注意 format 属性放在 post 属性之后

### Dynamic Paths

可以使用尖括号`<>`来声明路径片段为动态的，例如：
```rust
#[get("/hello/<name>")]
fn hello(name: &str) -> String {
    format!("Hello, {}!", name)
}
```
绑定到根目录：
```rust
fn main() {
    rocket::ignite()
		.mount("/", routes![hello])
		.launch();
}
```

访问 `http://localhost:8000/hello/John`