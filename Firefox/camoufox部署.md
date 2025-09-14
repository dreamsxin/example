# Camoufox Server 部署与测试文档

## 项目简介

Camoufox Server 是一个基于 Playwright 的浏览器服务器，提供 WebSocket 连接端点，支持连接到中心调度服务器并上报状态信息。本项目适用于需要模拟浏览器行为、绕过自动化检测的场景。

## 环境要求

- Node.js 14+ 和 npm 6+
- Camoufox 浏览器（基于 Firefox 的定制浏览器）
- Windows 或 Linux 操作系统

## 安装步骤

### 1. 克隆或下载项目

将项目文件复制到本地工作目录。

### 2. 安装 Node.js 依赖

在项目根目录执行以下命令：

```bash
npm install
```

项目依赖包括：

- playwright: ^1.55.0
- httpserver: ^0.3.0

### 3. 安装 Camoufox 浏览器

- **Windows**: 下载 Camoufox 浏览器并安装到本地目录，如 `E:\soft\camoufox-135.0.1-beta.24-win.x86_64\`
- **Linux**: 将 Camoufox 浏览器安装到 `/opt/camoufox/` 目录

## 配置文件

在项目根目录创建 `config.json` 文件，内容如下：

```json
{
  "centralServer": {
    "url": "wss://your-central-server.com/ws",
    "heartbeatInterval": 30000,
    "reconnectInterval": 5000,
    "jwtToken": "your-secure-jwt-token-here"
  },
  "browser": {
    "port": 9222,
    "headless": false
  }
}
```

## 代码文件配置

### Windows 环境配置

确保 `camoufox-server.mjs` 中的浏览器路径配置正确：

```javascript
const browser = await firefox.launchServer({
  executablePath: "E:\\soft\\camoufox-135.0.1-beta.24-win.x86_64\\camoufox.exe",
  // 其他配置...
});
```

### Linux 环境配置

在 Linux 环境下，需要修改浏览器路径和运行模式：

```javascript
const browser = await firefox.launchServer({
  executablePath: "/opt/camoufox/camoufox",
  headless: config.browser.headless, // 从配置文件读取无头模式设置
  // 其他配置...
});
```

## 运行方法

### 直接运行

在项目根目录执行：

```bash
node camoufox-server.mjs
```

### 使用 PM2 进程管理（推荐）

1. 安装 PM2：

```bash
npm install -g pm2
```

2. 使用 PM2 启动服务：

```bash
pm2 start camoufox-server.mjs
```

3. 设置 PM2 开机自启：

```bash
pm2 startup
pm2 save
```

## Windows 部署指南

### 1. 准备环境

- 安装 Node.js 14+ 和 npm
- 下载并安装 Camoufox 浏览器
- 安装项目依赖

### 2. 配置文件设置

创建 `config.json` 文件，根据实际需求调整配置参数。

### 3. 防火墙设置

确保 Windows 防火墙允许端口 9222 入站连接。

### 4. 服务管理

可使用批处理文件或计划任务实现开机自启。

## Linux 部署指南

### 1. 环境准备

```bash
# 安装 Node.js 和 npm
sudo apt update
sudo apt install -y nodejs npm

# 创建 Camoufox 安装目录
sudo mkdir -p /opt/camoufox
sudo chown -R $USER:$USER /opt/camoufox

# 复制 Camoufox 浏览器文件到安装目录
# 假设 Camoufox 解压在当前目录
cp -r camoufox-*/* /opt/camoufox/
```

### 2. 项目部署

```bash
# 创建项目目录
mkdir -p /opt/playwright-demo
cd /opt/playwright-demo

# 复制项目文件（使用 SCP 从 Windows 复制或直接下载）
scp -r user@windows-machine:/path/to/playwright-demo/* .

# 安装依赖
npm install
```

### 3. 配置调整

- 修改 `config.json`，设置 `centralServer.url` 和 `browser.headless: true`
- 更新 `camoufox-server.mjs` 中的 `executablePath` 为 `/opt/camoufox/camoufox`

### 4. 防火墙配置

```bash
sudo ufw allow 9222/tcp
sudo ufw reload
```

### 5. 服务管理

#### 使用 PM2

```bash
# 安装 PM2
npm install -g pm2

# 启动服务
pm2 start camoufox-server.mjs

# 设置开机自启
pm2 startup
sudo env PATH=$PATH:/usr/bin /usr/lib/node_modules/pm2/bin/pm2 startup systemd -u $USER --hp /home/$USER
pm2 save
```

#### 使用 systemd

创建服务文件 `/etc/systemd/system/camoufox-server.service`：

```ini
[Unit]
Description=Camoufox Server
After=network.target

[Service]
Type=simple
User=your-user
WorkingDirectory=/opt/playwright-demo
ExecStart=/usr/bin/node camoufox-server.mjs
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

启动服务并设置开机自启：

```bash
sudo systemctl daemon-reload
sudo systemctl start camoufox-server
sudo systemctl enable camoufox-server
sudo systemctl status camoufox-server
```

## 测试方法

### 基本连接测试

1. 启动 Camoufox Server
2. 检查控制台输出的 WebSocket 端点
3. 使用 Playwright 客户端连接测试：

```javascript
const browser = await chromium.connect("ws://localhost:9222/your-websocket-id");
// 进行简单的页面访问测试
const page = await browser.newPage();
await page.goto("https://example.com");
console.log("Page title:", await page.title());
await browser.close();
```

### 功能测试

1. 验证中心服务器连接状态（含 JWT 验证）
2. 测试浏览器自动化功能
3. 检查心跳机制是否正常工作

### 性能测试

模拟高并发请求，测试服务器稳定性和响应速度。

## 日志管理

Camoufox Server 的运行日志会输出到控制台，可通过以下方式保存：

```bash
# Windows
node camoufox-server.mjs > server.log 2>&1

# Linux
node camoufox-server.mjs >> server.log 2>&1
```

如果使用 PM2，可通过以下命令查看日志：

```bash
pm2 logs camoufox-server
```

## 故障排查

### 常见问题

1. **无法启动浏览器**：检查 `executablePath` 是否正确，确保 Camoufox 浏览器安装完整
2. **端口被占用**：修改 `config.json` 中的 `browser.port` 为其他可用端口
3. **中心服务器连接失败**：检查网络连接、`centralServer.url` 配置和 JWT token 是否正确
4. **浏览器自动化检测问题**：参考 `chrome.mjs` 中的参数配置，增强浏览器的隐蔽性

## 维护与更新

### 定期更新

- 检查并更新 Node.js 依赖：`npm update`
- 关注 Camoufox 浏览器的更新版本
- 定期备份配置文件和日志

### 服务重启

```bash
# 使用 PM2
pm2 restart camoufox-server

# 使用 systemd
sudo systemctl restart camoufox-server
```

## 安全注意事项

1. 避免在生产环境中使用 `--disable-web-security` 等不安全的浏览器参数
2. 确保中心服务器连接使用安全的 WebSocket（wss://）
3. 定期更新依赖库，避免安全漏洞
4. 保护 JWT token，避免泄露
5. 限制对浏览器服务器端口的访问权限
