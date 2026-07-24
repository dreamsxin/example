# Spec - AI 智能测试效率平台 v1.2

> 生成日期：2026-07-24
> 基于：PRD V1.0（初稿）
> 状态：待确认

---

## 1. 产品定义

- **一句话描述**：以工具注册中心收编存量测试工具、以知识库存档沉淀测试经验、以统一报告汇集执行结果，帮助测试团队统一管理测试资产、集中统计数据、建设可追溯的团队成果体系。
- **目标用户**：测试工程师（一线工具开发与执行）、测试组长（资产管理与团队建设）、平台管理员（运维与安全）、管理层（质量趋势关注）
- **核心问题**：测试过程缺乏统一支撑——工具成果分散不可见、执行结果散落各处、知识经验难沉淀。平台以已有成果为起点，系统化整合这些能力。

---

## 2. MVP 范围（锁定——不在此列表的功能一律不做）

| 优先级 | 功能 | 验收标准摘要 | RICE 评分 |
| :--- | :--- | :--- | :--- |
| P0 | 权限与安全（模块 F） | RBAC 四角色 + 敏感配置加密 + 操作审计日志 | Reach=10, Impact=2, Confidence=0.95, Effort=2 → 9.5 |
| P0 | 工具注册中心（模块 J-1） | 元数据登记 + 多维检索 + 重复提示 + owner 制 | Reach=7, Impact=2, Confidence=0.9, Effort=2 → 6.3 |
| P0 | 工具分级接入 L1/L2（模块 J-2） | L1 登记接入 + L2 结果上报 SDK + 上报 API + 失败重试不阻塞 | Reach=7, Impact=3, Confidence=0.85, Effort=3 → 5.95 |
| P0 | 工具准入安全治理（模块 J-3） | 准入三条件 + 敏感信息扫描 + 敏感配置收归 + 下线存档 | Reach=7, Impact=2, Confidence=0.9, Effort=2 → 6.3 |
| P0 | 执行结果汇集与大盘（模块 E-Lite） | L2 工具结果统一入库 + 通过率/趋势统计 + 导出 + 质量大盘 | Reach=9, Impact=2, Confidence=0.9, Effort=2 → 8.1 |
| P0 | 知识库文档存档（模块 G-Lite） | 多格式上传 + 元数据标注 + 文档列表/详情 + 增量更新 | Reach=8, Impact=3, Confidence=0.9, Effort=2 → 10.8 |
| P0 | 知识库全文搜索（模块 G-Search） | PostgreSQL 全文检索 + 关键词高亮 + 来源过滤 + 搜索结果预览 | Reach=8, Impact=2, Confidence=0.9, Effort=1 → 14.4 |

---

## 3. 明确不做（Out-of-Scope — 锁定）

| 不做的功能 | 原因 | 何时考虑 |
| :--- | :--- | :--- |
| AI 用例智能生成（模块 I-1） | AI 能力暂不接入 MVP，需先跑通工具收编与知识存档链路 | V1.1 |
| AI 失败根因分析（模块 I-2） | 同上，需历史执行数据积累 | V1.1 |
| 知识回流管道（模块 I-3） | 需先有执行数据与缺陷闭环 | V1.1 |
| AI 知识问答（RAG/向量检索） | MVP 用全文搜索替代；AI 问答需 LLM 接入，暂不引入 | V1.1 |
| 测试用例管理（模块 A） | MVP 聚焦工具收编与知识存档；用例管理列为下一阶段 | V1.1 |
| 任务调度（模块 B） | 依赖用例管理；MVP 工具通过 L2 SDK 自主上报结果 | V1.1 |
| 自动化执行引擎（模块 C） | MVP 不做平台侧执行；工具各自运行后上报结果 | V1.2 |
| 向量数据库 + Embedding | MVP 用 PostgreSQL 全文检索；向量检索随 AI 问答一起引入 | V1.1 |
| LLM 接入 | MVP 不接入 LLM；商用 API（DeepSeek/GPT）在 V1.1 AI 模块引入 | V1.1 |
| 移动端自动化（Appium） | MVP 聚焦 Web 工具收编 | V2.0 |
| 性能测试（JMeter 集成） | 优先级低于工具收编 | V1.2 |
| L3 容器调度接入 | 需先完成 L1/L2 收编 | V1.2 |
| GraphRAG 多跳推理 | 需先完成向量检索基线 | V2.0 |
| CI/CD 深度集成 | MVP 仅支持手动操作 | V1.2 |
| 全公司通用知识库 | 知识库边界限定测试域 | 不做 |
| 自研大模型 / 测试框架 | 采用成熟方案集成 | 不做 |

---

## 4. 技术架构（锁定 — 含版本锚定）

