# 用 Codex / ZCode 驱动 AI 合规系统开发：SDD 全流程提示词与命令指南

以下指南围绕第一期系统（上架前合规风险拦截引擎 + 合规知识库 + 文档自动化处理 + 合规任务编排）的四个核心模块，按照 SDD（Specification-Driven Development）的六阶段工作流，给出每一步可直接使用的提示词和命令。


## 前置准备：环境安装与初始化

### 1. Codex CLI 安装

```bash
# macOS / Linux 官方独立安装器
curl -fsSL https://chatgpt.com/codex/install.sh | sh

# 或通过 npm 统一管理版本
npm install -g @openai/codex

# 或 Homebrew（macOS）
brew install --cask codex
```

注意不要混装多种渠道，否则 PATH 中优先命中的旧版本可能导致升级失败。

### 2. 项目初始化 + SDD 工作区脚手架

```bash
# 创建项目目录
mkdir amazon-compliance-ai && cd amazon-compliance-ai
git init

# 安装 spec-driven-codex 工作区
npx spec-driven-codex init

# 启动 Codex CLI
codex
```

`spec-driven-codex init` 会创建 `.sdd/` 工作区目录，并在 `~/.codex/prompts/` 下安装 SDD 流程所需的提示词文件。生成的目录结构如下：

```
amazon-compliance-ai/
├── .sdd/
│   ├── README.md            # 命令概览
│   ├── description.md       # 需求描述模板
│   ├── target-spec.txt      # 当前活跃规格名
│   ├── steering/            # 发现笔记
│   └── specs/               # 规格、任务、归档
└── ~/.codex/prompts/
    ├── sdd-steering.md
    ├── sdd-requirements.md
    ├── sdd-design.md
    ├── sdd-tasks.md
    ├── sdd-implement.md
    └── sdd-archive.md
```

### 3. 生成 AGENTS.md（项目说明书）

```bash
# 在 Codex 交互会话中执行
/init
```

AGENTS.md 是 Codex 的“项目宪法”，越靠近目标代码的文件优先级越高，Codex 按“全局规则 → 项目根目录 → 当前工作目录”的顺序合并指令。

生成后手动补充合规系统的专属约束：

```markdown
# AGENTS.md — amazon-compliance-ai

## 项目概述
AI 驱动的亚马逊合规前置检测系统。第一期覆盖文案合规检测、
图片版权溯源、类目属性预检、合规文档 OCR、任务自动编排。

## 技术栈
- 后端：Python 3.11 + FastAPI + Pydantic v2
- 前端：React 18 + TypeScript + Vite
- AI 编排：LangGraph（多模型交叉核验）
- 知识库：Neo4j（法规知识图谱）+ pgvector（语义检索）
- 任务队列：Celery + Redis
- 测试：pytest + playwright

## 目录结构
- `src/engines/` — 检测引擎（文案/图片/属性）
- `src/knowledge/` — 法规知识图谱与 RAG
- `src/documents/` — 文档 OCR 与自动审核
- `src/orchestrator/` — 任务编排引擎
- `src/api/` — API 路由层
- `tests/` — 测试（与 src 镜像）

## 代码规范
- 公共函数必须有类型注解
- 禁止硬编码法规规则，所有规则从知识库读取
- 所有 AI 判断结果必须附带置信度和证据片段
- 提交前必须运行 `pytest -q && ruff check`

## 验证入口
- 单元测试：`pytest tests/unit -q`
- 集成测试：`pytest tests/integration -q`
- 端到端：`playwright test`
```


## 阶段一：Steering — 产品上下文捕获

**命令**：
```
/sdd-steering
```

**提示词**：

