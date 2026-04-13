# 🚀 项目名称（Project Name）

**面向 AI 辅助编程（如 ChatGPT / Copilot / Claude 等）的项目初始化 README.md 范本**，重点强化了：规范工作流、约束 AI 行为、代码结构约定等。

> 一句话描述项目目标（清晰、具体、可验证）

---

## 📌 项目简介

本项目旨在实现：

* 🎯 核心目标：
* 🧩 主要功能：
* ⚙️ 技术栈：
* 👥 目标用户：

---

## 🤖 AI 协作开发约定（非常重要）

本项目采用 AI 辅助开发，请严格遵守以下规则：

### 1. 输出规范

* 所有代码必须：

  * ✅ 可直接运行（无伪代码）
  * ✅ 包含必要注释
  * ❌ 不允许省略实现（如 `// TODO`）
* 修改代码时：

  * 必须说明**修改原因**
  * 必须提供**完整文件**（非片段）

---

### 2. 上下文约束

AI 在编写代码时必须：

* 仅基于当前仓库结构
* 不得假设不存在的文件/模块
* 不得随意引入未声明依赖

---

### 3. 风格一致性

* 遵循已有代码风格（eslint / prettier / black 等）
* 命名规则：

  * 变量：`camelCase`
  * 常量：`UPPER_CASE`
  * 类：`PascalCase`

---

### 4. 安全与边界

禁止 AI：

* ❌ 生成敏感信息（密钥、token）
* ❌ 引入高风险依赖
* ❌ 修改核心配置文件（除非明确要求）

---

## 🗂️ 项目结构

```bash
project-root/
├── src/                # 核心源码
│   ├── modules/        # 功能模块
│   ├── utils/          # 工具函数
│   └── main.*          # 入口文件
├── tests/              # 测试代码
├── docs/               # 文档
├── scripts/            # 自动化脚本
├── config/             # 配置文件
├── .env.example        # 环境变量示例
└── README.md
```

---

## ⚙️ 环境配置

### 1. 安装依赖

```bash
# 示例（根据实际技术栈修改）
npm install
# 或
pip install -r requirements.txt
```

---

### 2. 环境变量

复制并配置：

```bash
cp .env.example .env
```

---

## ▶️ 启动项目

```bash
# 示例
npm run dev
# 或
python main.py
```

---

## 🧪 测试

```bash
# 示例
npm test
# 或
pytest
```

---

## 🔄 开发工作流（AI + 人类协作）

### 分支策略

* `main`：稳定版本
* `dev`：开发分支
* `feature/*`：功能开发

---

### 提交流程

1. 创建分支
2. 使用 AI 辅助开发
3. 本地测试通过
4. 提交 PR

---

### Commit 规范

```bash
feat: 新功能
fix: 修复问题
refactor: 重构
docs: 文档更新
test: 测试相关
chore: 其他修改
```

---

## 🧠 AI 使用指南（推荐 Prompt 模板）

### 生成代码

```text
你是一个严格遵守项目 README 规范的工程师。

请基于以下要求生成代码：
- 文件路径：
- 功能描述：
- 输入输出：
- 约束：

请输出完整文件代码，不要省略。
```

---

### 修改代码

```text
请修改以下文件：

要求：
1. 说明修改点
2. 给出完整文件
3. 不改变无关逻辑
```

---

### Debug

```text
以下代码报错，请：
1. 分析原因
2. 提供修复方案
3. 给出完整修复代码
```

---

## 📏 代码质量约束

* 单个函数 ≤ 50 行
* 单个文件 ≤ 500 行
* 必须有错误处理
* 必须有日志（关键路径）

---

## 📚 文档规范

* 所有模块必须有 README
* API 必须有说明：

  * 参数
  * 返回值
  * 示例

---

## 🔐 安全规范

* 使用 `.env` 管理敏感信息
* 禁止硬编码：

  * API Key
  * 数据库密码

---

## 📦 依赖管理

* 所有依赖必须：

  * 有明确用途
  * 写入依赖文件（package.json / requirements.txt）