| 层 | 技术 | 实际版本 | 锁定原因 |
| :--- | :--- | :--- | :--- |
| 前端框架 | Vue.js | 3.4.x | 团队熟悉；生态成熟 |
| 前端 UI 库 | Element Plus | 2.7.x | Vue 3 生态最成熟的组件库 |
| 图标库 | Lucide（lucide-vue-next） | 0.400.x | 统一描边 SVG 图标，语义明确，无 emoji |
| 后端框架 | Gin | 1.10.x | 高性能 HTTP 框架；Go 生态主流；与现有 Go 代码库一致 |
| ORM | GORM | 2.0.x | Go 生态最成熟 ORM；支持 AutoMigrate + Hook + 预加载 |
| 数据迁移 | golang-migrate | 4.17.x | 版本化数据库迁移；SQL 文件管理，可审计 |
| 关系数据库 | PostgreSQL | 16 | 全文索引原生支持（pg_trgm + tsvector）；JSON 字段支持 |
| 缓存 | Redis | 7.2.x | 缓存 + 会话存储 + asynq broker |
| 任务队列 | asynq | 0.25.x | Go 原生异步任务库（基于 Redis）；轻量无需额外进程管理 |
| 文档解析 | pdfcpu + godocx + goquery + goldmark | latest | 多格式解析（PDF/Word/HTML/Markdown） |
| JWT 库 | golang-jwt/jwt | 5.2.x | JWT 签发与验证 |
| 密码加密 | golang.org/x/crypto/bcrypt | latest | bcrypt 加盐哈希 |
| 容器化 | Docker + Docker Compose | 24.x / 2.x | MVP 阶段用 Compose 编排 |
| 前端构建 | Vite | 5.x | Vue 3 官方推荐构建工具 |
| 认证方案 | JWT（Access 15min + Refresh 7d） | - | 无状态认证 |
| 反向代理 | Nginx | 1.25.x | 静态资源 + 反向代理 |

> **MVP 不含**：Milvus（向量库）、BGE-M3（Embedding）、LangChain（RAG 编排）、DeepSeek/GPT（LLM）、Playwright（执行引擎）。这些在 V1.1 AI 模块引入时接入。

### 部署架构（MVP — Docker Compose）

```
                    ┌─────────────┐
                    │   Nginx     │ :80/:443
                    │  反向代理    │
                    └──────┬──────┘
              ┌────────────┼────────────┐
              v            v            v
        ┌──────────┐ ┌──────────┐ ┌──────────┐
        │ Frontend │ │ Backend  │ │  Worker  │
        │ (Vite    │ │ (Go +    │ │ (asynq   │
        │  build)  │ │  Gin)    │ │  Worker) │
        └──────────┘ └────┬─────┘ └────┬─────┘
                           │            │
              ┌────────────┼────────────┤
              v            v
        ┌──────────┐ ┌──────────┐
        │PostgreSQL│ │  Redis   │
        │   :5432  │ │  :6379   │
        └──────────┘ └──────────┘
```

> MVP 仅 5 个容器（Nginx + Frontend 静态资源 + Backend API + Worker + PostgreSQL + Redis）。Backend 与 Worker 共用同一 Go 镜像，通过启动参数区分模式（`serve` / `worker`）。无 Milvus、无 LLM 服务。部署轻量，平台组可快速拉起。

---

## 5. API 端点清单（锁定——开发时以此为唯一依据）

> 认证：除 `/auth/login` 外所有接口需 Bearer Token（JWT Access Token）
> 统一响应格式：`{ "code": 0, "message": "success", "data": {...} }`
> 分页参数：`page`（从 1 开始）、`page_size`（默认 20，最大 100）
> 错误码：0=成功，400=参数错误，401=未认证，403=无权限，404=不存在，409=冲突，500=服务器错误

### 5.1 认证与用户（模块 F）

| Method | Path | 功能 | 认证 | 请求体 | 响应体 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| POST | `/api/v1/auth/login` | 登录 | 无 | `{username, password}` | `{access_token, refresh_token, user:{id,username,role}}` |
| POST | `/api/v1/auth/refresh` | 刷新 Token | Refresh Token | `{refresh_token}` | `{access_token}` |
| POST | `/api/v1/auth/logout` | 登出 | 是 | - | `{}` |
| GET | `/api/v1/auth/me` | 获取当前用户信息 | 是 | - | `{id, username, display_name, role, email}` |
| GET | `/api/v1/users` | 用户列表（管理员） | 是(admin) | query: `page, page_size, keyword, role` | `{items:[{id,username,display_name,role,email,status}], total}` |
| POST | `/api/v1/users` | 创建用户（管理员） | 是(admin) | `{username, password, display_name, role, email}` | `{id, username, ...}` |
| PUT | `/api/v1/users/{id}` | 更新用户 | 是(admin) | `{display_name?, role?, email?, status?}` | `{id, ...}` |
| PUT | `/api/v1/users/{id}/password` | 修改密码 | 是 | `{old_password, new_password}` | `{}` |
| GET | `/api/v1/roles` | 角色列表 | 是 | - | `{items:[{id,name,permissions:[]}]}` |

