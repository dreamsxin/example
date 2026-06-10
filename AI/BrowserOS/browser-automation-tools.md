# BrowserOS Browser Automation Tools 技术文档

> **文档版本**：2026-06  
> **适用范围**：BrowserOS 浏览器自动化工具全栈实现  
> **代码路径**：`packages/browseros-agent/apps/server/src/`  
> **底层协议**：CDP (Chrome DevTools Protocol) over WebSocket

---

## 目录

1. [架构总览](#1-架构总览)
2. [MCP 工具注册机制](#2-mcp-工具注册机制)
3. [Browser Tools 列表与 API](#3-browser-tools-列表与-api)
4. [核心框架层 (framework.ts)](#4-核心框架层-frameworkts)
5. [Browser 核心子系统](#5-browser-核心子系统)
6. [输入子系统 (Input)](#6-输入子系统-input)
7. [快照子系统 (Snapshot)](#7-快照子系统-snapshot)
8. [安全机制 (Trust Boundary)](#8-安全机制-trust-boundary)
9. [ToolResponse 与 Post-Action](#9-toolresponse-与-post-action)
10. [大内容处理 (output-file.ts)](#10-大内容处理-output-filets)
11. [数据流与调用链路](#11-数据流与调用链路)
12. [OOPIF (跨域 iframe) 支持](#12-oopif-跨域-iframe-支持)
13. [附录：目录结构一览](#13-附录目录结构一览)

---

## 1. 架构总览

BrowserOS 的 browser automation 系统由三层组成，通过 CDP 与定制版 Chromium 浏览器通信：

```
┌─────────────────────────────────────────────────────────────────┐
│                      MCP 工具层（TypeScript）                    │
│  tabs / navigate / snapshot / diff / act / read / grep /       │
│  screenshot / wait / run                                      │
│  (tools/browser/*.ts)                                       │
├─────────────────────────────────────────────────────────────────┤
│                    Browser Core 层（TypeScript）                 │
│  BrowserSession ──► PageManager / Observer / Input / Nav     │
│  (browser/core/*.ts)                                        │
├─────────────────────────────────────────────────────────────────┤
│                      CDP 后端层（TypeScript）                    │
│  CdpBackend ─► WebSocket ─► Chrome DevTools Protocol     │
│  (browser/backends/cdp.ts)                                │
├─────────────────────────────────────────────────────────────────┤
│                   Chromium 浏览器（C++，定制版）                 │
│  BrowserOS 定制 Chromium，支持通过 CDP 远程控制                │
└─────────────────────────────────────────────────────────────────┘
```

### 核心设计原则

| 原则 | 说明 |
|------|------|
| **MCP over CDP** | 工具通过 MCP (Model Context Protocol) 对外暴露，底层通过 CDP 与浏览器通信 |
| **Accessibility Tree 优先** | 用 AX Tree 而非 CSS 选择器定位元素，稳定性更高 |
| **Ref 句柄隔离** | 可交互元素分配 `[ref=eN]` 短句柄，Agent 只需传 ref，无需理解 DOM 结构 |
| **Trust Boundary** | 所有页面内容用随机 nonce 包裹，防止 prompt injection |
| **OOPIF 透明处理** | 跨域 iframe 通过 FrameRegistry 对 Agent 完全透明 |

---

## 2. MCP 工具注册机制

**入口文件：** `tools/browser/register.ts`

```typescript
export function registerBrowserTools(
  server: McpServer,
  session: BrowserSession,
  defaults: ToolDefaults,
): void
```

### 注册流程

1. 遍历 `BROWSER_TOOLS` 数组（来自 `registry.ts`）
2. 为每个工具调用 `server.tool(name, description, schema, handler)`
3. handler 内部调用 `executeTool`，记录执行时间和成功/失败 metrics
4. 每个工具绑定到同一个 `BrowserSession` 实例

### 工具列表定义

**文件：** `tools/browser/registry.ts`

```typescript
export const BROWSER_TOOLS: ToolDefinition<ZodTypeAny>[] = [
  tabsTool,
  navigateTool,
  snapshotTool,
  diffTool,
  actTool,
  readTool,
  grepTool,
  screenshotTool,
  waitTool,
  runTool,
]
```

---

## 3. Browser Tools 列表与 API

### 3.1 tabs — 标签页管理

**文件：** `tools/browser/tabs.ts`

支持操作：

| 操作 | 说明 |
|------|------|
| `list` | 列出所有已打开的页面（pageId、标题、URL） |
| `new` | 打开一个新标签页 |
| `close` | 关闭指定页面 |
| `switch` | 切换到指定页面（将其置于前台） |
| `move` | 将页面移动到其他浏览器窗口 |

执行后返回最新 pages 列表。

---

### 3.2 navigate — 页面导航

**文件：** `tools/browser/navigate.ts`

```typescript
{
  action: 'url' | 'back' | 'forward' | 'reload',
  url?: string,        // action='url' 时必填
  pageId?: number,     // 可选，默认当前活动页面
}
```

- 导航完成后等待 `document.readyState === 'complete'`
- 返回新页面快照（含 URL、标题、AX Tree）

---

### 3.3 snapshot — 页面快照

**文件：** `tools/browser/snapshot.ts`

捕获当前页面的 Accessibility Tree，以缩进的语义树文本形式返回。

```typescript
{ pageId?: number }
```

响应规则：
- 内容 ≤ 5000 词 且 ≤ 50000 字符 → 内联在响应中
- 超过阈值 → 写入系统临时目录，返回文件路径

快照内容用 `wrapUntrusted(text, url)` 包裹（见[第 8 节](#8-安全机制-trust-boundary)）。

---

### 3.4 diff — 变化感知

**文件：** `tools/browser/diff.ts`

```typescript
{ pageId?: number }
```

展示自上次快照以来的页面变化：
- URL 未变：返回基于 LCS 的行级 diff（`+`/`-` 前缀）
- URL 已变：返回完整快照（隐式完成了一次导航）

---

### 3.5 act — 页面交互

**文件：** `tools/browser/act.ts`

统一的页面交互工具，支持 11 种交互类型：

| kind | 说明 | 主要参数 |
|------|------|---------|
| `click` | 点击 ref 元素 | `ref`, `button?`, `modifiers?`, `doubleClick?` |
| `click_at` | 点击指定坐标 | `x`, `y`, `button?` |
| `type` | 向 ref 元素输入文字 | `ref`, `text`, `submit?`, `slowly?` |
| `type_at` | 向坐标位置输入文字 | `x`, `y`, `text` |
| `fill` | 清空后填充 ref 元素 | `ref`, `value` |
| `press` | 按键或组合键 | `key`（如 `Control+a`） |
| `hover` | 悬停在 ref 元素 | `ref` |
| `hover_at` | 悬停在坐标 | `x`, `y` |
| `select` | 选择下拉选项 | `ref`, `value` |
| `scroll` | 滚动 | `x`, `y`, `deltaX?`, `deltaY?` |
| `drag_at` | 拖拽 | `from: {x,y}`, `to: {x,y}` |

执行后自动读取 diff，返回交互引发的页面变化。

---

### 3.6 read — 内容提取

**文件：** `tools/browser/read.ts`

```typescript
{
  pageId?: number,
  format: 'markdown' | 'text' | 'links',
}
```

- `markdown`：通过 `content-markdown.ts` 将页面内容转为 Markdown
- `text`：纯文本
- `links`：提取所有链接的 href + 文本

超长内容同样写入临时文件。

---

### 3.7 grep — 内容搜索

**文件：** `tools/browser/grep.ts`

```typescript
{
  pattern: string,     // 正则表达式
  over: 'ax' | 'content',
  pageId?: number,
}
```

- `over='ax'`：在 AX Tree 文本中搜索
- `over='content'`：在页面可见文本中搜索

返回匹配行及行号，最多返回 200 行。

---

### 3.8 screenshot — 截图

**文件：** `tools/browser/screenshot.ts`

```typescript
{
  pageId?: number,
  fullPage?: boolean,   // 默认 false（仅当前视口）
}
```

通过 CDP `Page.captureScreenshot` 截取 PNG 图片，以 base64 格式返回 MCP image content。

---

### 3.9 wait — 等待条件

**文件：** `tools/browser/wait.ts`

```typescript
{
  condition: 'text' | 'selector' | 'time',
  value: string,        // text内容 / CSS选择器 / 毫秒数
  pageId?: number,
}
```

- `text`：等待页面出现指定文本（轮询 AX Tree）
- `selector`：等待 CSS 选择器匹配到元素
- `time`：等待固定毫秒数

默认超时 10s，最大 30s，轮询间隔 300ms。

---

### 3.10 run — 执行 JavaScript

**文件：** `tools/browser/run.ts`

```typescript
{
  code: string,   // 任意 JavaScript 代码
  pageId?: number,
}
```

代码包装为 `(async () => { ... })()` 执行，支持 `return` 语句返回值。  
通过 CDP `Runtime.evaluate` 在页面上下文中执行。

---

## 4. 核心框架层 (framework.ts)

**文件：** `tools/browser/framework.ts`

### 4.1 核心类型

```typescript
interface ToolDefinition<S extends ZodTypeAny> {
  name: string
  description: string
  schema: S
  annotations?: ToolAnnotations
  handler(args: z.infer<S>, ctx: ToolContext): Promise<ToolResult>
}

interface ToolContext {
  session: BrowserSession
  defaults: ToolDefaults
}

interface ToolAnnotations {
  readOnlyHint?: boolean      // 工具是否只读
  destructiveHint?: boolean   // 是否可能破坏性操作
  idempotentHint?: boolean   // 是否幂等
}
```

### 4.2 工具工厂

```typescript
function defineTool<S extends ZodTypeAny>(def: ToolDefinition<S>): ToolDefinition<S>
```

### 4.3 辅助函数

| 函数 | 用途 |
|------|------|
| `textResult(text)` | 构建纯文本响应 |
| `errorResult(msg)` | 构建错误响应 |
| `clampTimeout(ms, max)` | 限制超时上限 |
| `abortableDelay(ms, signal)` | 可取消的延迟 |

### 4.4 核心执行函数

```typescript
async function executeTool<S extends ZodTypeAny>(
  def: ToolDefinition<S>,
  rawArgs: unknown,
  ctx: ToolContext,
): Promise<CallToolResult>
```

执行流程：
1. Zod schema 验证参数
2. 调用 `def.handler(args, ctx)`
3. 通过 `ToolResponse` 构建最终响应（含 post-action）

---

## 5. Browser 核心子系统

### 5.1 BrowserSession

**文件：** `browser/core/session.ts`

```typescript
class BrowserSession {
  observe(): Observer
  input(): Input
  nav(): Navigation
  cdp(): CdpConnection
  pages(): PageManager
}
```

`BrowserSession` 是协调中心，持有并暴露所有子系统的访问接口。各工具通过 `ctx.session` 访问这些子系统。

---

### 5.2 CdpConnection & CdpBackend

**文件：** `browser/backends/cdp.ts`, `browser/core/connection.ts`

```typescript
interface CdpConnection extends ProtocolApi {
  session(sessionId: SessionId): ProtocolApi
  isConnected(): boolean
  connectionEpoch(): number
}
```

`CdpBackend` 实现 `CdpConnection`，通过 WebSocket 连接浏览器：

- **连接管理**：带指数退避的自动重连
- **Keepalive**：定期发送 `Browser.getVersion` 心跳保持连接活跃
- **消息路由**：用 `Map<id, PendingRequest>` 匹配请求响应
- **Session 路由**：通过 `sessionId` 区分不同 Tab 的 CDP session
- **请求超时**：每个 CDP 调用有独立超时计时器

连接建立后，同一 `ProtocolApi` 对象既代表根 browser session，调用 `.session(id)` 可获取特定 tab session。

---

### 5.3 PageManager

**文件：** `browser/core/pages.ts`

负责管理所有浏览器 Tab：

```typescript
class PageManager {
  list(): PageInfo[]
  getActive(): PageInfo | undefined
  new(url?: string): Promise<PageInfo>
  close(pageId: number): Promise<void>
  show(pageId: number): Promise<void>     // 切换到前台
  move(pageId: number, windowId: number): Promise<void>
}

interface PageInfo {
  pageId: number
  title: string
  url: string
  sessionId: SessionId
}
```

`pageId` 是 BrowserOS 分配的稳定整数 ID，与 CDP 的 `targetId` 字符串相互映射。

---

### 5.4 Navigation

**文件：** `browser/core/navigation.ts`

```typescript
class Navigation {
  goto(pageId: number, url: string): Promise<void>
  reload(pageId: number): Promise<void>
  back(pageId: number): Promise<void>
  forward(pageId: number): Promise<void>
}
```

所有导航方法都会轮询 `document.readyState`，等待页面 `complete` 后才返回。

---

### 5.5 Observer 与 FrameRegistry

**文件：** `browser/core/observer/observer.ts`, `browser/core/observer/frames.ts`

`Observer` 负责页面状态管理：
- 为每个 pageId 缓存最新快照文本和 `RefMap`
- 通过 `FrameRegistry` 支持 iframe stitching（最大深度 5）
- 获取快照时并行采集所有 frame 的 AX Tree

`FrameRegistry` 跟踪 OOPIF (跨域 iframe) 会话：

```typescript
class FrameRegistry {
  registerPage(pageSession, pageId, sessionId): Promise<void>
  resolveFrameTarget(pageId, frameId): FrameTarget
}

interface FrameTarget {
  session: ProtocolApi
  axParams: { frameId?: FrameId }
}
```

解析规则：
- 主框架 → 页面 session，无 frameId
- 跨域 OOPIF → 其专属 session，无 frameId
- 同域 iframe → 页面 session + frameId 参数

---

## 6. 输入子系统 (Input)

### 6.1 坐标与元素定位 (geometry.ts)

**文件：** `browser/core/input/geometry.ts`

```typescript
// 三级 fallback：getContentQuads → getBoxModel → getBoundingClientRect
async function getElementCenter(session, backendNodeId): Promise<{x, y}>

async function scrollIntoView(session, backendNodeId): Promise<void>
async function focusElement(session, backendNodeId): Promise<void>
async function jsClick(session, backendNodeId): Promise<void>  // 降级点击
async function getInputValue(session, backendNodeId): Promise<string>
async function callOnElement(session, backendNodeId, fn, args?): Promise<unknown>
```

坐标在元素所在 frame session 的坐标系内计算，OOPIF 场景下坐标无需手动转换。

---

### 6.2 鼠标事件 (mouse.ts)

**文件：** `browser/core/input/mouse.ts`

```typescript
async function dispatchClick(session, x, y, button, clickCount, modifiers): Promise<void>
async function dispatchHover(session, x, y): Promise<void>
async function dispatchScroll(session, x, y, deltaX, deltaY): Promise<void>
async function dispatchDrag(session, from, to): Promise<void>
```

每个点击事件序列：`mouseMoved` → `mousePressed` → `mouseReleased`  
拖拽序列：`mouseMoved(from)` → `mousePressed(from)` → `mouseMoved(to)` → `mouseReleased(to)`

---

### 6.3 键盘事件 (keyboard.ts)

**文件：** `browser/core/input/keyboard.ts`

```typescript
async function typeText(session, text): Promise<void>
async function clearField(session): Promise<void>           // Cmd/Ctrl+A → Backspace
async function pressCombo(session, key): Promise<void>      // "Enter", "Control+a" 等
```

**按键组合格式：** `"modifier1+modifier2+key"`，例如 `"Control+Shift+a"`

`typeText` 逐字符发送 `keyDown` + `char` + `keyUp`，确保框架（Vue/React）的键盘事件处理能正常触发。

平台修饰键：macOS 用 `Meta (Cmd)`，其他系统用 `Control`。

---

## 7. 快照子系统 (Snapshot)

### 7.1 AX Tree 获取

**文件：** `browser/core/observer/ax-tree.ts`

```typescript
async function fetchAxTree(session, params?: {frameId?}): Promise<AXNode[]>
```

通过 CDP `Accessibility.getFullAXTree` 获取整棵 AX 树，返回扁平节点数组。

---

### 7.2 角色分类 (roles.ts)

**文件：** `browser/core/snapshot/roles.ts`

| 集合 | 说明 | 行为 |
|------|------|------|
| `INTERACTIVE_ROLES` | 可操作元素（button/link/textbox/...） | 分配 `[ref=eN]` 句柄 |
| `NAMED_CONTENT_ROLES` | 有语义的内容（heading/img/cell/...） | 展示但不分配 ref |
| `SKIP_ROLES` | 无结构角色（StaticText/none/...） | 跳过节点，子节点上浮 |
| `ROOT_ROLES` | 文档根（RootWebArea/WebArea） | 跳过，从其子节点开始渲染 |
| `VALUE_ROLES` | 有当前值的元素（textbox/combobox/...） | 在输出中追加 `: "value"` |

---

### 7.3 快照渲染 (render.ts)

**文件：** `browser/core/snapshot/render.ts`

```typescript
function renderSnapshot(nodes: AXNode[], refMap: RefMap): string
```

渲染逻辑（深度优先遍历）：

1. **ROOT_ROLES** → 跳过，递归处理其子节点
2. **SKIP_ROLES** → 跳过节点本身，将子节点上浮到当前缩进级别
3. **INTERACTIVE_ROLES** → 通过 `RefMap.mint()` 分配 `eN` ref，输出 `role "name" [ref=eN]`
4. **NAMED_CONTENT_ROLES** → 输出 `role "name"`（无 ref）
5. **VALUE_ROLES** → 追加当前值：`role "name" [ref=eN]: "current_value"`

示例输出：
```
heading "Search Results"
  link "Result 1 - Click here" [ref=e1]
  link "Result 2 - Another item" [ref=e2]
  textbox "Search" [ref=e3]: "hello"
  button "Submit" [ref=e4]
```

---

### 7.4 Ref 系统 (refs.ts)

**文件：** `browser/core/snapshot/refs.ts`

```typescript
class RefMap {
  mint(node: {backendNodeId, role, name, frameId?}): string   // 返回 "eN"
  get(ref: string): RefEntry | undefined
}

interface RefEntry {
  ref: string
  backendNodeId: number
  role: string
  name: string
  nth: number          // 同 (frameId, role, name) 的第几个，用于消歧
  frameId: FrameId | undefined
}
```

- 每次快照创建新的 `RefMap`，`ref` 从 `e1` 重新计数
- `nth` 字段用于区分页面上名称相同的同类元素
- `ref` 对 Agent 透明，Agent 只需在 `act` 工具中传入 `ref` 字段

---

### 7.5 Ref 解析 (resolve.ts)

**文件：** `browser/core/observer/resolve.ts`

```typescript
async function resolveRefEntry(
  session: ProtocolApi,
  entry: RefEntry,
  axParams?: {frameId?},
): Promise<ResolvedElement>
```

**两级解析策略：**

1. **快速路径**：用缓存的 `backendNodeId` 直接调用 `DOM.resolveNode`，验证节点是否仍活跃
2. **降级路径**：如果节点已失效，重新获取 AX Tree，按 `(role, name, nth)` 重新查找节点

这种策略比缓存 CSS 选择器更鲁棒，能正确处理 DOM 动态更新后的 ref 复用。

---

### 7.6 Cursor 增强 (cursor-augment.ts)

**文件：** `browser/core/observer/cursor-augment.ts`

AX Tree 只涵盖带 ARIA role 的元素，SPA 中大量 `cursor:pointer` 的 `<div>` 不会出现在 AX Tree 中。`cursor-augment.ts` 通过注入 JavaScript 扫描这类元素：

扫描条件（满足其一即可）：
- `cursor: pointer` 且不是因为继承自父元素
- 有 `onclick` 属性/处理器
- 有非 `-1` 的 `tabindex`
- `contenteditable`

扫描后：
1. 给命中元素打 `data-__bcid` 临时标记
2. 通过 `DOM.describeNode` 获取 `backendNodeId`
3. 清理标记属性

命中结果附加到 AX Tree 渲染中（以 reason 标注，如 `[cursor:pointer]`）。

---

### 7.7 快照 Diff (diff.ts)

**文件：** `browser/core/snapshot/diff.ts`

```typescript
function diffSnapshots(prev: string, curr: string): string
function diffSnapshotObservations(
  prevSnapshot: SnapshotState | undefined,
  currSnapshot: SnapshotState,
): string
```

- `diffSnapshots`：基于 LCS (Longest Common Subsequence) 的行级文本 diff
  - 新增行：`+ line`
  - 删除行：`- line`
  - 无变化：省略（输出压缩）
- `diffSnapshotObservations`：URL 变化时自动退化为返回完整新快照

---

## 8. 安全机制 (Trust Boundary)

**文件：** `tools/browser/trust-boundary.ts`

```typescript
function wrapUntrusted(text: string, origin: string): string
```

所有页面内容（快照、读取结果、grep 结果）都用随机 nonce 包裹：

```
<browseros-page-content nonce="a1b2c3d4" origin="https://example.com">
...page content...
</browseros-page-content>
```

**目的：** 防止 prompt injection 攻击。  
恶意页面可能在其内容中嵌入看似系统指令的文字，`wrapUntrusted` 给这些内容加上明确的边界，使 LLM 能够区分「系统消息」与「页面内容」。  
每次调用生成全新随机 nonce，确保 nonce 无法被预测或重放。

---

## 9. ToolResponse 与 Post-Action

**文件：** `tools/response.ts`

```typescript
class ToolResponse {
  text(content: string): this
  image(base64: string, mimeType: string): this
  includeSnapshot(pageId?: number): this      // 工具执行后自动附加快照
  includeScreenshot(pageId?: number): this    // 工具执行后自动附加截图
  includePages(): this                        // 工具执行后自动附加页面列表
  build(): ToolResult
}
```

大多数写操作工具（navigate、act 等）会在结果中调用 `includeSnapshot()`，使 Agent 在执行动作后立即得到最新页面状态，无需额外调用 `snapshot` 工具。

---

## 10. 大内容处理 (output-file.ts)

**文件：** `tools/browser/output-file.ts`

```typescript
async function writeTempToolOutputFile(
  toolName: string,
  content: string,
  ext: string,
): Promise<string>  // 返回文件路径
```

当工具输出内容超过阈值时（默认 5000 词 / 50000 字符），将内容写入系统临时目录，返回文件路径而非内联内容，避免 MCP 响应过大影响 LLM 上下文处理。

---

## 11. 数据流与调用链路

以 `act({kind: 'click', ref: 'e3'})` 为例，完整调用链路：

```
act tool handler
 └─► ctx.session.observe().resolveRef('e3')
       ├─► RefMap.get('e3')  →  RefEntry{backendNodeId, role, name, nth, frameId}
       └─► FrameRegistry.resolveFrameTarget(pageId, frameId)
             →  FrameTarget{session, axParams}
 └─► resolveRefEntry(session, entry)
       ├─► DOM.resolveNode(backendNodeId)  [验证节点活跃]
       └─► (if stale) fetchAxTree → findByRoleNameNth → refresh backendNodeId
 └─► scrollIntoView(session, backendNodeId)    [geometry.ts]
 └─► getElementCenter(session, backendNodeId)  [geometry.ts]
       └─► DOM.getContentQuads(backendNodeId)  →  {x, y}
 └─► dispatchClick(session, x, y, ...)        [mouse.ts]
       └─► Input.dispatchMouseEvent(mouseMoved)
       └─► Input.dispatchMouseEvent(mousePressed)
       └─► Input.dispatchMouseEvent(mouseReleased)
 └─► ToolResponse.includeSnapshot()
       └─► Observer.snapshot(pageId)
             ├─► fetchAxTree(session) + frame stitching
             ├─► findCursorHits(session)  [cursor-augment.ts]
             ├─► renderSnapshot(nodes, new RefMap())  [render.ts]
             └─► wrapUntrusted(text, url)  [trust-boundary.ts]
```

---

## 12. OOPIF (跨域 iframe) 支持

OOPIF 是跨域 iframe 由独立进程渲染的浏览器机制，其 AX Tree 和 DOM 操作需要单独的 CDP session。

BrowserOS 的处理方案：

1. **FrameRegistry 监听 `attachedToTarget` 事件**  
   当浏览器创建 OOPIF session 时，自动注册到 `oopifSessions` map（以 frameId 为 key）

2. **快照时 frame stitching**  
   Observer 在渲染快照时，对 AX Tree 中的 iframe 节点，递归获取子 frame 的 AX Tree 并内联（最大深度 5）

3. **交互时 session 路由**  
   `resolveFrameTarget` 根据 frameId 返回正确的 session：
   - 跨域 OOPIF → 返回其专属 CDP session
   - 同域 iframe → 返回页面 session + frameId 参数

4. **坐标系自动对齐**  
   鼠标事件在元素所在 frame session 的坐标系中分发，无需在页面级和 frame 级之间手动做坐标转换

这套机制使 Agent 对 iframe 的存在完全透明，可以直接通过 ref 操作 iframe 内的元素。

---

## 13. 附录：目录结构一览

```
packages/browseros-agent/apps/server/src/
├── tools/
│   ├── browser/
│   │   ├── registry.ts          # 工具列表定义
│   │   ├── register.ts          # MCP 注册逻辑
│   │   ├── framework.ts         # ToolDefinition / defineTool / executeTool
│   │   ├── act.ts               # 页面交互（11种）
│   │   ├── navigate.ts          # 页面导航
│   │   ├── snapshot.ts          # AX Tree 快照
│   │   ├── diff.ts              # 快照 diff
│   │   ├── screenshot.ts        # 截图
│   │   ├── read.ts              # 内容提取
│   │   ├── grep.ts              # 内容搜索
│   │   ├── wait.ts              # 条件等待
│   │   ├── run.ts               # JS 执行
│   │   ├── tabs.ts              # 标签页管理
│   │   ├── trust-boundary.ts    # 安全包裹
│   │   └── output-file.ts       # 大内容写文件
│   └── response.ts              # ToolResponse 构建器
│
└── browser/
    ├── backends/
    │   └── cdp.ts               # CDP WebSocket 客户端
    └── core/
        ├── connection.ts        # CdpConnection 接口定义
        ├── session.ts           # BrowserSession 协调中心
        ├── pages.ts             # PageManager
        ├── navigation.ts        # Navigation
        ├── input/
        │   ├── input.ts         # Input 动作层
        │   ├── geometry.ts      # 坐标计算与元素定位
        │   ├── mouse.ts         # CDP 鼠标事件
        │   └── keyboard.ts      # CDP 键盘事件
        ├── observer/
        │   ├── observer.ts      # 快照/ref/diff 状态管理
        │   ├── ax-tree.ts       # AX Tree 获取
        │   ├── frames.ts        # FrameRegistry (OOPIF)
        │   ├── resolve.ts       # Ref → backendNodeId 解析
        │   └── cursor-augment.ts # SPA 隐式交互元素扫描
        └── snapshot/
            ├── render.ts        # AX Tree → 语义文本渲染
            ├── refs.ts          # RefMap / RefEntry
            ├── diff.ts          # LCS 行级 diff
            ├── roles.ts         # 角色分类常量
            └── ax-types.ts      # CDP AX 节点类型定义
```
