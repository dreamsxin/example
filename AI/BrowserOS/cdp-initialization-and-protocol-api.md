# BrowserOS CDP 服务初始化与连接建立

> 梳理 Chromium CDP 服务初始化、Agent 连接建立、ProtocolApi 自动生成的完整技术链路

---

## 1. 架构概览

BrowserOS 的 CDP (Chrome DevTools Protocol) 通信体系分为两层：

```
┌─────────────────────────────────────────────────────────┐
│                   Chromium 进程 (C++)                    │
│  BrowserOSServerManager → DevToolsAgentHost             │
│  ├─ HTTP /json/version (端口发现)                       │
│  └─ WS /devtools/page/{id} (CDP 双向通信)               │
├─────────────────────────────────────────────────────────┤
│                   Agent 进程 (TypeScript)                │
│  CdpBackend.connect() → fetchVersionFromHost()         │
│  → WebSocket → createProtocolApi(send, on)             │
│  → ProtocolApi (55 个域, 动态 Proxy 绑定)               │
└─────────────────────────────────────────────────────────┘
```

---

## 2. CDP 初始化生命周期（6 阶段）

### 阶段 1: Chromium 启动触发

**文件:** `packages/browseros/chromium_patches/chrome/browser/chrome_browser_main.cc`

Chromium 的 `BrowserMainLoop` 完成后，自动调用：

```cpp
// browseros_server_manager.cc:129
void BrowserOSServerManager::Start() {
  // 1. 检查 --disable-browseros-server 开关（如果禁用则退出）
  if (command_line->HasSwitch(browseros::kDisableServer)) return

  // 2. HandleOrphanProcess() — 清理上次崩溃残留的 sidecar 进程
  HandleOrphanProcess()

  // 3. 状态转换 + 写崩溃恢复文件
  state_ = BrowserOSServerState::kStarting
  creation_time_ = base::Time::Now().InMillisecondsSinceUnixEpoch()
  server_utils::WriteStateFile({ pid, creation_time })

  // 4. 核心三步
  ConfigurePorts()                  // → 解析端口
  StartRemoteDebuggingServer()      // → 启动 CDP HTTP/WS 服务器
  ScheduleSidecarServerStart()      // → 启动 sidecar node 进程
}
```

**状态机:**

```
kStopped  →  kStarting  →  kRunning  →  kStopped
  ↑          (Start)      (sidecar 就绪)   (Stop)
```

---

### 阶段 2: 端口分配（4 级优先级）

**文件:** `browseros_server_manager.cc:256`

```cpp
void BrowserOSServerManager::ConfigurePorts() {
  int cdp_port;

  // 优先级 1 (最高): 标准 Chromium 开关
  if (command_line->HasSwitch("remote-debugging-port")) {
    cdp_port = ParseInt(switch_value)
    if (cdp_port != 0) {
      ports_.cdp = cdp_port  // 精确使用，不探测
      return
    }
  }

  // 优先级 2: BrowserOS 自定义开关
  if (command_line->HasSwitch("browseros-cdp-port"))
    cdp_port = ParseInt(switch_value)

  // 优先级 3: 上次保存的端口 pref
  cdp_port = local_state->GetInteger(kCDPServerPort)

  // 优先级 4: 默认值 9100
  if (cdp_port == 0) cdp_port = kDefaultCDPPort  // 9100

  // 自动端口探测: 从 start 开始，最多 100 个连续端口
  ports_.cdp = FindAvailablePort(cdp_port, excluded, /*allow_reuse=*/false)
}
```

**默认端口常量** (`browseros_server_prefs.h:19`):

| 服务 | 端口 | 说明 |
|------|------|------|
| CDP | 9100 | Chrome DevTools Protocol |
| Proxy | 9000 | 浏览器代理 |
| Server | 9200 | Agent HTTP 服务 |
| Extension | 9300 | 扩展通信 |

**FindAvailablePort 实现** (`browseros_server_utils.cc:77`):

