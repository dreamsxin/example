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

这套完整清单涵盖了从**基础自动化**到**多指纹环境隔离**再到**AI 原生交互**的全方位能力，可支持任意复杂的网页自动化场景。
