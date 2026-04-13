# AI Agent Assistant (Rust + Tauri)

一个基于 **Rust + Tauri** 构建的桌面端 AI Agent 助手。  
目标是提供一个安全、可扩展、低资源占用的本地客户端，用于对话、工具调用、任务编排、上下文管理与插件化扩展。

**核心设计哲学**：
- **宏观规划**：`ROADMAP.md` 是**人可读**的任务路线图，记录项目目标、里程碑、任务分解与进度状态。
- **微观经验**：基于 **PDCA 环** 的**技能系统**是 **AI 可读**的原子能力封装，每个技能是一个独立的 Markdown 文件，包含输入、输出、执行逻辑与自我改进记录。
- **人机协作闭环**：人负责制定宏观规划（ROADMAP），AI 在执行中不断沉淀微观技能（PDCA 技能环），两者通过项目文件系统联动，实现可持续迭代。

---

## 1. 项目目标

本项目用于实现一个桌面 AI Agent 助手，具备以下能力：

- 多轮对话
- Agent 任务执行
- 工具调用（Tool Calling）
- **宏观规划管理**：自动读取、更新 `ROADMAP.md`
- **微观技能环**：基于 PDCA 自动生成、调用、改进技能文件
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

- **前端负责展示与交互**
- **Rust 后端负责核心逻辑、工具调用、状态管理**
- **AI Provider 通过统一适配层接入**
- **Agent 能力必须模块化，禁止逻辑散落**
- **所有跨边界调用必须显式声明**
- **默认最小权限原则**

### 3.2 宏观规划与微观经验（ROADMAP + PDCA 技能环）

#### 宏观规划：`ROADMAP.md`
- 位置：项目根目录
- 作用：**人可读、AI 可写**的任务路线图
- 内容：
  - 项目总体目标
  - 里程碑（Milestones）
  - 任务分解（Epics / Tasks）
  - 每个任务的状态（`pending` / `in_progress` / `done` / `blocked`）
  - 依赖关系
  - 当前焦点任务
  - 压缩后的历史记忆摘要
- 工作流：
  - Agent 启动时自动读取 `ROADMAP.md`，恢复进度
  - 用户提出新需求时，Agent 更新 `ROADMAP.md`
  - 任务完成后自动标记状态，并触发技能提炼

#### 微观经验：PDCA 技能环
- 位置：`src-tauri/skills/` 目录（每个技能一个 `.md` 文件）
- 作用：**AI 可读、AI 可写**的原子能力封装，代表一个可复用的“怎么做”
- 内容结构（遵循 PDCA 四阶段）：
  ```markdown
  ---
  name: "calculate_sum"
  version: "1.0.0"
  inputs:
    - name: numbers
      type: list<number>
  outputs:
    - type: number
  triggers:
    - "求和"
    - "累加"
  ---

  # Plan（计划）
  接收数字列表，返回总和。

  # Do（执行）
  ```rust
  fn execute(numbers: Vec<f64>) -> f64 {
      numbers.iter().sum()
  }
  ```

  # Check（检查）
  - 单元测试：`assert_eq!(execute(vec![1,2,3]), 6)`
  - 性能：O(n)

  # Act（改进）
  - 2026-04-13: 初始生成
  - 待改进：支持大整数溢出检测
  ```
- 工作流：
  - Agent 在执行任务时，优先查找已有技能
  - 若无匹配技能，Agent 通过 **Plan → Do → Check → Act** 自主生成新技能
  - 技能被调用多次后，Agent 可根据反馈自动改进技能（修改 `Act` 部分）
  - 技能文件可被人类审查、修改、删除

