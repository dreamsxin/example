
- cargo build 可以构建项目
- cargo run 可以运行项目
- cargo test 可以测试项目
- cargo doc 可以为项目构建文档
- cargo publish 可以将库发布到 crates.io。
- 
## 创建运行项目
```shell
cargo new hello-rust
cargo run
```

## 添加依赖

**Rust 包的仓库地址**

- https://crates.io/

使用命令
```shell
cargo add enigo
```
或直接修改 `Cargo.toml` 添加信息：
```toml
[dependencies]
enigo = "0.2.1"
```

#‌# crate-type
用于指定编译单元的类型，主要有两种类型：lib 和 bin。

-‌ lib‌：当使用crate-type = "lib"时，编译器会生成一个库文件（.rlib），这个库文件可以被其他 crate 引用和使用。
-‌ ‌bin‌：当使用crate-type = "bin"时，编译器会生成一个可执行文件。这种类型的crate包含一个main函数，可以直接运行‌。
