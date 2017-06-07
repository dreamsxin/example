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

我们可以实现多个动态路径片段，以及通过 `FromParam` trait 任意类型：
```rust
#[get("/hello/<name>/<age>/<cool>")]
fn hello(name: &str, age: u8, cool: bool) -> String {
    if cool {
      format!("You're a cool {} year old, {}!", age, name)
    } else {
      format!("{}, we need to talk about your coolness.", name)
    }
}
```

访问 `http://localhost:8000/hello/John/18/true`

如果请求地址不对，将会返回404错误。我们可以捕获错误，并定制错误信息。

```rust
#[error(401)]
fn unauthorized(req: &Request) -> String { }

#[error(404)]
fn not_found(req: &Request) -> String { }

fn main() {
	rocket::ignite().catch(errors![unauthorized, not_found])
}
```

完整例子可以查看 https://github.com/SergioBenitez/Rocket/tree/v0.2.8/examples/errors/src

### Dynamic Segments

前面用到了 `FromParam` 它匹配单个片段，现在讲的 `FromSegments` 区别就是它将使用`<param..>`匹配多个片段。
用它我们可以返回静态文件的内容：
```rust
use std::path::{Path, PathBuf};

use rocket::response::NamedFile;

#[get("/page/<file..>")]
fn files(file: PathBuf) -> Option<NamedFile> {
    NamedFile::open(Path::new("static/").join(file)).ok()
}
```

### 请求守卫 Request Guards

有时候我们需要获取Headers 或 Cookies信息，只要传入对应 ` FromRequest` trait 的参数即可：

```rust
#[get("/")]
fn index(cookies: &Cookies, content: ContentType) -> String { ... }
```

我们也可以实现基于` FromRequest` 的自定义类型`APIKey`：

```rust
use rocket::Outcome;
use rocket::http::Status;
use rocket::request::{self, Request, FromRequest};

struct APIKey(String);

Returns true if `key` is a valid API key string.
fn is_valid(key: &str) -> bool {
    key == "valid_api_key"
}

impl<'a, 'r> FromRequest<'a, 'r> for APIKey {
    type Error = ();

    fn from_request(request: &'a Request<'r>) -> request::Outcome<APIKey, ()> {
        let keys: Vec<_> = request.headers().get("x-api-key").collect();
        if keys.len() != 1 {
            return Outcome::Failure((Status::BadRequest, ()));
        }

        let key = keys[0];
        if !is_valid(keys[0]) {
            return Outcome::Forward(());
        }

        return Outcome::Success(APIKey(key.to_string()));
    }
}

#[get("/sensitive")]
fn sensitive(key: APIKey) -> &'static str {
    "Sensitive data."
}
```

### 表单 Forms

表单是 Web 应用程序中最常见的数据类型。假如我们一个表单包含两个字段，一个复选框 complete，一个文本输入框 description：
```rust
#[derive(FromForm)]
struct Task {
    complete: bool,
    description: String,
}

#[post("/todo", data = "<task>")]
fn new(task: Form<Task>) -> String { ... }
```

这里`Form`类型实现了`FromData` trait，它的泛型参数需要实现 `FromForm` trait。
参数解析失败可以返回 `Option` 或 `Result` 类型数据，实现错误的捕获。

### JSON 数据

```rust
#[derive(Deserialize)]
struct Task {
    description: String,
    complete: bool
}

#[post("/todo", data = "<task>")]
fn new(task: JSON<Task>) -> String { ... }
```

### Streaming

```rust
#[post("/upload", format = "text/plain", data = "<data>")]
fn upload(data: Data) -> io::Result<Plain<String>> {
    data.stream_to_file("/tmp/upload.txt").map(|n| Plain(n.to_string()))
}
```

### Query Strings

```rust
#![feature(plugin, custom_derive)]
#![plugin(rocket_codegen)]

extern crate rocket;

#[derive(FromForm)]
struct Person<'r> {
    name: &'r str,
    age: Option<u8>
}

#[get("/hello?<person>")]
fn hello(person: Person) -> String {
    if let Some(age) = person.age {
        format!("Hello, {} year old named {}!", age, person.name)
    } else {
        format!("Hello {}!", person.name)
    }
}

fn main() {
    rocket::ignite().mount("/", routes![hello]).launch()
}
```