```cpp
int FindAvailablePort(int start, excluded_set, allow_reuse) {
  for (int i = 0; i < 100; i++) {          // 最多 100 次
    int port = start + i
    if (port > 65535) break                // 超出范围
    if (excluded.count(port)) continue     // 跳过已占用的

    if (IsPortAvailable(port, allow_reuse))
      return port                          // 找到可用端口
  }
  return start  // 全部失败→回退到原始端口（仅警告）
}

bool IsPortAvailable(int port, allow_reuse) {
  // 验证: !IsWellKnownPort && IsPortAllowedForScheme("http")
  // TCP bind: IPv4 localhost + IPv6 localhost
  // 不允许端口重用 (allow_reuse=false) 以保证精确检测
}
```

---

### 阶段 3: CDP 服务器绑定

**文件:** `browseros_server_manager.cc:436`

```cpp
void BrowserOSServerManager::StartRemoteDebuggingServer() {
  // 模式分支:
  //   - 标准模式: --remote-debugging-port → 启动 HTTP/WS 服务器
  //   - Agent CDP 模式: Agent 自提供 CDP → Unix domain socket pipe

  if (agent_cdp_mode) {
    // Pipe 模式: 注册 CDP socket factory
    content::DevToolsAgentHost::StartRemoteDebuggingPipeHandler(
        base::BindOnce(&BrowserOSCDPSocketFactory::Create))
    return
  }

  // 标准模式: 使用 Chromium 原生 API
  content::DevToolsAgentHost::StartRemoteDebuggingServer(
      ports_.cdp,    // e.g., 9100
      "0.0.0.0"      // 绑定所有接口
  )

  // Chromium 内部自动创建 net::HttpServer，注册路由:
  //
  //  GET /json/version
  //    → 返回:
  //    {
  //      "Browser": "BrowserOS/134.0.0.0",
  //      "Protocol-Version": "1.3",
  //      "webSocketDebuggerUrl": "ws://127.0.0.1:9100/devtools/browser/{guid}"
  //    }
  //
  //  GET /json/list
  //    → 返回可调试 target 列表
  //    [{ "id": "...", "type": "page", "url": "about:blank", ... }]
  //
  //  WS /devtools/page/{targetId}
  //    → WebSocket 升级，CDP 双向帧通信
}
```

---

### 阶段 4: Agent 端连接（重试与超时）

**文件:** `packages/browseros-agent/apps/server/src/browser/backends/cdp.ts`

```typescript
// cdp.ts:63
async connect(): Promise<void> {
  if (this.connected) this.disconnect()   // 如果已连接则断开

  this.signal = Promise.withResolvers()   // 用于中断
  this.epoch++                            // 递增连接代数

  // Step 1: 发现
  const { host, port } = this.config
  this.host = host ?? '127.0.0.1'
  this.wsBaseUrl = `ws://${this.host}:${port}`

  // Step 2: HTTP GET /json/version
  this.version = await this.fetchVersionFromHost()

  // Step 3: 建立 WebSocket
  const wsUrl = new URL(this.version.webSocketDebuggerUrl)
  const ws = new WebSocket(wsUrl)
  this.ws = ws

  ws.addEventListener('message', handleMessage)
  ws.addEventListener('close', handleClose)
  ws.addEventListener('error', handleError)

  await once(ws, 'open')                  // 阻塞等待连接
  this.connected = true
}
```

**fetchVersionFromHost 重试逻辑** (`cdp.ts:94`):

```typescript
async fetchVersionFromHost(): Promise<DevToolsVersionInfo> {
  const url = `http://${this.host}:${this.port}/json/version`

  for (let attempt = 0; attempt < 10; attempt++) {   // 最多 10 次
    try {
      const resp = await fetch(url, {
        signal: AbortSignal.timeout(5000),            // 5s 超时
      })
      const json = await resp.json()
      if (!json.webSocketDebuggerUrl)
        throw new Error('Missing webSocketDebuggerUrl')
      return json
    } catch (err) {
      if (attempt < 9) {
        await Promise.race([
          sleep(1000),               // 1s 重试间隔
          this.signal.promise,       // 中断信号
        ])
      }
    }
  }
  throw new Error(`Failed after 10 attempts`)
}
```

**连接常量:**

| 常量 | 值 | 说明 |
|------|-----|------|
| CDP_CONNECT_MAX_ATTEMPTS | 10 | 最大重试次数 |
| CDP_CONNECT_RETRY_DELAY | 1000ms | 重试间隔 |
| CDP_VERSION_FETCH_TIMEOUT | 5000ms | 单次请求超时 |
| CDP_MAX_MESSAGE_SIZE | 256MB | WS 消息大小上限 |

---

### 阶段 5: 请求/响应机制

**文件:** `cdp.ts:223`（内部实现）

```typescript
// 每条命令: id 自增 → pending Map → WS send → 匹配 id resolve
private sendRequest(method: string, params?: Record<string, unknown>) {
  const id = this.nextId++
  const msg = JSON.stringify({ id, method, params })

  return new Promise((resolve, reject) => {
    this.pending.set(id, { resolve, reject })
    this.ws!.send(msg)
  })
}

