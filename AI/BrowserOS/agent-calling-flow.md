# BrowserOS AI Agent 调用链路分析

> 本文档分析 BrowserOS 中 AI Agent 如何调用 AI SDK ToolSet、如何输入消息、如何解析输出消息的完整技术流程。
>
> 分析范围：`packages/browseros-agent/apps/server/src/agent/` 及关联模块

---

## 1. 架构总览

BrowserOS 的 AI Agent 采用 **Vercel AI SDK** 作为核心框架，在其 `ToolLoopAgent` 之上封装了浏览器上下文感知、工具适配、上下文压缩、消息规范化等能力。

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                            客户端 (Chrome Extension)                         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐                         │
│  │  Sidepanel  │  │   NewTab    │  │  Scheduled  │                         │
│  │   Chat UI   │  │   Chat UI   │  │   Runner    │                         │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘                         │
└─────────┼────────────────┼────────────────┼────────────────────────────────┘
          │                │                │
          └────────────────┴────────────────┘
                             │ HTTP POST /api/chat
                             ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                         服务端 (Node.js Hono Server)                         │
│                                                                              │
│  ┌─────────────────────────────────────────────────────────────────────┐    │
│  │                        ChatService.processMessage()                  │    │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────────┐ │    │
│  │  │  消息输入   │→ │  Session    │→ │  AiSdkAgent.create()        │ │    │
│  │  │  处理       │  │  管理       │  │  (ToolSet 构建)             │ │    │
│  │  └─────────────┘  └─────────────┘  └─────────────────────────────┘ │    │
│  │                             │                                      │    │
│  │                             ▼                                      │    │
│  │  ┌─────────────────────────────────────────────────────────────┐   │    │
│  │  │              createAgentUIStreamResponse()                   │   │    │
│  │  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐        │   │    │
│  │  │  │ LLM     │→ │ Tool    │→ │ LLM     │→ │ Tool    │ ...    │   │    │
│  │  │  │ Stream  │  │ Execute │  │ Stream  │  │ Execute │        │   │    │
│  │  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘        │   │    │
│  │  │                              ↑                              │   │    │
│  │  │                              │ Compaction (上下文压缩)       │   │    │
│  │  └─────────────────────────────────────────────────────────────┘   │    │
│  └─────────────────────────────────────────────────────────────────────┘    │
│                                    │                                         │
│                                    ▼ SSE Stream                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. 输入消息处理流程

### 2.1 入口：`ChatService.processMessage()`

```typescript
// api/services/chat-service.ts
async processMessage(request: ChatRequest, abortSignal: AbortSignal): Promise<Response>
```

**调用链路：**

```
HTTP Request ──→ processMessage()
                    │
                    ├── 1. resolveLLMConfig() ──→ 获取 provider/model/apiKey
                    │
                    ├── 2. build ResolvedAgentConfig
                    │
                    ├── 3. SessionStore.get(conversationId)
                    │      ├── 存在？检查 MCP/Workspace 变化 → 可能 rebuildSession()
                    │      └── 不存在？创建新 Session
                    │
                    ├── 4. formatUserMessage() ──→ 包装用户消息
                    │
                    ├── 5. appendUserMessage() ──→ 保存原始消息到 Session
                    │
                    └── 6. createAgentUIStreamResponse() ──→ 返回 SSE 流
```

### 2.2 Agent 创建：`AiSdkAgent.create()`

```typescript
// agent/ai-sdk-agent.ts
static async create(config: AiSdkAgentConfig): Promise<AiSdkAgent>
```

**步骤详解：**

