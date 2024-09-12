
- env_logger
其底层依赖于 log 库，用于实际的日志输出。它允许开发者通过环境变量来配置日志级别和输出格式。这个库是 Rust 生态系统中广泛使用的日志记录工具之一，特别适合在命令行应用（CLI）中使用。

## 使用

```shell
cargo add log env_logger
```

```rust 
use log::{info, error};
 
fn main() {
    env_logger::init();
 
    info!("这是一个信息级别的日志");
    error!("这是一个错误级别的日志");
}

// or
fn main() {
    env_logger::init();
 
    log::info!("这是一个信息级别的日志");
    log::error!("这是一个错误级别的日志");
}
```
## 显示级别
通过设置 RUST_LOG 环境变量，你可以控制显示哪些日志信息。

PowerShell
```shell
$env:RUST_LOG = "debug"
cargo run
```

Linux
```shell
RUST_LOG=debug cargo run
```
