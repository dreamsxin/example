## 从零开始：安装 Tauri CLI → 创建项目 → 编译

### 第一步：安装 Tauri CLI

```bash
cargo install tauri-cli --locked
```

> 已安装可跳过（当前环境已有 tauri-cli 2.10.1）。

---

### 第二步：创建新项目

**方式 A — 纯 Rust 后端（无前端框架，推荐新手）**

```bash
cargo new my-app
cd my-app
cargo tauri init
```

`cargo tauri init` 会交互式询问：

| 提示 | 说明 | 示例值 |
|------|------|--------|
| App name | 应用显示名称 | `My App` |
| Window title | 主窗口标题 | `My App` |
| Where are your web assets? | 前端资源目录（相对于 src-tauri） | `../dist` |
| What is your dev server URL? | 开发时前端地址（无框架填空） | 留空回车 |
| What is your frontend dev command? | 启动前端的命令（无框架留空） | 留空回车 |
| What is your frontend build command? | 构建前端的命令（无框架留空） | 留空回车 |

初始化后项目结构：
```
my-app/
├── src/                  # Rust 库/应用代码
├── src-tauri/
│   ├── src/main.rs       # Tauri 入口
│   ├── Cargo.toml
│   ├── tauri.conf.json
│   ├── capabilities/
│   └── icons/
├── dist/                 # 前端资源（手动创建或构建生成）
└── Cargo.toml
```

**方式 B — 用官方模板（包含前端框架）**

```bash
# React + Vite + Tauri
cargo tauri create --template react-ts my-app

# Vue + Vite + Tauri
cargo tauri create --template vue-ts my-app

# 或用 npm create
npm create tauri-app@latest my-app
```

---

### 第三步：配置 `tauri.conf.json`（关键）

`src-tauri/tauri.conf.json` 最小可工作配置：

```json
{
  "$schema": "https://schema.tauri.app/config/2",
  "productName": "My App",
  "identifier": "com.example.myapp",
  "version": "0.1.0",
  "build": {
    "frontendDist": "../dist"
  },
  "app": {
    "withGlobalTauri": true,
    "windows": [
      {
        "label": "main",
        "title": "My App",
        "width": 800,
        "height": 600
      }
    ]
  },
  "bundle": {
    "targets": "all",
    "icon": [
      "icons/icon.ico",
      "icons/icon.png",
      "icons/icon.icns"
    ]
  }
}
```

---

### 第四步：生成图标（macOS 必须有 .icns）

```bash
# 准备一张 1024x1024 的 PNG，让 Tauri 自动生成所有尺寸
cargo tauri icon path/to/your-icon.png
```

会在 `src-tauri/icons/` 下自动生成 `.icns`、`.ico`、各尺寸 `.png`。

---

### 第五步：开发模式运行

```bash
cargo tauri dev
```

热重载，修改 Rust 代码后自动重编。

---

### 第六步：构建打包（生成 .app / .dmg / .exe / .msi）

```bash
# 构建当前平台的所有格式
cargo tauri build

# 仅构建指定格式
cargo tauri build --bundles dmg        # macOS DMG
cargo tauri build --bundles app        # macOS .app
cargo tauri build --bundles msi        # Windows MSI
cargo tauri build --bundles nsis       # Windows NSIS 安装包
cargo tauri build --bundles deb        # Linux .deb
```

输出目录：
```
src-tauri/target/release/bundle/
├── macos/        → My App.app  /  My App.dmg
├── windows/      → My App.msi  /  setup.exe
└── deb/          → my-app.deb
```

---

### 平台前置依赖（macOS 特别说明）

| 依赖 | 安装方式 |
|------|---------|
| Xcode Command Line Tools | `xcode-select --install` |
| Rust target（Apple Silicon）| `rustup target add aarch64-apple-darwin` |
| Rust target（Intel Mac）| `rustup target add x86_64-apple-darwin` |
| 通用二进制（两架构都打）| `cargo tauri build --target universal-apple-darwin` |

不需要代码签名也可以构建 `.app` / `.dmg`，但分发给他人时 macOS Gatekeeper 会拦截，需要在 `tauri.conf.json` 的 `macOS.signingIdentity` 填入证书身份或用 `codesign` 手动签名。

---

### 一键命令速查

```bash
# 安装 CLI
cargo install tauri-cli --locked

# 新建项目
cargo new my-app && cd my-app && cargo tauri init

# 生成图标
cargo tauri icon icon.png

# 开发调试
cargo tauri dev

# 发布构建
cargo tauri build

# macOS 通用包（arm64 + x86_64）
cargo tauri build --target universal-apple-darwin
```所有可在 Windows 上直接修复的问题已修复。以下是完整问题清单和处理状态：

---

## 问题汇总

### 已修复（`tauri.conf.json`）

| # | 问题 | 修复内容 |
|---|------|---------|
| 1 | `category` 使用了 UTI 格式 `public.app-category.developer-tool` | 改为 Tauri 标准值 `"DeveloperTools"` |
| 2 | `icon` 列表缺少 macOS 必需的 `.icns` 文件 | 添加了 `"icons/icon.icns"` |
| 3 | `resources` 把所有平台库都打包进 macOS bundle | 改为 `"libs/macos-arm64/*"` 仅打包 macOS 库 |
| 4 | `exceptionDomain` 设为空字符串 | 改为 `null` |
| 5 | `dist/` 目录不存在导致 frontendDist 找不到 | 创建了 `dist/index.html` 功能页面 |

---

### 仍需在 macOS 上手动完成

**生成 `icons/icon.icns`**（必须在 macOS 上执行）：

```bash
cd /path/to/brosdk-sdk-rust
mkdir icon.iconset
sips -z 16 16     icons/icon.png --out icon.iconset/icon_16x16.png
sips -z 32 32     icons/icon.png --out icon.iconset/icon_16x16@2x.png
sips -z 32 32     icons/icon.png --out icon.iconset/icon_32x32.png
sips -z 64 64     icons/icon.png --out icon.iconset/icon_32x32@2x.png
sips -z 128 128   icons/icon.png --out icon.iconset/icon_128x128.png
sips -z 256 256   icons/icon.png --out icon.iconset/icon_128x128@2x.png
sips -z 256 256   icons/icon.png --out icon.iconset/icon_256x256.png
sips -z 512 512   icons/icon.png --out icon.iconset/icon_256x256@2x.png
sips -z 512 512   icons/icon.png --out icon.iconset/icon_512x512.png
sips -z 1024 1024 icons/icon.png --out icon.iconset/icon_512x512@2x.png
iconutil -c icns icon.iconset -o icons/icon.icns
rm -rf icon.iconset
```

> **或者**直接使用 Tauri 官方工具自动生成所有尺寸图标：
> ```bash
> cargo tauri icon icons/icon.png
> ```
> 此命令会自动在 `icons/` 目录下生成 `.icns`、`.ico`、各尺寸 `.png`，无需手动操作。

之后运行打包：
```bash
cargo tauri build
```