// WS 消息路由:
ws.onmessage = (event) => {
  const data = JSON.parse(event.data)
  if (data.id !== undefined) {
    // 命令响应: 匹配 pending Map
    const p = this.pending.get(data.id)
    if (data.error) p.reject(new CDPError(data.error))
    else p.resolve(data.result)
  } else {
    // CDP 事件推送: 分发到注册的 handler
    this.dispatchEvent(data.method, data.params)
  }
}
```

**第一条 CDP 命令**发生在 `new Browser(cdp)` 中：

```typescript
// browser.ts
const targets = await cdp.Target.getTargets({})
//  → send("Target.getTargets", {})
//  → id=1, ws.send('{"id":1,"method":"Target.getTargets","params":{}}')
//  → 收到 '{"id":1,"result":{"targetInfos":[...]}}'
//  → pending.delete(1), resolve(result)
```

---

### 阶段 6: 核心子系统管线

**文件:** `packages/browseros-agent/apps/server/src/browser/browser.ts`

```typescript
export class Browser {
  readonly cdp: CdpBackend

  constructor(cdp: CdpBackend) {
    this.cdp = cdp
    this.session = cdp.session('browser-session')

    // 懒初始化核心子系统（按依赖顺序）:
    //   1. StorageManager       — 本地存储
    //   2. ContextManager       — 浏览器上下文
    //   3. PageManager          — 页面会话管理
    //   4. NetworkManager       — 网络拦截
    //   5. BrowserContextProxy  — 上下文代理
    //   6. ExtensionsManager    — Agent 扩展管理
    //   7. ConsoleManager       — 控制台日志
    //   8. OverlayManager       — 元素高亮
    //   9. InspectorManager     — 元素检查
    //  10. WindowManager        — 窗口管理
    //  11. TargetManager        — CDP target 管理
    //  12. TabsManager          — 标签页管理
    //  13. BookmarkManager      — 书签管理
  }
}
```

---

## 3. ProtocolApi 自动生成系统

### 3.1 生成流程概览

```
Chromium 构建产物: protocol.json
        │  (由 inspector_protocol_config.json 配置)
        │  (包含 BrowserOS 自定义域: Bookmarks, History 等)
        ▼
