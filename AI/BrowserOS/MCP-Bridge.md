# 基于Chrome Debugger API的MCP Bridge标签同步与能力增强方案

以下是方案的核心流程图，用 **Mermaid** 描述，清晰展示了扩展、Bridge 服务、MCP 工具与 CDP 之间的数据流和关键步骤。

```mermaid
flowchart TB
    subgraph Chrome浏览器
        Ext[Chrome扩展]
        Ext -->|监听| Events[chrome.tabs / windows / tabGroups 事件]
        Events -->|debounce| Sync[同步任务]
        Sync -->|1. chrome.tabs.query| TabState[获取标签状态]
        Sync -->|2. chrome.debugger.getTargets| TargetMap[获取 targetId ↔ tabId 映射]
        Sync -->|3. 合并快照| Snapshot[生成完整状态快照]
        Snapshot -->|POST /extension/state| Bridge
    end

    subgraph MCP Bridge服务
        Bridge[Bridge HTTP 服务]
        Bridge -->|存储| Store[Bridge Store\n(tabId→targetId, windows, groups)]
        Store -->|提供查询| BridgeAPI[API 端点]
        BridgeAPI -->|状态读取| Tools
    end

    subgraph MCP工具层
        Tools[MCP Tools\n(tabs/windows/tab_groups/snapshot/act)]
        Tools -->|优先使用 Bridge 数据| Merge[合并真实 tab 元数据]
        Tools -->|若 bridge 未连接| Fallback[降级到原生 CDP]
        Tools -->|执行操作时| CDP[CDP 命令]
        CDP -->|Target.getTargets| RealTarget[获取实时 targetId]
        RealTarget -->|与 Bridge 映射交叉校验| Valid[确认同一浏览器实例]
        Valid -->|成功| Execute[执行操作]
        Valid -->|失败| Disable[禁用 Bridge 能力]
    end

    Bridge -.->|长期轮询/WebSocket| Ext
    Ext -->|定期拉取命令| Bridge

    style Ext fill:#f9f,stroke:#333,stroke-width:2px
    style Bridge fill:#bbf,stroke:#333,stroke-width:2px
    style Tools fill:#bfb,stroke:#333,stroke-width:2px
```

### 文字流程说明

1. **扩展侧同步**  
   - 监听所有标签、窗口、分组事件（均做防抖）。  
   - 触发同步时：  
     - 调用 `chrome.tabs.query` 获取完整标签状态。  
     - 调用 `chrome.debugger.getTargets` 获取 CDP target 列表，对 `type === "page"` 且存在 `tabId` 的项建立 `tabId → targetId` 映射。  
     - 合并成完整快照（包含 windowId、index、active、pinned、groupId 等）。  
   - 将快照通过 HTTP POST 上报给 Bridge 服务（或通过 WebSocket 实时推送）。

2. **Bridge 服务维护状态**  
   - 存储 `tabsById`、`targetToTab`、`windowsById`、`groupsById` 等映射。  
   - 提供状态查询接口（`/extension/state`、`/extension/health` 等）和命令拉取接口（`/extension/commands`）。  
   - 扩展通过长轮询或 WebSocket 主动拉取待执行命令，并返回结果。

3. **MCP 工具层融合**  
   - 现有 MCP 工具（如 `listChrome`、窗口管理、标签组管理）不再完全依赖 CDP，而是优先从 Bridge Store 获取真实的 `tabId`、`windowId`、索引、分组等元数据。  
   - 对于需要执行的操作（如打开、关闭、激活），仍通过 CDP 命令执行，但会利用 Bridge 提供的 `targetId` 来定位目标。  
   - 在敏感操作前，会调用 `Target.getTargets()` 实时刷新 targetId，并与 Bridge 上报的映射交叉校验，确保扩展与 MCP 连接的是同一个浏览器实例，否则自动降级或提示安装。

4. **能力降级与提示**  
   - 当 Bridge 未连接时，工具返回明确的安装指引。  
   - 引入能力层（`capabilities`），动态决定哪些功能可用（窗口、标签组、真实 tabId 等）。

### 关键设计原则

- **真实数据源优先**：`chrome.debugger.getTargets()` 提供稳定的 targetId 映射，避免 URL/title 匹配的不确定性。  
- **扩展主动上报**：Bridge 服务无法主动向扩展发起请求，因此采用扩展定期同步 + 命令拉取（long polling）模式。  
- **交叉校验**：每次操作前比对 Bridge 映射与 CDP 实时 targetId，确保同一浏览器实例，防止跨实例误操作。  
- **渐进式增强**：未安装扩展时，现有 CDP 功能仍可工作（仅部分能力受限），安装后自动升级。

该流程完整覆盖了从数据采集、上报、存储到 MCP 工具融合的全链路，既保证了可靠性，又兼容了现有架构。