* 禁止：

  * 临时依赖
  * 未使用依赖

---

## 🧩 扩展建议

* 引入 CI/CD（GitHub Actions）
* 自动化测试覆盖率 ≥ 80%
* 使用 Linter 保持一致性

---

## 📌 TODO（项目初始化阶段）

* [ ] 初始化项目结构
* [ ] 配置开发环境
* [ ] 添加基础功能
* [ ] 接入 AI 工作流

---

## 📄 License

MIT / Apache 2.0 / 自定义

---

## 🙌 贡献指南

欢迎贡献，请遵守：

* 代码规范
* 提交流程
* AI 使用约定

------------------------------------------------------------------------------------
**专门适配「Rust + Tauri 开发 AI Agent 助手」项目初始化的 README.md 范本**。
**适合 AI 编程协作、约束清晰、工作流规范、目录结构明确、便于后续让 AI 按规则产出代码**。

你可以直接复制为项目根目录 `README.md` 使用。

---

# AI Agent Assistant (Rust + Tauri)

一个基于 **Rust + Tauri** 构建的桌面端 AI Agent 助手。  
目标是提供一个安全、可扩展、低资源占用的本地客户端，用于对话、工具调用、任务编排、上下文管理与插件化扩展。

---

## 1. 项目目标

本项目用于实现一个桌面 AI Agent 助手，具备以下能力：

- 多轮对话
- Agent 任务执行
- 工具调用（Tool Calling）
- 本地数据存储
- 会话与上下文管理
- 可扩展命令系统
- 桌面端 UI 交互
- 安全的前后端边界控制

---

## 2. 技术栈