| 步骤 | 代码 | 说明 |
|------|------|------|
| 1 | `createLanguageModel(config)` | 根据 provider 创建对应 LLM 客户端 |
| 2 | `buildBrowserToolSet(session, { readOnly })` | 构建浏览器核心工具集（10 个工具） |
| 3 | `buildKlavisToolSet(handle)` | 构建 Klavis 外部 MCP 工具 |
| 4 | `createMcpClients(specs)` | 连接自定义 MCP 服务器 |
| 5 | `buildFilesystemToolSet(workingDir)` | 构建文件系统工具（7 个） |
| 6 | `buildNudgeToolSet()` | 构建 Nudge 工具（2 个） |
| 7 | 合并所有工具 + 去重 | 浏览器工具名预留，防止外部覆盖 |
| 8 | `buildSystemPrompt()` | 构建系统提示词（v6，12 个 section） |
| 9 | `createCompactionPrepareStep()` | 配置上下文压缩 |
| 10 | `new ToolLoopAgent({...})` | 创建 AI SDK Agent |

### 2.3 消息包装：`formatUserMessage()`

```typescript
// agent/format-message.ts
export function formatUserMessage(
  message: string,
  browserContext?: BrowserContext,
  selectedText?: string,
  selectedTextSource?: { url: string; title: string }
): string
```

**输出格式：**

```markdown
## Browser Context
**Window ID:** 1
**Active Tab:** Tab 123 (Page ID: 456) - "Example Page" (https://example.com)
**Selected Tabs (2):**
  1. Tab 124 (Page ID: 457) - "Another Page" (https://another.com)
  2. Tab 125 (Page ID: 458) - "Third Page" (https://third.com)

---

<selected_text (from "Example Page" — https://example.com)>
用户选中的网页文本内容...
</selected_text>

<USER_QUERY>
用户的原始问题
</USER_QUERY>
```

**安全处理：**
- `sanitizeForPrompt()` 去除用户消息中的 XML 标签注入（`<selected_text>`、`<USER_QUERY>` 等）
- 防止 prompt injection 攻击

### 2.4 Session 消息双轨存储

```typescript
// 轨道 1：保存原始消息（用户可见）
session.agent.appendUserMessage(request.message)  // 原始文本

// 轨道 2：构建 prompt 消息（LLM 可见）
const promptUserText = contextPrefix + userContent  // 包装后的文本

// 最后一条用户消息替换为包装版本
const promptUiMessages = messages.map(msg =>
  msg.id === wrappedUserMessageId
    ? { ...msg, parts: [{ type: 'text', text: promptUserText }] }
    : msg
)
```

**恢复机制：** `onFinish` 回调中将包装后的消息恢复为原始文本，确保客户端状态与原始输入一致。

---

## 3. ToolSet 构建与调用流程

### 3.1 工具集合并与去重

```typescript
// agent/ai-sdk-agent.ts (create 方法中)
const tools = {
  ...browserTools,        // 10 个浏览器工具（最高优先级）
  ...externalMcpTools,    // Klavis + 自定义 MCP 工具
  ...filesystemTools,     // 7 个文件系统工具
  ...buildNudgeToolSet(), // 2 个 Nudge 工具
}

// 浏览器工具名保护
function withoutReservedBrowserToolNames(
  tools: ToolSet,
  reservedNames: Set<string>
): ToolSet
```

**冲突解决优先级：**
1. **浏览器工具**（`tabs`、`navigate`、`snapshot` 等）→ 永远保留
2. **自定义 MCP 工具** → 覆盖 Klavis 同名工具
3. **Klavis 工具** → 被自定义 MCP 覆盖时丢弃

### 3.2 浏览器工具适配：`buildBrowserToolSet()`

```typescript
// agent/tool-adapter.ts
export function buildBrowserToolSet(
  session: BrowserSession,
  options: BrowserToolSetOptions = {}
): ToolSet
```

**每个工具的 AI SDK 包装：**