### 5.2 执行结果汇集与大盘（模块 E-Lite）

| Method | Path | 功能 | 认证 | 请求体 | 响应体 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| GET | `/api/v1/reports` | 报告列表 | 是 | query: `page, page_size, tool_id, result, date_from, date_to` | `{items:[{id, tool_id, tool_name, result, duration, executed_at, fail_detail?}], total}` |
| GET | `/api/v1/reports/{id}` | 报告详情 | 是 | - | `{id, tool_id, tool_name, tool_version, result, duration, fail_detail, evidence_url, executed_at, reported_at, reported_by}` |
| GET | `/api/v1/reports/export` | 导出报告 | 是 | query: `format(html/csv), tool_id?, date_from?, date_to?` | 文件流 |
| GET | `/api/v1/dashboard` | 质量大盘 | 是 | query: `period(week/month), date_from, date_to` | `{pass_rate_trend:[{date, rate}], tool_ranking:[{tool_id, name, owner, call_count, success_rate}], result_distribution:{pass, fail, error}, total_executions, active_tools}` |
| GET | `/api/v1/dashboard/summary` | 管理层摘要 | 是 | query: `period` | `{period, total_executions, avg_pass_rate, active_tools, highlight_tools:[{name, owner, call_count}], total_documents, total_tools}` |

### 5.3 知识库文档存档（模块 G-Lite）

| Method | Path | 功能 | 认证 | 请求体 | 响应体 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| POST | `/api/v1/knowledge/upload` | 上传文档 | 是 | multipart: `file, source, author, security_level(public/internal/confidential), tags[]` | `{document_id, status:processing}` |
| GET | `/api/v1/knowledge/documents` | 文档列表 | 是 | query: `page, page_size, status, source, security_level, keyword` | `{items:[{id, filename, source, author, status, security_level, file_size, created_at}], total}` |
| GET | `/api/v1/knowledge/documents/{id}` | 文档详情 | 是 | - | `{id, filename, source, author, status, security_level, file_size, content_text, tags[], metadata:{entities[], summary, word_count}, created_at, processed_at}` |
| GET | `/api/v1/knowledge/documents/{id}/download` | 下载原始文档 | 是 | - | 文件流 |
| DELETE | `/api/v1/knowledge/documents/{id}` | 删除文档 | 是 | - | `{}` |
| GET | `/api/v1/knowledge/documents/{id}/status` | 处理状态 | 是 | - | `{status(processing/ready/failed), progress, error?}` |
| POST | `/api/v1/knowledge/documents/{id}/reprocess` | 重新处理 | 是 | - | `{status:processing}` |
| GET | `/api/v1/knowledge/stats` | 知识库统计 | 是 | - | `{total_documents, total_size, by_type:{pdf, word, markdown, html}, by_security_level:{...}, recent_uploads:[{id, filename, created_at}]}` |

### 5.4 知识库全文搜索（模块 G-Search）

| Method | Path | 功能 | 认证 | 请求体 | 响应体 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| GET | `/api/v1/knowledge/search` | 全文搜索 | 是 | query: `q, page, page_size, source?, security_level?, type?` | `{items:[{document_id, filename, snippet(highlighted), score, source, security_level, created_at}], total, query_time_ms}` |
| GET | `/api/v1/knowledge/search/suggestions` | 搜索建议 | 是 | query: `q` | `{items:[{text, count}]}` |

> 全文搜索基于 PostgreSQL `tsvector` + `ts_rank` + `pg_trgm`（模糊匹配），无需向量数据库。

### 5.5 工具注册与接入（模块 J）

