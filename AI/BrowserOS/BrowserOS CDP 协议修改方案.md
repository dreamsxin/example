# BrowserOS CDP 协议修改方案

本文面向 Chromium / BrowserOS 内核开发人员，按 CDP domain 说明 BrowserOS 需要对原生 Chromium CDP 做哪些协议修改、哪些 domain 是新增的、哪些字段是新增的、这些字段的用途是什么，以及这些修改如何支撑 MCP tool。

## 1. 修改目标

原生 Chromium CDP 更偏 DevTools 调试目标，核心对象是 `Target`、`Page`、`Runtime`。这对单页面调试足够，但对 AI 通过 MCP 管理浏览器工作区不够：AI 需要知道真实窗口、真实标签页、标签组、书签、历史记录，以及 `targetId` 与浏览器 tab/window 的关系。

本方案新增或扩展以下 CDP 能力：

| Domain | 修改类型 | 目的 |
| --- | --- | --- |
| `Browser` | 扩展已有 domain | 增加窗口、标签页、标签组、target/tab 互转能力。 |
| `Target` | 扩展已有 domain | 在 `TargetInfo` 中补充 `tabId/windowId`，让 target 可映射回真实浏览器对象。 |
| `Bookmarks` | 新增 domain | 通过 CDP 查询、搜索、创建、更新、移动、删除书签。 |
| `History` | 新增 domain | 通过 CDP 查询最近历史、搜索历史、删除 URL 或时间范围历史。 |

优先级建议：

1. 先实现 `Browser` domain 扩展，这是 `tabs`、`windows`、`tab_groups` 和页面类 MCP tools 的核心依赖。
2. 再实现 `Target.TargetInfo.tabId/windowId`，作为 target 层 fallback 和未来扩展能力。
3. 最后实现 `Bookmarks` / `History`，当前主要通过 `run` raw CDP 使用，未来可封装成一等 MCP tools。

## 2. 修改文件范围

### 2.1 协议定义

| 文件 | 修改 |
| --- | --- |
| `third_party/blink/public/devtools_protocol/domains/Browser.pdl` | 扩展已有 `Browser` domain，新增类型和命令。 |
| `third_party/blink/public/devtools_protocol/domains/Target.pdl` | 扩展已有 `Target.TargetInfo`，新增 `tabId/windowId` 字段。 |
| `third_party/blink/public/devtools_protocol/domains/Bookmarks.pdl` | 新增 `Bookmarks` domain。 |
| `third_party/blink/public/devtools_protocol/domains/History.pdl` | 新增 `History` domain。 |
| `third_party/blink/public/devtools_protocol/browser_protocol.pdl` | include 新增 `Bookmarks.pdl` / `History.pdl`。 |
| `third_party/blink/public/devtools_protocol/BUILD.gn` | 将新增 PDL 文件加入 protocol 生成输入。 |
| `chrome/browser/devtools/inspector_protocol_config.json` | 暴露新增 Browser/Bookmarks/History 命令；History 需要 async 配置。 |

### 2.2 Chromium 实现

| 文件 | 修改 |
| --- | --- |
| `chrome/browser/devtools/protocol/browser_handler.cc` / `.h` | 实现新增 Browser domain 命令。 |
| `chrome/browser/devtools/protocol/browser_handler_android.cc` | Android 侧实现 target/tab 互转；其余不支持能力返回 MethodNotFound。 |
| `content/browser/devtools/protocol/target_handler.cc` | 构建 `TargetInfo` 时填充 `tabId/windowId`。 |
| `chrome/browser/devtools/protocol/bookmarks_handler.cc` / `.h` | 实现 `Bookmarks` domain。 |
| `chrome/browser/devtools/protocol/history_handler.cc` / `.h` | 实现 `History` domain。 |
| `chrome/browser/devtools/chrome_devtools_session.cc` / `.h` | 注册 Bookmarks/History handlers。 |

## 3. `Browser` Domain 修改方案

`Browser` 是原生 Chromium CDP 已有 domain。本方案不是新增 `Browser` domain，而是在其中新增浏览器工作区相关类型和命令。

### 3.1 新增类型总表

