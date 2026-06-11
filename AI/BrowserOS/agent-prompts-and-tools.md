# BrowserOS Agent 提示词与工具总览

> 本文档汇总 BrowserOS 中所有提供给 AI Agent 的**系统提示词（Prompts）**和**注册的工具（Tools）**。
>
> 生成时间：2026-06-11

---

## 目录

- [1. 提示词总览](#1-提示词总览)
  - [1.1 主系统提示词](#11-主系统提示词)
  - [1.2 压缩/摘要提示词](#12-压缩摘要提示词)
  - [1.3 SOUL 提示词](#13-soul-提示词)
  - [1.4 消息格式化](#14-消息格式化)
  - [1.5 MCP 服务器指令](#15-mcp-服务器指令)
  - [1.6 精炼提示词](#16-精炼提示词)
- [2. 工具总览](#2-工具总览)
  - [2.1 浏览器核心工具（10 个）](#21-浏览器核心工具10-个)
  - [2.2 文件系统工具（7 个）](#22-文件系统工具7-个)
  - [2.3 Nudge 工具（2 个）](#23-nudge-工具2-个)
  - [2.4 遗留浏览器工具（59 个）](#24-遗留浏览器工具59-个)
  - [2.5 Klavis Strata 工具（动态）](#25-klavis-strata-工具动态)
  - [2.6 自定义 MCP 工具（动态）](#26-自定义-mcp-工具动态)
- [3. 工具注册架构](#3-工具注册架构)
  - [3.1 AI SDK Agent 内部注册](#31-ai-sdk-agent-内部注册)
  - [3.2 MCP 服务器外部注册](#32-mcp-服务器外部注册)
- [4. 关键文件索引](#4-关键文件索引)

---

## 1. 提示词总览

### 1.1 主系统提示词

| 属性 | 值 |
|------|-----|
| **文件** | `agent/prompt.ts` |
| **导出函数** | `buildSystemPrompt(options?)` |
| **版本** | v6 |
| **用途** | 构造完整的 Agent 系统提示词，注入到每次 LLM 调用中 |

**Section 组成（共 12 个，可通过 `exclude` 选项排除）：**

| Section | 函数 | 条件渲染 | 核心内容 |
|---------|------|----------|----------|
| `role-and-mode` | `getRoleAndMode()` | 根据 `workspaceDir` / `isScheduledTask` / `chatMode` | Agent 角色定义（有/无 workspace）、模式感知 |
| `security` | `getSecurity()` | 始终 | 指令层级、提示注入防护、数据安全、安全规则 |
| `capabilities` | `getCapabilities()` | 根据 `workspaceDir` | 浏览器控制（10 tools）、外部集成（Strata）、文件系统 |
| `execution` | `getExecution()` | 根据 `origin`（newtab/sidepanel） | 执行哲学、New-Tab 特殊规则、多标签工作流、观察→行动→验证 |
| `tool-selection` | `getToolSelection()` | 根据 `origin` | 工具选择策略、观察/交互偏好、导航策略、Strata vs 浏览器 |
| `external-integrations` | `getExternalIntegrations()` | 根据 `connectedApps` / `declinedApps` | Klavis Strata 接入规则、发现流程、认证流程、可用服务列表 |
| `error-recovery` | `getErrorRecovery()` | 根据 `workspaceDir` | 浏览器错误恢复、JS 错误、Strata 错误、重试预算、文件系统错误 |
| `workspace` | `getWorkspace()` | 仅在 `workspaceDir` 存在时 | 工作目录路径、文件系统工具列表 |
| `nudges` | `getNudges()` | 始终（但 scheduled/chat 模式会排除） | `suggest_app_connection`（阻塞式前置工具）、`suggest_schedule`（后置工具） |
| `style` | `getStyle()` | 根据 `workspaceDir` | 工具调用风格、叙述规则、简洁性 |
| `user-context` | `getUserContext()` | 根据 `chatMode` / `userSystemPrompt` | 用户偏好、页面上下文（包括 scheduled task 特殊规则） |
| `soul` | `getSoul()` | 仅在 `soulContent` 存在时 | SOUL.md 内容注入 |
| `security-reminder` | `getSecurityReminder()` | 始终 | 最终安全提醒 + 执行提醒 |

**模式条件：**

| 条件 | 触发行为 |
|------|----------|
| `isScheduledTask` | 追加"后台定时任务"说明；排除 `nudges`；页面上下文追加隐藏页规则 |
| `chatMode` | 角色追加"只读聊天模式"；排除 `nudges`；限制浏览器工具为只读 |
| `origin === 'newtab'` | 执行规则追加 New-Tab 特殊规则（禁止 navigate/close 当前页） |
| `workspaceDir` 存在 | 追加文件系统工具说明和 workspace section |
| `soulContent` 存在 | 追加 `<soul>` 标签包裹的 SOUL.md 内容 |

---

### 1.2 压缩/摘要提示词

| 属性 | 值 |
|------|-----|
| **文件** | `agent/compaction/prompt.ts` |
| **用途** | 上下文窗口管理——当对话过长时，通过 LLM 压缩历史消息 |

**导出函数：**

| 函数 | 用途 |
|------|------|
| `buildSummarizationSystemPrompt()` | 摘要助手的系统提示词："你是上下文摘要助手，只输出结构化摘要" |
| `buildSummarizationPrompt(existingSummary?)` | 用户提示词：要求将对话转录总结为固定 Markdown 格式（Goal / Constraints / Progress / Key Decisions / Active State / Next Steps / Critical Context） |
| `buildTurnPrefixPrompt()` | 大回合前缀摘要：当回合被分割时，总结前缀部分以便后缀理解 |
| `messagesToTranscript(messages)` | 将 `ModelMessage[]` 转换为 `[User]` / `[Assistant]` / `[Tool Call]` / `[Tool Result]` 格式的转录文本 |

**摘要格式（Markdown）：**
```markdown
## Goal
## Constraints & Preferences
## Progress
### Done
### In Progress
### Blocked
## Key Decisions
## Active State
## Next Steps
## Critical Context
```

---

### 1.3 SOUL 提示词

| 属性 | 值 |
|------|-----|
| **文件** | `agent/soul-prompt.ts` |
| **导出函数** | `readSoulPrompt()` |
| **来源** | 读取 `~/.browseros/SOUL.md`（如果存在） |
| **用途** | 作为被动提示词上下文注入系统提示词，不暴露 Soul UI 或工具 |

---

### 1.4 消息格式化

| 属性 | 值 |
|------|-----|
| **文件** | `agent/format-message.ts` |
| **导出函数** | `formatUserMessage(message, browserContext?, selectedText?, selectedTextSource?)` |
| **用途** | 将用户原始消息包装为 LLM 提示词格式 |

**输出格式：**
```
## Browser Context
**Active Tab:** Tab {id} (Page ID: {pageId}) - "{title}" ({url})
**Selected Tabs ({n}):** ...

---

<selected_text (from "{title}" — {url})>
{selectedText}
</selected_text>

<USER_QUERY>
{message}
</USER_QUERY>
```

**安全处理：**
- 过滤 XML 风格标签（防止提示注入）：`<selected_text|USER_QUERY|page_context|AGENT_PROMPT|soul|security|workspace>`
- 清理选中文本中的 `"` 为 `'`

---

### 1.5 MCP 服务器指令

| 属性 | 值 |
|------|-----|
| **文件** | `api/services/mcp/mcp-prompt.ts` |
| **导出常量** | `MCP_INSTRUCTIONS` |
| **用途** | MCP 服务器级别的 `instructions` 字段，随 MCP 握手发送给客户端 |

**内容要点：**
- 浏览器自动化：Observe → Act → Verify 工作流
- 障碍处理：Cookie 横幅、登录门、CAPTCHA、2FA
- 错误恢复：Ref 失效、元素不可见、2 次失败后求助
- 外部集成（Klavis Strata）：40+ 服务，连接检查 → 发现 → 执行流程
- 认证流程：auth 错误时获取新 authUrl

---

### 1.6 精炼提示词

| 属性 | 值 |
|------|-----|
| **文件** | `lib/clients/llm/refine-prompt.ts` |
| **导出函数** | `refinePrompt(llmConfig, request, browserosId?)` |
| **用途** | 将用户的粗略 prompt 精炼为清晰的定时任务指令 |

**系统提示词要点：**
- "你将帮助用户为一个定时浏览器自动化任务写提示词"
- 要求：具体说明做什么和在哪里、明确返回结果、足够完整以无人值守运行
- 如果太模糊，使用 `[your competitor's URL]` 等占位符
- 返回 ONLY 重写后的提示词，无其他内容

---

## 2. 工具总览

### 2.1 浏览器核心工具（10 个）

> 位置：`tools/browser/`  
> 注册：`registerBrowserTools()` → MCP 服务器  
> 内部使用：`buildBrowserToolSet()` → AI SDK ToolSet  
> 定义方式：`defineTool({ name, description, input: zodSchema, handler })`

| # | 工具名 | 文件 | 描述 | 只读标记 |
|---|--------|------|------|----------|
| 1 | `tabs` | `tabs.ts` | 列出页面、打开后台/隐藏页、关闭页面 | ❌ |
| 2 | `navigate` | `navigate.ts` | 导航到 URL、后退、前进、刷新；返回新 snapshot | ❌ |
| 3 | `snapshot` | `snapshot.ts` | 可访问性树（Accessibility Tree），含 `[ref=e12]` 引用 | ✅ |
| 4 | `diff` | `diff.ts` | 自上次 snapshot/diff 以来的变化 | ✅ |
| 5 | `act` | `act.ts` | 点击、填充、输入、按键、悬停、选择、滚动、坐标操作 | ❌ |
| 6 | `read` | `read.ts` | 提取 Markdown、文本或链接 | ✅ |
| 7 | `grep` | `grep.ts` | 在 snapshot/内容中搜索，无需转储整页 | ✅ |
| 8 | `screenshot` | `screenshot.ts` | 视觉截图 | ✅ |
| 9 | `wait` | `wait.ts` | 等待文本、选择器或时间 | ✅ |
| 10 | `run` | `run.ts` | 页面上下文 JavaScript，用于小型 DOM/页面状态脚本 | ❌ |

**Chat 模式限制：** 仅保留 `readOnlyHint = true` 的工具 + `tabs`（仅限 `action="list"`）

---

### 2.2 文件系统工具（7 个）

> 位置：`tools/filesystem/`  
> 注册：`buildFilesystemToolSet(cwd)` → AI SDK ToolSet  
> 条件：仅在 `workingDir` 存在且非 chat 模式时启用

| # | 工具名 | 文件 | 描述 |
|---|--------|------|------|
| 1 | `filesystem_read` | `read.ts` | 读取文件内容（文本或图片） |
| 2 | `filesystem_write` | `write.ts` | 创建或覆盖文件 |
| 3 | `filesystem_edit` | `edit.ts` | 定向查找-替换编辑 |
| 4 | `filesystem_ls` | `ls.ts` | 列出目录内容 |
| 5 | `filesystem_find` | `find.ts` | 按名称模式搜索文件 |
| 6 | `filesystem_grep` | `grep.ts` | 按正则搜索文件内容 |
| 7 | `filesystem_bash` | `bash.ts` | 执行 shell 命令 |

---

### 2.3 Nudge 工具（2 个）

> 位置：`agent/nudge-tools.ts`  
> 注册：`buildNudgeToolSet()` → AI SDK ToolSet  
> 特点：UI 哨兵工具——返回的 JSON 文本被聊天 UI 拦截，渲染为交互卡片

| # | 工具名 | 描述 | 触发时机 |
|---|--------|------|----------|
| 1 | `suggest_schedule` | 建议将当前任务设置为定时任务 | **后置**——完成任务后，如果任务适合定时运行（新闻、监控、报告等） |
| 2 | `suggest_app_connection` | 建议连接外部应用 | **阻塞式前置**——任务涉及未连接的外部服务时，必须在任何浏览器工作前调用 |

**关键行为：**
- `suggest_app_connection`：响应必须**只包含该工具调用**，不能有任何前后文本
- `suggest_schedule`：调用后**不要写任何关于调度的文本**
- Scheduled task / Chat 模式下禁用这两个工具

---

### 2.4 遗留浏览器工具（59 个）

> 位置：`tools/legacy/`  
> 注册：`registerLegacyBrowserTools()` → MCP 服务器  
> 使用条件：`useNewTools === false` 时替代新浏览器工具

| 分类 | 数量 | 工具名 |
|------|------|--------|
| **导航** | 8 | `get_active_page`, `list_pages`, `navigate_page`, `new_page`, `new_hidden_page`, `show_page`, `move_page`, `close_page` |
| **观察** | 7 | `take_snapshot`, `get_page_content`, `get_page_links`, `get_dom`, `search_dom`, `take_screenshot`, `evaluate_script` |
| **输入** | 17 | `click`, `click_at`, `hover`, `hover_at`, `type_at`, `drag_at`, `focus`, `clear`, `fill`, `check`, `uncheck`, `upload_file`, `press_key`, `drag`, `scroll`, `handle_dialog`, `select_option` |
| **页面操作** | 3 | `save_pdf`, `save_screenshot`, `download_file` |
| **窗口** | 6 | `list_windows`, `create_window`, `create_hidden_window`, `close_window`, `activate_window`, `set_window_visibility` |
| **书签** | 6 | `get_bookmarks`, `create_bookmark`, `remove_bookmark`, `update_bookmark`, `move_bookmark`, `search_bookmarks` |
| **历史** | 4 | `search_history`, `get_recent_history`, `delete_history_url`, `delete_history_range` |
| **Tab Groups** | 5 | `list_tab_groups`, `group_tabs`, `update_tab_group`, `ungroup_tabs`, `close_tab_group` |
| **信息** | 1 | `browseros_info` |
| **Nudges** | 2 | `suggest_schedule`, `suggest_app_connection` |

---

### 2.5 Klavis Strata 工具（动态）

> 位置：`api/services/klavis/strata-proxy.ts`  
> 注册：`registerKlavisTools()` → MCP 服务器  
> 内部使用：`buildKlavisToolSet()` → AI SDK ToolSet  
> 来源：Klavis Strata MCP 服务器（40+ 外部服务：Gmail、Slack、GitHub、Notion 等）

| # | 工具名 | 描述 |
|---|--------|------|
| 1 | `connector_mcp_servers` | 检查外部服务是否已连接，返回 `{ connected, authUrl }` |
| 2+ | 动态工具 | 来自 Klavis Strata 的 `discover_server_categories_or_actions`、`get_category_actions`、`get_action_details`、`execute_action`、`search_documentation` 等 |

**连接流程：**
1. 后台通过 `connectKlavisInBackground()` 建立到 Klavis Strata 的 HTTP 连接
2. 列出所有可用工具及其 JSON Schema
3. 将 Schema 转换为 Zod 形状
4. 包装为 AI SDK / MCP 工具

---

### 2.6 自定义 MCP 工具（动态）

> 位置：`agent/mcp-builder.ts`  
> 注册：`createMcpClients()` → AI SDK ToolSet  
> 来源：用户在 UI 中配置的自定义 MCP 服务器（SSE / HTTP 传输）

| # | 工具名 | 描述 |
|---|--------|------|
| 1+ | 动态工具 | 用户自定义 MCP 服务器暴露的工具 |

**连接流程：**
1. 从 `browserContext.customMcpServers` 读取用户配置
2. 自动检测传输类型（SSE / HTTP）
3. 使用 `@ai-sdk/mcp` 的 `createMCPClient()` 连接
4. 合并所有客户端的工具到统一 ToolSet
5. 如果工具名与 Klavis 工具冲突，自定义工具优先

---

## 3. 工具注册架构

### 3.1 AI SDK Agent 内部注册

当用户发送聊天消息时，`ChatService.processMessage()` 创建 `AiSdkAgent`，内部组装工具集：

```
AiSdkAgent.create(config)
│
├── buildBrowserToolSet(session, { readOnly: chatMode })
│   └── 包装 BROWSER_TOOLS (10个) 为 AI SDK tool()
│   └── Chat 模式：过滤为 readOnlyHint=true 的工具 + tabs(list only)
│
├── buildKlavisToolSet(klavisRef.handle)
│   └── 来自 Klavis Strata 的动态工具 (40+ 服务)
│   └── 条件：klavisRef 存在且 enabledMcpServers.length > 0
│
├── createMcpClients(specs)
│   └── 用户自定义 MCP 服务器工具
│   └── 冲突时自定义工具覆盖 Klavis 工具
│
├── buildFilesystemToolSet(workingDir)
│   └── 7 个文件系统工具
│   └── 条件：非 Chat 模式且 workingDir 存在
│
└── buildNudgeToolSet()
    └── suggest_schedule + suggest_app_connection
    └── 条件：非 Scheduled / Chat 模式
```

**工具去重机制：**
- 浏览器工具名预留集合：`reservedBrowserToolNames`
- 外部 MCP 工具若与浏览器工具名冲突，会被跳过（记录警告日志）
- 自定义 MCP 工具若与 Klavis 工具名冲突，自定义工具优先

**最终工具集合并顺序：**
```ts
const tools = {
  ...browserTools,      // 10 个浏览器工具
  ...externalMcpTools,  // Klavis + 自定义 MCP 工具
  ...filesystemTools,   // 7 个文件系统工具
  ...nudgeTools,        // 2 个 Nudge 工具
}
```

### 3.2 MCP 服务器外部注册

BrowserOS 同时暴露一个 MCP 服务器供外部客户端（如 Cursor、Claude Desktop）连接：

```
createMcpServer(deps)
│
├── registerTools(mcpServer, deps)
│   ├── 条件 useNewTools !== false:
│   │   └── registerBrowserTools(mcpServer, browserSession)
│   │       └── 注册 BROWSER_TOOLS (10个) 为 MCP 工具
│   └── 否则:
│       └── registerLegacyBrowserTools(mcpServer, browser)
│           └── 注册 59 个遗留工具为 MCP 工具
│
└── registerKlavisTools(mcpServer, klavisRef.handle)
    ├── 注册 connector_mcp_servers
    └── 注册所有 Klavis Strata 动态工具
```

**MCP 服务器元数据：**
- 名称：`browseros_mcp`
- 标题：`BrowserOS MCP server`
- Instructions：`MCP_INSTRUCTIONS`（见 1.5 节）

---

## 4. 关键文件索引

### 提示词文件

| 文件路径 | 说明 |
|----------|------|
| `agent/prompt.ts` | **主系统提示词**——12 个 section 的构建器 |
| `agent/compaction/prompt.ts` | **压缩/摘要提示词**——上下文窗口管理 |
| `agent/soul-prompt.ts` | **SOUL 提示词**——读取 SOUL.md |
| `agent/format-message.ts` | **消息格式化**——包装用户消息 + 浏览器上下文 |
| `api/services/mcp/mcp-prompt.ts` | **MCP 服务器指令**——MCP 握手 instructions |
| `lib/clients/llm/refine-prompt.ts` | **精炼提示词**——定时任务 prompt 优化 |

### 工具文件

| 文件路径 | 说明 |
|----------|------|
| `tools/browser/register.ts` | 注册浏览器核心工具到 MCP 服务器 |
| `tools/browser/registry.ts` | **浏览器工具注册表**（10 个工具定义） |
| `tools/browser/framework.ts` | 工具定义框架：`defineTool`、`executeTool` |
| `tools/browser/{tabs,navigate,snapshot,diff,act,read,grep,screenshot,wait,run}.ts` | 各浏览器工具实现 |
| `tools/filesystem/build-toolset.ts` | 构建文件系统工具集（7 个） |
| `tools/filesystem/{read,write,edit,ls,find,grep,bash}.ts` | 各文件系统工具实现 |
| `agent/nudge-tools.ts` | **Nudge 工具**（suggest_schedule, suggest_app_connection） |
| `tools/legacy/register.ts` | 注册遗留浏览器工具到 MCP 服务器 |
| `tools/legacy/registry.ts` | **遗留工具注册表**（59 个工具定义） |
| `api/services/klavis/strata-proxy.ts` | Klavis Strata 工具连接和注册 |
| `api/services/mcp/register-mcp.ts` | MCP 服务器工具统一注册入口 |
| `api/services/mcp/mcp-server.ts` | MCP 服务器创建 |
| `agent/mcp-builder.ts` | 自定义 MCP 客户端连接和工具合并 |
| `agent/tool-adapter.ts` | 浏览器工具 → AI SDK ToolSet 适配器 |
| `agent/chat-mode.ts` | Chat 模式允许的工具白名单 |

### Agent 核心文件

| 文件路径 | 说明 |
|----------|------|
| `agent/ai-sdk-agent.ts` | **AI SDK Agent**——工具集组装、系统提示词构建、ToolLoopAgent 创建 |
| `api/services/chat-service.ts` | **聊天服务**——会话管理、消息处理、Agent 生命周期 |
| `agent/types.ts` | Agent 配置类型定义 |
| `agent/compaction.ts` | 上下文压缩逻辑（LLM 摘要 + 滑动窗口） |
| `agent/session-store.ts` | 会话存储 |