```
捕获 amazon-compliance-ai 项目的产品上下文。

我们需要构建第一期 AI 合规前置检测系统的产品定义。
请从以下维度整理：

1. 产品定位：将合规审查从“上架后被动应对”前移至
   “上架前主动拦截”，同时将高频合规操作自动化。
2. 目标用户：亚马逊中国跨境卖家（中小卖家为主），
   覆盖美国站和英国站。
3. 核心痛点：
   - 文案违禁词导致 Listing 被下架
   - 图片版权侵权引发投诉
   - 类目必填属性缺失导致上架失败
   - VAT 发票和合规材料人工审核效率低
4. 第一期范围（MVP）：
   - 文案合规检测（广告法违禁词、功效夸大、隐性侵权）
   - 基础风险评分（0-100，绿/黄/红三级）
   - 覆盖美国站和英国站
5. 关键约束：
   - 高风险违规召回率 ≥ 95%
   - 误拦率 ≤ 8%
   - 单次文案检测响应时间 < 5 秒
6. 风险：法规更新频率快，需建立自动化监测机制

请在 .sdd/steering/ 下输出上下文文档。
```

Codex 会在 `.sdd/steering/` 下生成发现笔记，捕获产品上下文、干系人和风险。


## 阶段二：Requirements — 需求规格定义

**前置操作**：更新 `.sdd/description.md`，写入本次迭代的问题描述和功能概要。

**命令**：
```
/sdd-requirements
```

**提示词**：

```
基于 .sdd/steering/ 中的上下文，为“文案合规检测引擎”
定义需求规格和验收标准。

功能需求（FR）：
- FR-001：支持上传商品标题、五点描述、A+ 文案、Search Terms
- FR-002：检测广告法违禁词（绝对化用语、功效承诺、医疗暗示）
- FR-003：检测隐性知识产权风险（未授权品牌名、专利暗示）
- FR-004：按目标站点自动切换检测规则集（US/UK）
- FR-005：输出逐条风险清单，每条含风险等级、触发规则、
  建议修正话术
- FR-006：输出 0-100 合规风险评分，对应绿/黄/红三级决策

非功能需求（NFR）：
- NFR-001：单次检测响应时间 < 5 秒（1000 字文案）
- NFR-002：高风险违规召回率 ≥ 95%
- NFR-003：误拦率 ≤ 8%
- NFR-004：检测结果必须附带置信度和证据片段

验收标准（AC）：
- AC-001：输入含“最好”的标题，系统必须标记为高风险
- AC-002：同一文案在美国站和英国站的检测结果应有差异
- AC-003：评分 60 分以下的文案，系统输出红灯并阻断提交
- AC-004：每条风险必须提供至少一个合规替代话术

更新 .sdd/description.md 并输出规格文档。
```

Codex 会生成包含 SCN（场景）→ REQ（需求）→ AC（验收标准）追溯链的规格文档，确保每个需求都有可验证的验收标准。


## 阶段三：Architecture — 架构设计

**命令**：
```
/sdd-design
```

**提示词**：

```
基于 .sdd/specs/ 中的需求规格，设计文案合规检测引擎的
技术架构。

系统采用四层架构：
1. 知识层：Neo4j 知识图谱存储法规规则（实体-关系结构），
   pgvector 存储法规原文 embedding 支持语义检索。
2. 推理层：多模型交叉核验。文案检测调用文本分类模型，
   各模型独立输出判断后由编排层进行权重判定和冲突消解。
3. 执行层：工作流编排引擎，将检测结果转化为执行任务。
4. 交互层：FastAPI REST 接口 + WebSocket 实时推送。

关键设计决策：
- 规则不硬编码，从 Neo4j 读取，支持热更新
- 每条规则附带：法规原文出处、生效日期、适用品类、
  违规后果、平台执行案例
- 检测结果输出结构化 JSON：
  {risk_level, rule_id, evidence_span, confidence, suggestion}
- 支持站点差异化：规则节点带 jurisdiction 属性
  （US/UK/DE/JP），查询时按站点过滤

请输出架构文档到 .sdd/specs/，包含：
- 系统架构图（Mermaid）
- 数据流说明
- Neo4j 知识图谱 Schema 定义
- API 契约（OpenAPI 3.0）
- 部署架构（Docker Compose）
```

架构设计阶段需要产出可执行的 API 契约和数据模型。SDD 的核心理念是通过结构化文档引导 AI 生成过程，避免“需求-设计-任务”三阶段中的信息丢失。


## 阶段四：Tasks — 任务分解

**命令**：
```
/sdd-tasks
```

**提示词**：