| Method | Path | 功能 | 认证 | 请求体 | 响应体 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| GET | `/api/v1/tools` | 工具列表 | 是 | query: `page, page_size, business_domain, type, owner, access_level, status` | `{items:[{id, name, business_domain, type, owner, repo_url, access_level, doc_url, current_level, status, created_at}], total}` |
| POST | `/api/v1/tools` | 注册工具 | 是 | `{name, business_domain, type(ui/api/data/env/other), repo_url, doc_url, description, access_level}` | `{id, tool_token, ...}` |
| GET | `/api/v1/tools/{id}` | 工具详情 | 是 | - | `{id, name, business_domain, type, owner, repo_url, doc_url, description, access_level, current_level, status, scan_status, created_at, stats:{execution_count, success_rate, last_execution_at}}` |
| PUT | `/api/v1/tools/{id}` | 更新工具信息 | 是 | `{name?, description?, doc_url?, access_level?}` | `{id, ...}` |
| DELETE | `/api/v1/tools/{id}` | 下线工具（保留历史数据） | 是 | - | `{}` |
| PUT | `/api/v1/tools/{id}/owner` | 变更负责人 | 是(admin) | `{new_owner_id}` | `{}` |
| POST | `/api/v1/tools/{id}/scan` | 触发安全扫描 | 是 | - | `{scan_id, status:running}` |
| GET | `/api/v1/tools/{id}/scan-result` | 安全扫描结果 | 是 | - | `{scan_id, status(passed/failed), issues:[{type, severity, file, line, description}], scanned_at}` |
| POST | `/api/v1/tools/report` | L2 结果上报（SDK 调用，Token 认证） | 是(tool_token) | `{tool_name, tool_version, business_domain, result(pass/fail/error), duration, fail_detail?, evidence_url?, executed_at}` | `{accepted: true, report_id}` |
| GET | `/api/v1/tools/{id}/executions` | 工具执行历史 | 是 | query: `page, page_size, status, date_from, date_to` | `{items:[{id, tool_version, result, duration, executed_at, fail_detail?}], total}` |
| GET | `/api/v1/tools/{id}/contributions` | 贡献统计 | 是 | query: `period` | `{tool_id, name, owner, call_count, success_rate, rank, trend:[{date, count}]}` |
| GET | `/api/v1/tools/suggestions` | 重复工具提示 | 是 | query: `keyword` | `{items:[{tool_id, name, similar_tools:[{id, name, similarity}], suggestion}]}` |
| POST | `/api/v1/tools/{id}/sdk-config` | 获取 L2 SDK 配置 | 是 | - | `{endpoint, token, retry_policy, batch_size}` |

### 5.6 审计日志（模块 F）

| Method | Path | 功能 | 认证 | 请求体 | 响应体 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| GET | `/api/v1/audit-logs` | 审计日志列表 | 是(admin) | query: `page, page_size, user, action, resource_type, date_from, date_to` | `{items:[{id, user, action, resource_type, resource_id, detail, ip, created_at}], total}` |

### 5.7 系统配置（模块 F）

| Method | Path | 功能 | 认证 | 请求体 | 响应体 |
| :--- | :--- | :--- | :--- | :--- | :--- |
| GET | `/api/v1/configs` | 配置列表 | 是(admin) | query: `category` | `{items:[{key, value, category, encrypted, description}]}` |
| PUT | `/api/v1/configs/{key}` | 更新配置 | 是(admin) | `{value}` | `{}` |
| GET | `/api/v1/health` | 健康检查 | 无 | - | `{status:healthy, services:{db:up, redis:up, worker:up}}` |

---

## 6. 数据库表清单（锁定）

### 6.1 核心表

| 表名 | 核心字段 | 索引 | 关联 |
| :--- | :--- | :--- | :--- |
| `users` | id(PK), username(UQ), password_hash, display_name, email, role(FK roles), status, created_at, updated_at | username(UQ), email, role | roles |
| `roles` | id(PK), name(UQ), permissions(JSONB) | name(UQ) | - |
| `registered_tools` | id(PK), name, business_domain, type(ui/api/data/env/other), owner_id(FK users), repo_url, doc_url, description(TEXT), access_level(L1/L2/L3/L4), status(active/inactive/offline), scan_status(none/passed/failed), scan_result(JSONB), tool_token_hash(UQ), created_at, updated_at | name, business_domain, type, owner_id, access_level, status | users |
| `tool_executions` | id(PK), tool_id(FK registered_tools), tool_version, result(pass/fail/error), duration(INT), fail_detail(TEXT), evidence_url, executed_at, reported_at, reported_by(FK users, nullable) | tool_id, result, executed_at | registered_tools, users |
| `knowledge_documents` | id(PK), filename, source, author, security_level(public/internal/confidential), status(processing/ready/failed), file_path, file_size, content_text(TEXT), content_tsv(TSVECTOR), tags(JSONB), metadata(JSONB), error_message(TEXT), created_at, processed_at | status, security_level, source, content_tsv(GIN) | - |
| `audit_logs` | id(PK), user_id(FK users), action, resource_type, resource_id, detail(JSONB), ip, created_at | user_id, action, resource_type, created_at | users |
| `system_configs` | id(PK), key(UQ), value(TEXT), category, encrypted(BOOL), description, updated_by(FK users), updated_at | key(UQ), category | users |

### 6.2 全文搜索索引