```typescript
for (const def of BROWSER_TOOLS) {
  toolSet[def.name] = tool({
    description: def.description,
    inputSchema: def.input,      // Zod schema
    execute: async (params, executeOptions) => {
      // 1. 超时控制（120s）
      const signal = withBrowserToolTimeout(executeOptions?.abortSignal)
      
      // 2. Chat Mode 只读保护
      const result = readOnlyGuard(def, params, options) ??
        await executeTool(def, params, { session, signal })
      
      // 3. 指标上报
      metrics.log('tool_executed', { tool_name: def.name, ... })
      
      return { content: result.content, isError: result.isError }
    },
    toModelOutput: ({ output }) => {
      // 4. 输出格式转换
      const result = output as { content: ContentBlock[]; isError: boolean }
      if (result.isError) return { type: 'error-text', value: text }
      return contentToModelOutput(result.content)  // text | content (media)
    },
  })
}
```

### 3.3 工具执行：`executeTool()`

```typescript
// tools/browser/framework.ts
export async function executeTool(
  def: ToolDefinition,
  rawArgs: unknown,
  ctx: ToolContext
): Promise<ToolResult>
```

**执行流程：**

```
executeTool()
    │
    ├── 1. throwIfAborted(signal) ──→ 检查是否已取消
    │
    ├── 2. def.input.safeParse(rawArgs) ──→ Zod 参数验证
    │      └── 失败？返回 errorResult("Invalid arguments...")
    │
    ├── 3. new ToolResponse() ──→ 创建响应收集器
    │
    ├── 4. abortable(def.handler(parsed.data, ctx, response), signal)
    │      ├── handler 调用 browser core 方法
    │      │      ├── session.observe(pageId) → Observer
    │      │      ├── session.input(pageId) → Input
    │      │      ├── session.nav(pageId) → Navigation
    │      │      └── session.cdp(method, params) → 原始 CDP
    │      │
    │      └── 支持 AbortSignal 取消
    │
    ├── 5. response.buildForSession(session) ──→ 构建最终结果
    │
    └── 6. 附加 tabId metadata（如果工具涉及 page 参数）
```

### 3.4 外部 MCP 工具包装

```typescript
// agent/ai-sdk-agent.ts
for (const [name, t] of Object.entries(rawExternalMcpTools)) {
  externalMcpTools[name] = {
    ...t,
    execute: async (...args) => {
      const startTime = performance.now()
      try {
        const result = await originalExecute(...args)
        metrics.log('tool_executed', { tool_name: name, duration_ms: ..., success: true })
        return result
      } catch (error) {
        metrics.log('tool_executed', { tool_name: name, success: false, error_message: ... })
        throw error
      }
    }
  }
}
```

**与浏览器工具的区别：**
- 浏览器工具通过 `executeTool()` 统一执行
- 外部 MCP 工具直接调用原始 `execute`，仅包装 metrics

---

## 4. AI SDK Agent 循环：`ToolLoopAgent`

### 4.1 Agent 配置

```typescript
new ToolLoopAgent({
  model,                    // LanguageModel（OpenAI/Anthropic/Gemini 等）
  instructions,             // 系统提示词（buildSystemPrompt 输出）
  tools,                    // 合并后的 ToolSet
  stopWhen: [stepCountIs(AGENT_LIMITS.MAX_TURNS)],  // 最多 100 轮
  prepareStep,              // 每步前的上下文压缩
  providerOptions: {        // ChatGPT Pro 特殊配置
    openai: {
      store: false,
      reasoningEffort: 'high',
      reasoningSummary: 'auto',
      include: ['reasoning.encrypted_content'],
    }
  }
})
```

### 4.2 单步执行流程

