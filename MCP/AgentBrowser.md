## 🧩 MCP 工具定义

所有工具均通过 `client.call(toolName, arguments)` 调用，返回 JSON 格式结果。

### 1. 核心快照工具（最重要）

| 工具 | 功能 | 参数 | 返回值 |
|------|------|------|--------|
| `browser_snapshot` | 获取页面可访问性树快照，每个元素附带唯一引用（如 `@e1`） | `{ verbose?: boolean, annotate?: boolean }` | `{ snapshot: string, refs: { [ref: string]: { role, name, selector } } }` |

- **`snapshot`**：文本形式的可访问性树，每行格式类似 `[ref] role "accessible name"`，例如 `@e1 button "Submit"`。
- **`refs`**：引用到实际 CSS 选择器的映射，供其他工具使用。

AI 应**首先调用 `browser_snapshot`** 获取当前页面结构，然后根据其中的 `@ref` 执行后续操作。

### 2. 元素交互工具（支持 selector 或 ref）

| 工具 | 对应 CLI | 参数 | 返回值 |
|------|----------|------|--------|
| `browser_click` | `click <sel>` | `{ target: string }` (selector 或 ref，如 `"@e1"`) | `{ success: boolean }` |
| `browser_dblclick` | `dblclick <sel>` | `{ target: string }` | `{ success }` |
| `browser_fill` | `fill <sel> <text>` | `{ target: string, text: string }` | `{ success }` |
| `browser_type` | `type <sel> <text>` | `{ target: string, text: string }` | `{ success }` |
| `browser_press` | `press <key>` | `{ key: string }` | `{ success }` |
| `browser_hover` | `hover <sel>` | `{ target: string }` | `{ success }` |
| `browser_select` | `select <sel> <val>` | `{ target: string, value: string }` | `{ success }` |
| `browser_check` | `check <sel>` | `{ target: string }` | `{ success }` |
| `browser_uncheck` | `uncheck <sel>` | `{ target: string }` | `{ success }` |
| `browser_focus` | `focus <sel>` | `{ target: string }` | `{ success }` |
| `browser_upload` | `upload <sel> <files>` | `{ target: string, files: string[] }` | `{ success }` |
| `browser_drag` | `drag <src> <tgt>` | `{ source: string, target: string }` | `{ success }` |
| `browser_scroll` | `scroll <dir> [px]` | `{ direction: "up"\|"down"\|"left"\|"right", pixels?: number, selector?: string }` | `{ success }` |
| `browser_scroll_into_view` | `scrollintoview <sel>` | `{ target: string }` | `{ success }` |
| `browser_keyboard_type` | `keyboard type <text>` | `{ text: string }` | `{ success }` |
| `browser_keyboard_insert` | `keyboard inserttext <text>` | `{ text: string }` | `{ success }` |
| `browser_keydown` | `keydown <key>` | `{ key: string }` | `{ success }` |
| `browser_keyup` | `keyup <key>` | `{ key: string }` | `{ success }` |

> **target 参数**：可以是 CSS 选择器（如 `"#submit"`）或快照中的引用（如 `"@e1"`）。MCP 服务端会自动将 `@e1` 转换为实际选择器。

### 3. 导航工具

| 工具 | 对应 CLI | 参数 | 返回值 |
|------|----------|------|--------|
| `browser_navigate` | `open <url>` | `{ url: string }` | `{ success }` |
| `browser_back` | `back` (无直接命令，但可通过 `eval` 实现) | `{}` | `{ success }` |
| `browser_forward` | `forward` | `{}` | `{ success }` |
| `browser_reload` | `reload` | `{}` | `{ success }` |
| `browser_close` | `close` | `{}` | `{ success }` |

### 4. 获取信息工具