```sql
-- 知识库全文搜索：tsvector 列 + GIN 索引
ALTER TABLE knowledge_documents ADD COLUMN content_tsv TSVECTOR
  GENERATED ALWAYS AS (
    to_tsvector('simple', coalesce(filename, '') || ' ' || coalesce(content_text, ''))
  ) STORED;
CREATE INDEX idx_knowledge_content_tsv ON knowledge_documents USING GIN(content_tsv);

-- 模糊搜索：pg_trgm 扩展
CREATE EXTENSION IF NOT EXISTS pg_trgm;
CREATE INDEX idx_knowledge_filename_trgm ON knowledge_documents USING GIN(filename gin_trgm_ops);
CREATE INDEX idx_knowledge_content_trgm ON knowledge_documents USING GIN(content_text gin_trgm_ops);
```

> MVP 全文搜索完全基于 PostgreSQL，无需 Milvus 或向量数据库。V1.1 引入 AI 问答时再添加 `knowledge_chunks` 表和 Milvus Collection。

---

## 7. 页面清单（锁定）

| 页面 | 路由 | 核心组件 | 对应 API | 设计 Token 主题 |
| :--- | :--- | :--- | :--- | :--- |
| 登录 | `/login` | LoginForm | POST `/auth/login` | 居中卡片，品牌色背景 |
| 质量大盘 | `/dashboard` | SummaryCard x4, TrendChart, ResultPieChart, ToolRankingTable, RecentDocuments | GET `/dashboard`, GET `/dashboard/summary` | 浅色卡片网格 |
| 工具中心-列表 | `/tools` | ToolCard, DomainFilter, LevelBadge, ContributionRank, RegisterButton | GET `/tools` | 卡片网格布局 |
| 工具详情 | `/tools/:id` | ToolMetaCard, ScanResultPanel, ExecutionHistory, ContributionChart, SdkConfigGuide | GET `/tools/{id}`, GET `/tools/{id}/executions`, GET `/tools/{id}/contributions` | 顶部信息 + Tab 切换 |
| 执行结果-列表 | `/reports` | ReportTable, ResultFilter, ExportButton, DateRangePicker | GET `/reports` | 表格 + 状态色标 |
| 执行结果-详情 | `/reports/:id` | ReportSummary, EvidenceLink, FailDetailCard | GET `/reports/{id}` | 报告卡片布局 |
| 知识库-文档列表 | `/knowledge/documents` | DocumentTable, UploadDialog, StatusBadge, SecurityLevelTag, BulkActions | GET `/knowledge/documents`, POST `/knowledge/upload` | 表格 + 上传弹窗 |
| 知识库-文档详情 | `/knowledge/documents/:id` | DocumentViewer, MetaPanel, TagEditor, DownloadButton | GET `/knowledge/documents/{id}` | 左侧文档内容 + 右侧元数据 |
| 知识库-搜索 | `/knowledge/search` | SearchBar, SearchResultList, HighlightSnippet, FilterPanel | GET `/knowledge/search` | 搜索框 + 结果列表 |
| 用户管理 | `/settings/users` | UserTable, RoleSelect, CreateUserDialog | GET `/users`, POST `/users` | 表格 + 弹窗 |
| 审计日志 | `/settings/audit` | AuditLogTable, FilterBar | GET `/audit-logs` | 只读表格 + 筛选 |
| 系统配置 | `/settings/configs` | ConfigForm, EncryptToggle | GET `/configs`, PUT `/configs/{key}` | 表单分组布局 |

---

## 8. 设计 Token（锁定）

### 8.1 配色

| Token | 值 | 用途 |
| :--- | :--- | :--- |
| `--color-primary` | `#2563EB` | 主色（按钮、链接、选中态） |
| `--color-primary-hover` | `#1D4ED8` | 主色悬停 |
| `--color-primary-light` | `#EFF6FF` | 主色浅底（选中行背景） |
| `--color-success` | `#16A34A` | 通过/成功 |
| `--color-success-light` | `#F0FDF4` | 成功浅底 |
| `--color-warning` | `#EA580C` | 警告 |
| `--color-warning-light` | `#FFF7ED` | 警告浅底 |
| `--color-danger` | `#DC2626` | 失败/错误 |
| `--color-danger-light` | `#FEF2F2` | 失败浅底 |
| `--color-info` | `#0891B2` | 提示信息 |
| `--color-bg-page` | `#F8FAFC` | 页面背景 |
| `--color-bg-card` | `#FFFFFF` | 卡片背景 |
| `--color-bg-sidebar` | `#1E293B` | 侧边栏背景（深色） |
| `--color-text-primary` | `#0F172A` | 主文本 |
| `--color-text-secondary` | `#64748B` | 次要文本 |
| `--color-text-disabled` | `#CBD5E1` | 禁用文本 |
| `--color-text-inverse` | `#F8FAFC` | 侧边栏文本 |
| `--color-border` | `#E2E8F0` | 边框 |
| `--color-border-light` | `#F1F5F9` | 浅边框 |