```
ToolLoopAgent Step
    │
    ├── 1. prepareStep({ messages, steps, model })
    │      ├── 检查 token 数量
    │      ├── 超过阈值？触发 compaction
    │      └── 返回 { messages, experimental_context }
    │
    ├── 2. normalizeMessagesForModel(messages, options)
    │      ├── 不支持 media-in-tool-results 的 provider？
    │      │   └── 提取图片/文件到独立 user message
    │      └── 支持的 provider？直接返回
    │
    ├── 3. model.doGenerate() / model.doStream()
    │      ├── 发送 messages + system prompt + tools schema
    │      └── 接收 LLM 输出流
    │
    ├── 4. 解析 LLM 输出
    │      ├── 纯文本？→ 返回 assistant message
    │      └── tool call？→ 解析 tool name + args
    │
    ├── 5. 执行工具
    │      ├── 查找 tools[name]
    │      ├── 调用 tool.execute(args)
    │      ├── 接收 ToolResult
    │      └── 通过 toModelOutput() 转换格式
    │
    └── 6. 将 tool result 加入 messages，进入下一轮
```

---

## 5. 输出解析与流式返回

### 5.1 流式响应：`createAgentUIStreamResponse()`

```typescript
// ai SDK 提供
import { createAgentUIStreamResponse } from 'ai'

return createAgentUIStreamResponse({
  agent: session.agent.toolLoopAgent,  // ToolLoopAgent
  uiMessages: promptUiMessages,        // 包含包装后消息的 UIMessage[]
  abortSignal,                         // 取消信号
  onFinish: async ({ messages }) => {  // 完成回调
    // 恢复原始用户消息
    const restored = messages.map(msg =>
      msg.id === wrappedUserMessageId
        ? { ...msg, parts: [{ type: 'text', text: request.message }] }
        : msg
    )
    session.agent.messages = filterValidMessages(restored)
  }
})
```

**返回格式：** SSE (Server-Sent Events) 流

```
event: data
data: { "type": "step-start", ... }

event: data
data: { "type": "text-delta", "textDelta": "Hello" }

event: data
data: { "type": "tool-call", "toolCall": { "toolName": "snapshot", "args": {} } }

event: data
data: { "type": "tool-result", "toolResult": { ... } }

event: data
data: { "type": "text-delta", "textDelta": " I've taken" }

event: finish
data: { "finishReason": "stop", "usage": { ... } }
```

### 5.2 消息验证与清理

```typescript
// agent/message-validation.ts

// 检查消息是否有有效内容
function hasMessageContent(message: UIMessage): boolean

// 过滤无效消息（空 parts、纯空白文本）
function filterValidMessages(messages: UIMessage[]): UIMessage[]

// Session 重建时清理失效工具引用
function sanitizeMessagesForToolset(
  messages: UIMessage[],
  toolNames: Set<string>
): UIMessage[]
```

**清理规则：**
- `tool-${name}` 类型的 part，如果 `name` 不在当前 toolset 中，则移除
- 移除后如果消息没有有效 part，则丢弃整条消息

---

## 6. 上下文压缩（Compaction）

### 6.1 触发条件

```typescript
// agent/compaction.ts
currentTokens = getCurrentTokenCount(steps, messages, config)
if (currentTokens <= config.triggerThreshold) {
  return { messages, experimental_context: state }  // 不压缩
}
```

**阈值计算：**
- `contextWindow` = 128K（默认）
- `triggerThreshold` = contextWindow × triggerRatio（0.6）= ~76K tokens
- `reserveTokens` = 10K（预留空间）

### 6.2 压缩层级（渐进式）

```
Token 超限
    │
    ├── Level 1: stripBinaryContent() ──→ 去除图片/文件的 base64 数据
    │      └── 仍超限？
    │
    ├── Level 2: pruneMessages() ──→ 移除最近 N 条消息之前的旧 tool calls
    │      └── 仍超限？
    │
    ├── Level 3: reduceToolOutputs() ──→ 截断旧工具输出（保留最近 2 条完整）
    │      └── 仍超限？
    │
    └── Level 4: compactMessages() ──→ LLM 摘要压缩
           ├── findSafeSplitPoint() ──→ 找到安全分割点
           ├── summarizeMessages() ──→ 摘要历史消息
           ├── summarizeTurnPrefix() ──→ 摘要被分割的当前轮次前缀
           └── 合并为单条 summary message
```

