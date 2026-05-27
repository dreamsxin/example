# SDD-范例

---

## 项目结构总览

```
your-project/
├── CONVENTIONS.md           # 项目级宪法（根目录，Codex 首要读取）
├── .codex/
│   ├── RULES.md             # Codex 指令入口
│   ├── SPEC.md              # 当前功能规范（按需替换）
│   └── MEMORY.md            # 项目决策记忆
├── web/                     # 前端 Vue 项目
│   ├── src/
│   │   ├── api/             # 接口请求层
│   │   ├── components/      # 公共组件
│   │   ├── views/           # 页面视图
│   │   ├── router/          # 路由配置
│   │   ├── stores/          # Pinia 状态管理
│   │   └── utils/           # 工具函数
│   └── package.json
├── server/                  # 后端 Go 项目
│   ├── cmd/                 # 入口文件
│   ├── internal/
│   │   ├── handler/         # HTTP 处理器（路由层）
│   │   ├── service/         # 业务逻辑层
│   │   ├── repository/      # 数据访问层（Gorm）
│   │   ├── model/           # 数据模型
│   │   ├── middleware/      # 中间件（JWT、CORS 等）
│   │   └── dto/             # 数据传输对象
│   ├── config/              # 配置文件
│   └── go.mod
└── README.md
```

---

## 文件一：`CONVENTIONS.md`（项目宪法）

直接复制到项目根目录：

```text
# 项目编码规范

## 项目概述
- 项目类型: 后端管理系统
- 前端: Vue 3 + TypeScript + Element Plus + Pinia
- 后端: Go 1.21+ + Gin + Gorm (MySQL)
- 协作模式: 前后端分离，RESTful API

---

## 一、前端规范 (Vue 3)

### 1.1 技术栈约束
- Vue 3 Composition API（`<script setup lang="ts">`）
- 状态管理: Pinia（禁止使用 Vuex）
- UI 框架: Element Plus（禁止混用其他 UI 库）
- HTTP 请求: Axios（统一封装在 `web/src/api/` 中）
- 路由: Vue Router 4

### 1.2 目录与命名规范
- 页面组件放在 `views/`，命名采用 PascalCase（如 `UserList.vue`）
- 公共组件放在 `components/`，命名采用 PascalCase
- API 请求函数放在 `api/`，按模块分文件（如 `api/user.ts`）
- Store 放在 `stores/`，按模块分文件（如 `stores/user.ts`）
- 文件名: 一律小写加连字符，组件内部使用 PascalCase

### 1.3 编码规范
- 所有 API 请求必须通过 `api/` 层的封装函数调用，页面中禁止直接使用 axios
- 所有异步操作必须有错误处理（try-catch 或 `.catch()`）
- 表单必须包含输入校验（Element Plus Form Validation）
- 组件 props 必须明确定义类型和默认值
- 使用 TypeScript 严格模式，禁止使用 `any`（除非有充分理由并注释说明）

### 1.4 样式规范
- 使用 Scoped CSS（`<style scoped>`）
- 禁止使用 `!important`
- 间距使用 Element Plus 内置的 spacing 体系

---

## 二、后端规范 (Go/Gin/Gorm)

### 2.1 架构约束（三层分离）
严格遵循以下分层，禁止跨层调用：
``` ``` ```
handler（路由层） → service（业务逻辑层） → repository（数据访问层）
``` ``` ```
- **handler**: 仅负责请求参数绑定、调用 service、返回响应
- **service**: 包含所有业务逻辑，不得直接操作 `*gorm.DB`
- **repository**: 封装所有数据库查询，只接受和返回 model 或 dto

### 2.2 目录规范
- 入口文件: `cmd/main.go`
- 数据模型: `internal/model/`（每个文件一个 model，文件名与表名对应）
- DTO: `internal/dto/`（请求体和响应体的结构定义）
- 错误码: 统一在 `internal/dto/errors.go` 中定义

### 2.3 命名规范
- 文件名: 小写加下划线（如 `user_repository.go`）
- 结构体: PascalCase（如 `UserService`）
- 方法/函数: PascalCase（导出）, camelCase（非导出）
- 接口: 以 `Interface` 结尾（如 `UserRepositoryInterface`）

### 2.4 编码规范
- 所有数据库操作必须使用 Gorm 的参数化查询，禁止拼接 SQL
- 所有 handler 必须对输入参数进行校验（使用 Gin 的 `binding:"required"` 等标签）
- 错误处理: 不得忽略任何 error，必须向上传递或记录日志
- 日志: 使用 `logrus` 或 `zap`，关键操作必须记录日志
- 配置: 所有配置通过 `config/` 目录下的 YAML 文件管理，禁止硬编码
- 数据库迁移: 使用 Gorm AutoMigrate，禁止手动执行 SQL DDL

### 2.5 API 规范
- RESTful 风格: GET(查), POST(增), PUT(改), DELETE(删)
- URL 格式: `/api/v1/资源名`（如 `/api/v1/users`）
- 统一响应格式:
  ``` ``` ```json
  {
    "code": 0,
    "message": "success",
    "data": {}
  }
  ``` ``` ```
- 分页查询参数: `page`, `page_size`，响应中包含 `total`, `list`
- JWT 认证: 除登录/注册外，所有接口必须经过 JWT 中间件校验

### 2.6 测试规范
- service 层必须编写单元测试（使用 `testify`）
- handler 层编写集成测试（使用 `httptest`）
- 测试文件以 `_test.go` 结尾，与源文件同目录

---

## 三、安全约束
- 密码: 使用 bcrypt 加密存储，禁止明文或简单哈希
- JWT secret: 从环境变量或配置文件读取，禁止硬编码
- 用户输入: 前后端都必须校验，后端为主
- SQL 注入: 必须使用 Gorm 参数化查询
- CORS: 仅允许明确配置的前端域名

---

## 四、禁止事项
- 禁止在 handler 中直接写业务逻辑
- 禁止在 service 中直接操作 `*gorm.DB`
- 禁止使用 `any` 类型（TypeScript）
- 禁止忽略 error（Go）
- 禁止引入规范未提及的技术栈（如突然引入 Redis 但不更新本文件）
- 禁止跳过测试编写
```