## 响应 Responses

我们可以返回任何实现了 ` Responder` trait 特性的数据。

### Responder

一个响应包含HTTP status、headers 和 body。
- 内置类型：`String`、`&str`、`File`、`Option`、`Result`
- 自定义类型：`Content`、`Flash`

更多查看 https://api.rocket.rs/rocket/response/index.html

响应的数据可以是固定大小或者流式的，比如字符串使用固定大小的 body，文件使用流式的 body。

### Wrapping

响应数据可以包裹响应数据，可以有以下形式（R: Responder）：
```rust
struct WrappingResponder<R>(R);
```

返回 202：
```rust
#[get("/")]
fn accept() -> status::Accepted<String> {
    status::Accepted(Some("I accept!".to_string()))
}
```

返回 JSON：
```rust
#[get("/")]
fn json() -> content::JSON<&'static str> {
    content::JSON("{ 'hi': 'world' }")
}
```

### Result

如果实现 Responder，不管变量是什么都将使用`OK`或`Err`。如果没有实现，则将内容打印到控制台，并将该请求转发到 500 错误捕获器（error catcher）。

```rust
#[get("/users/<name>")]
fn user(name: &RawStr) -> Result<&'static str, Redirect> {
    match name.as_str() {
        "Sergio" => Ok("Hello, Sergio!"),
        _ => Err(Redirect::to("/users/login")),
    }
}
```

### Option

如果值为 Some，将会封装响应器发给客户端。否则将该请求转发到 404 错误捕获器。

```rust
#[get("/users/<name>")]
fn user(name: &str) -> Option<&'static str> {
    if name == "Sergio" {
        Some("Hello, Sergio!")
    } else {
        None
    }
}
```

### 跳转

```rust
use rocket::response::Redirect;

#[get("/")]
fn index() -> Redirect {
    Redirect::to("/hello/Unknown")
}
```

### Errors

如果没有对应的捕获器，将会使用 500 错误捕获器。

```rust
#![feature(plugin)]
#![plugin(rocket_codegen)]

extern crate rocket;

#[cfg(test)] mod tests;

use rocket::response::content;

#[get("/hello/<name>/<age>")]
fn hello(name: &str, age: i8) -> String {
    format!("Hello, {} year old named {}!", age, name)
}

#[error(404)]
fn not_found(req: &rocket::Request) -> content::HTML<String> {
    content::HTML(format!("<p>Sorry, but '{}' is not a valid path!</p>
            <p>Try visiting /hello/&lt;name&gt;/&lt;age&gt; instead.</p>",
            req.uri()))
}

fn main() {
    rocket::ignite()
        .mount("/", routes![hello])
        .catch(errors![not_found])
        .launch();
}
```

### Failure

我们也可以手动请求捕获器。

```rust
#[get("/")]
fn just_fail() -> Failure {
    Failure(Status::NotAcceptable)
}
```

### 返回 JSON

```rust
#[derive(Serialize)]
struct Task { ... }

#[get("/todo")]
fn todo() -> JSON<Task> { ... }

#[error(404)]
fn not_found() -> JSON<Value> {
    JSON(json!({
        "status": "error",
        "reason": "Resource was not found."
    }))
}
```

### 模版 Templates

Rocket 内置了模版引擎，可以通过配置 `template_dir` 设置模版目录，默认目录为`templates/`：

```rust
use rocket_contrib::Template;

#[derive(Serialize)]
struct TemplateContext {
    name: String,
    items: Vec<String>
}

#[get("/hello/<name>")]
fn get(name: String) -> Template {
    let context = TemplateContext {
        name: name,
        items: vec!["One", "Two", "Three"].iter().map(|s| s.to_string()).collect()
    };

    Template::render("index", &context)
}
```

模版文件`index.html.hbs`：

```html

<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8" />
    <title>Handlebars Demo</title>
  </head>
  <body>
    <h1>Hi {{name}}</h1>
    <h3>Here are your items:</h3>
    <ul>
      {{#each items}}
        <li>{{this}}</li>
      {{/each}}
    </ul>

    <p>Try going to <a href="/hello/YourName">/hello/YourName</a></p>
  </body>
</html>
```

