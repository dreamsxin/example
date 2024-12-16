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


## hello world

https://rustwasm.github.io/docs/book/game-of-life/hello-world.html

```shell
cargo generate --git https://github.com/rustwasm/wasm-pack-template
```
输入项目名 `wasm-game-of-life`

```shell
cd wasm-game-of-life
```

**wasm-game-of-life/Cargo.toml**
`Cargo.toml` 定义 dependencies 依赖关系 `wasm-bindgen`，定义 crate-type 为 `["cdylib", "rlib"]` 用于生成 `.wasm` 库.

**wasm-game-of-life/src/lib.rs**
导入 js 的 alert 函数，导出 rust 的 greet 函数。
```rs
mod utils;

use wasm_bindgen::prelude::*;

// When the `wee_alloc` feature is enabled, use `wee_alloc` as the global
// allocator.
#[cfg(feature = "wee_alloc")]
#[global_allocator]
static ALLOC: wee_alloc::WeeAlloc = wee_alloc::WeeAlloc::INIT;

#[wasm_bindgen]
extern {
    fn alert(s: &str);
}

#[wasm_bindgen]
pub fn greet() {
    alert("Hello, wasm-game-of-life!");
}
```

**构建**
1. cargo 将 rust 代码，编译为 WebAssembly 的二进制文件。
2. wasm-bindgen 生成 js 接口文件，调用 WebAssembly。
```shell
wasm-pack build
```
生成 pkg 目录。
- wasm-game-of-life/pkg/wasm_game_of_life.js
- wasm-game-of-life/pkg/wasm_game_of_life_bg.wasm
- wasm-game-of-life/pkg/wasm_game_of_life_bg.ts

**使用**
创建新的子目录 `wasm-game-of-life/www`
```shell
npm init wasm-app www
```