---

## 文件二：`.codex/RULES.md`（Codex 指令入口）

```markdown
# Codex 工作规则

## 启动规则（每次会话开始时强制执行）
1. **立即读取**项目根目录下的 `CONVENTIONS.md`，并严格遵守其中所有规范
2. **立即读取** `.codex/MEMORY.md`（如果存在），了解已做出的技术决策
3. **立即读取** `.codex/SPEC.md`，了解当前要开发的功能任务

## 工作规则
- 严格遵守 `CONVENTIONS.md` 中定义的所有分层、命名、编码规范
- 编码前，先列出实施计划（受影响的文件清单和修改内容）供我确认
- 每完成一个独立功能，提示我进行测试验证
- 如需做出任何技术决策（如引入新依赖、调整架构），必须先询问我确认
- 确认后的决策，记录到 `.codex/MEMORY.md`

## 禁止行为
- 不得修改规范文件（`CONVENTIONS.md`, `RULES.md`, `SPEC.md`），除非明确要求
- 不得引入 `CONVENTIONS.md` 中未提及的新依赖或框架
- 不得跳过三层架构中的任何一层
- 不得在 handler 中编写业务逻辑
- 不得忽略错误处理

## 输出规范
- 代码注释使用中文
- 每个导出函数/方法必须有注释说明用途
- 复杂逻辑必须有行内注释解释
```

---

## 文件三：`.codex/SPEC.md`（功能规范模板）

以下是一个示例，代表“用户管理”功能。**每次开发新功能时替换此文件内容**。

```text
# 功能规范: 用户管理模块

## 功能描述
实现后台管理系统的用户增删改查功能，支持分页、搜索和状态管理。

## 用户故事
- 作为管理员，我希望查看所有注册用户的列表，以便管理用户
- 作为管理员，我希望创建新用户，以便添加系统成员
- 作为管理员，我希望编辑用户信息，以便更新用户资料
- 作为管理员，我希望禁用/启用用户账号，以便控制访问权限

## 验收标准
- [ ] 用户列表页支持分页展示，每页默认 10 条
- [ ] 支持按用户名、邮箱搜索用户
- [ ] 创建用户时，必填项为用户名、邮箱、密码、角色
- [ ] 编辑用户时，不允许修改邮箱（唯一标识）
- [ ] 禁用用户后，该用户无法登录
- [ ] 删除用户为软删除（设置 deleted_at 时间戳）

## 数据模型 (model/user.go)
``` ``` ```go
type User struct {
    ID        uint           `gorm:"primarykey" json:"id"`
    Username  string         `gorm:"uniqueIndex;size:50;not null" json:"username"`
    Email     string         `gorm:"uniqueIndex;size:100;not null" json:"email"`
    Password  string         `gorm:"size:255;not null" json:"-"`
    Role      string         `gorm:"size:20;default:user" json:"role"`
    Status    int            `gorm:"default:1" json:"status"`  // 1:启用, 0:禁用
    CreatedAt time.Time      `json:"created_at"`
    UpdatedAt time.Time      `json:"updated_at"`
    DeletedAt gorm.DeletedAt `gorm:"index" json:"deleted_at"`
}
``` ``` ```

## API 设计

| 方法 | 路径 | 说明 | 权限 |
|:---|:---|:---|:---|
| GET | /api/v1/users | 获取用户列表（分页+搜索） | 管理员 |
| POST | /api/v1/users | 创建新用户 | 管理员 |
| PUT | /api/v1/users/:id | 更新用户信息 | 管理员 |
| PUT | /api/v1/users/:id/status | 启用/禁用用户 | 管理员 |
| DELETE | /api/v1/users/:id | 删除用户（软删除） | 超级管理员 |

## 任务清单
### 后端
1. 创建 `User` model
2. 创建 `UserRepository`（CRUD + 分页查询）
3. 创建 `UserService`（业务逻辑 + 密码加密）
4. 创建 `UserHandler`（5 个 API 端点）
5. 注册路由到 `router.go`
6. 编写 `UserService` 单元测试
7. 编写 `UserHandler` 集成测试

### 前端
1. 创建 `api/user.ts`（API 请求封装）
2. 创建 `views/user/UserList.vue`（列表页 + 搜索 + 分页）
3. 创建 `views/user/UserForm.vue`（创建/编辑表单弹窗）
4. 配置路由 `/users`
5. 接入 Pinia store（如需要跨组件共享用户状态）
```

---

## 文件四：`.codex/MEMORY.md`（初始为空）

```text
# 项目记忆

## 技术决策记录
_（此文件由 Codex 在每次做出重要技术决策后自动追加，初始为空）_

---
```

---

## 在 Codex 中的启动语

以后每次在 Codex 中打开这个项目，用这句话作为开场：

> **“请读取 `.codex/RULES.md` 并严格执行其中的所有指令。”**

Codex 会依次加载 `CONVENTIONS.md` → `MEMORY.md` → `SPEC.md`，然后告诉你它已理解规范，可以开始工作。

---
