
- https://github.com/mmadfox/go-crx3
- https://github.com/Cryptoistaken/CrxPull

### ⚙️ 各工具一览
| 工具名称 | 核心功能 | 主要支持格式 | 技术/使用方式 | 适用人群 |
| :--- | :--- | :--- | :--- | :--- |
| **crxpull** | 下载CRX文件并自动解压为ZIP和源码文件夹 | CRX, ZIP, 源码 | npm包 / npx命令行 | 开发者和研究人员 |
| **chrome-extension-fetch** | 轻量级CRX下载及ZIP转换 | CRX, ZIP | npm包 / 命令行 | 开发者和普通用户 |
| **crx3** | CRX3打包/解包/下载，支持AI (MCP) | CRX3, ZIP | Go 命令行工具 (CLI) | 开发者、高级用户 |
| **CRX Extractor** | 提取CRX文件内容(解包) | 源码 | npm包 | 开发者 (分析/修改扩展) |
| **uncrx-rs** | 将CRX扩展转换为ZIP文件 | ZIP | Rust库 (crates.io) | Rust开发者 |
| **chromium-web-store** | 为受限浏览器提供扩展商店功能并管理更新 | CRX | 浏览器扩展 | 使用Ungoogled Chromium等受限浏览器的用户 |
| **Extension Downloader** | 跨浏览器扩展批量下载和管理 | 各浏览器扩展 | Rust / Go 命令行工具 | 系统管理员、高级用户 |
| **CRX下载器** | 从Edge商店等下载扩展源代码 | CRX, ZIP, XPI | 浏览器扩展 (Edge) | 主要使用Edge浏览器的用户 |
| **Chrome Extension Downloader** | 下载CRX/ZIP格式的Chrome和Edge扩展 | CRX, ZIP | 浏览器扩展 | 普通用户 |

### 🛠️ 使用指南与注意事项

选择合适的工具后，可以按以下指南开始使用：

#### 快速上手指南
1.  **准备扩展ID**：在使用绝大多数工具前，你需要知道目标插件的扩展ID。访问Chrome网上应用店，找到你想要的插件，其链接URL末尾的一长串字符通常就是扩展ID。
2.  **选择工具并执行**：根据你的技术背景和操作环境选择合适的工具。

| 工具名称 | 使用示例 |
| :--- | :--- |
| **crxpull** | `npx crxpull cjpalhdlnbpafiamejdnhcphjbkeiagm`（下载uBlock Origin） |
| **crx3** | `crx3 download cjpalhdlnbpafiamejdnhcphjbkeiagm` |