### 8.2 字体

| Token | 值 | 用途 |
| :--- | :--- | :--- |
| `--font-family` | `'Inter', 'Noto Sans SC', sans-serif` | 全局字体 |
| `--font-size-xs` | `12px` | 辅助文字 |
| `--font-size-sm` | `13px` | 表格次要文字 |
| `--font-size-base` | `14px` | 正文 |
| `--font-size-lg` | `16px` | 标题 |
| `--font-size-xl` | `20px` | 页面标题 |
| `--font-size-2xl` | `24px` | 大标题 |
| `--font-weight-normal` | `400` | 正文 |
| `--font-weight-medium` | `500` | 表格表头 |
| `--font-weight-semibold` | `600` | 卡片标题 |
| `--font-weight-bold` | `700` | 页面标题 |

### 8.3 间距与圆角

| Token | 值 | 用途 |
| :--- | :--- | :--- |
| `--spacing-xs` | `4px` | 紧凑间距 |
| `--spacing-sm` | `8px` | 组件内间距 |
| `--spacing-md` | `12px` | 组件间间距 |
| `--spacing-lg` | `16px` | 卡片内间距 |
| `--spacing-xl` | `24px` | 区块间距 |
| `--spacing-2xl` | `32px` | 页面区域间距 |
| `--radius-sm` | `4px` | 按钮、输入框 |
| `--radius-md` | `6px` | 卡片 |
| `--radius-lg` | `8px` | 弹窗 |
| `--shadow-sm` | `0 1px 2px rgba(0,0,0,0.05)` | 轻阴影 |
| `--shadow-md` | `0 4px 6px rgba(0,0,0,0.07)` | 中阴影 |
| `--shadow-lg` | `0 10px 15px rgba(0,0,0,0.1)` | 重阴影 |

### 8.4 图标

| 规范 | 值 |
| :--- | :--- |
| 图标库 | Lucide（lucide-vue-next） |
| 行内图标 | `16px` |
| 按钮内图标 | `20px` |
| 独立图标 | `24px` |
| 描边宽度 | `1.5px`（全局一致） |
| 禁止 | 使用 emoji 作为功能图标 |

### 8.5 对标品牌

Linear（简洁布局） + Stripe（数据表格） + Notion（文档感知识库）

---

## 9. 验收标准（锁定——QA 测试时以此为唯一依据）

| 编号 | 功能 | EARS 格式验收标准 | 优先级 |
| :--- | :--- | :--- | :--- |
| AC-01 | 登录 | When 用户输入正确的用户名和密码，系统**必须**返回 JWT Access Token（15min 有效）和 Refresh Token（7d 有效） | P0 |
| AC-02 | 登录 | If 用户名或密码错误，系统**必须**返回 401 + "用户名或密码错误" | P0 |
| AC-03 | 登录 | If 连续登录失败 5 次，系统**必须**锁定该账户 15 分钟 | P0 |
| AC-04 | 权限 | While 用户角色为"测试工程师"，系统**必须**禁止访问用户管理和系统配置页面 | P0 |
| AC-05 | 工具注册 | When 成员登记工具元数据（名称、业务域、类型、负责人、仓库地址），系统**必须**在工具中心展示该工具并生成 L2 上报 Token | P0 |
| AC-06 | 工具注册 | When 新注册工具名称与已有工具相似度超阈值，系统**必须**提示重复工具列表供确认 | P0 |
| AC-07 | 工具接入 | When L2 工具通过 SDK 上报执行结果，系统**必须**将其纳入统一报告与质量大盘统计 | P0 |
| AC-08 | 工具接入 | If L2 上报请求失败，系统**必须**允许 SDK 侧重试且不阻塞工具自身运行 | P0 |
| AC-09 | 工具接入 | When 工具负责人获取 SDK 配置，系统**必须**返回 endpoint、token、重试策略和批量大小配置 | P0 |
| AC-10 | 工具安全 | When 工具申请上架，系统**必须**验证三个准入条件：Git 托管 + 敏感信息扫描通过 + 指定负责人；缺一不予上架 | P0 |
| AC-11 | 工具安全 | When 工具下线，系统**必须**保留历史执行数据与文档存档 | P0 |
| AC-12 | 执行结果 | When 用户查看执行结果列表，系统**必须**支持按工具、结果、时间范围筛选 | P0 |
| AC-13 | 执行结果 | When 用户导出报告，系统**必须**生成 HTML 或 CSV 格式文件 | P0 |
| AC-14 | 质量大盘 | When 管理层查看大盘，系统**必须**展示通过率趋势、工具调用排行、结果分布 | P0 |
| AC-15 | 知识库 | When 用户上传 PDF/Word/Markdown/HTML 文档，系统**必须**自动解析提取全文内容并标注元数据 | P0 |
| AC-16 | 知识库 | When 文档处理完成，系统**必须**标注元数据（来源、作者、时间戳、密级） | P0 |
| AC-17 | 知识库 | When 用户查看文档详情，系统**必须**展示文档内容、元数据和支持原始文件下载 | P0 |
| AC-18 | 全文搜索 | When 用户输入关键词搜索，系统**必须**返回匹配的文档列表，结果中高亮关键词 | P0 |
| AC-19 | 全文搜索 | When 搜索无匹配结果，系统**必须**明确显示"未找到相关文档" | P0 |
| AC-20 | 全文搜索 | When 用户按来源或密级筛选，系统**必须**仅返回符合筛选条件的搜索结果 | P0 |
| AC-21 | 审计 | When 任何用户执行工具注册变更、知识库增删、报告查看操作，系统**必须**记录完整审计日志 | P0 |
| AC-22 | 安全 | When 敏感配置（密码、Token、凭据）存储，系统**必须**加密落库，界面展示脱敏 | P0 |
| AC-23 | 贡献追溯 | When 查看工具贡献统计，系统**必须**展示调用次数、成功率、排名及趋势 | P0 |