┌──────────────────────────────────────────────┐
│         Codegen 脚本 (Bun + TypeScript)        │
│  scripts/codegen/cdp-protocol.ts              │
│                                               │
│  parseProtocol()  →  解析 protocol.json       │
│       │                                       │
│       ├── emitDomainFile()  →  domains/*.ts   │
│       ├── emitDomainApiFile()  →  domain-apis/*.ts │
│       ├── emitProtocolApiFile()  →  protocol-api.ts │
│       ├── emitCreateApiFile()  →  create-api.ts   │
│       └── writePackageJson()  →  更新 exports     │
└──────────────────────────────────────────────┘
        │
        ▼
 packages/cdp-protocol/src/generated/
  ├── domains/        55 个类型定义文件
  ├── domain-apis/    55 个 API 包装接口
  ├── protocol-api.ts 统一的 ProtocolApi 接口
  └── create-api.ts   动态 Proxy 工厂
```

### 3.2 协议源（Chromium 构建产物）

CDP 协议定义是一个 JSON 文件，由 Chromium 构建系统生成。BrowserOS 通过 patch `inspector_protocol_config.json` 扩展了原生协议，添加了自定义域和命令。

**文件:** `packages/browseros/chromium_patches/chrome/browser/devtools/inspector_protocol_config.json`

```json
// BrowserOS 向 Browser 域添加:
{
  "domain": "Browser",
  "commands": [
    "getWindows", "getActiveWindow", "createWindow", "closeWindow",
    "activateWindow", "setWindowVisibility", "getTabs", "getActiveTab",
    "getTabInfo", "createTab", "closeTab", "activateTab", "moveTab",
    "duplicateTab", "pinTab", "unpinTab", "showTab",
    "getTabGroups", "createTabGroup", "updateTabGroup", "closeTabGroup",
    "addTabsToGroup", "removeTabsFromGroup", "moveTabGroup"
    // ... 更多窗口/标签页管理命令
  ]
}

// BrowserOS 添加的全新域:
{
  "domain": "Bookmarks",
  "commands": [
    "getBookmarks", "searchBookmarks", "createBookmark",
    "updateBookmark", "moveBookmark", "removeBookmark"
  ]
}

{
  "domain": "History",
  "commands": [
    "search", "getRecent", "deleteUrl", "deleteRange"
  ]
}
```

**Protocol JSON 结构:**

```typescript
// scripts/codegen/lib/protocol-parser.ts
interface Protocol {
  version: { major: string; minor: string }
  domains: ProtocolDomain[]
}

interface ProtocolDomain {
  domain: string          // e.g., "Page", "Browser", "Bookmarks"
  description?: string
  dependencies?: string[]  // 跨域引用
  types?: ProtocolType[]   // 自定义类型 (枚举、对象等)
  commands?: ProtocolCommand[]  // RPC 方法
  events?: ProtocolEvent[]      // 推送事件
}

interface ProtocolCommand {
  name: string
  description?: string
  parameters?: ProtocolProperty[]
  returns?: ProtocolProperty[]
}
```

### 3.3 Codegen 脚本结构

**目录:** `packages/browseros-agent/scripts/codegen/`

| 文件 | 作用 |
|------|------|
| `cdp-protocol.ts` | 入口脚本，编排整个生成流程 |
| `lib/protocol-parser.ts` | 读取并验证 `protocol.json` |
| `lib/naming.ts` | 命名转换（domain → kebab-case, camelCase → PascalCase） |
| `lib/type-emitter.ts` | CDP 类型 → TypeScript 类型映射 |
| `lib/domain-emitter.ts` | 生成 `domains/*.ts`（类型定义） |
| `lib/domain-api-emitter.ts` | 生成 `domain-apis/*.ts`（API 包装接口） |
| `lib/protocol-api-emitter.ts` | 生成 `protocol-api.ts` + `create-api.ts` |

**运行命令:**

```bash
# 1. 构建 Chromium 生成 protocol.json
autoninja -C out/Default_arm64 chrome

# 2. 运行 codegen
CDP_PROTOCOL_JSON=/path/to/chromium/out/Default_arm64/gen/third_party/blink/public/devtools_protocol/protocol.json \
bun run gen:cdp
```

### 3.4 类型映射规则

**文件:** `scripts/codegen/lib/type-emitter.ts`

| CDP 类型 | TypeScript 类型 |
|----------|----------------|
| `string` | `string` |
| `integer` | `number` |
| `number` | `number` |
| `boolean` | `boolean` |
| `any` | `unknown` |
| `binary` | `string` |
| `object` (无属性) | `Record<string, unknown>` |
| `object` (有属性) | `interface { ... }` |
| `string` (有枚举) | `'v1' \| 'v2' \| 'v3'` |
| `array` | `T[]` |
| `$ref` (引用) | 引用的类型名 (如 `FrameId`) |

### 3.5 生成的文件结构（以 Page 域为例）

**`domains/page.ts`** — 类型定义（约 1166 行）:

```typescript
// ── AUTO-GENERATED from CDP protocol. DO NOT EDIT. ──

// 跨域依赖导入
import type { Debugger } from './debugger'
import type { DOM } from './dom'
import type { Network } from './network'
import type { Runtime } from './runtime'

// ── Types ──
export type FrameId = string
export type ScriptIdentifier = string

export interface Frame {
  id: FrameId
  parentId?: FrameId
  loaderId: Network.LoaderId
  name?: string
  url: string
  // ...
}

export interface Viewport {
  x: number
  y: number
  width: number
  height: number
  scale: number
}

// ── Commands ──
export interface NavigateParams {
  url: string
  referrer?: string
  transitionType?: 'link' | 'typed' | 'address_bar' | /* ... */
  frameId?: FrameId
}