### 6.3 LLM 摘要流程

```typescript
// agent/compaction.ts
async function callSummarizer(
  model: LanguageModel,
  messages: ModelMessage[],
  userPrompt: string,
  timeoutMs: number,
  maxOutputTokens: number,
): Promise<string | null>
```

**提示词结构：**

```
System: <Summarization Assistant 系统提示词>

User:
<conversation_transcript>
  [message 1]
  [message 2]
  ...
</conversation_transcript>

<summarization_prompt>
  请总结上述对话，保留关键信息...
</summarization_prompt>
```

---

## 7. 消息规范化（Message Normalization）

### 7.1 Provider 兼容性处理

```typescript
// agent/message-normalization.ts
export function normalizeMessagesForModel(
  messages: ModelMessage[],
  options: MessageNormalizationOptions
): ModelMessage[]
```

**问题：** 部分 Provider（Google Gemini 旧版）不支持在 tool result 中直接包含图片/文件。

**解决方案：**

```
Before (不支持 media-in-tool-results):
  user: "take a screenshot"
  assistant: tool-call(screenshot)
  tool: result { type: 'content', value: [{ type: 'image-data', data: 'base64...' }] }

After (提取到独立 user message):
  user: "take a screenshot"
  assistant: tool-call(screenshot)
  tool: result { type: 'text', value: '[Image omitted for brevity]' }
  user: "Attached image(s) from tool result:" + [ImagePart]
```

**支持 media-in-tool-results 的 Provider：**
- Anthropic (Claude)
- OpenAI (GPT-4o)
- Azure
- Bedrock
- Google Gemini-3（新模型）

---

## 8. 会话生命周期管理

### 8.1 Session 结构

```typescript
// agent/session-store.ts
interface AgentSession {
  agent: AiSdkAgent                    // AI SDK Agent 实例
  hiddenPageId?: number                // 定时任务的隐藏页面
  browserContext?: BrowserContext      // 浏览器上下文
  mcpServerKey?: string                // MCP 服务器指纹（变化检测）
  workingDir?: string                  // 工作目录（变化检测）
}
```

### 8.2 变化检测与重建

```
Session 已存在
    │
    ├── MCP 配置变化？
    │      └── rebuildSession()
    │            ├── 保存历史消息
    │            ├── 销毁旧 Agent
    │            ├── 创建新 Agent（新工具集）
    │            └── sanitizeMessagesForToolset() ──→ 清理失效工具引用
    │
    ├── Workspace 变化？
    │      └── rebuildSession()
    │            ├── 添加上下文变更提示
    │            └── 更新文件系统工具
    │
    └── 无变化 → 复用 Session
```

### 8.3 消息恢复机制

```typescript
// 新 Session + 有历史消息
if (isNewSession && request.previousConversation?.length) {
  for (const msg of request.previousConversation) {
    session.agent.messages.push({
      id: crypto.randomUUID(),
      role: msg.role === 'assistant' ? 'assistant' : 'user',
      parts: [{ type: 'text', text: msg.content }],
    })
  }
}
```

---

## 9. Chat Mode 限制

### 9.1 工具白名单

```typescript
// agent/chat-mode.ts
export const CHAT_MODE_ALLOWED_TOOLS = new Set([
  'tabs',      // 仅支持 action="list"
  'read',
  'grep',
  'snapshot',
  'diff',
  'screenshot',
  'wait',
])
```

### 9.2 只读保护

```typescript
// agent/tool-adapter.ts
function readOnlyGuard(def, params, options): ToolResult | null {
  if (!options.readOnly || def.name !== 'tabs') return null
  const action = params?.action ?? 'list'
  if (action === 'list') return null
  return errorResult('tabs: chat mode only supports action="list".')
}
```

**Chat Mode 禁用：**
- 导航工具（`navigate`、`act` 等）
- 文件系统工具
- Nudge 工具（`suggest_schedule`、`suggest_app_connection`）