| 类型 | 修改类型 | 用途 |
| --- | --- | --- |
| `Browser.TabID` | 新增类型 | 表示真实浏览器 tab id。 |
| `Browser.WindowType` | 新增类型 | 表示窗口类型，例如 normal、popup、app。 |
| `Browser.WindowInfo` | 新增类型 | 描述真实浏览器窗口。 |
| `Browser.TabInfo` | 新增类型 | 描述真实浏览器标签页，并连接 `tabId` 与 `targetId`。 |
| `Browser.TabGroupID` | 新增类型 | 表示浏览器 tab group id。 |
| `Browser.TabGroupInfo` | 新增类型 | 描述标签组状态。 |

### 3.2 `Browser.TabID`

Schema：

```ts
type Browser.TabID = number
```

用途：

- 作为真实浏览器标签页标识。
- MCP 内部 `pageId` 是 agent 侧合成 id，`tabId` 是浏览器内核真实 id。
- `tabs close`、`tab_groups create/ungroup`、`PageManager.refresh` 都依赖 `pageId -> tabId`。

### 3.3 `Browser.WindowType`

Schema：

```ts
type Browser.WindowType =
  | 'normal'
  | 'popup'
  | 'app'
  | 'devtools'
  | 'app_popup'
  | 'picture_in_picture'
```

用途：

- 让 MCP 能区分普通窗口、弹窗、应用窗口、DevTools 窗口等。
- `windows list` 输出窗口类型，避免 AI 把 devtools/popup 当普通工作窗口处理。

### 3.4 `Browser.WindowInfo`

Schema：

```ts
interface Browser.WindowInfo {
  windowId: Browser.WindowID
  windowType: Browser.WindowType
  bounds: Browser.Bounds
  isActive: boolean
  isVisible: boolean
  tabCount: number
  activeTabId?: Browser.TabID
  browserContextId?: Browser.BrowserContextID
}
```

新增字段：

| 字段 | 类型 | 必需 | 用途 |
| --- | --- | --- | --- |
| `windowId` | `Browser.WindowID` | 是 | 真实窗口 id，供 `windows close/activate/set_visibility` 使用。 |
| `windowType` | `Browser.WindowType` | 是 | 表示窗口类型。 |
| `bounds` | `Browser.Bounds` | 是 | 表示窗口位置和尺寸，复用原生类型。 |
| `isActive` | `boolean` | 是 | 表示是否为当前激活窗口。 |
| `isVisible` | `boolean` | 是 | 区分可见窗口和 hidden MCP window。 |
| `tabCount` | `integer` | 是 | 表示窗口内 tab 数量。 |
| `activeTabId` | `Browser.TabID` | 否 | 窗口当前激活 tab。 |
| `browserContextId` | `Browser.BrowserContextID` | 否 | 多 browser context 场景保留。 |

### 3.5 `Browser.TabInfo`

Schema：

```ts
interface Browser.TabInfo {
  tabId: Browser.TabID
  targetId: Target.TargetID
  url: string
  title: string
  isActive: boolean
  isLoading: boolean
  loadProgress: number
  isPinned: boolean
  isHidden: boolean
  windowId?: Browser.WindowID
  index?: number
  browserContextId?: Browser.BrowserContextID
  groupId?: Browser.TabGroupID
}
```

新增字段：

| 字段 | 类型 | 必需 | 用途 |
| --- | --- | --- | --- |
| `tabId` | `Browser.TabID` | 是 | 真实 tab id，用于关闭、刷新、移动、分组。 |
| `targetId` | `Target.TargetID` | 是 | DevTools target id，用于 `Target.attachToTarget`。 |
| `url` | `string` | 是 | 当前 tab URL。 |
| `title` | `string` | 是 | 当前 tab 标题。 |
| `isActive` | `boolean` | 是 | 标记是否为 active tab。 |
| `isLoading` | `boolean` | 是 | 标记页面是否仍在加载。 |
| `loadProgress` | `number` | 是 | 页面加载进度。 |
| `isPinned` | `boolean` | 是 | 标记 tab 是否 pinned。 |
| `isHidden` | `boolean` | 是 | 标记 tab 是否在 hidden window 或 hidden 状态。 |
| `windowId` | `Browser.WindowID` | 否 | tab 所属窗口；hidden 场景可省略。 |
| `index` | `integer` | 否 | tab 在窗口中的位置。 |
| `browserContextId` | `Browser.BrowserContextID` | 否 | 多 context 场景保留。 |
| `groupId` | `Browser.TabGroupID` | 否 | tab 所属标签组。 |