| 工具 | 对应 CLI | 参数 | 返回值 |
|------|----------|------|--------|
| `browser_get_text` | `get text <sel>` | `{ target: string }` | `{ text: string }` |
| `browser_get_html` | `get html <sel>` | `{ target: string }` | `{ html: string }` |
| `browser_get_value` | `get value <sel>` | `{ target: string }` | `{ value: string }` |
| `browser_get_attr` | `get attr <sel> <attr>` | `{ target: string, attribute: string }` | `{ value: string }` |
| `browser_get_title` | `get title` | `{}` | `{ title: string }` |
| `browser_get_url` | `get url` | `{}` | `{ url: string }` |
| `browser_get_count` | `get count <sel>` | `{ selector: string }` | `{ count: number }` |
| `browser_get_box` | `get box <sel>` | `{ target: string }` | `{ x, y, width, height }` |
| `browser_get_styles` | `get styles <sel>` | `{ target: string, properties?: string[] }` | `{ styles: object }` |

### 5. 状态检查工具

| 工具 | 对应 CLI | 参数 | 返回值 |
|------|----------|------|--------|
| `browser_is_visible` | `is visible <sel>` | `{ target: string }` | `{ visible: boolean }` |
| `browser_is_enabled` | `is enabled <sel>` | `{ target: string }` | `{ enabled: boolean }` |
| `browser_is_checked` | `is checked <sel>` | `{ target: string }` | `{ checked: boolean }` |

### 6. 等待工具

| 工具 | 对应 CLI | 参数 | 返回值 |
|------|----------|------|--------|
| `browser_wait` | `wait <selector>` 或 `wait <ms>` 等 | `{ selector?: string, ms?: number, text?: string, url?: string, loadState?: "load"\|"networkidle", fn?: string, state?: "visible"\|"hidden" }` | `{ success }` |

### 7. 标签页管理

| 工具 | 对应 CLI | 参数 | 返回值 |
|------|----------|------|--------|
| `browser_tab_list` | `tab` | `{}` | `{ tabs: [{ index, url, title }] }` |
| `browser_tab_new` | `tab new [url]` | `{ url?: string }` | `{ tabIndex: number }` |
| `browser_tab_switch` | `tab <n>` | `{ index: number }` | `{ success }` |
| `browser_tab_close` | `tab close [n]` | `{ index?: number }` | `{ success }` |

### 8. 截图与 PDF

| 工具 | 对应 CLI | 参数 | 返回值 |
|------|----------|------|--------|
| `browser_screenshot` | `screenshot [path]` | `{ path?: string, fullPage?: boolean, format?: "png"\|"jpeg", quality?: number }` | `{ path: string, data?: string }` (Base64 可选) |
| `browser_pdf` | `pdf <path>` | `{ path: string }` | `{ success }` |

### 9. 执行脚本

| 工具 | 对应 CLI | 参数 | 返回值 |
|------|----------|------|--------|
| `browser_eval` | `eval <js>` | `{ script: string, base64?: boolean }` | `{ result: any }` |

### 10. 调试工具

| 工具 | 对应 CLI | 参数 | 返回值 |
|------|----------|------|--------|
| `browser_console_messages` | (扩展) | `{ limit?: number }` | `{ messages: [{ level, text }] }` |
| `browser_network_requests` | `network requests` | `{ filter?: string, type?: string, method?: string, status?: string }` | `{ requests: [] }` |
| `browser_network_har_start` | `network har start` | `{}` | `{ success }` |
| `browser_network_har_stop` | `network har stop [path]` | `{ path?: string }` | `{ harPath: string }` |

### 11. Cookie 与存储

| 工具 | 对应 CLI | 参数 | 返回值 |
|------|----------|------|--------|
| `browser_cookies_get` | `cookies` | `{}` | `{ cookies: [] }` |
| `browser_cookies_set` | `cookies set <name> <val>` | `{ name: string, value: string }` | `{ success }` |
| `browser_cookies_clear` | `cookies clear` | `{}` | `{ success }` |
| `browser_storage_local_get` | `storage local [key]` | `{ key?: string }` | `{ data: object }` |
| `browser_storage_local_set` | `storage local set <k> <v>` | `{ key: string, value: string }` | `{ success }` |
| `browser_storage_local_clear` | `storage local clear` | `{}` | `{ success }` |
| `browser_storage_session_*` | 同上 | 类似 | 类似 |