#### 两者联动
- `ROADMAP.md` 中的任务可以关联一个或多个技能（如 `skills: ["calculate_sum", "file_write"]`）
- Agent 执行任务时，从 `ROADMAP.md` 读取任务，然后调用对应技能完成
- 技能执行成功后，Agent 更新 `ROADMAP.md` 中的任务状态，并可能在 `Act` 阶段沉淀改进记录

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
- **ROADMAP.md 的读写与解析**
- **PDCA 技能环的生成、存储、检索与自我改进**

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
├── ROADMAP.md                     # 宏观规划文件（人可读，AI 可写）
│
├── src/                           # 前端源码
│   ├── components/                # UI 组件
│   ├── pages/                     # 页面
│   ├── hooks/                     # 前端 hooks
│   ├── services/                  # 前端服务层
│   ├── stores/                    # 状态管理
│   ├── types/                     # TS 类型定义
│   ├── utils/                     # 工具函数
│   └── main.tsx                   # 前端入口
│
├── src-tauri/
│   ├── src/
│   │   ├── main.rs                # Rust 入口
│   │   ├── commands/              # Tauri command 按模块拆分
│   │   ├── agent/                 # Agent 核心逻辑
│   │   │   ├── mod.rs
│   │   │   ├── executor.rs        # Agent 执行器
│   │   │   ├── planner.rs         # 规划器（读写 ROADMAP.md）
│   │   │   ├── session.rs         # 会话上下文
│   │   │   ├── memory.rs          # 记忆模块
│   │   │   └── skill/             # PDCA 技能环管理
│   │   │       ├── mod.rs
│   │   │       ├── loader.rs      # 加载技能文件
│   │   │       ├── generator.rs   # 生成新技能
│   │   │       ├── executor.rs    # 执行技能
│   │   │       └── improver.rs    # 自我改进技能
│   │   ├── tools/                 # 工具系统
│   │   │   ├── mod.rs
│   │   │   ├── registry.rs        # 工具注册中心
│   │   │   ├── file_tool.rs
│   │   │   └── shell_tool.rs      # 如启用需严格权限控制
│   │   ├── providers/             # AI 模型提供商适配
│   │   │   ├── mod.rs
│   │   │   ├── openai.rs
│   │   │   ├── anthropic.rs
│   │   │   └── local.rs
│   │   ├── models/                # 数据结构
│   │   ├── db/                    # 持久化层
│   │   ├── config/                # 配置管理
│   │   ├── errors/                # 错误定义
│   │   ├── logging/               # 日志封装
│   │   └── utils/                 # Rust 工具模块
│   │
│   ├── skills/                    # 微观技能文件存储（PDCA 环）
│   │   ├── calculate_sum.md
│   │   ├── write_to_file.md
│   │   └── ...
│   │
│   ├── capabilities/              # Tauri capability 配置
│   ├── icons/
│   ├── tauri.conf.json
│   └── Cargo.toml
│
├── docs/
│   ├── architecture.md            # 架构说明
│   ├── workflow.md                # 开发工作流
│   ├── ai-rules.md                # AI 协作规范
│   └── api.md                     # 命令/API 说明
│
├── scripts/                       # 构建/检查脚本
├── .env.example
├── Cargo.toml                     # 如 workspace 模式可选
├── package.json
├── README.md
└── LICENSE
```

---

## 6. 模块约束

### 6.1 commands
- 仅作为 Tauri 暴露层
- 不承载核心业务逻辑
- 必须调用 `agent/`、`tools/`、`providers/` 等模块完成实际工作

### 6.2 agent
- 负责任务编排、上下文管理、工具调度
- **必须包含 `planner.rs` 用于读写 `ROADMAP.md`**
- **必须包含 `skill/` 模块用于管理 PDCA 技能环**
- 不直接依赖 UI
- 不允许与 Tauri 强耦合

### 6.3 tools
- 所有工具必须注册到统一 Registry
- 所有工具必须定义：
  - 名称
  - 输入参数
  - 输出结构
  - 错误行为
  - 权限要求
- 工具调用必须支持超时控制

### 6.4 providers
- 所有模型调用必须经过 Provider 抽象层
- 不允许在业务代码中直接写死某一厂商 API
- 必须支持未来替换不同模型后端

### 6.5 db
- 所有持久化逻辑统一放在 `db/`
- 禁止在业务模块中散落读写文件逻辑
- Schema 变更必须有迁移方案

### 6.6 skills（PDCA 环）
- 技能文件必须遵循固定 Markdown 模板
- 技能必须包含 `name`, `version`, `inputs`, `outputs` 等元数据
- 技能文件必须包含 `Plan`, `Do`, `Check`, `Act` 四个章节
- 技能生成时，`Do` 部分可以包含伪代码或自然语言描述，但最终会被替换为可执行代码（由 Agent 在后续调用中填充）
- Agent 在技能执行失败时，应记录失败原因到 `Check` 部分，并可能触发 `Act` 改进

---

## 7. 开发工作流

### 7.1 分支策略
- `main`：稳定可发布版本
- `dev`：日常开发集成分支
- `feature/*`：功能开发分支
- `fix/*`：问题修复分支
- `refactor/*`：重构分支

### 7.2 开发流程（含 ROADMAP 与技能环）
1. 从 `dev` 创建功能分支
2. **若涉及新功能，先在 `ROADMAP.md` 中添加对应任务（状态 `pending`）**
3. 编写或更新设计说明
4. 让 AI 按规范生成代码：
   - AI 首先读取 `ROADMAP.md` 获取当前焦点任务
   - AI 尝试复用已有技能（`src-tauri/skills/`）
   - 若无匹配技能，AI 通过 PDCA 环生成新技能（写入 `skills/`）
   - AI 生成实现代码，并调用技能执行
5. 本地运行与测试
6. 任务完成后，更新 `ROADMAP.md` 中任务状态为 `done`
7. 自查通过后提交 PR
8. 代码审查
9. 合并到 `dev`
10. 发布前从 `dev` 合并到 `main`

### 7.3 Commit 规范
使用 Conventional Commits：
```text
feat: 新增技能加载模块
fix: 修复 ROADMAP.md 解析错误
refactor: 重构 skill 执行器
docs: 更新 README 与架构文档
test: 增加 skill generator 单元测试
chore: 调整 lint 和格式化配置
```

---

## 8. AI 编码约束

AI 在参与本项目开发时，必须遵守以下规则。

### 8.1 宏观规划约束
- AI **必须**在每次会话启动时读取项目根目录的 `ROADMAP.md`
- AI **必须**在用户提出新需求时，评估是否需要更新 `ROADMAP.md`（新增任务、修改状态等）
- AI **必须**在完成任务后，将对应任务标记为 `done`，并可选地添加完成摘要
- AI **禁止**随意删除 `ROADMAP.md` 中的已有任务

### 8.2 微观技能约束
- AI 在执行原子操作时，**优先查找** `src-tauri/skills/` 目录下是否有匹配技能
- 若技能存在，直接调用技能执行（通过 `skill/executor.rs`）
- 若技能不存在，AI **必须**按照 PDCA 模板生成新技能，并保存到 `skills/` 目录
- 技能生成后，AI **必须**在后续执行中使用该技能（而不是重新实现逻辑）
- AI 在技能执行失败时，**应该**分析失败原因，并尝试更新技能的 `Act` 部分（自我改进）
- AI **禁止**硬编码技能逻辑到业务代码中；所有可复用的能力必须封装为技能文件

### 8.3 输出规则
- 默认输出完整文件
- 所有代码必须能直接运行
- 必须保留已有功能，除非明确要求删除
- 不允许只给思路不写实现
- 不允许擅自更改接口定义

### 8.4 改动规则
每次改动必须附带：
1. 修改了哪些文件
2. 为什么修改
3. 是否影响已有接口
4. 是否需要同步文档

### 8.5 Rust 代码规则
- 优先显式类型
- 错误统一使用 `Result<T, AppError>` 或统一错误体系
- 禁止无边界 `unwrap()` / `expect()` 出现在生产路径
- 必须考虑并发安全
- 异步任务必须可追踪
- 对外结构体必须可序列化时显式 `derive`

### 8.6 Tauri 边界规则
- `#[tauri::command]` 仅作入口适配
- 不在 command 中直接写复杂逻辑
- 所有前后端交互数据必须定义类型
- 所有敏感能力必须明确 capability 控制

### 8.7 前端规则
- UI 状态与业务状态分离
- API 调用统一放在 `services/`
- 不在组件中直接拼装复杂业务逻辑
- 所有用户输入必须校验
- 所有异步交互必须显示 loading/error 状态

---

## 9. 安全约束

这是一个 Agent 项目，安全优先级高于功能速度。

### 禁止事项
- 禁止硬编码 API Key
- 禁止默认启用危险 Shell 命令执行
- 禁止无确认删除文件
- 禁止读取用户未授权路径
- 禁止将敏感日志输出到前端
- 禁止在生产环境打印完整请求密钥或 token

### 必须做到
- 所有密钥来自环境变量或安全存储
- 所有文件访问必须做路径校验
- 所有工具执行必须记录审计日志
- 所有高风险操作必须要求显式用户确认
- 所有外部请求必须设置超时

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
- 默认配置写入 `config/`
- 敏感信息不进入仓库
- 所有配置项要有默认值或明确校验错误

---

## 11. 日志与可观测性

建议使用 `tracing`。

### 要求
- 关键流程必须记录日志：
  - 会话创建
  - Agent 执行开始/结束
  - **ROADMAP.md 的读取与更新**
  - **技能加载、生成、改进**
  - Tool 调用
  - Provider 请求失败
- 日志级别区分：
  - `error`
  - `warn`
  - `info`
  - `debug`
- 不记录敏感信息原文

---

## 12. 错误处理规范

### 原则
- 错误必须结构化
- 用户可见错误与内部错误分离
- 不将底层敏感错误直接暴露给 UI

### 建议
- 使用统一错误类型 `AppError`
- 按模块拆分错误来源
- 对外返回可读消息，对内保留详细上下文

---

## 13. 测试规范

### Rust 侧
- 单元测试：核心业务逻辑
- 集成测试：Provider / Tool / Agent 流程
- 对关键模块必须有测试：
  - Tool Registry
  - Agent Executor
  - Session Store
  - Config Loader
  - **ROADMAP 解析器**
  - **技能加载与执行器**

### 前端侧
- 核心组件渲染测试
- 状态管理测试
- 关键交互测试

### 最低要求
- 新增核心功能必须附带测试
- 修复 bug 必须增加回归测试

---

## 14. 代码风格

### Rust
- 使用 `cargo fmt`
- 使用 `cargo clippy`
- 模块命名使用 `snake_case`
- 类型名使用 `PascalCase`
- 常量使用 `SCREAMING_SNAKE_CASE`

### TypeScript
- 使用 ESLint + Prettier
- 类型优先，减少 `any`
- 文件命名统一风格：
  - 组件：`PascalCase.tsx`
  - 工具/服务：`camelCase.ts` 或 `kebab-case.ts`

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
- 所有测试通过
- 所有 lint 通过
- 文档同步更新
- 配置与 capability 审查通过
- 高风险工具默认关闭

---

## 17. 里程碑建议

### M1: 基础框架
- [ ] 初始化 Tauri 项目
- [ ] 接入前端框架
- [ ] 建立 Rust 模块结构
- [ ] 完成基础 command 通信
- [ ] 建立日志与配置系统
- [ ] **支持读取和解析 `ROADMAP.md`**

### M2: 对话能力
- [ ] Provider 接口抽象
- [ ] 接入第一个模型 API
- [ ] 实现对话会话管理
- [ ] 前端聊天 UI

### M3: Agent 能力 + 宏观规划
- [ ] Tool Registry
- [ ] Tool 调度机制
- [ ] Agent Executor
- [ ] **Agent 能够自动更新 `ROADMAP.md`**
- [ ] 任务执行日志

### M4: 微观技能环（PDCA）
- [ ] 技能文件加载器
- [ ] 技能生成器（根据 PDCA 模板创建新技能）
- [ ] 技能执行器
- [ ] 技能自我改进器（更新 `Act` 部分）
- [ ] Agent 优先调用已有技能

### M5: 持久化与增强
- [ ] SQLite 持久化
- [ ] 会话历史查询
- [ ] Prompt 模板管理
- [ ] 配置页面

### M6: 安全增强
- [ ] capability 收敛
- [ ] 文件权限校验
- [ ] 高风险操作确认机制
- [ ] 审计日志完善

---

## 18. 推荐的 AI Prompt 模板

### 新增功能（含宏观规划更新）
```text
你正在为一个 Rust + Tauri 的 AI Agent 桌面助手项目编写代码。

请严格遵守 README 中的架构和约束：
1. 不要假设不存在的模块
2. 不要新增未声明依赖
3. command 层只做入口适配，不写核心逻辑
4. 输出完整文件
5. 代码必须可运行
6. 包含必要错误处理
7. **如果任务会改变项目范围或里程碑，请先更新 ROADMAP.md**
8. **对于可复用的原子操作，请生成新的 PDCA 技能文件**

本次任务：
- 文件路径：
- 功能目标：
- 输入输出：
- 影响模块：
- 约束条件：
```

### 修改现有功能（含技能改进）
```text
请基于当前项目结构修改代码，并遵守 README 规范。

要求：
1. 先说明修改点
2. 再输出完整文件
3. 不改变无关行为
4. 保持现有模块边界
5. 所有新增逻辑必须有错误处理
6. **如果修改影响已有技能的行为，请同时更新对应的技能文件（特别是 Act 部分）**

修改目标：
- 文件：
- 问题：
- 期望行为：
```

### 修复 Bug（含技能回归测试）
```text
请定位并修复以下问题，遵守 Rust + Tauri 项目 README 规范。

要求：
1. 分析根因
2. 说明修复方案
3. 输出完整文件
4. 禁止使用临时性 patch
5. 若影响接口，必须明确指出
6. **如果 bug 与某个技能相关，请在修复后更新该技能的 Check 部分（增加回归测试用例）**

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
8. **不跳过 ROADMAP.md 的更新步骤**
9. **不手动复制粘贴技能逻辑；必须复用技能文件**

---

## 20. 后续文档

建议尽快补齐以下文档：

- `docs/architecture.md`
- `docs/ai-rules.md`
- `docs/workflow.md`
- `docs/api.md`
- `docs/security.md`
- `docs/roadmap-format.md`（详细说明 ROADMAP.md 的格式规范）
- `docs/skill-format.md`（详细说明 PDCA 技能文件的格式规范）

---

## 21. License

根据需要选择：
- MIT
- Apache-2.0
- Proprietary

---

## 22. 贡献说明

欢迎贡献，但必须遵守：
- 架构约束
- AI 协作约束
- 安全规范
- Commit 规范
- 测试要求
- **ROADMAP 与技能环的维护规范**

---

## 23. 当前状态

> 项目处于初始化阶段，README 中的目录结构和规则为后续开发的统一基线。
> 所有新代码、文档、AI 生成内容均必须以本文件为准。
> **ROADMAP.md 和 skills/ 目录尚未创建，请根据第 5 节目录结构初始化。**