用途：

- 这是 MCP page registry 的核心结构。
- `PageManager` 通过 `Browser.getTabs({ includeHidden: true })` 获取 `TabInfo[]`，再建立 `pageId -> tabId/targetId/windowId` 映射。
- 页面类 MCP tools 最终依赖 `targetId` attach 到 page target；标签管理类 tools 依赖 `tabId` 操作真实 tab。

### 3.6 `Browser.TabGroupID`

Schema：

```ts
type Browser.TabGroupID = string
```

用途：

- 表示 Chromium tab group id。
- MCP `tab_groups` tool 用它更新、关闭已有标签组。

### 3.7 `Browser.TabGroupInfo`

Schema：

```ts
interface Browser.TabGroupInfo {
  groupId: Browser.TabGroupID
  windowId: Browser.WindowID
  title: string
  color: string
  collapsed: boolean
  tabIds: Browser.TabID[]
}
```

新增字段：

| 字段 | 类型 | 必需 | 用途 |
| --- | --- | --- | --- |
| `groupId` | `Browser.TabGroupID` | 是 | 标签组 id。 |
| `windowId` | `Browser.WindowID` | 是 | 标签组所属窗口。 |
| `title` | `string` | 是 | 标签组标题。 |
| `color` | `string` | 是 | 标签组颜色，支持 grey、blue、red、yellow、green、pink、purple、cyan、orange。 |
| `collapsed` | `boolean` | 是 | 标签组是否折叠。 |
| `tabIds` | `Browser.TabID[]` | 是 | 组内 tab ids。 |

### 3.8 新增窗口管理命令

这些命令都是在已有 `Browser` domain 中新增。

| 命令 | 参数 | 返回值 | 用途 |
| --- | --- | --- | --- |
| `Browser.getWindows` | 无 | `windows: WindowInfo[]` | 列出所有浏览器窗口。 |
| `Browser.getActiveWindow` | 无 | `window?: WindowInfo` | 获取当前 active window。 |
| `Browser.createWindow` | `url?`, `bounds?`, `windowType?`, `hidden?`, `browserContextId?` | `window: WindowInfo` | 创建可见或 hidden window。 |
| `Browser.closeWindow` | `windowId` | 无 | 关闭窗口。 |
| `Browser.activateWindow` | `windowId` | 无 | 激活窗口。 |
| `Browser.setWindowVisibility` | `windowId`, `visible`, `activate?` | `window: WindowInfo`, `replaced: boolean`, `previousWindowId: WindowID` | 显示或隐藏窗口。 |

Schema：

```ts
interface BrowserGetWindowsParams {}
interface BrowserGetWindowsResult {
  windows: Browser.WindowInfo[]
}

interface BrowserGetActiveWindowParams {}
interface BrowserGetActiveWindowResult {
  window?: Browser.WindowInfo
}

interface BrowserCreateWindowParams {
  url?: string
  bounds?: Browser.Bounds
  windowType?: Browser.WindowType
  hidden?: boolean
  browserContextId?: Browser.BrowserContextID
}
interface BrowserCreateWindowResult {
  window: Browser.WindowInfo
}

interface BrowserCloseWindowParams {
  windowId: Browser.WindowID
}
interface BrowserCloseWindowResult {}

interface BrowserActivateWindowParams {
  windowId: Browser.WindowID
}
interface BrowserActivateWindowResult {}

interface BrowserSetWindowVisibilityParams {
  windowId: Browser.WindowID
  visible: boolean
  activate?: boolean
}
interface BrowserSetWindowVisibilityResult {
  window: Browser.WindowInfo
  replaced: boolean
  previousWindowId: Browser.WindowID
}
```

实现注意：