### 12. 弹窗处理

| 工具 | 对应 CLI | 参数 | 返回值 |
|------|----------|------|--------|
| `browser_dialog_accept` | `dialog accept [text]` | `{ promptText?: string }` | `{ success }` |
| `browser_dialog_dismiss` | `dialog dismiss` | `{}` | `{ success }` |
| `browser_dialog_status` | `dialog status` | `{}` | `{ open: boolean, type?: string, message?: string }` |

### 13. 其他设置工具

| 工具 | 对应 CLI | 参数 | 返回值 |
|------|----------|------|--------|
| `browser_set_viewport` | `set viewport <w> <h> [scale]` | `{ width: number, height: number, scale?: number }` | `{ success }` |
| `browser_set_device` | `set device <name>` | `{ name: string }` | `{ success }` |
| `browser_set_geo` | `set geo <lat> <lng>` | `{ latitude: number, longitude: number }` | `{ success }` |
| `browser_set_offline` | `set offline [on\|off]` | `{ enabled: boolean }` | `{ success }` |
| `browser_set_headers` | `set headers <json>` | `{ headers: object }` | `{ success }` |
| `browser_set_credentials` | `set credentials <u> <p>` | `{ username: string, password: string }` | `{ success }` |
| `browser_set_media` | `set media [dark\|light]` | `{ colorScheme: "dark"\|"light" }` | `{ success }` |

### 14. 批量执行

| 工具 | 对应 CLI | 参数 | 返回值 |
|------|----------|------|--------|
| `browser_batch` | `batch --json` | `{ commands: Array<[string, ...any]>, bail?: boolean }` | `{ results: any[] }` |

### 15. 剪贴板

| 工具 | 对应 CLI | 参数 | 返回值 |
|------|----------|------|--------|
| `browser_clipboard_read` | `clipboard read` | `{}` | `{ text: string }` |
| `browser_clipboard_write` | `clipboard write <text>` | `{ text: string }` | `{ success }` |

---

## 💡 详细示例：基于快照的 AI 自动化流程

假设 AI 需要完成：打开网页，填写搜索框，点击搜索按钮，获取结果数量。

### 步骤 1：获取快照

```javascript
const snapshot = await client.call('browser_snapshot', { verbose: true });
console.log(snapshot.snapshot);
```

**输出示例**（文本形式）：
```
@e1 textbox "Search" focusable: true
@e2 button "Go" focusable: true
@e3 link "Home"
@e4 heading "Results:"
@e5 listitem "Item 1"
...
```
同时返回 `snapshot.refs` 映射：
```json
{
  "refs": {
    "@e1": { "role": "textbox", "name": "Search", "selector": "#search-input" },
    "@e2": { "role": "button", "name": "Go", "selector": "button[type='submit']" }
  }
}
```

### 步骤 2：使用引用进行交互

AI 提取到搜索框引用 `@e1` 和按钮引用 `@e2`，然后调用：

```javascript
// 填写搜索框
await client.call('browser_fill', { target: '@e1', text: 'puppeteer' });
// 点击搜索按钮
await client.call('browser_click', { target: '@e2' });
```

### 步骤 3：等待结果并获取数据

```javascript
// 等待结果出现
await client.call('browser_wait', { selector: '.results', timeout: 5000 });
// 获取结果数量
const { text: countText } = await client.call('browser_get_text', { target: '.result-count' });
// 或者通过 eval
const { result: count } = await client.call('browser_eval', {
  script: '() => document.querySelectorAll(".result-item").length'
});
```

### 完整示例脚本（AI 可逐行执行）

