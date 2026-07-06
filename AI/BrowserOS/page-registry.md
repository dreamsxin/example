`page registry` 是 BrowserOS MCP 自己维护的一张“页面登记表”，核心作用是把 **AI 看到的 `pageId`** 映射到 **浏览器/CDP 真实对象**。

一句话：

```text
page registry = MCP pageId -> Chromium tabId / targetId / sessionId 的映射表
```

为什么需要它
MCP tool 不想让 AI 直接操作 CDP 的 `targetId`，因为它又长又不稳定，也不适合展示给模型。所以 BrowserOS 给每个页面分配一个简单数字：

```text
[1] https://example.com
[2] https://github.com
```

AI 后续就可以说：

```text
snapshot page=1
navigate page=2
tabs close page=1
```

而内部再把 `page=1` 转成真实 CDP 信息。

核心数据结构在 [pages.ts](D:/work/BrowserOS/packages/browseros-agent/packages/browser-core/src/core/pages.ts:9)：

```ts
interface PageInfo {
  pageId: number      // MCP 自己生成的 ID
  targetId: string    // CDP target ID，用于 attach
  tabId: number       // 浏览器 tab ID，用于关闭/刷新/分组
  url: string
  title: string
  isActive: boolean
  isLoading: boolean
  loadProgress: number
  isPinned: boolean
  isHidden: boolean
  windowId?: number
  index?: number
  groupId?: string
}
```

`PageManager` 里有两张表：

```ts
pages = Map<pageId, PageInfo>
sessions = Map<targetId, sessionId>
```

定义在 [pages.ts](D:/work/BrowserOS/packages/browseros-agent/packages/browser-core/src/core/pages.ts:52)。

它怎么建立
当调用 `tabs list` 时，实际会调用：

```ts
ctx.session.pages.list()
```

见 [tabs.ts](D:/work/BrowserOS/packages/browseros-agent/packages/browser-mcp/src/tools/tabs.ts:27)。

`PageManager.list()` 再调用：

```ts
Browser.getTabs({ includeHidden: true })
```

见 [pages.ts](D:/work/BrowserOS/packages/browseros-agent/packages/browser-core/src/core/pages.ts:67)。

返回每个 `TabInfo` 后，PageManager 会：

1. 用 `targetId` 或 `tabId` 查找已有 page。
2. 如果找到，就更新已有 `PageInfo`。
3. 如果没找到，就分配新的 `pageId = nextPageId++`。
4. 如果 registry 里某个 target 已经不在浏览器 tab 列表里，就删除它。

关键逻辑在 [pages.ts](D:/work/BrowserOS/packages/browseros-agent/packages/browser-core/src/core/pages.ts:76)。

为什么同时用 `targetId` 和 `tabId`
代码里匹配已有页面时是：

```ts
findByTarget(tab.targetId) ?? findByTab(tab.tabId)
```

见 [pages.ts](D:/work/BrowserOS/packages/browseros-agent/packages/browser-core/src/core/pages.ts:79)。

原因是：

- `targetId` 用来 attach 到页面执行 CDP 操作。
- `tabId` 用来识别真实浏览器 tab。
- 如果 target 变了，但 tab 还是同一个，可以用 `tabId` 保住同一个 MCP `pageId`。

它怎么给页面类 tools 用
当 `snapshot page=1`、`act page=1`、`download page=1` 这类 tool 需要操作页面时，会调用：

```ts
ctx.session.pages.getSession(pageId)
```

`getSession()` 会：

1. 从 `pages` 表里找到 `PageInfo`。
2. 取出 `targetId`。
3. 调用原生 CDP：

```ts
Target.attachToTarget({ targetId, flatten: true })
```

4. 拿到 `sessionId`。
5. 缓存在 `sessions: Map<targetId, sessionId>`。

实现见 [pages.ts](D:/work/BrowserOS/packages/browseros-agent/packages/browser-core/src/core/pages.ts:114) 和 attach 逻辑 [pages.ts](D:/work/BrowserOS/packages/browseros-agent/packages/browser-core/src/core/pages.ts:344)。

后续页面操作就用这个 page-scoped CDP session：

```text
snapshot -> Accessibility / DOM
act -> Input / DOM / Runtime
navigate -> Page.navigate
screenshot -> Page.captureScreenshot
evaluate -> Runtime.evaluate
```

它和 `tabs` tool 的关系
`tabs` tool 是 page registry 的主要入口：

- `tabs list`：刷新 registry，并把 `pageId` 展示给 AI
- `tabs active`：通过 `Browser.getActiveTab()` 找 active tab，再映射回 registry
- `tabs new`：通过 `Browser.createTab()` 创建 tab，然后登记成新的 `pageId`
- `tabs close`：通过 `pageId -> tabId` 调 `Browser.closeTab`

对应代码在 [tabs.ts](D:/work/BrowserOS/packages/browseros-agent/packages/browser-mcp/src/tools/tabs.ts:4)。

一个完整例子：

```text
用户/AI 调 tabs list
  -> Browser.getTabs()
  -> 返回 tabId=10, targetId=abc, url=...
  -> registry 分配 pageId=1

AI 调 snapshot page=1
  -> registry 查 pageId=1
  -> 得到 targetId=abc
  -> Target.attachToTarget(targetId=abc)
  -> 用 session 调 Accessibility/DOM

AI 调 tabs close page=1
  -> registry 查 pageId=1
  -> 得到 tabId=10
  -> Browser.closeTab({ tabId: 10 })
  -> registry 删除 pageId=1
```

所以，page registry 的价值是把三种 ID 统一起来：

```text
pageId   = MCP 给 AI 用的简单 ID
tabId    = 浏览器标签页管理 ID
targetId = CDP 页面调试 ID
sessionId = attach 后的 CDP 会话 ID
```