- `hidden` window 是 MCP 后台页面的重要能力。
- `setWindowVisibility` 可能替换窗口，调用方必须使用返回的 `window.windowId`。

### 3.9 新增标签页管理命令

这些命令都是在已有 `Browser` domain 中新增。

| 命令 | 参数 | 返回值 | 用途 |
| --- | --- | --- | --- |
| `Browser.getTabs` | `windowId?`, `includeHidden?` | `tabs: TabInfo[]` | 列出真实 tabs，支撑 MCP page registry。 |
| `Browser.getActiveTab` | `windowId?` | `tab?: TabInfo` | 获取 active tab。 |
| `Browser.getTabInfo` | `targetId?`, `tabId?` | `tab: TabInfo` | 按 target 或 tab 查询 tab 信息。 |
| `Browser.createTab` | `url?`, `windowId?`, `index?`, `background?`, `pinned?`, `browserContextId?` | `tab: TabInfo` | 创建新 tab。 |
| `Browser.closeTab` | `targetId?`, `tabId?` | 无 | 关闭 tab。 |
| `Browser.activateTab` | `targetId?`, `tabId?` | 无 | 激活 tab。 |
| `Browser.moveTab` | `targetId?`, `tabId?`, `windowId?`, `index?` | `tab: TabInfo` | 移动 tab。 |
| `Browser.duplicateTab` | `targetId?`, `tabId?` | `tab: TabInfo` | 复制 tab。 |
| `Browser.pinTab` | `targetId?`, `tabId?` | `tab: TabInfo` | pin tab。 |
| `Browser.unpinTab` | `targetId?`, `tabId?` | `tab: TabInfo` | unpin tab。 |
| `Browser.showTab` | `targetId?`, `tabId?`, `windowId?`, `index?`, `activate?` | `tab: TabInfo` | 将 hidden tab 显示到窗口。 |

Schema：

```ts
interface BrowserGetTabsParams {
  windowId?: Browser.WindowID
  includeHidden?: boolean
}
interface BrowserGetTabsResult {
  tabs: Browser.TabInfo[]
}

interface BrowserGetActiveTabParams {
  windowId?: Browser.WindowID
}
interface BrowserGetActiveTabResult {
  tab?: Browser.TabInfo
}

interface BrowserGetTabInfoParams {
  targetId?: Target.TargetID
  tabId?: Browser.TabID
}
interface BrowserGetTabInfoResult {
  tab: Browser.TabInfo
}

interface BrowserCreateTabParams {
  url?: string
  windowId?: Browser.WindowID
  index?: number
  background?: boolean
  pinned?: boolean
  browserContextId?: Browser.BrowserContextID
}
interface BrowserCreateTabResult {
  tab: Browser.TabInfo
}

interface BrowserCloseTabParams {
  targetId?: Target.TargetID
  tabId?: Browser.TabID
}
interface BrowserCloseTabResult {}

interface BrowserActivateTabParams {
  targetId?: Target.TargetID
  tabId?: Browser.TabID
}
interface BrowserActivateTabResult {}

interface BrowserMoveTabParams {
  targetId?: Target.TargetID
  tabId?: Browser.TabID
  windowId?: Browser.WindowID
  index?: number
}
interface BrowserMoveTabResult {
  tab: Browser.TabInfo
}

interface BrowserDuplicateTabParams {
  targetId?: Target.TargetID
  tabId?: Browser.TabID
}
interface BrowserDuplicateTabResult {
  tab: Browser.TabInfo
}

interface BrowserPinTabParams {
  targetId?: Target.TargetID
  tabId?: Browser.TabID
}
interface BrowserPinTabResult {
  tab: Browser.TabInfo
}

interface BrowserUnpinTabParams {
  targetId?: Target.TargetID
  tabId?: Browser.TabID
}
interface BrowserUnpinTabResult {
  tab: Browser.TabInfo
}

interface BrowserShowTabParams {
  targetId?: Target.TargetID
  tabId?: Browser.TabID
  windowId?: Browser.WindowID
  index?: number
  activate?: boolean
}
interface BrowserShowTabResult {
  tab: Browser.TabInfo
}
```

实现注意：