```
将文案合规检测引擎的架构设计拆解为可执行的任务清单。

每个任务必须包含：任务 ID、目标、涉及文件、依赖关系、
验收标准、验证方式。

建议任务分解：

TASK-001：搭建项目骨架
  - 目标：FastAPI 应用 + Neo4j/pgvector 连接 + Celery
  - 文件：src/api/main.py, src/knowledge/graph.py, docker-compose.yml
  - 验证：curl /health 返回 200

TASK-002：实现 Neo4j 知识图谱 Schema
  - 目标：创建 Rule/Regulation/Jurisdiction 节点和关系
  - 文件：src/knowledge/schema.cypher, src/knowledge/graph.py
  - 依赖：TASK-001
  - 验证：pytest tests/unit/test_graph.py

TASK-003：实现文案检测引擎
  - 目标：接收文案文本，返回风险清单 JSON
  - 文件：src/engines/text_engine.py, src/engines/models.py
  - 依赖：TASK-002
  - 验证：pytest tests/unit/test_text_engine.py

TASK-004：实现风险评分模型
  - 目标：多风险项加权评分，输出 0-100 分和三级决策
  - 文件：src/engines/scorer.py
  - 依赖：TASK-003
  - 验证：pytest tests/unit/test_scorer.py

TASK-005：实现 API 接口层
  - 目标：POST /api/v1/detect/text 接口
  - 文件：src/api/routes/detect.py
  - 依赖：TASK-004
  - 验证：pytest tests/integration/test_api.py
```

Codex 会将设计拆解为可操作的任务列表，每个任务都应具备独立的验收标准，确保 AI 在写代码前先确认需求和执行边界。


## 阶段五：Implementation — 编码实现

**命令**：
```
/sdd-implement
```

Codex 会逐任务实现代码并附带测试。如需控制单个任务的实现，可以用自然语言指定：

```
实现 TASK-003：文案检测引擎。

要求：
1. 使用 Pydantic v2 定义输入输出模型：
   - TextDetectionRequest: {text, site, category}
   - RiskItem: {risk_level, rule_id, evidence_span, confidence, suggestion}
   - TextDetectionResponse: {risks, score, decision}
2. 从 Neo4j 查询规则时，必须按 site 参数过滤 jurisdiction 属性
3. 检测逻辑采用多模型交叉核验：规则匹配引擎 + 文本分类模型
   独立输出后，由 conflict_resolver 进行权重判定
4. 每个风险项必须附带证据片段（从原文截取）和置信度
5. 禁止硬编码规则，所有规则从 Neo4j 读取

参考已有代码风格：src/knowledge/graph.py

完成后运行 pytest tests/unit/test_text_engine.py 并说明
覆盖了哪些边界情况、哪些未覆盖。
```

**AGENTS.md 约束生效**：Codex 在每次执行前会读取 AGENTS.md，自动遵守类型注解、测试命令、代码规范等约束。

**Plan Mode 辅助**：对于复杂任务（如多模型交叉核验的冲突消解逻辑），可先按 `Shift+Tab` 进入 Plan Mode，让 Codex 先规划再执行：

```
/plan
设计多模型交叉核验的冲突消解逻辑。

背景：文案检测有两个独立判断源——规则匹配引擎（精确但覆盖面窄）
和文本分类模型（覆盖面广但可能误判）。需要设计冲突消解策略。

要求：
- 两个模型都标记为高风险时，取并集，置信度加权
- 规则引擎标记高风险、模型标记低风险时，以规则引擎为准
- 模型标记高风险、规则引擎标记低风险时，需人工复核
- 输出每个判断的来源（rule_based / model_based / both）
```

Plan Mode 让 Codex 先读取当前代码和项目状态，提出方案后再执行。


## 阶段六：Verification & Archive — 验证与归档

### 验证

```
/sdd-verify
```

或使用 Codex 的 review 能力进行代码审查：

```
/review
审查 src/engines/text_engine.py 的实现。

检查项：
1. 是否所有公共函数都有类型注解？
2. 是否所有规则从 Neo4j 读取（无硬编码）？
3. 风险项的 evidence_span 是否确实存在于原文中？
4. 置信度阈值逻辑是否与规格文档 AC-001 到 AC-004 一致？
5. 是否有未处理的异常路径（如 Neo4j 连接失败）？

对每个问题给出文件位置、行号和修复建议。
```