---

## 10. 边界与约束

- 浏览器支持：Chrome 90+、Edge 90+、Firefox 88+；不支持 IE
- 响应式断点：`1280px`（桌面主目标）、`1024px`（平板）、`768px`（手机仅支持只读查看大盘）
- 性能目标：
  - API P95 延迟 ≤ 500ms
  - 全文搜索 P95 延迟 ≤ 200ms（PostgreSQL tsvector + GIN 索引）
  - 文档解析异步处理，不阻塞 API 响应
  - 报告导出 CSV ≤ 5s
- 单文件限制：后端单文件 ≤ 300 行业务逻辑；前端单组件 ≤ 300 行
- 安全约束：
  - JWT Access Token 15min，Refresh Token 7d
  - 密码 bcrypt 加盐（cost=12）
  - 敏感配置 AES-256 加密存储
  - CORS 仅允许前端域名
  - SQL 注入防护：GORM 参数化查询 + 输入校验
  - XSS 防护：Vue 模板自动转义 + CSP Header
- L2 上报约束：
  - 上报 Token 独立于用户 JWT，仅限 `/api/v1/tools/report` 端点
  - 上报失败自动重试最多 3 次，间隔指数退避
  - 支持批量上报（单次最多 50 条）

---

## 11. 内嵌已知坑

> 项目为新建，暂无历史踩坑记录。以下为技术栈已知风险项。

| 坑 | 技术栈指纹 | 根因 | 修法 |
| :--- | :--- | :--- | :--- |
| asynq 任务不执行 | asynq-0.25 + redis-7.2 | broker 地址配置错误或 Redis 未启动 | 确认 `REDIS_ADDR=redis:6379`，Docker Compose 网络连通，Worker 进程已启动 |
| Vue 3 Element Plus 样式冲突 | vue-3.4 + element-plus-2.7 | 全局样式覆盖顺序不对 | Element Plus 样式在自定义 CSS 之前引入 |
| GORM 连接池耗尽 | gorm-2.0 + postgresql-16 | 高并发下未调优连接池或未关闭 rows | 设置 `db.DB().SetMaxOpenConns(25)`、`SetMaxIdleConns(10)`；查询后 `rows.Close()` |
| GORM AutoMigrate 误删列 | gorm-2.0 | AutoMigrate 只增不删，但生产环境不可控 | 生产环境禁用 AutoMigrate，使用 golang-migrate 显式管理迁移脚本 |
| PostgreSQL 全文搜索中文分词 | postgresql-16 | 默认 `simple` 分词器对中文按字切分，效果有限 | MVP 先用 `simple` + `pg_trgm` 模糊匹配兜底；V1.1 引入 `zhparser` 扩展或迁移至向量检索 |
| godocx 表格解析丢失 | godocx | 复杂嵌套表格解析不完整 | 解析时遍历 `document.Tables`，表格内容追加到正文 |
| pdfcpu 大文件解析阻塞 | pdfcpu | 超大 PDF（100MB+）解析阻塞 goroutine | asynq 异步处理 + 文件大小限制 50MB + context 超时控制 |
| Go goroutine 泄漏 | go-1.22 | 文档解析协程未正确退出或未设超时 | 使用 `context.WithTimeout` + `errgroup` 管理协程生命周期 |

---

## 12. 端到端验证步骤（Spec 锁定的最后一项）