- 单 tab 操作命令中，`targetId` 和 `tabId` 应二选一。
- `Browser.getTabs({ includeHidden: true })` 必须返回 hidden window 中的 tabs，否则 MCP hidden pages 会丢失。
- `Browser.createTab` 返回后，`Browser.getTabInfo({ tabId })` 应能查到该 tab。

### 3.10 新增标签组管理命令

这些命令都是在已有 `Browser` domain 中新增。

| 命令 | 参数 | 返回值 | 用途 |
| --- | --- | --- | --- |
| `Browser.getTabGroups` | `windowId?` | `groups: TabGroupInfo[]` | 查询所有或指定窗口标签组。 |
| `Browser.createTabGroup` | `tabIds`, `title?` | `group: TabGroupInfo` | 用一组 tabs 创建标签组。 |
| `Browser.updateTabGroup` | `groupId`, `title?`, `color?`, `collapsed?` | `group: TabGroupInfo` | 更新标签组属性。 |
| `Browser.closeTabGroup` | `groupId` | 无 | 关闭标签组内所有 tabs。 |
| `Browser.addTabsToGroup` | `groupId`, `tabIds` | `group: TabGroupInfo` | 将 tabs 加入已有标签组。 |
| `Browser.removeTabsFromGroup` | `tabIds` | 无 | 将 tabs 从标签组移出。 |
| `Browser.moveTabGroup` | `groupId`, `windowId?`, `index?` | `group: TabGroupInfo` | 移动标签组。 |

Schema：

```ts
interface BrowserGetTabGroupsParams {
  windowId?: Browser.WindowID
}
interface BrowserGetTabGroupsResult {
  groups: Browser.TabGroupInfo[]
}

interface BrowserCreateTabGroupParams {
  tabIds: Browser.TabID[]
  title?: string
}
interface BrowserCreateTabGroupResult {
  group: Browser.TabGroupInfo
}

interface BrowserUpdateTabGroupParams {
  groupId: Browser.TabGroupID
  title?: string
  color?: string
  collapsed?: boolean
}
interface BrowserUpdateTabGroupResult {
  group: Browser.TabGroupInfo
}

interface BrowserCloseTabGroupParams {
  groupId: Browser.TabGroupID
}
interface BrowserCloseTabGroupResult {}

interface BrowserAddTabsToGroupParams {
  groupId: Browser.TabGroupID
  tabIds: Browser.TabID[]
}
interface BrowserAddTabsToGroupResult {
  group: Browser.TabGroupInfo
}

interface BrowserRemoveTabsFromGroupParams {
  tabIds: Browser.TabID[]
}
interface BrowserRemoveTabsFromGroupResult {}

interface BrowserMoveTabGroupParams {
  groupId: Browser.TabGroupID
  windowId?: Browser.WindowID
  index?: number
}
interface BrowserMoveTabGroupResult {
  group: Browser.TabGroupInfo
}
```

实现注意：

- `tabIds` 来自真实浏览器 tab id，不是 MCP page id。
- 内核实现需要处理无序、重复、跨窗口 tab ids 的错误边界，避免 `TabStripModel` CHECK。

### 3.11 新增 target/tab 互转命令

这些命令都是在已有 `Browser` domain 中新增。

| 命令 | 参数 | 返回值 | 用途 |
| --- | --- | --- | --- |
| `Browser.getTabForTarget` | `targetId?` | `tabId`, `windowId` | 从 DevTools target 找真实 tab/window。 |
| `Browser.getTargetForTab` | `tabId` | `targetId`, `windowId` | 从真实 tab 找 DevTools target。 |

Schema：

```ts
interface BrowserGetTabForTargetParams {
  targetId?: Target.TargetID
}
interface BrowserGetTabForTargetResult {
  tabId: Browser.TabID
  windowId: Browser.WindowID
}

interface BrowserGetTargetForTabParams {
  tabId: Browser.TabID
}
interface BrowserGetTargetForTabResult {
  targetId: Target.TargetID
  windowId: Browser.WindowID
}
```

用途：

- 补齐 `Target` 与浏览器 tab/window 的直接互转能力。
- Android 当前主要支持这两个命令；桌面也应支持。