验证阶段需要产出“测试、构建、差异检查、代码审查和失败说明”组成的证据链，确保 AI 生成代码的可靠性。

### 归档

```
/sdd-archive
```

```
总结本次迭代的成果、经验教训和下一步计划。

输出：
1. 已完成的任务清单（TASK-001 至 TASK-005）
2. 测试覆盖情况
3. 遗留问题和已知限制
4. 下一步迭代建议（图片检测引擎、知识库扩展至德国站）
```


## ZCode 的等价操作

ZCode 是智谱 AI 推出的 Agentic Development Environment（ADE），完整接入 GLM-5.2 模型，定位为“把 GLM-5.2 的推理能力封装成可操作的编程搭档”。与 Codex 的核心差异在于：Codex 用 OS 级沙箱和 Rust 编译器的严格类型系统兜底安全性，ZCode 用权限决策链（deny > ask > allow）和状态机保证过程可诊断。

### ZCode 中的 SDD 等价操作

**1. 工作区初始化**：在 ZCode 中选择项目文件夹作为工作区，ZCode 会自动读取 AGENTS.md 作为项目规则。

**2. 自定义命令**：ZCode 支持将重复使用的提示词保存为 Command，通过 `/` 快速调用。可以将上述 SDD 各阶段的提示词模板分别保存为 `/sdd-steering`、`/sdd-requirements` 等命令。

**3. Skill 机制**：ZCode 的 Skill 机制适合承载“一套完整的工作方法论”。将 SDD 六阶段流程定义为一个 Skill，在输入框输入 `$` 即可调用技能面板。

**4. 核心提问公式**：ZCode 的黄金提问公式为“目标 + 位置 + 约束”。例如：

```
目标：实现文案合规检测引擎的风险评分模型
位置：src/engines/scorer.py
约束：
- 输入为 RiskItem 列表，输出 0-100 分和三级决策
- 高风险项权重 5，中风险 3，低风险 1
- 总分 = 100 - sum(权重 × 数量)，最低 0 分
- 85 分以上绿灯，60-84 黄灯，60 以下红灯
- 写 pytest 测试覆盖三个等级边界
```

**5. 关键区别**：ZCode 每一步实际操作前都会请求用户许可，适合需要严格人工审核的合规场景；Codex 在沙箱内自主执行，适合快速迭代。


## 完整命令序列速查

```bash
# === 初始化 ===
mkdir amazon-compliance-ai && cd amazon-compliance-ai
git init
npx spec-driven-codex init
codex

# === SDD 六阶段（Codex CLI 中依次执行） ===
/init                              # 生成 AGENTS.md
/sdd-steering                      # 阶段一：产品上下文
/sdd-requirements                  # 阶段二：需求规格
/sdd-design                        # 阶段三：架构设计
/sdd-tasks                         # 阶段四：任务分解
/sdd-implement                     # 阶段五：编码实现
/sdd-verify                        # 阶段六：验证
/sdd-archive                       # 阶段六：归档

# === 辅助命令 ===
/plan                              # 进入 Plan Mode（先规划再执行）
/review                            # 代码审查
/sdd-highway                       # 快速通道（自动执行 design→tasks→implement）
```


## 关键实践建议

**第一，spec 保持精简。** OpenAI Codex 团队内部的实践是 spec 通常只有 10 条要点左右，然后就进入开发。spec 的目的是在人和 AI 之间对齐“要做什么”，而不是写一份完美的文档。

**第二，AGENTS.md 是约束的核心载体。** 合规系统对“不可硬编码规则”“必须附带证据片段”等约束，通过 AGENTS.md 固化为项目级规则，Codex 和 ZCode 都会自动读取并遵守。

**第三，每个任务带测试结束。** 将验收标准嵌入到 `/sdd-implement` 的提示词中，要求 Codex 完成每个任务后运行对应测试并说明覆盖情况，形成可复验的证据链。

**第四，ZCode 用于需要人工审核的环节。** 合规场景中，法规规则审核、风险评分阈值调整等关键决策建议在 ZCode 中执行，利用其“先出方案、经你确认、再动手”的权限决策链确保人在回路中。
