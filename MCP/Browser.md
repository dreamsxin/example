# 📚 浏览器自动化 MCP 工具功能清单

## 第一部分：基础网页自动化工具（16个）

适用于单浏览器场景，所有操作作用于当前活动页面。

### 1.1 页面导航与控制

| 工具 | 功能描述 |
|------|----------|
| `navigate_page` | 导航到指定 URL |
| `new_page` | 创建新标签页 |
| `close_page` | 关闭当前或指定标签页 |
| `list_pages` | 列出所有标签页 |
| `select_page` | 切换到指定标签页 |

### 1.2 元素交互

| 工具 | 功能描述 |
|------|----------|
| `click` | 点击元素 |
| `fill` | 填充输入框 / 文本域 |
| `press_key` | 按下键盘按键 |
| `hover` | 鼠标悬停 |
| `drag` | 拖拽元素 |
| `upload_file` | 上传文件 |

### 1.3 弹窗处理

| 工具 | 功能描述 |
|------|----------|
| `handle_dialog` | 处理 alert / confirm / prompt 弹窗 |

### 1.4 数据提取与页面分析

| 工具 | 功能描述 |
|------|----------|
| `evaluate_script` | 在页面中执行 JavaScript 并返回结果 |
| `take_snapshot` | 获取页面可访问性树文本快照 |
| `take_screenshot` | 截图（PNG/JPEG，可选全页或指定元素） |

### 1.5 等待与同步

| 工具 | 功能描述 |
|------|----------|
| `wait_for` | 等待指定文本或元素出现 |

### 1.6 调试与监控

| 工具 | 功能描述 |
|------|----------|
| `list_console_messages` | 获取控制台日志 |
| `list_network_requests` | 获取网络请求列表 |

---

## 第二部分：多指纹浏览器环境管理工具（35个）

用于管理多个独立的浏览器实例（环境），每个环境拥有独立的指纹配置、用户数据、代理等。

### 2.1 环境生命周期管理

| 工具 | 功能描述 |
|------|----------|
| `create_environment` | 创建新指纹环境（不启动） |
| `start_environment` | 启动指定环境（启动 Chrome 实例） |
| `stop_environment` | 停止指定环境 |
| `restart_environment` | 重启环境（应用新指纹） |
| `delete_environment` | 删除环境及其所有数据 |
| `list_environments` | 列出所有环境（可过滤状态） |
| `get_environment_info` | 获取环境详情（配置、状态、wsUrl） |

### 2.2 指纹配置管理

| 工具 | 功能描述 |
|------|----------|
| `set_proxy` | 设置代理 |
| `set_user_agent` | 设置 User-Agent |
| `set_viewport` | 设置视口尺寸 |
| `set_timezone` | 设置时区 |
| `set_geolocation` | 设置地理位置 |
| `set_webrtc_mode` | 设置 WebRTC 泄露策略 |
| `get_fingerprint` | 获取当前指纹配置 |

### 2.3 环境内页面操作（带 envId）

所有基础工具的环境绑定版本，用于指定在哪个指纹环境中执行操作。

| 工具 | 对应基础工具 |
|------|--------------|
| `env_navigate_page` | `navigate_page` |
| `env_take_screenshot` | `take_screenshot` |
| `env_take_snapshot` | `take_snapshot` |
| `env_evaluate_script` | `evaluate_script` |
| `env_click` | `click` |
| `env_fill` | `fill` |
| `env_press_key` | `press_key` |
| `env_hover` | `hover` |
| `env_drag` | `drag` |
| `env_upload_file` | `upload_file` |
| `env_handle_dialog` | `handle_dialog` |
| `env_wait_for` | `wait_for` |
| `env_new_page` | `new_page` |
| `env_close_page` | `close_page` |
| `env_list_pages` | `list_pages` |
| `env_select_page` | `select_page` |

### 2.4 数据隔离与迁移

| 工具 | 功能描述 |
|------|----------|
| `export_cookies` | 导出环境的 Cookies |
| `import_cookies` | 导入 Cookies 到环境 |
| `export_storage` | 导出 LocalStorage / SessionStorage |
| `import_storage` | 导入存储数据 |
| `clear_browser_data` | 清除浏览器数据（缓存、Cookie、存储等） |

### 2.5 批量操作与编排

| 工具 | 功能描述 |
|------|----------|
| `batch_execute` | 在多个环境并行执行同一操作 |
| `sequence_execute` | 在单个环境执行操作序列 |
| `schedule_action` | 延迟执行操作 |
| `wait_for_any_env` | 等待任意环境满足条件 |

### 2.6 监控与调试