export interface NavigateResult {
  frameId: FrameId
  loaderId?: Network.LoaderId
  errorText?: string
}

export interface CaptureScreenshotParams {
  format?: 'jpeg' | 'png' | 'webp'
  quality?: number
  clip?: Viewport
  fromSurface?: boolean
  captureBeyondViewport?: boolean
}

export interface CaptureScreenshotResult {
  data: string  // base64 编码
}

// ── Events ──
export interface FrameNavigatedEvent {
  frame: Frame
  type: 'Navigation'
}

export interface LoadEventFiredEvent {
  timestamp: Network.MonotonicTime
}

// ... 约 40+ 类型, 50+ 命令, 25+ 事件
```

**`domain-apis/page.ts`** — API 包装接口（约 298 行）:

```typescript
import type {
  NavigateParams, NavigateResult,
  CaptureScreenshotParams, CaptureScreenshotResult,
  FrameNavigatedEvent, LoadEventFiredEvent,
  // ...
} from '../domains/page'

export interface PageApi {
  // ── 命令方法 ──
  navigate(params: NavigateParams): Promise<NavigateResult>
  captureScreenshot(params?: CaptureScreenshotParams): Promise<CaptureScreenshotResult>
  printToPDF(params?: PrintToPDFParams): Promise<PrintToPDFResult>
  reload(params?: ReloadParams): Promise<void>
  // ... ~50 个命令方法

  // ── 事件方法 ──
  on(event: 'frameNavigated',
     handler: (params: FrameNavigatedEvent) => void): () => void
  on(event: 'loadEventFired',
     handler: (params: LoadEventFiredEvent) => void): () => void
  // ... ~25 个事件处理
}
```

### 3.6 统一 ProtocolApi 接口

**`protocol-api.ts`** — 组合所有 55 个域:

```typescript
// ── AUTO-GENERATED from CDP protocol. DO NOT EDIT. ──

import type { AccessibilityApi } from './domain-apis/accessibility'
import type { AnimationApi } from './domain-apis/animation'
import type { BookmarksApi } from './domain-apis/bookmarks'    // BrowserOS 自定义
import type { BrowserApi } from './domain-apis/browser'
import type { DOMApi } from './domain-apis/dom'
import type { HistoryApi } from './domain-apis/history'        // BrowserOS 自定义
import type { PageApi } from './domain-apis/page'
import type { RuntimeApi } from './domain-apis/runtime'
// ... 所有 55 个域的导入

export interface ProtocolApi {
  readonly Accessibility: AccessibilityApi
  readonly Animation: AnimationApi
  readonly Bookmarks: BookmarksApi
  readonly Browser: BrowserApi
  readonly DOM: DOMApi
  readonly History: HistoryApi
  readonly Page: PageApi
  readonly Runtime: RuntimeApi
  // ... 所有 55 个域
}
```

### 3.7 动态 Proxy 工厂（核心魔法）

**`create-api.ts`** — 只需两个原始回调，自动生成全部 55 个域的方法:

```typescript
export type RawSend = (
  method: string,
  params?: Record<string, unknown>,
) => Promise<unknown>

