# Rust and WebAssembly

https://github.com/rustwasm/wasm-bindgen

## 准备工作

### 安装 wasm-pack

wasm-pack 是构建、测试和发布 Rust生成的 WebAssembly 的一站式商店。

https://rustwasm.github.io/wasm-pack/installer/

```shell
cargo install wasm-pack
```

### 安装 cargo-generate

通过利用预先存在的 git 存储库作为模板，帮助您快速启动并运行新的 Rust 项目。

```shell
cargo install cargo-generate
```

### 安装 npm


```shell
cargo generate --git https://github.com/rustwasm/wasm-pack-template
```