## 4. `Target` Domain 修改方案

`Target` 是原生 Chromium CDP 已有 domain。本方案只扩展 `Target.TargetInfo`，不新增 `Target` domain。

### 4.1 新增字段

Schema：

```ts
interface Target.TargetInfo {
  // 原生字段省略。
  subtype?: string

  // BrowserOS 新增字段。
  tabId?: Browser.TabID
  windowId?: Browser.WindowID
}
```

新增字段：

| 字段 | 类型 | 必需 | 用途 |
| --- | --- | --- | --- |
| `tabId` | `Browser.TabID` | 否 | 对 page/frame target，返回所属浏览器 tab id。 |
| `windowId` | `Browser.WindowID` | 否 | 当 target 属于某个窗口中的 tab 时，返回窗口 id。 |

用途：

- 让 `Target.getTargets` 的返回值可以直接关联真实 tab/window。
- 给 `browser-core` 的 backend target 列举提供底层映射能力。
- 当前 MCP page registry 主路径仍是 `Browser.getTabs`，但该扩展可以作为 fallback 和未来工具能力。

实现注意：

- 只对与 tab 相关的 target 返回字段。
- worker、browser target 等没有 tab 身份的 target 可以不返回。
- 只改 PDL 不够，必须在 `target_handler.cc::BuildTargetInfo` 中实际填充。

## 5. `Bookmarks` Domain 修改方案

`Bookmarks` 是新增 CDP domain。原生 Chromium CDP 不提供这一组标准书签管理命令。

### 5.1 新增 domain

Schema：

```pdl
domain Bookmarks
  depends on Browser
```

注册要求：

- 在 `browser_protocol.pdl` 中 include `domains/Bookmarks.pdl`。
- 在 `devtools_protocol/BUILD.gn` 中加入 `"domains/Bookmarks.pdl"`。
- 在 `inspector_protocol_config.json` 中加入 `Bookmarks` domain 的命令 include。
- 在 `ChromeDevToolsSession` 中注册 `BookmarksHandler`。

### 5.2 新增类型

```ts
type Bookmarks.BookmarkID = string

type Bookmarks.BookmarkNodeType = 'url' | 'folder'

interface Bookmarks.BookmarkNode {
  id: Bookmarks.BookmarkID
  parentId?: Bookmarks.BookmarkID
  index?: number
  title: string
  url?: string
  type: Bookmarks.BookmarkNodeType
  dateAdded: number
  dateLastUsed?: number
}
```

新增字段：

| 字段 | 类型 | 必需 | 用途 |
| --- | --- | --- | --- |
| `id` | `BookmarkID` | 是 | bookmark 或 folder 节点 id。 |
| `parentId` | `BookmarkID` | 否 | 父 folder id。 |
| `index` | `integer` | 否 | 节点在父 folder 中的位置。 |
| `title` | `string` | 是 | bookmark 或 folder 标题。 |
| `url` | `string` | 否 | URL bookmark 地址；folder 不返回。 |
| `type` | `BookmarkNodeType` | 是 | `url` 或 `folder`。 |
| `dateAdded` | `number` | 是 | 添加时间，毫秒时间戳。 |
| `dateLastUsed` | `number` | 否 | 最近使用时间，毫秒时间戳。 |

### 5.3 新增命令

| 命令 | 参数 | 返回值 | 用途 |
| --- | --- | --- | --- |
| `Bookmarks.getBookmarks` | `folderId?` | `nodes: BookmarkNode[]` | 查询全部书签，或查询某个 folder 的 descendants。 |
| `Bookmarks.searchBookmarks` | `query`, `maxResults?` | `results: BookmarkNode[]` | 按标题或 URL substring 搜索书签。 |
| `Bookmarks.createBookmark` | `title`, `url?`, `parentId?`, `index?` | `node: BookmarkNode` | 创建 bookmark 或 folder。 |
| `Bookmarks.updateBookmark` | `id`, `title?`, `url?` | `node: BookmarkNode` | 更新标题或 URL。 |
| `Bookmarks.moveBookmark` | `id`, `parentId?`, `index?` | `node: BookmarkNode` | 移动 bookmark 或 folder。 |
| `Bookmarks.removeBookmark` | `id` | 无 | 删除 bookmark 或 folder；folder 递归删除。 |