```bash
# ============ 环境启动 ============
# 1. 启动全部服务
docker-compose up -d

# 2. 等待服务就绪
curl -s http://localhost:8080/api/v1/health
# 断言：返回 {"code":0, "data":{"status":"healthy"}}

# ============ 核心成功流 ============
# 3. 登录获取 Token
TOKEN=$(curl -s -X POST http://localhost:8080/api/v1/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username":"admin","password":"admin123"}' | jq -r '.data.access_token')
# 断言：TOKEN 非空

# 4. 注册工具
TOOL_ID=$(curl -s -X POST http://localhost:8080/api/v1/tools \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"name":"订单接口测试工具","business_domain":"order","type":"api","repo_url":"https://git.company.com/test/order-tool","doc_url":"https://wiki.company.com/order-tool","description":"订单模块接口自动化测试"}' \
  | jq -r '.data.id')
# 断言：返回 tool_id 非空，access_level=L1，tool_token 非空

# 5. 获取 SDK 配置
curl -s -X POST http://localhost:8080/api/v1/tools/$TOOL_ID/sdk-config \
  -H "Authorization: Bearer $TOKEN"
# 断言：返回 endpoint, token, retry_policy, batch_size

# 6. L2 结果上报
curl -s -X POST http://localhost:8080/api/v1/tools/report \
  -H "Authorization: Bearer <tool_token>" \
  -H "Content-Type: application/json" \
  -d '{"tool_name":"订单接口测试工具","tool_version":"1.0.0","business_domain":"order","result":"pass","duration":1200,"executed_at":"2026-07-24T12:00:00Z"}'
# 断言：返回 accepted=true, report_id 非空

# 7. 查看执行结果
curl -s http://localhost:8080/api/v1/reports?tool_id=$TOOL_ID \
  -H "Authorization: Bearer $TOKEN"
# 断言：列表包含刚上报的结果

# 8. 查看质量大盘
curl -s http://localhost:8080/api/v1/dashboard \
  -H "Authorization: Bearer $TOKEN"
# 断言：tool_ranking 包含已注册工具，total_executions >= 1

# 9. 上传知识文档
curl -s -X POST http://localhost:8080/api/v1/knowledge/upload \
  -H "Authorization: Bearer $TOKEN" \
  -F "file=@docs/test-guide.md" \
  -F "source=测试组" \
  -F "author=admin" \
  -F "security_level=internal"
# 断言：返回 document_id，status=processing

# 10. 等待文档处理完成，查看详情
curl -s http://localhost:8080/api/v1/knowledge/documents/<document_id> \
  -H "Authorization: Bearer $TOKEN"
# 断言：status=ready，content_text 非空，metadata 包含来源/作者/密级

# 11. 全文搜索
curl -s "http://localhost:8080/api/v1/knowledge/search?q=测试指南" \
  -H "Authorization: Bearer $TOKEN"
# 断言：返回匹配文档，snippet 包含高亮关键词

# ============ 关键错误流 ============
# 12. 全文搜索无结果
curl -s "http://localhost:8080/api/v1/knowledge/search?q=量子力学超弦理论" \
  -H "Authorization: Bearer $TOKEN"
# 断言：total=0，返回"未找到相关文档"

# 13. 无权限访问
curl -s http://localhost:8080/api/v1/users \
  -H "Authorization: Bearer <tester_token>"
# 断言：返回 403

# 14. 工具准入安全扫描失败
curl -s -X POST http://localhost:8080/api/v1/tools/$TOOL_ID/scan \
  -H "Authorization: Bearer $TOKEN"
# 断言（如仓库含硬编码密码）：scan_status=failed, issues 非空
```

---

## 13. 变更记录

| 日期 | 变更内容 | 原因 | 影响范围 |
| :--- | :--- | :--- | :--- |
| 2026-07-24 | Spec v1.0 初版生成 | 基于 PRD V1.0 生成开发契约 | 全文 |
| 2026-07-24 | Spec v1.1：收窄 MVP 范围 | 用户决策：AI 不进 MVP；MVP 聚焦工具注册+报告上报+知识存档+全文搜索。移除用例管理/任务调度/执行引擎/向量检索/LLM/RAG 问答，技术栈去掉 Milvus/BGE-M3/LangChain/DeepSeek/Playwright，改用 PostgreSQL 全文检索 | MVP 范围、技术架构、API、DB、页面、验收标准、E2E |
| 2026-07-24 | Spec v1.2：后端技术栈从 Python 切换为 Go | 用户决策：与现有 Go 代码库保持一致；Flask→Gin、SQLAlchemy→GORM、Celery→asynq、python-docx/PyMuPDF→pdfcpu/godocx、Gunicorn 移除（Go 内置 HTTP）、端口 5000→8080；已知坑同步更新为 Go 技术栈风险项 | 技术架构、部署架构、边界约束、已知坑、E2E 验证 |