export type RawOn = (
  event: string,
  handler: (params: unknown) => void,
) => () => void

function createDomainProxy(
  domain: string,
  send: RawSend,
  on: RawOn,
): unknown {
  return new Proxy(Object.create(null), {
    get(_, method: string) {
      // 事件: .on('event', handler)
      if (method === 'on') {
        return (event: string, handler: (params) => void) =>
          on(`${domain}.${event}`, handler)
      }
      // 命令: .navigate({ url: "..." })
      return (params?: Record<string, unknown>) =>
        send(`${domain}.${method}`, params)
    },
  })
}

export function createProtocolApi(
  send: RawSend,
  on: RawOn,
): ProtocolApi {
  return {
    Accessibility: createDomainProxy('Accessibility', send, on),
    Animation:     createDomainProxy('Animation', send, on),
    Bookmarks:     createDomainProxy('Bookmarks', send, on),
    Browser:       createDomainProxy('Browser', send, on),
    DOM:           createDomainProxy('DOM', send, on),
    History:       createDomainProxy('History', send, on),
    Page:          createDomainProxy('Page', send, on),
    Runtime:       createDomainProxy('Runtime', send, on),
    // ... 所有 55 个域
  } as unknown as ProtocolApi
}
```

**Proxy 的工作原理:**

```
调用: cdp.Page.navigate({ url: "https://example.com" })
  │
  ▼
Proxy.get(target, "navigate")
  → method !== 'on'
  → 返回函数: (params) => send("Page.navigate", params)
  │
  ▼
send("Page.navigate", { url: "https://example.com" })
  → 内部: sendRequest("Page.navigate", { url: "..." })
  → id=42, ws.send('{"id":42,"method":"Page.navigate","params":{"url":"..."}}')
  │
  ▼