Schema：

```ts
interface BookmarksGetBookmarksParams {
  folderId?: Bookmarks.BookmarkID
}
interface BookmarksGetBookmarksResult {
  nodes: Bookmarks.BookmarkNode[]
}

interface BookmarksSearchBookmarksParams {
  query: string
  maxResults?: number
}
interface BookmarksSearchBookmarksResult {
  results: Bookmarks.BookmarkNode[]
}

interface BookmarksCreateBookmarkParams {
  title: string
  url?: string
  parentId?: Bookmarks.BookmarkID
  index?: number
}
interface BookmarksCreateBookmarkResult {
  node: Bookmarks.BookmarkNode
}

interface BookmarksUpdateBookmarkParams {
  id: Bookmarks.BookmarkID
  title?: string
  url?: string
}
interface BookmarksUpdateBookmarkResult {
  node: Bookmarks.BookmarkNode
}

interface BookmarksMoveBookmarkParams {
  id: Bookmarks.BookmarkID
  parentId?: Bookmarks.BookmarkID
  index?: number
}
interface BookmarksMoveBookmarkResult {
  node: Bookmarks.BookmarkNode
}

interface BookmarksRemoveBookmarkParams {
  id: Bookmarks.BookmarkID
}
interface BookmarksRemoveBookmarkResult {}
```

当前 MCP 影响：

- 当前没有一等 `bookmarks` MCP tool。
- `run` tool 可以通过 `browser.cdp('Bookmarks.*')` raw CDP 调用。

## 6. `History` Domain 修改方案

`History` 是新增 CDP domain。原生 Chromium CDP 不提供这一组标准历史记录管理命令。

### 6.1 新增 domain

Schema：

```pdl
domain History
```

注册要求：

- 在 `browser_protocol.pdl` 中 include `domains/History.pdl`。
- 在 `devtools_protocol/BUILD.gn` 中加入 `"domains/History.pdl"`。
- 在 `inspector_protocol_config.json` 中加入 `History` domain 的命令 include。
- `History` 命令是 async handler，必须在 `inspector_protocol_config.json` 中配置 async。
- 在 `ChromeDevToolsSession` 中注册 `HistoryHandler`。

### 6.2 新增类型

```ts
type History.HistoryEntryID = string

interface History.HistoryEntry {
  id: History.HistoryEntryID
  url: string
  title: string
  lastVisitTime: number
  visitCount: number
  typedCount: number
}
```

新增字段：

| 字段 | 类型 | 必需 | 用途 |
| --- | --- | --- | --- |
| `id` | `HistoryEntryID` | 是 | history entry id。 |
| `url` | `string` | 是 | 历史记录 URL。 |
| `title` | `string` | 是 | 历史记录标题。 |
| `lastVisitTime` | `number` | 是 | 最近访问时间，毫秒时间戳。 |
| `visitCount` | `integer` | 是 | 访问次数。 |
| `typedCount` | `integer` | 是 | 用户手动输入访问次数。 |

### 6.3 新增命令

| 命令 | 参数 | 返回值 | 用途 |
| --- | --- | --- | --- |
| `History.search` | `query`, `maxResults?`, `startTime?`, `endTime?` | `entries: HistoryEntry[]` | 按文本和时间范围搜索历史。 |
| `History.getRecent` | `maxResults?` | `entries: HistoryEntry[]` | 查询最近历史记录。 |
| `History.deleteUrl` | `url` | 无 | 删除指定 URL 的历史记录。 |
| `History.deleteRange` | `startTime`, `endTime` | 无 | 删除时间范围内历史记录。 |

Schema：

```ts
interface HistorySearchParams {
  query: string
  maxResults?: number
  startTime?: number
  endTime?: number
}
interface HistorySearchResult {
  entries: History.HistoryEntry[]
}

interface HistoryGetRecentParams {
  maxResults?: number
}
interface HistoryGetRecentResult {
  entries: History.HistoryEntry[]
}

interface HistoryDeleteUrlParams {
  url: string
}
interface HistoryDeleteUrlResult {}

interface HistoryDeleteRangeParams {
  startTime: number
  endTime: number
}
interface HistoryDeleteRangeResult {}
```

