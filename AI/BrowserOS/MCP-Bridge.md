这个补充是关键突破：`chrome.debugger.getTargets()` 能直接给出 `TargetInfo.id -> targetId` 和 `TargetInfo.tabId`，这比我前面提到的 URL/title/nonce 匹配方案稳很多。Chrome 官方文档也确认 `TargetInfo.id` 是 target id，`tabId` 在 `type == "page"` 时定义，且使用 `chrome.debugger` 需要 `"debugger"` 权限。参考：[chrome.debugger API](https://developer.chrome.com/docs/extensions/reference/api/debugger)。

我会把方案调整为：**扩展负责建立真实 Chrome 标签模型，MCP 负责把它融合进现有 CDP 页面会话模型。**

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