| 工具 | 功能描述 |
|------|----------|
| `get_network_requests` | 获取环境内的网络请求列表 |
| `get_console_messages` | 获取控制台日志 |
| `start_monitoring` | 开始监控环境（流式，支持 network/console/page 事件） |
| `stop_monitoring` | 停止监控 |

---

## 第三部分：Agent Browser 高级接口（8个）

为 AI Agent 提供的更高级、更自然的交互接口，基于结构化快照和自然语言。

### 3.1 会话管理

| 工具 | 功能描述 |
|------|----------|
| `create_agent_session` | 创建 Agent 浏览器会话，绑定指纹环境 |
| `get_session_snapshot` | 获取当前会话的结构化快照（语义化视图 + 稳定引用） |
| `restore_session` | 从快照恢复会话状态 |
| `close_agent_session` | 关闭 Agent 会话 |

### 3.2 智能交互

| 工具 | 功能描述 |
|------|----------|
| `agent_interact` | 通过自然语言或结构化引用与页面交互（如“点击登录按钮”） |
| `agent_extract` | 根据 Schema 从页面中提取结构化数据 |
| `agent_wait` | 智能等待页面状态变化（如等待某个元素出现或消失） |
| `agent_task` | 执行一个完整的、端到端的任务（如“登录并购买商品”） |
| `agent_observe` | 观察并总结页面上的变化（相比前一快照） |
| `agent_human_handoff` | 当 Agent 遇到无法处理的场景（如验证码）时，请求人工介入 |

---

## 总计工具数量

| 分类 | 工具数量 |
|------|----------|
| 基础网页自动化 | 16 |
| 多指纹环境管理 | 35 |
| Agent Browser 高级接口 | 8 |
| **总计** | **59** |

> 注：部分工具（如 `env_*` 系列）是基于基础工具的派生，在实际代码实现中可通过统一路由复用逻辑。

---

## 传输模式支持

所有上述工具均可通过以下两种 MCP 传输模式调用：

- **`stdio` 模式**：本地高性能调用，适用于 Agent 与浏览器在同一主机。
- **`Streamable HTTP` 模式**（原 SSE 的演进）：远程调用、多客户端并发、服务端主动推送事件。

---

## 使用示例（Agent 会话 + 指纹环境 + 任务执行）

```javascript
// 1. 创建指纹环境
const env = await mcp.call('create_environment', { name: 'shopping_bot', proxy: 'us.proxy:8080' });
await mcp.call('start_environment', { envId: env.envId });

// 2. 创建 Agent 会话
const session = await mcp.call('create_agent_session', { envId: env.envId });

// 3. 获取页面快照（供 Agent 决策）
const snapshot = await mcp.call('get_session_snapshot', { sessionId: session.sessionId });
console.log(snapshot.view); // 语义化文本视图

// 4. 执行自然语言任务
await mcp.call('agent_task', {
  sessionId: session.sessionId,
  taskDescription: '登录 amazon.com，搜索 "laptop"，将第一个商品加入购物车'
});

// 5. 人工接管（遇到验证码）
await mcp.call('agent_human_handoff', {
  sessionId: session.sessionId,
  reason: 'captcha_detected'
});

// 6. 清理
await mcp.call('close_agent_session', { sessionId: session.sessionId });
await mcp.call('stop_environment', { envId: env.envId });
```

---

# 🧪 MCP 工具详细示例

所有示例假设已建立 MCP 客户端连接，支持 `stdio` 或 `Streamable HTTP` 模式。`client.call(toolName, arguments)` 为异步调用。

---

## 一、P0 核心必备工具示例（基础自动化）

### 1.1 导航与页面控制

```javascript
// 打开网页
await client.call('navigate_page', { url: 'https://example.com/login' });

// 新建标签页
const { pageId } = await client.call('new_page', { url: 'https://google.com' });

// 列出所有标签页
const pages = await client.call('list_pages', {});
console.log(pages); // [{ pageId, url, title }, ...]

// 切换回第一个标签页
await client.call('select_page', { pageId: pages[0].pageId });

// 关闭当前标签页
await client.call('close_page', {});
```

### 1.2 元素交互

```javascript
// 填写表单
await client.call('fill', { selector: '#username', text: 'alice' });
await client.call('fill', { selector: '#password', text: 'secret' });

// 点击登录按钮
await client.call('click', { selector: 'button[type="submit"]' });

// 按键（例如提交搜索）
await client.call('press_key', { key: 'Enter' });

// 悬停菜单
await client.call('hover', { selector: '.dropdown-menu' });

// 上传文件
await client.call('upload_file', { selector: 'input[type="file"]', path: '/path/to/file.png' });
```

### 1.3 数据提取与分析