---

## 10. 关键数据流图

### 10.1 完整请求-响应流程

```
┌──────────┐    HTTP POST    ┌──────────────┐
│  Client  │ ───────────────→│  ChatService │
│ (React)  │   /api/chat     │ .process()   │
└──────────┘                 └──────┬───────┘
                                    │
                    ┌───────────────┼───────────────┐
                    ▼               ▼               ▼
            ┌──────────┐   ┌──────────────┐  ┌─────────────┐
            │  format  │   │   Session    │  │   AiSdk     │
            │  UserMsg │   │   Store      │  │   .create() │
            └────┬─────┘   └──────────────┘  └──────┬──────┘
                 │                                    │
                 │      ┌─────────────────────────────┘
                 │      │
                 ▼      ▼
            ┌─────────────────────────┐
            │ createAgentUIStream()   │
            │   (SSE Stream)          │
            └───────────┬─────────────┘
                        │
            ┌───────────┼───────────┐
            ▼           ▼           ▼
       text-delta   tool-call   tool-result
       "Let me..."  snapshot    { content: [...] }
            │           │           │
            └───────────┴───────────┘
                        │
                        ▼
                   ┌──────────┐
                   │  Client  │
                   │  (React) │
                   └──────────┘
```

### 10.2 工具调用时序

```
Timeline:
  t0:  User sends "go to google.com"
  │
  t1:  LLM generates tool-call: { name: "navigate", args: { url: "google.com" } }
  │
  t2:  AI SDK calls tool.execute(args)
  │     ├── tool-adapter.ts: withBrowserToolTimeout(120s)
  │     ├── framework.ts: executeTool(def, args, { session, signal })
  │     ├── handler: session.nav(pageId).goto(url)
  │     └── return { content: [{ type: 'text', text: 'Navigated to...' }] }
  │
  t3:  toModelOutput() converts → { type: 'text', value: 'Navigated to...' }
  │
  t4:  Tool result added to messages
  │
  t5:  LLM generates: "I've navigated to Google. What would you like to do?"
  │
  t6:  text-delta streamed to client
```

---

## 11. 关键文件索引

| 文件 | 说明 |
|------|------|
| `agent/ai-sdk-agent.ts` | **核心 Agent 类**，ToolSet 构建、系统提示词、模型配置 |
| `agent/tool-adapter.ts` | 浏览器工具 → AI SDK Tool 适配器，含超时和只读保护 |
| `api/services/chat-service.ts` | **消息处理入口**，Session 管理、消息包装、流式响应 |
| `agent/format-message.ts` | 用户消息包装（BrowserContext + selected_text + USER_QUERY） |
| `agent/message-validation.ts` | 消息有效性检查、工具引用清理 |
| `agent/session-store.ts` | Session 内存存储（Map<conversationId, AgentSession>） |
| `agent/types.ts` | AgentConfig、ProviderConfig 类型定义 |
| `agent/compaction.ts` | 上下文压缩（prune → reduce → LLM summarize） |
| `agent/compaction/prompt.ts` | 压缩/摘要提示词 |
| `agent/compaction/utils.ts` | Token 估算、安全分割点查找 |
| `agent/message-normalization.ts` | Provider 兼容的消息规范化（media 提取） |
| `tools/browser/framework.ts` | 工具执行框架（Zod 验证、AbortSignal、ToolResponse） |
| `tools/browser/register.ts` | 浏览器工具注册入口 |
| `tools/browser/registry.ts` | 浏览器工具定义列表 |
| `tools/filesystem/build-toolset.ts` | 文件系统工具构建 |
| `tools/legacy/register.ts` | 遗留工具注册 |
| `agent/nudge-tools.ts` | Nudge 工具（UI 哨兵） |
| `agent/mcp-builder.ts` | MCP 客户端构建 |
| `browser/core/session.ts` | BrowserSession（CDP 会话协调） |