```javascript
// 1. 导航
await client.call('browser_navigate', { url: 'https://example.com/search' });

// 2. 获取快照，理解页面
const snap = await client.call('browser_snapshot', {});
// AI 解析 snap.snapshot，找到搜索框和按钮的 ref，例如 @e1, @e2

// 3. 填充搜索框
await client.call('browser_fill', { target: '@e1', text: 'MCP tools' });

// 4. 点击搜索按钮
await client.call('browser_click', { target: '@e2' });

// 5. 等待结果加载完成
await client.call('browser_wait', { loadState: 'networkidle' });

// 6. 获取结果标题
const { title } = await client.call('browser_get_title', {});
console.log('Page title:', title);

// 7. 截图保存
await client.call('browser_screenshot', { path: './search_result.png', fullPage: true });

// 8. 提取所有结果链接的 href
const { result: links } = await client.call('browser_eval', {
  script: '() => Array.from(document.querySelectorAll("a.result-link")).map(a => a.href)'
});
```

### 处理弹窗示例

```javascript
// 点击可能触发 confirm 的按钮
await client.call('browser_click', { target: '#delete-btn' });

// 检查是否有弹窗
const { open, type, message } = await client.call('browser_dialog_status', {});
if (open && type === 'confirm') {
  await client.call('browser_dialog_accept', {}); // 点击确定
}
```

### 批量执行示例（减少往返）

```javascript
await client.call('browser_batch', {
  commands: [
    ['open', 'https://example.com'],
    ['fill', '@e1', 'query'],
    ['click', '@e2'],
    ['wait', '--load', 'networkidle'],
    ['screenshot', 'result.png']
  ],
  bail: true
});
```

---

## 📌 总结

- **AI 唯一需要理解的是 `browser_snapshot` 返回的引用格式**（如 `@e1`）。  
- 所有交互工具均接受 `target: "@e1"` 或标准 CSS 选择器。  
- 无需自然语言处理，AI 直接解析结构化快照文本即可定位元素。  
- 提供完整的等待、数据提取、弹窗处理、网络监控等能力，覆盖 `agent-browser` 全部功能。  

这套 MCP 工具清单让 AI 能够**高效、精确**地控制浏览器，同时保持与 CLI 命令的一一对应，便于实现和调试。

这是对你之前提出的 MCP 工具清单的完整补充说明。它将每个工具的功能与 Chrome DevTools Protocol (CDP) 的具体命令一一对应，旨在帮助你理解其实现原理和技术边界。

这套工具清单与 **`agent-browser`** 的设计哲学一致：通过提供基于快照（Snapshot）和稳定引用（Ref）的接口，让 AI 能高效、精确地控制浏览器。

### ⚙️ MCP 工具与 CDP 命令对应说明

> 注：`agent_` 开头的工具属于高级封装，由多个基础 CDP 命令组合实现，旨在简化 AI Agent 的决策流程。

---

### 📌 核心必备工具

这些工具构成了浏览器自动化的基础。