**关键突破：`chrome.debugger.getTargets()` 能直接给出 `TargetInfo.id -> targetId` 和 `TargetInfo.tabId`，这比我前面提到的 URL/title/nonce 匹配方案稳很多。Chrome 官方文档也确认 `TargetInfo.id` 是 target id，`tabId` 在 `type == "page"` 时定义，且使用 `chrome.debugger` 需要 `"debugger"` 权限。参考：[chrome.debugger API](https://developer.chrome.com/docs/extensions/reference/api/debugger)。**

方案调整：**扩展负责建立真实 Chrome 标签模型，MCP 负责把它融合进现有 CDP 页面会话模型。**

**推荐架构**
```text
Chrome Extension
  chrome.tabs / chrome.windows / chrome.tabGroups
  chrome.debugger.getTargets()
        |
        | state snapshots + lifecycle events + command channel
        v
MCP Bridge Service
  tabId <-> targetId map
  window/tab/group model
        |
        v
BrowserSession
  PageManager + WindowManager + TabGroupManager
        |
        v
现有 MCP tools
  tabs / windows / tab_groups / snapshot / act / ...
```

**扩展侧职责**
扩展维护一个以 `tabId` 为主键的状态表：

```ts
interface ExtensionTabState {
  tabId: number
  targetId?: string
  windowId: number
  index: number
  url?: string
  title?: string
  active: boolean
  pinned: boolean
  highlighted: boolean
  discarded: boolean
  audible?: boolean
  muted?: boolean
  status?: 'loading' | 'complete'
  groupId?: number
}
```

每次同步时执行三步：

1. `chrome.tabs.query({})` 获取真实标签状态。
2. `chrome.debugger.getTargets()` 获取 CDP target 列表。
3. 对 `target.type === "page"` 且 `target.tabId` 存在的项建立 `tabId -> target.id` 映射。

然后把完整快照上报给 MCP bridge。生命周期监听只作为触发器，不作为唯一事实来源：

```js
chrome.tabs.onCreated.addListener(scheduleSync)
chrome.tabs.onUpdated.addListener(scheduleSync)
chrome.tabs.onRemoved.addListener((tabId) => {
  tabToTargetMap.delete(tabId)
  scheduleSync()
})
chrome.tabs.onActivated.addListener(scheduleSync)
chrome.tabs.onMoved.addListener(scheduleSync)
chrome.tabs.onAttached.addListener(scheduleSync)
chrome.tabs.onDetached.addListener(scheduleSync)
chrome.windows.onCreated.addListener(scheduleSync)
chrome.windows.onRemoved.addListener(scheduleSync)
chrome.windows.onFocusChanged.addListener(scheduleSync)
chrome.tabGroups?.onCreated?.addListener(scheduleSync)
chrome.tabGroups?.onUpdated?.addListener(scheduleSync)
chrome.tabGroups?.onRemoved?.addListener(scheduleSync)
chrome.tabGroups?.onMoved?.addListener(scheduleSync)
```

这里建议所有事件都 debounce 后调用 `syncFullSnapshot()`，因为标签移动、窗口切换、group 更新经常成组发生。

**MCP Bridge 设计**
MCP 服务新增一个 bridge store，而不是把扩展上报直接塞进 CDP connection：

```ts
interface ChromeBridgeState {
  connected: boolean
  lastSeenAt: number
  sequence: number
  tabsById: Map<number, BridgeTab>
  targetToTab: Map<string, number>
  windowsById: Map<number, BridgeWindow>
  groupsById: Map<number, BridgeTabGroup>
}
```

新增本地端点：

```text
POST /extension/hello
POST /extension/state
POST /extension/event
GET  /extension/health
GET  /extension/commands
POST /extension/commands/:id/result
```

命令通道建议用“扩展主动拉取”或 WebSocket。原因是 MCP 服务不能主动调用扩展，扩展可以稳定访问 `http://127.0.0.1:<mcpPort>`。第一版用 long polling 最简单，后续再换 WebSocket。

**与现有项目的融合点**
重点改这几处：

- [src/browser/pages.ts](D:/work/browseros-mcp/src/browser/pages.ts)
  - `listChrome()` 继续调用 `Target.getTargets()`，但优先用 bridge 的 `tabId -> targetId` 映射合并真实 tab 元数据。
  - `tabId` 不再 synthetic，而是真实 Chrome `tab.id`。
  - `isActive/windowId/index/groupId/isPinned/isLoading` 来自扩展状态。
- [src/browser/windows.ts](D:/work/browseros-mcp/src/browser/windows.ts)
  - Chrome 模式下如果 bridge 已连接，则支持 `list/create/close/activate`。
  - `set_visibility` 需要降级：普通 Chrome 扩展不能真正创建 BrowserOS 的 hidden window，只能支持 minimize/focus 或标记为 unsupported。
- [src/mcp/tools/tab-groups.ts](D:/work/browseros-mcp/src/mcp/tools/tab-groups.ts)
  - 不应再直接 raw 调 `Browser.getTabGroups`。
  - 抽一个 `TabGroupManager`：BrowserOS provider 走 `Browser.*` CDP，Chrome bridge provider 走 `chrome.tabGroups` 命令。
- [src/mcp/tools/register.ts](D:/work/browseros-mcp/src/mcp/tools/register.ts)
  - 现在 `windows` / `tab_groups` 被 `backend: ['browseros']` 过滤。应改成“Chrome bridge 可用时注册”，或始终注册但在未连接 bridge 时返回安装提示。

**后端模式建议**
不要把它叫单纯 `chrome`，建议引入能力层：

```ts
backend: 'browseros' | 'chrome'
capabilities: {
  extensionBridge: boolean
  windows: boolean
  tabGroups: boolean
  realTabIds: boolean
}
```

这样普通 Chrome 仍可运行，安装扩展后自动升级能力。

**安装提示策略**
当用户在 Chrome 模式调用 `windows` 或 `tab_groups`，且 bridge 未连接时，返回明确提示：

```text
当前连接的是标准 Chrome，原生 CDP 无法提供完整窗口/标签组模型。
请安装 BrowserOS MCP Bridge 扩展并授予 tabs/windows/tabGroups/debugger 权限。
扩展连接成功后，此工具会自动启用。
```

**关键风险**
`chrome.debugger.getTargets()` 是最可靠的映射来源，但仍建议每次执行敏感操作前刷新一次映射，因为 `targetId` 可能随页面生命周期变化。另一个要防的是“扩展装在 A Chrome，MCP CDP 连到 B Chrome”：MCP bridge 应把扩展上报的 `targetId` 与 `Target.getTargets()` 结果做交集校验，交集为空就判定不是同一个浏览器实例，禁用 bridge。