当前 MCP 影响：

- 当前没有一等 `history` MCP tool。
- `run` tool 可以通过 `browser.cdp('History.*')` raw CDP 调用。

## 7. MCP 适配收益

| 能力 | 依赖 CDP 修改 | 对 MCP 的收益 |
| --- | --- | --- |
| 窗口管理 | `Browser.WindowInfo` + window commands | `windows` tool 可列出、创建、关闭、激活、隐藏/显示窗口。 |
| 标签页管理 | `Browser.TabInfo` + tab commands | `tabs` tool 可稳定维护 `pageId -> tabId/targetId` 映射。 |
| hidden pages | `WindowInfo.isVisible`、`TabInfo.isHidden`、`createWindow(hidden)`、`getTabs(includeHidden)` | AI 可在隐藏窗口中执行后台任务，不打扰用户。 |
| 页面类 tools | `TabInfo.targetId` | `snapshot/act/navigate/evaluate` 等能 attach 到正确 page target。 |
| 标签组管理 | `TabGroupInfo` + tab group commands | `tab_groups` tool 可创建、更新、折叠、移出、关闭标签组。 |
| target fallback | `TargetInfo.tabId/windowId` | raw target 列举可映射回浏览器 tab/window。 |
| 书签 | `Bookmarks` domain | raw CDP 可管理书签，未来可封装一等 MCP tool。 |
| 历史记录 | `History` domain | raw CDP 可查询/删除历史，未来可封装一等 MCP tool。 |

## 8. 验收标准

### 8.1 协议生成

- Chromium protocol 生成后，`Browser` backend 包含新增窗口、标签页、标签组、target/tab 互转命令。
- `Target.TargetInfo` 生成类型包含 optional `tabId` / `windowId`。
- 生成产物包含 `Bookmarks` / `History` domain。
- `History` handler 生成签名与 async 实现匹配。

### 8.2 Raw CDP 验收

```ts
await browser.cdp('Browser.getWindows', {})
await browser.cdp('Browser.getTabs', { includeHidden: true })
await browser.cdp('Browser.getTabGroups', {})
await browser.cdp('Target.getTargets', {})
await browser.cdp('Bookmarks.searchBookmarks', { query: 'test', maxResults: 10 })
await browser.cdp('History.search', { query: '', maxResults: 10 })
```

### 8.3 MCP 验收

- `tabs list` 能返回可见和 hidden pages。
- `tabs new hidden=true` 能创建 hidden window 中的 page。
- `tabs close page=<id>` 能关闭对应真实 tab。
- `windows list/create/close/activate/set_visibility` 能正常工作。
- `tab_groups list/create/update/ungroup/close` 能正常工作。
- 页面类 tools 对同一个 `pageId` 操作时不会误 attach 到其他 tab。

## 9. 常见风险

| 风险 | 影响 | 规避 |
| --- | --- | --- |
| `Browser.getTabs` 不返回 hidden tabs | hidden MCP pages 丢失 | `includeHidden=true` 必须覆盖 hidden windows。 |
| `TabInfo.targetId` 不稳定 | page registry 重复创建 page id | 保证同一 tab 生命周期内 target 映射稳定。 |
| `TabInfo.tabId` 缺失 | tabs close / tab_groups 无法工作 | 所有 page tab 必须返回 `tabId`。 |
| `setWindowVisibility` 替换窗口但调用方继续使用旧 id | 后续窗口操作失败 | 调用方必须使用返回的 `window.windowId`。 |
| 只改 `Target.pdl` 不填充运行时字段 | `Target.getTargets` 看不到 `tabId/windowId` | 修改 `target_handler.cc::BuildTargetInfo`。 |
| `History` async 配置遗漏 | C++ handler 签名不匹配 | 在 `inspector_protocol_config.json` 中标记 async。 |
| tab group 操作不处理无序/重复 tab ids | Chromium CHECK 或分组异常 | 实现前校验并排序/去重/报错。 |