```javascript
// 执行 JS 获取标题
const { result: title } = await client.call('evaluate_script', {
  function: '() => document.title'
});

// 获取完整 HTML（高性能，适合大页面）
const { html, size } = await client.call('get_page_html', { compress: false });
console.log(`HTML size: ${size} bytes`);

// 仅获取 #main 区域 HTML
const { html: mainHtml } = await client.call('get_page_html', { selector: '#main' });

// 获取页面文本快照（供 AI 理解结构）
const { snapshot } = await client.call('take_snapshot', {});
console.log(snapshot); // 可访问性树文本

// 截图
const { data: screenshotBase64 } = await client.call('take_screenshot', { fullPage: true });
```

### 1.4 等待与弹窗处理

```javascript
// 等待文本出现
await client.call('wait_for', { text: '欢迎', timeout: 5000 });

// 等待元素出现
await client.call('wait_for', { selector: '.user-avatar', timeout: 10000 });

// 点击可能触发弹窗的按钮
await client.call('click', { selector: '#delete-btn' });

// 处理弹窗（必须紧接着调用）
await client.call('handle_dialog', { action: 'accept' });  // 确定
// 或取消
await client.call('handle_dialog', { action: 'dismiss' });
// 或 prompt 输入
await client.call('handle_dialog', { action: 'accept', promptText: '用户输入' });
```

### 1.5 调试基础

```javascript
// 获取控制台日志
const { messages } = await client.call('list_console_messages', { limit: 20 });
messages.forEach(msg => console.log(`${msg.level}: ${msg.text}`));
```

---

## 二、P1 Agent Browser 高级接口示例

Agent Browser 接口专为 **AI Agent 自然交互** 设计，通过 **结构化快照** + **稳定引用** 大幅降低 Agent 对选择器的依赖，并支持端到端任务执行。

### 2.1 获取结构化快照 (`get_snapshot`)

Agent 的第一步通常是获取当前页面的快照，用于理解页面布局和可交互元素。

```javascript
const snapshot = await client.call('get_snapshot', { sessionId });
```

**快照结构示例**（文本形式，实际返回 JSON）：
```
[1] link: "Home", ref: L1
[2] textbox: "Search products", ref: I1
[3] button: "Search", ref: B1
[4] heading: "Today's Deals", ref: H1
[5] link: "View all", ref: L2
```

每个可交互元素都有一个 **稳定引用（ref）**，Agent 后续交互可直接使用该引用，无需手写 CSS 选择器。

### 2.2 自然语言交互 (`agent_interact`)

Agent 根据快照决定操作，然后调用 `agent_interact`。

```javascript
// 点击 ref 为 B1 的按钮（“Search”按钮）
await client.call('agent_interact', {
  sessionId,
  instruction: 'click',
  ref: 'B1'
});

// 在输入框 ref I1 中输入文本
await client.call('agent_interact', {
  sessionId,
  instruction: 'fill',
  ref: 'I1',
  text: 'laptop'
});

// 或者使用自然语言描述，由 MCP Server 自动解析
await client.call('agent_interact', {
  sessionId,
  instruction: '点击“登录”按钮'
  // 不提供 ref 时，Server 会根据快照中的语义自动匹配
});
```

### 2.3 智能等待 (`agent_wait`)

等待页面状态满足条件，比基础 `wait_for` 更智能。

```javascript
// 等待某个引用指向的元素出现
await client.call('agent_wait', {
  sessionId,
  condition: 'ref_exists',
  ref: 'L2',    // 等待“View all”链接出现
  timeout: 10000
});

// 等待页面出现包含“购物车”文本的元素
await client.call('agent_wait', {
  sessionId,
  condition: 'text_appears',
  text: '购物车',
  timeout: 8000
});

// 等待页面 URL 包含特定字符串
await client.call('agent_wait', {
  sessionId,
  condition: 'url_contains',
  value: '/cart',
  timeout: 5000
});
```

### 2.4 结构化数据提取 (`agent_extract`)

根据 JSON Schema 从当前页面提取数据，AI 自动定位元素。

```javascript
const schema = {
  type: 'object',
  properties: {
    productName: { type: 'string', selector: '.product-title' },
    price: { type: 'number', selector: '.price' },
    inStock: { type: 'boolean', selector: '.stock-status', textMatch: 'In stock' }
  }
};

const { data } = await client.call('agent_extract', {
  sessionId,
  schema
});
console.log(data); // { productName: "Laptop", price: 999.99, inStock: true }
```

### 2.5 端到端任务执行 (`agent_task`)

这是最强大的接口：用自然语言描述整个任务，MCP Server 内部自动规划并执行步骤。