### 桌面端
- [Tauri](https://tauri.app/)
- Rust
- TypeScript
- 前端框架（任选其一）：
  - React
  - Vue
  - Svelte

### Rust 侧建议依赖
- `tauri`
- `serde`
- `serde_json`
- `tokio`
- `reqwest`
- `anyhow`
- `thiserror`
- `tracing`
- `uuid`
- `chrono`

### 可选能力
- SQLite：本地会话和任务存储
- 文件系统接入：本地知识库
- 插件系统：扩展工具调用
- OpenAI / Anthropic / 本地模型 API 适配层

---

## 3. 核心设计原则

### 3.1 架构原则

本项目采用以下架构原则：

- **前端负责展示与交互**
- **Rust 后端负责核心逻辑、工具调用、状态管理**
- **AI Provider 通过统一适配层接入**
- **Agent 能力必须模块化，禁止逻辑散落**
- **所有跨边界调用必须显式声明**
- **默认最小权限原则**

### 3.2 AI 协作原则

本项目默认会使用 AI 辅助开发。  
所有 AI 生成内容必须遵守以下约束：

1. 不允许输出伪代码
2. 不允许省略关键实现
3. 不允许擅自新增依赖
4. 不允许假设不存在的文件或模块
5. 不允许随意修改项目结构
6. 修改代码时必须说明：
   - 修改目标
   - 修改文件
   - 影响范围
7. 除非明确要求，否则应输出**完整文件**而不是零散片段
8. 优先复用已有模块，避免重复实现
9. 所有异步逻辑必须有错误处理
10. 所有工具调用必须考虑超时、失败与取消机制

---

## 4. 项目范围

### 当前范围
- 基础桌面 UI
- 对话界面
- Rust 命令调用
- Agent 执行框架
- Tool Registry
- Provider 抽象层
- 会话存储
- 日志系统

### 暂不包含
- 自动执行高风险本地命令
- 无权限控制的文件删除/覆盖
- 隐式联网行为
- 任意插件热加载执行
- 未审计的脚本运行

---

## 5. 推荐目录结构

```text
.
├── src/                         # 前端源码
│   ├── components/             # UI 组件
│   ├── pages/                  # 页面
│   ├── hooks/                  # 前端 hooks
│   ├── services/               # 前端服务层
│   ├── stores/                 # 状态管理
│   ├── types/                  # TS 类型定义
│   ├── utils/                  # 工具函数
│   └── main.tsx                # 前端入口
│
├── src-tauri/
│   ├── src/
│   │   ├── main.rs             # Rust 入口
│   │   ├── commands/           # Tauri command 按模块拆分
│   │   ├── agent/              # Agent 核心逻辑
│   │   │   ├── mod.rs
│   │   │   ├── executor.rs     # Agent 执行器
│   │   │   ├── planner.rs      # 规划器（可选）
│   │   │   ├── session.rs      # 会话上下文
│   │   │   └── memory.rs       # 记忆模块
│   │   ├── tools/              # 工具系统
│   │   │   ├── mod.rs
│   │   │   ├── registry.rs     # 工具注册中心
│   │   │   ├── file_tool.rs
│   │   │   └── shell_tool.rs   # 如启用需严格权限控制
│   │   ├── providers/          # AI 模型提供商适配
│   │   │   ├── mod.rs
│   │   │   ├── openai.rs
│   │   │   ├── anthropic.rs
│   │   │   └── local.rs
│   │   ├── models/             # 数据结构
│   │   ├── db/                 # 持久化层
│   │   ├── config/             # 配置管理
│   │   ├── errors/             # 错误定义
│   │   ├── logging/            # 日志封装
│   │   └── utils/              # Rust 工具模块
│   │
│   ├── capabilities/           # Tauri capability 配置
│   ├── icons/
│   ├── tauri.conf.json
│   └── Cargo.toml
│
├── docs/
│   ├── architecture.md         # 架构说明
│   ├── workflow.md             # 开发工作流
│   ├── ai-rules.md             # AI 协作规范
│   └── api.md                  # 命令/API说明
│
├── scripts/                    # 构建/检查脚本
├── .env.example
├── Cargo.toml                  # 如 workspace 模式可选
├── package.json
├── README.md
└── LICENSE
````

---

## 6. 模块约束

### 6.1 commands

* 仅作为 Tauri 暴露层
* 不承载核心业务逻辑
* 必须调用 `agent/`、`tools/`、`providers/` 等模块完成实际工作

### 6.2 agent

* 负责任务编排、上下文管理、工具调度
* 不直接依赖 UI
* 不允许与 Tauri 强耦合

### 6.3 tools

* 所有工具必须注册到统一 Registry
* 所有工具必须定义：

  * 名称
  * 输入参数
  * 输出结构
  * 错误行为
  * 权限要求
* 工具调用必须支持超时控制

### 6.4 providers

* 所有模型调用必须经过 Provider 抽象层
* 不允许在业务代码中直接写死某一厂商 API
* 必须支持未来替换不同模型后端

### 6.5 db

* 所有持久化逻辑统一放在 `db/`
* 禁止在业务模块中散落读写文件逻辑
* Schema 变更必须有迁移方案

---

## 7. 开发工作流

### 7.1 分支策略

* `main`：稳定可发布版本
* `dev`：日常开发集成分支
* `feature/*`：功能开发分支
* `fix/*`：问题修复分支
* `refactor/*`：重构分支

### 7.2 开发流程

1. 从 `dev` 创建功能分支
2. 编写或更新设计说明
3. 再让 AI 按规范生成代码
4. 本地运行与测试
5. 自查通过后提交 PR
6. 代码审查
7. 合并到 `dev`
8. 发布前从 `dev` 合并到 `main`

### 7.3 Commit 规范

使用 Conventional Commits：

```text
feat: 新增 Agent 工具注册模块
fix: 修复会话状态丢失问题
refactor: 重构 provider 抽象层
docs: 更新 README 与架构文档
test: 增加 executor 单元测试
chore: 调整 lint 和格式化配置
```

---

## 8. AI 编码约束

AI 在参与本项目开发时，必须遵守以下规则。

### 8.1 输出规则

* 默认输出完整文件
* 所有代码必须能直接运行
* 必须保留已有功能，除非明确要求删除
* 不允许只给思路不写实现
* 不允许擅自更改接口定义

### 8.2 改动规则

每次改动必须附带：

1. 修改了哪些文件
2. 为什么修改
3. 是否影响已有接口
4. 是否需要同步文档

### 8.3 Rust 代码规则

* 优先显式类型
* 错误统一使用 `Result<T, AppError>` 或统一错误体系
* 禁止无边界 `unwrap()` / `expect()` 出现在生产路径
* 必须考虑并发安全
* 异步任务必须可追踪
* 对外结构体必须可序列化时显式 `derive`

### 8.4 Tauri 边界规则

* `#[tauri::command]` 仅作入口适配
* 不在 command 中直接写复杂逻辑
* 所有前后端交互数据必须定义类型
* 所有敏感能力必须明确 capability 控制

### 8.5 前端规则

* UI 状态与业务状态分离
* API 调用统一放在 `services/`
* 不在组件中直接拼装复杂业务逻辑
* 所有用户输入必须校验
* 所有异步交互必须显示 loading/error 状态

---

## 9. 安全约束

这是一个 Agent 项目，安全优先级高于功能速度。

### 禁止事项

* 禁止硬编码 API Key
* 禁止默认启用危险 Shell 命令执行
* 禁止无确认删除文件
* 禁止读取用户未授权路径
* 禁止将敏感日志输出到前端
* 禁止在生产环境打印完整请求密钥或 token

### 必须做到

* 所有密钥来自环境变量或安全存储
* 所有文件访问必须做路径校验
* 所有工具执行必须记录审计日志
* 所有高风险操作必须要求显式用户确认
* 所有外部请求必须设置超时

---

## 10. 配置管理

### 环境变量示例

创建 `.env` 文件并参考 `.env.example`：

```env
OPENAI_API_KEY=
ANTHROPIC_API_KEY=
MODEL_PROVIDER=openai
MODEL_NAME=gpt-4.1
APP_LOG_LEVEL=info
```

### 配置原则

* 默认配置写入 `config/`
* 敏感信息不进入仓库
* 所有配置项要有默认值或明确校验错误

---

## 11. 日志与可观测性

建议使用 `tracing`。

### 要求

* 关键流程必须记录日志：

  * 会话创建
  * Agent 执行开始/结束
  * Tool 调用
  * Provider 请求失败
* 日志级别区分：

  * `error`
  * `warn`
  * `info`
  * `debug`
* 不记录敏感信息原文

---

## 12. 错误处理规范

### 原则

* 错误必须结构化
* 用户可见错误与内部错误分离
* 不将底层敏感错误直接暴露给 UI

### 建议

* 使用统一错误类型 `AppError`
* 按模块拆分错误来源
* 对外返回可读消息，对内保留详细上下文

---

## 13. 测试规范

### Rust 侧

* 单元测试：核心业务逻辑
* 集成测试：Provider / Tool / Agent 流程
* 对关键模块必须有测试：

  * Tool Registry
  * Agent Executor
  * Session Store
  * Config Loader

### 前端侧

* 核心组件渲染测试
* 状态管理测试
* 关键交互测试

### 最低要求

* 新增核心功能必须附带测试
* 修复 bug 必须增加回归测试

---

## 14. 代码风格

### Rust

* 使用 `cargo fmt`
* 使用 `cargo clippy`
* 模块命名使用 `snake_case`
* 类型名使用 `PascalCase`
* 常量使用 `SCREAMING_SNAKE_CASE`

### TypeScript

* 使用 ESLint + Prettier
* 类型优先，减少 `any`
* 文件命名统一风格：

  * 组件：`PascalCase.tsx`
  * 工具/服务：`camelCase.ts` 或 `kebab-case.ts`

---

## 15. 本地开发

### 安装依赖

```bash
pnpm install
```

或

```bash
npm install
```

### 启动前端开发

```bash
pnpm dev
```

### 启动 Tauri 开发模式

```bash
pnpm tauri dev
```

### Rust 检查

```bash
cargo fmt --all
cargo clippy --all-targets --all-features -- -D warnings
cargo test
```

---

## 16. 发布流程

### 本地构建

```bash
pnpm tauri build
```

### 发布要求

* 所有测试通过
* 所有 lint 通过
* 文档同步更新
* 配置与 capability 审查通过
* 高风险工具默认关闭

---

## 17. 里程碑建议

### M1: 基础框架

* [ ] 初始化 Tauri 项目
* [ ] 接入前端框架
* [ ] 建立 Rust 模块结构
* [ ] 完成基础 command 通信
* [ ] 建立日志与配置系统

### M2: 对话能力

* [ ] Provider 接口抽象
* [ ] 接入第一个模型 API
* [ ] 实现对话会话管理
* [ ] 前端聊天 UI

### M3: Agent 能力

* [ ] Tool Registry
* [ ] Tool 调度机制
* [ ] Agent Executor
* [ ] 任务执行日志

### M4: 持久化与增强

* [ ] SQLite 持久化
* [ ] 会话历史查询
* [ ] Prompt 模板管理
* [ ] 配置页面

### M5: 安全增强

* [ ] capability 收敛
* [ ] 文件权限校验
* [ ] 高风险操作确认机制
* [ ] 审计日志完善

---

## 18. 推荐的 AI Prompt 模板

### 新增功能

```text
你正在为一个 Rust + Tauri 的 AI Agent 桌面助手项目编写代码。

请严格遵守 README 中的架构和约束：
1. 不要假设不存在的模块
2. 不要新增未声明依赖
3. command 层只做入口适配，不写核心逻辑
4. 输出完整文件
5. 代码必须可运行
6. 包含必要错误处理

本次任务：
- 文件路径：
- 功能目标：
- 输入输出：
- 影响模块：
- 约束条件：
```

### 修改现有功能

```text
请基于当前项目结构修改代码，并遵守 README 规范。

要求：
1. 先说明修改点
2. 再输出完整文件
3. 不改变无关行为
4. 保持现有模块边界
5. 所有新增逻辑必须有错误处理

修改目标：
- 文件：
- 问题：
- 期望行为：
```

### 修复 Bug

```text
请定位并修复以下问题，遵守 Rust + Tauri 项目 README 规范。

要求：
1. 分析根因
2. 说明修复方案
3. 输出完整文件
4. 禁止使用临时性 patch
5. 若影响接口，必须明确指出

问题描述：
- 报错信息：
- 复现步骤：
- 相关文件：
```

---

## 19. 不可违反的约束

以下规则优先级最高：

1. 不破坏模块边界
2. 不绕过权限控制
3. 不在生产逻辑中使用 `unwrap()`
4. 不直接在 UI 层调用底层危险能力
5. 不在没有审计的情况下执行本地命令
6. 不硬编码密钥、路径、模型配置
7. 不输出与当前仓库结构不一致的实现

---

## 20. 后续文档

建议尽快补齐以下文档：

* `docs/architecture.md`
* `docs/ai-rules.md`
* `docs/workflow.md`
* `docs/api.md`
* `docs/security.md`

---

## 21. License

根据需要选择：

* MIT
* Apache-2.0
* Proprietary

---

## 22. 贡献说明

欢迎贡献，但必须遵守：

* 架构约束
* AI 协作约束
* 安全规范
* Commit 规范
* 测试要求

---

## 23. 当前状态

> 项目处于初始化阶段，README 中的目录结构和规则为后续开发的统一基线。
> 所有新代码、文档、AI 生成内容均必须以本文件为准。

---

### 最该补的两个文件

#### 1. `docs/ai-rules.md`
专门约束 AI：
- 回答必须输出完整文件
- 不能脑补目录
- 不能跳过错误处理
- command / agent / provider / tool 分层必须遵守
- Rust 禁止 `unwrap()`
- 新模块必须先注册到 `mod.rs`

#### 2. `docs/architecture.md`
专门说明架构：
- Tauri 前后端边界
- Agent 执行流程
- Tool Registry 怎么注册
- Provider 接口长什么样
- Session / Memory 怎么流转