pending.get(42).resolve({ frameId: "...", loaderId: "..." })
```

每条 CDP 命令在运行时自动转换为 `"{domain}.{method}"` 格式，无需为每个方法单独编写代码。`domain-apis/*.ts` 中的类型定义仅用于编译时类型检查，运行时完全由 Proxy 动态路由。

### 3.8 在 CdpBackend 中使用

```typescript
// cdp.ts
import { createProtocolApi, type RawOn, type RawSend }
  from '@browseros/cdp-protocol/create-api'
import type { ProtocolApi }
  from '@browseros/cdp-protocol/protocol-api'

// 声明合并: CdpBackend 同时满足 ProtocolApi 接口
interface CdpBackend extends ProtocolApi {}

class CdpBackend {
  constructor(config: CdpBackendConfig) {
    // 提供两个原始回调
    const rawSend: RawSend = (method, params) => this.sendRequest(method, params)
    const rawOn: RawOn = (event, handler) => this.registerHandler(event, handler)

    // 一次性注入全部 55 个域的方法
    Object.assign(this, createProtocolApi(rawSend, rawOn))
  }
}

// 使用:
// cdp.Page.navigate({url})       → 类型安全 ✓
// cdp.DOM.setFileInputFiles(...) → 类型安全 ✓
// cdp.Target.getTargets()        → 类型安全 ✓
```

---

## 4. 完整生命周期序列

```
ChromeBrowserMain
  |
  v
BrowserOSServerManager::GetInstance()           [单例]
  |
  v
BrowserOSServerManager::Start()                [browseros_server_manager.cc:129]
  │
  ├── 1. CheckDisableSwitch()
  ├── 2. HandleOrphanProcess()                 [清理残留 sidecar]
  ├── 3. state_ = kStarting
  ├── 4. WriteStateFile(pid, creation_time)    [崩溃恢复]
  │
  ├── 5. ConfigurePorts()                      [行 256]
  │     ├── --remote-debugging-port=9100       [最高优先级]
  │     ├── --browseros-cdp-port=9100
  │     ├── pref kCDPServerPort
  │     ├── kDefaultCDPPort = 9100             [默认值]
  │     └── FindAvailablePort(9100)            [IPv4+IPv6 bind, 最多 100 次]
  │
  ├── 6. StartRemoteDebuggingServer()          [行 436]
  │     └── content::DevToolsAgentHost::StartRemoteDebuggingServer(
  │           ports_.cdp, "0.0.0.0")
  │          ├── GET  /json/version
  │          ├── GET  /json/list
  │          └── WS   /devtools/page/{targetId}
  │
  └── 7. ScheduleSidecarServerStart()          [行 541]
        └── process_controller->Launch(config)
            cdp_port = ports_.cdp
  |
  v
┌───────────────────────────────────────────┐
│          Agent 进程 (TypeScript)           │
│                                           │
│  Application.start()                      │
│   ├── new CdpBackend({ port: 9100 })      │
│   ├── cdp.connect()                       │
│   │    ├── GET /json/version (重试 10 次)  │
│   │    ├── new WebSocket(wsUrl)            │
│   │    └── await once(ws, 'open')          │
│   │                                       │
│   ├── new Browser(cdp)                     │
│   │    └── cdp.Target.getTargets()  ← 第一条 CDP 命令 │
│   │                                       │
│   ├── createHttpServer(...)               │
│   └── writeServerConfig(...)              │
└───────────────────────────────────────────┘
```

---

## 5. 关键文件索引

| 文件 | 路径 | 作用 |
|------|------|------|
| **Chromium C++** | | |
| `browseros_server_manager.cc` | `packages/browseros/chromium_patches/chrome/browser/browseros/server/` | CDP 服务器启动逻辑 |
| `browseros_server_manager.h` | 同上 | BrowserOSServerManager 类定义 |
| `browseros_server_prefs.h` | 同上 | 默认端口常量 (CDP=9100) |
| `browseros_server_utils.cc` | 同上 | FindAvailablePort, IsPortAvailable |
| `browseros_switches.h` | `packages/browseros/chromium_patches/chrome/browser/browseros/core/` | 命令行开关定义 |
| `chrome_browser_main.cc` | `packages/browseros/chromium_patches/chrome/browser/` | Chromium 启动钩子 |
| `inspector_protocol_config.json` | `packages/browseros/chromium_patches/chrome/browser/devtools/` | BrowserOS CDP 协议扩展 |
| **Agent TypeScript** | | |
| `cdp.ts` | `packages/browseros-agent/apps/server/src/browser/backends/` | CDP WebSocket 客户端 |
| `types.ts` | 同上 | CdpBackend 接口定义 |
| `browser.ts` | `packages/browseros-agent/apps/server/src/browser/` | 浏览器抽象 + 子系统初始化 |
| `main.ts` | `packages/browseros-agent/apps/server/src/` | 应用启动入口 |
| `config.ts` | 同上 | 配置解析（含 cdpPort） |
| **Codegen** | | |
| `cdp-protocol.ts` | `packages/browseros-agent/scripts/codegen/` | Codegen 入口脚本 |
| `protocol-parser.ts` | `scripts/codegen/lib/` | 解析 protocol.json |
| `type-emitter.ts` | 同上 | CDP → TypeScript 类型映射 |
| `domain-emitter.ts` | 同上 | 生成 domain 类型文件 |
| `domain-api-emitter.ts` | 同上 | 生成 API 包装接口 |
| `protocol-api-emitter.ts` | 同上 | 生成 protocol-api.ts + create-api.ts |
| **生成产物** | | |
| `protocol-api.ts` | `packages/browseros-agent/packages/cdp-protocol/src/generated/` | 统一 ProtocolApi 接口 |
| `create-api.ts` | 同上 | Proxy 工厂函数 |
| `domains/` | 同上 | 55 个域类型定义文件 |
| `domain-apis/` | 同上 | 55 个域 API 包装接口 |