```javascript
const taskResult = await client.call('agent_task', {
  sessionId,
  taskDescription: '打开 amazon.com，搜索 "wireless mouse"，将第一个结果加入购物车，然后进入购物车页面确认商品数量为1',
  maxSteps: 15,         // 最多执行 15 步，防止无限循环
  onStep?: (step) => {} // 可选回调，实时获取进度
});
console.log(taskResult.status); // "completed" | "failed" | "human_handoff_needed"
```

**内部执行过程示意**：
1. Agent 获取快照，识别搜索框和按钮。
2. 输入“wireless mouse”并提交搜索。
3. 获取新快照，识别商品列表，定位第一个商品。
4. 点击“加入购物车”按钮。
5. 等待购物车更新提示，然后导航到购物车页面。
6. 验证商品数量，完成任务。

### 2.6 观察页面变化 (`agent_observe`)

在两次快照之间检测变化，返回结构化摘要。

```javascript
// 先获取第一次快照
const snapshot1 = await client.call('get_snapshot', { sessionId });

// 执行某些操作（例如点击）
await client.call('agent_interact', { sessionId, instruction: 'click', ref: 'B1' });

// 观察变化
const changes = await client.call('agent_observe', {
  sessionId,
  previousSnapshot: snapshot1   // 可选，若不传则对比上一步的快照
});
console.log(changes);
// 输出示例：
// {
//   "added": [{ ref: "C1", type: "button", text: "Confirm" }],
//   "removed": [{ ref: "L2", text: "Loading" }],
//   "urlChanged": "https://example.com/cart",
//   "textChanged": ["购物车 (1)"]
// }
```

### 2.7 人工介入 (`agent_human_handoff`)

当 Agent 遇到无法处理的情况（验证码、需要人工审批等）时，调用此工具。

```javascript
// Agent 任务中检测到验证码
const handoff = await client.call('agent_human_handoff', {
  sessionId,
  reason: 'captcha_detected',
  contextData: {
    screenshot: screenshotBase64,
    url: await client.call('evaluate_script', { function: '() => location.href' }),
    captchaType: 'reCAPTCHA'
  }
});
console.log(`Waiting for human, handoffId: ${handoff.handoffId}`);

// 轮询等待人工解决
while (handoff.status === 'waiting') {
  await sleep(2000);
  const status = await client.call('get_handoff_status', { handoffId: handoff.handoffId });
  if (status.status === 'resolved') {
    // 人工已完成操作，Agent 可以继续执行后续任务
    await client.call('agent_task', { sessionId, taskDescription: '继续完成下单流程' });
    break;
  }
}
```

## 三、完整综合示例：AI Agent 自动购物

以下示例展示了一个完整的 Agent 流程：创建环境 → 启动会话 → 执行自然语言任务 → 处理验证码（人工介入）→ 继续任务 → 清理资源。

```javascript
// 1. 创建并启动指纹环境
const { envId } = await client.call('create_environment', {
  name: 'shopping_bot',
  proxy: 'us.proxy:8080'
});
await client.call('start_environment', { envId });

// 2. 创建 Agent 会话
const { sessionId } = await client.call('create_agent_session', { envId });

try {
  // 3. 执行主要购物任务
  const result = await client.call('agent_task', {
    sessionId,
    taskDescription: `
      登录 amazon.com（用户名: test@example.com, 密码: demo123），
      搜索 "noise cancelling headphones"，
      按价格升序排序，
      选择第二件商品，
      加入购物车，
      进入购物车并截取全屏截图保存为 cart.png
    `,
    maxSteps: 20
  });

  if (result.status === 'human_handoff_needed') {
    // 4. 遇到验证码，请求人工介入
    const handoff = await client.call('agent_human_handoff', {
      sessionId,
      reason: result.handoffReason,
      contextData: result.context
    });
    
    // 等待人工处理（实际应用中可通过 Webhook 或轮询）
    await waitForHuman(handoff.handoffId);
    
    // 人工解决后，继续执行剩余步骤
    await client.call('agent_task', {
      sessionId,
      taskDescription: '继续完成剩余任务：进入购物车并截图'
    });
  }

  // 5. 提取购物车总价
  const { data: cartInfo } = await client.call('agent_extract', {
    sessionId,
    schema: {
      type: 'object',
      properties: {
        totalPrice: { type: 'string', selector: '.grand-total' },
        itemCount: { type: 'number', selector: '.cart-count' }
      }
    }
  });
  console.log('购物车信息:', cartInfo);

} finally {
  // 6. 清理
  await client.call('close_agent_session', { sessionId });
  await client.call('stop_environment', { envId });
}
```