| MCP 工具 | 功能说明 | 对应 CDP 命令 (主命令/组合) | 说明 |
| :--- | :--- | :--- | :--- |
| **导航与页面控制** |
| `browser_navigate` | 导航到指定 URL | `Page.navigate` | 加载新页面，可监听 `Page.frameNavigated` 等事件确认完成[reference:0]。 |
| `browser_new_page` | 新建标签页 | `Target.createTarget` | 创建一个新的浏览器上下文（Tab）[reference:1]。 |
| `browser_close_page` | 关闭标签页 | `Target.closeTarget` | 关闭指定的页面或标签页[reference:2]。 |
| `browser_list_pages` | 列出所有标签页 | `Target.getTargets` | 获取所有已发现的 Target（页面/Service Worker 等）信息[reference:3]。 |
| `browser_select_page` | 切换活动标签页 | `Target.activateTarget` | 激活（聚焦）指定的 Target[reference:4]。 |
| **元素交互** |
| `browser_click` | 点击元素 | `Runtime.evaluate` + `DOM` + `Input.dispatchMouseEvent` | 1. 通过 `DOM.getDocument` 获取文档根节点[reference:5]。<br>2. 通过 `DOM.querySelector` 定位目标元素，获取其坐标[reference:6]。<br>3. 通过 `Input.dispatchMouseEvent` 在目标位置合成并派发 `click` 事件[reference:7]。 |
| `browser_dblclick` | 双击元素 | `Input.dispatchMouseEvent` | 连续派发两次 `mousePressed` 和 `mouseReleased` 事件，并将 `clickCount` 参数设为 2[reference:8]。 |
| `browser_fill` | 填充输入框 | `DOM.querySelector` + `Runtime.evaluate` | 找到目标元素后，通过 `Runtime.evaluate` 执行 JS，将其 `value` 属性设置为指定文本，并手动触发 `input` 和 `change` 事件。 |
| `browser_type` | 逐字键入文本 | `Input.dispatchKeyEvent` | 为文本中的每个字符，先后派发 `keyDown` 和 `keyUp` 事件[reference:9]。 |
| `browser_press` | 按下组合键 | `Input.dispatchKeyEvent` | 用于派发非字符输入事件，如 `Enter`、`Tab` 或 `Control+A` 等组合键[reference:10]。 |
| `browser_hover` | 鼠标悬停 | `Input.dispatchMouseEvent` | 将鼠标移动到目标元素中心点，派发 `mouseMoved` 事件[reference:11]。 |
| `browser_drag` | 拖拽元素 | `Input.dispatchMouseEvent` | 1. 在源元素上派发 `mousePressed` 事件。<br>2. 派发一系列 `mouseMoved` 事件。<br>3. 在目标元素上派发 `mouseReleased` 事件。 |
| `browser_upload` | 上传文件 | `DOM.setFileInputFiles` | 直接操作 `<input type="file">` 元素，将指定的本地文件路径列表设置为该元素的值。 |
| **数据提取与分析** |
| `browser_evaluate_script` | 执行 JS | `Runtime.evaluate` | 在页面上下文中执行 JavaScript 表达式，返回执行结果的 `RemoteObject`[reference:12]。 |
| `browser_get_page_html` | 获取页面 HTML | `DOM.getDocument` + `DOM.getOuterHTML` | 1. 通过 `DOM.getDocument` 获取文档根节点的 `NodeId`[reference:13]。<br>2. 通过 `DOM.getOuterHTML` 获取该节点的完整 HTML 内容。 |
| `browser_take_snapshot` | 获取页面快照 | `Accessibility.getFullAXTree` | 获取整个页面的可访问性树（Accessibility Tree），并将其格式化为文本，每个节点会分配一个 `@ref` 标识符。 |
| `browser_take_screenshot` | 截图 | `Page.captureScreenshot` | 捕获当前页面或指定区域的屏幕截图，可以设置格式、质量等参数[reference:14][reference:15]。 |
| **等待与同步** |
| `browser_wait_for` | 等待条件满足 | `Runtime.evaluate` + 轮询 | 周期性执行一段 JS 脚本，检查条件（如元素存在、文本出现）是否满足，直到超时。 |
| **弹窗处理** |
| `browser_handle_dialog` | 处理 JS 弹窗 | `Page.handleJavaScriptDialog` | 接受或拒绝 `alert`、`confirm`、`prompt` 等 JavaScript 对话框[reference:16]。 |
| **调试基础** |
| `browser_list_console_messages` | 获取控制台消息 | `Runtime.enable` + `Runtime.consoleAPICalled` 事件 | 1. 通过 `Runtime.enable` 启用运行时事件监听。<br>2. 收集 `Runtime.consoleAPICalled` 事件中的日志数据。 |

---

### 📌 高级/专业工具

这些工具面向更复杂的自动化、指纹管理和 AI 集成场景。

