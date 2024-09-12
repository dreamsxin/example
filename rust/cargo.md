
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