### 使用 Streaming 返回数据

当数据很大的时候，将数据流到客户端，以免消耗大量内存：
```rust
#[get("/stream")]
fn stream() -> io::Result<Stream<UnixStream>> {
    UnixStream::connect("/path/to/my/socket").map(|s| Stream::from(s))
}
```
完整的例子：
```rust
#![feature(plugin)]
#![plugin(rocket_codegen)]

extern crate rocket;

use rocket::response::{content, Stream};

use std::io::{self, repeat, Repeat, Read, Take};
use std::fs::File;

type LimitedRepeat = Take<Repeat>;

#[get("/")]
fn root() -> content::Plain<Stream<LimitedRepeat>> {
    content::Plain(Stream::from(repeat('a' as u8).take(25000)))
}

#[get("/big_file")]
fn file() -> io::Result<Stream<File>> {
    // Generate this file using: head -c BYTES /dev/random > big_file.dat
    const FILENAME: &'static str = "big_file.dat";
    File::open(FILENAME).map(|file| Stream::from(file))
}

fn main() {
    rocket::ignite().mount("/", routes![root, file]).launch();
}
```

## State

许多Web应用程序需要维护状态。这可以像维护计数器的访问次数一样简单，也可以与访问工作队列和多个数据库一样复杂。
Rocket 提供的工具，以一个简单安全的方式实现它们。

### 管理状态 Managed State

使用过程很简单：

- 使用`manage` 初始状态值。
- 向任何请求处理程序中添加 State<T> 类型参数。

```rust
use std::sync::atomic::{AtomicUsize, Ordering};

use rocket::State;
use rocket::response::content;

struct HitCount(AtomicUsize);

fn rocket() -> rocket::Rocket {
    rocket::ignite()
        .manage(HitCount(AtomicUsize::new(0)))
}
```

### 增加状态 Adding State

可以调用`manage` 多次，来管理多个状态，假如我们要管理`HitCount` 和 `Config`：

```rust
rocket::ignite()
  .manage(HitCount(AtomicUsize::new(0)))
  .manage(Config::from(user_input));
```

### 检索状态 Retrieving State

```rust
#[get("/count")]
fn count(hit_count: State<HitCount>) -> String {
    let current_count = hit_count.0.load(Ordering::Relaxed);
    format!("Number of visits: {}", current_count)
}
```

可以获取多个状态：
```rust
#[get("/state")]
fn state(hit_count: State<HitCount>, config: State<Config>) -> T { ... }
```

我们可以通过 `FromRequest` 获取状态，调用方法`from_request`：
```rust
fn from_request(req: &'a Request<'r>) -> request::Outcome<T, ()> {
    let count = match <State<HitCount> as FromRequest>::from_request(req) {
        Outcome::Success(count) => count,
        ...
    };
    ...
}
```

## Unmanaged State

下面的代码，Rust 无法找到`HitCount`的管理，会出现错误：
```rust
#[get("/count")]
fn count(hit_count: State<HitCount>) -> String {
    let current_count = hit_count.0.load(Ordering::Relaxed);
    format!("Number of visits: {}", current_count)
}

fn main() {
    rocket::ignite()
        .manage(Config::from(user_input))
        .launch()
}
```

给路由增加属性`#[allow(unmanaged_state)]`，或者增加 crate 属性`#![allow(unmanaged_state)]`。

## 测试

创建实例：
```rust
let rocket = rocket::ignite().mount("/", routes![super::hello]);
```

创建请求：
```rust
let mut req = MockRequest::new(Method::Get, "/");
```

发送请求：
```rust
let mut response = req.dispatch_with(&rocket);
```

验证结果：
```rust
assert_eq!(response.status(), Status::Ok);

let body_str = response.body().and_then(|b| b.into_string());
assert_eq!(body_str, Some("Hello, world!".to_string()));
```

完整源码： https://github.com/SergioBenitez/Rocket/blob/v0.2.8/examples/testing/src/main.rs

## 连接数据库

使用第三方库：
- ORM https://github.com/diesel-rs/diesel
- PostgreSQL https://github.com/sfackler/rust-postgres
