#

BrowserOS 修改/扩展 CDP 的核心意义：把“浏览器 UI 层的真实状态和能力”暴露给自动化系统，而不只是在页面 Target 层做操作。

## 原生 Chrome CDP 更偏调试协议，擅长：

```text
操作某个 target/page
执行 JS
读 DOM/AX tree
截图/PDF
网络/Runtime/Performance 调试
```

但它不擅长回答这些浏览器级问题：

```text
用户当前看的是哪个 tab？
当前 active window 是哪个？
某个 window 里 active tab 是哪个？
tab 的真实顺序、隐藏状态、分组状态是什么？
如何创建 hidden window/tab？
如何稳定管理 tab groups？
如何按真实 window/tab 语义移动、显示、隐藏、关闭？
```

## BrowserOS 自定义 CDP 的价值就在这里。

**主要好处**

```text
1. 真正的 active tab/window 语义
   Browser.getActiveTab、Browser.getWindows 可以知道用户正在看的窗口和 tab。
   这解决了原生 Chrome CDP 无法可靠获取当前 active tab 的硬伤。

2. 稳定的 tab/window 管理
   Browser.getTabs、createTab、closeTab、moveTab、showTab 等比 Target.* 更贴近浏览器 UI。
   MCP 不用把 target 当 tab 猜。

3. 支持多窗口场景
   WindowInfo 带 isActive、isVisible、activeTabId、tabCount。
   agent 可以知道哪个窗口前台、哪个窗口隐藏、哪个窗口里哪个 tab 被选中。

4. 支持 tab groups
   Browser.getTabGroups/createTabGroup/updateTabGroup/addTabsToGroup/removeTabsFromGroup 等能力让 tab 分组成为一等能力。
   原生 Chrome CDP 没有这套 API。

5. 支持 hidden window / hidden tab
   对 agent 很有用：可以后台开临时页面、隔离任务、不打扰用户当前浏览。

6. pageId 映射更可靠
   BrowserOS 能提供真实 tabId、windowId、groupId、isActive 等信息。
   MCP 内部 pageId 可以更好地 reconcile，而不是只靠 targetId 猜。

7. 更适合 Agent
   Agent 需要的是“浏览器操作语义”，不是纯调试语义。
   BrowserOS CDP 把 tab、window、group、active 状态变成可查询、可控制、可验证的结构化能力。
```

**对 MCP 的意义**

```text
原生 Chrome CDP:
  MCP 只能做 page-level automation。
  tabs/window/group 只能部分支持或 best-effort。

BrowserOS CDP:
  MCP 可以提供完整 browser-level automation。
  tabs、windows、tab_groups、active page、hidden pages 都能稳定工作。
```

## 总结：

```text
原生 CDP 让 agent 能“操作网页”；
BrowserOS CDP 让 agent 能“理解并管理整个浏览器”。
```