| MCP 工具 | 功能说明 | 对应 CDP 命令 (主命令/组合) | 说明 |
| :--- | :--- | :--- | :--- |
| **Agent Browser 高级接口** |
| `agent_interact` | 智能元素交互 | `browser_snapshot` + `browser_click` / `browser_fill` | 接收 `@ref` 引用，将其映射为 CSS 选择器，再调用对应的基础交互工具。 |
| `agent_wait` | 智能等待 | `browser_wait_for` | 增强版等待，支持等待 `@ref` 出现/消失等高级条件。 |
| `agent_extract` | 结构化数据提取 | `browser_evaluate_script` | 根据提供的 JSON Schema 或指令，自动执行 JS 提取并结构化页面数据。 |
| `agent_task` | 端到端任务执行 | 多步骤编排 | 将自然语言任务分解为一系列 `agent_interact` 等子任务，并协调执行。 |
| `agent_observe` | 观察页面变化 | `browser_snapshot` 对比 | 对比前后两次快照，返回新增、删除或变化的元素 `@ref` 列表。 |
| `agent_human_handoff` | 请求人工介入 | (无) | 挂起自动化任务，保存当前状态，通过外部机制通知人工处理。 |
| **多指纹浏览器管理** |
| `browser_create_environment` | 创建隔离环境 | `Target.createBrowserContext` | 创建一个全新的、隔离的浏览器上下文（Incognito 模式），其 Cookies、Storage 等与现有环境完全隔离[reference:17]。 |
| `browser_start_environment` | 启动/连接环境 | `Target.createTarget` | 在指定的 BrowserContext 中创建一个新页面（Target），并建立 CDP 连接。 |
| `browser_stop_environment` | 停止环境 | `Target.closeTarget` | 关闭环境下的所有页面，并断开 CDP 连接。 |
| `browser_delete_environment` | 删除环境 | `Target.disposeBrowserContext` | 彻底销毁一个 `BrowserContext`，清理其所有数据。 |
| **指纹配置管理** |
| `browser_set_proxy` | 设置代理 | `Browser.setProxyOverride` / 命令行 | 通过 CDP 动态配置代理，或在启动浏览器时通过 `--proxy-server` 等参数预设。 |
| `browser_set_user_agent` | 设置 UA | `Network.setUserAgentOverride` | 覆盖全局或特定页面的 User-Agent 字符串[reference:18]。 |
| `browser_set_viewport` | 设置视口 | `Emulation.setDeviceMetricsOverride` | 覆盖页面的屏幕尺寸、设备缩放因子等指标，用于移动端调试[reference:19]。 |
| `browser_set_geolocation` | 设置地理位置 | `Emulation.setGeolocationOverride` | 覆盖浏览器默认的地理位置信息[reference:20]。 |
| `browser_set_timezone` | 设置时区 | `Emulation.setTimezoneOverride` | 覆盖浏览器时区。 |
| `browser_set_webrtc_mode` | 设置 WebRTC 策略 | 无直接 CDP 命令 | 通过修改浏览器启动参数（如 `--force-webrtc-ip-handling-policy`）实现。 |

---

### 💡 关键概念补充：`@ref` 是如何工作的？

快照中的 `@ref`（如 `@e1`）是理解这套工具的关键，它建立了一个从 AI 理解到浏览器操作的桥梁。

它的工作原理可以简单概括为以下三步：

1.  **获取树形结构**：通过 `Accessibility.getFullAXTree` 命令，从浏览器获取整个页面的可访问性树[reference:21]。
2.  **生成引用与格式转换**：MCP Server 遍历这棵树，为每一个可交互的节点（如按钮、输入框）生成一个唯一的标识符（如 `@e1`）。同时，将复杂的树形结构转换为 LLM 易于理解的文本格式[reference:22]。
3.  **建立映射关系**：Server 会记录下每个 `@ref` 与该节点在 DOM 树中位置的映射关系。当 AI 决定操作 `@e1` 时，Server 就能迅速定位到具体的 DOM 元素，并执行相应的 CDP 命令。

这套机制让 AI 免于直接处理复杂的 DOM 树和脆弱的 XPath，转而通过稳定、简洁的引用来与页面进行交互。

---
