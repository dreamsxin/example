# Teambition MCP 接入

> **用法**：整段粘贴给 agent。使用前把 `<TOKEN>` 替换成你的 userToken。

---

## 0. 背景与角色

你要通过 Teambition 开放平台的 MCP 接口维护我的项目任务。

- **我是谁**：该 token 属于我个人账号，`GetUsersMe` 可查（姓名 / 工号 / userId）。
- **目标**：建任务、拆子任务、改状态与时间、记工时，并能**按员工统计工时**。
- **多租户**：一个 userToken 只绑定一个企业（tenant）。跨企业必须换 token，
  不要试图用一个 token 访问另一个企业的资源——会返回 `10009`。
- **权限边界**：token 是我的个人权限。不要以我的名义修改别人名下的任务，
  除非我明确要求；需要他人协作时用「参与人 / 共同执行人」而非「执行人」。
- **不许猜**：项目 ID、阶段 ID、任务类型 ID、状态 ID、成员 ID 一律现场查出来，
  不要从任务标题、阶段名称或上下文推测，也不要沿用历史会话里的旧值。

---

## 1. 连接信息

```json
{
  "mcpServers": {
    "teambition-mcp": {
      "url": "https://open.teambition.com/api/mcp?userToken=<TOKEN>",
      "transport": "http",
      "description": "Teambition project & task management via official open platform MCP endpoint. Use for querying/summarizing tasks in projects and sprints, batch creating tasks, and updating existing tasks (status, due date, assignee, stage). Covers projects, task lists, tasks, subtasks and members."
    }
  }
}
```

注意：

- `description` 字段**必填**，安装器会校验，缺了直接报错。
- token 是 URL query 参数，会进入网关访问日志，且明文存在本地 `mcp.json` 里。
  不要把 token 写进代码仓库、文档、提交信息或额外扩散到对话里。
- 若需同时接多个企业，装成**多个独立 skill**（不同 server key），
  不要反复改同一个 `mcp.json` 后 regenerate —— 容易搞混当前连的是哪个企业。
- **每次会话开工第一件事是 `GetOrgInfo`**，确认当前 token 指向哪个企业，
  再开始任何读写。

---

## 2. 安装步骤（按序执行，勿跳步）

1. **安装**：用 `mcp-installer` skill

   ```bash
   bash scripts/mcp_to_skill.sh '<第 1 节的 JSON>'
   ```

   成功标志：输出 `Discovered N tool(s)`（该 endpoint 实测约 149 个）并生成独立 skill 目录。
   安装后 skill 目录内所有操作走 `bash run.sh`（见下）。

2. **调用入口固定是 `run.sh`**，不要直接 `python cli.py`（缺 `cyclopts` 等依赖会报错）：

   ```bash
   bash run.sh list-tools
   bash run.sh call-tool <ToolName> --some-flag <value>
   ```

3. **所有参数名是 kebab-case**：`--page-size`、`--project-id`、`--task-id`、
   `--request-body`。传驼峰会报 `Unknown option: --pageSize. Did you mean --page-size?`

4. **改了 `mcp.json` 必须 regenerate**，否则 `run.sh` 因 hash 校验拒绝执行：

   ```bash
   bash scripts/manage.sh regenerate
   ```

5. **临时文件放在工作目录，不要放 `/tmp`**。沙箱会不定期清空 `/tmp`
   （实测在一次会话内被清空两次，辅助脚本与已拉取的数据全部丢失）。
   建议 `<workdir>/.tbtmp/`。

6. **先装一个容错 JSON 解析器**再处理任何返回体（原因见第 4 节「非法转义」）。
   实现要点：按「转义对」逐个判定，`\` 后不是 `"`\`/bfnrtu` 的才补成 `\\`；
   不能全局替换反斜杠，否则会破坏合法的 `\\` 序列。

---

## 3. 操作规范（必须遵守）

1. **先取样再动手**。在一个项目里建任务前，先读同阶段 3~5 条既有任务，
   照抄它们的 `tasklistId` / `stageId` / `sfcId` / `tfsId` / `priority` / `visible`，
   保证新任务与存量任务口径一致。
2. **新建任务默认执行人设为我本人**，不擅自指派他人；需要协作就加参与人。
3. **每次写入后必须回读校验**，不要相信返回码。本接口集里存在
   「返回 204 / 200 但实际没写进去」的静默失败（见第 4 节）。
4. **批量操作前列清单**：把每条 `taskId` 与变更前后值列出来给我确认，确认后再执行。
5. **不改、不删别人名下的任务**。要推动他人时用评论或加参与人，并先问过我。
6. **归档 / 删除属不可逆操作**：先把该任务的 `note`、`stageId`、`sfcId`、`tfsId`
   完整存快照再动手（`RestoreTaskV3` 恢复时必须要这几个字段）。
7. **改动要能对上账**。每次操作后核对数量变化（如阶段内任务总数）。
   发现多出来的记录，要查清是同事并发新建、平台自动化触发，还是自己重复建了。
8. **我和你在 Teambition 里用的是同一个账号**，`ListTaskActivitiesV3` 的 `creatorId`
   无法区分「我在界面上手动改的」和「你通过 API 改的」。
   所以：凡不是由你发起的变更，直接告诉我它存在，**不要认领、也不要臆测原因**。
9. **不要重复创建**。如果上一次调用的显示层/解析层报错，先查这条数据到底写没写进去，
   再决定是否重试——盲目重试会造成重复任务。
10. 接口报错就把原始 `code` / `errorCode` / `errorMessage` 报给我，不要静默换路径。

---

## 4. 接口陷阱清单（实测，避免重复踩）

| # | 现象（实测错误码） | 结论 / 正确做法 |
|---|---|---|
| 1 | `QueryProjectsV3` 返回的项目数比实际少 | 该接口**按成员可见性过滤**。全租户枚举用 `SearchProjectsTQL --tql 'isArchived = false'`（返回 ID 数组），再用 `QueryProjectsV3 --project-ids` 批量补详情 |
| 2 | `10009 Resource not belong to tenant` | 该资源不属于当前 token 的企业。换 token，或确认 ID 抄错 |
| 3 | `SearchDoc` 任意关键词均返回 `421 MisdirectedRequest` | **该接口不可用**，别指望用它查文档。参数格式改从 `cli.py` 里 `cyclopts.Parameter(help=...)` 内嵌的 JSON Schema 读取 |
| 4 | `QueryAllTasksV3` 传 `--task-id` / `--parent-task-id` 都 `404 (10133)` | 该工具不可用。改用 **`QueryTaskV3`**：`--task-id` 支持逗号分隔批量（实测一次数十条正常），`--parent-task-id` 可列子任务 |
| 5 | TQL 用 `LIKE` → `10028 not support expression`；用 `ancestorIds` → `10028 invalid field name` | TQL **不支持 LIKE、不支持 ancestorIds**。`uniqueId` 可查。`SearchTasksByTQLV2` 的 `projectId` **必须写进 tql 字符串**，不能作为独立参数 |
| 6 | `SearchProjectTasksV3` 翻页取回同一批、顺序不按时间 | 分页不可靠。取全量 = `SearchTasksByTQLV2` 拿 ID → `QueryTaskV3` 批量补详情 |
| 7 | `json.loads` 抛 `Invalid \escape` | 返回体 `note` 字段可能含**非法 JSON 转义**（`\[`、`\_` 未按规范双写）。需按「转义对」逐个修复；全局替换反斜杠会破坏合法的 `\\` |
| 8 | `ArchiveTaskV3` 返回 `code 0 / 未知错误码: SyntaxError / System error`，重试无效 | **服务端 bug**（`cli.py` 发送的 `{'taskId':…}` 格式是对的）。替代：`DeleteTaskV3`（同样是移入回收站）；恢复用 `RestoreTaskV3`，需回传 `stageId`/`sfcId`/`tfsId` |
| 9 | `UpdateTaskParentV3` 报 `targetId is required` | 改父任务的 body 字段是 **`targetId`**；但 `CreateTaskV3` 建子任务用的却是 **`parentTaskId`** —— 两个接口字段名不一致，别混用 |
| 10 | `UpdateTaskCustomField`（不带 V3）返回 `204`，但字段没写进去、活动流水也无事件 | **静默失败**。真正生效的是 `UpdateTaskCusomFieldV3`（官方拼写少个 `t`），返回 `200` 并带 `updated` 时间戳，body 形如 `{customfieldId, type, value:[{id}]}` |
| 11 | `AggregateUserPlanTime` 报 `body should have required property '_subscriberId'` | **userToken 通道不可用**：`_subscriberId` 是隐藏字段，放 requestBody 内、与 requestBody 平级传都无效；扁平传参会把服务端打出 `500` 并断开会话。按人聚合工时只能自己遍历任务 |
| 12 | `GetUsersMe` 返回体含 `accessCode` | 是**凭证类字段**。能不调就不调；调用后不要把该值扩散 |
| 13 | `SearchProjectTagsV3` 可能返回 `Forbidden`（`resource(project.tag) action(8)`） | 取决于该 MCP 应用被授予的权限，标签能力未必可用。遇到就绕开，别反复重试 |
| 14 | 按团队/岗位名（如「研发组」「测试组」）去 `SearchTaskGroupsV3` 找不到 | **阶段(stage) ≠ 任务分组(task group)**。分组查出来的是 tasklist（常只有一个默认分组）；这类团队名通常在 `SearchStagesV3` 返回的 stage 里 |
| 15 | `--include-template true` 报 `Unused Tokens: ['true']` | boolean 型 flag 不接受值，直接给开关名 |
| 16 | 同一项目内**多套工作流、状态大量重名**（如两个「已完成」、两个「未完成」，tfsId 不同） | **不要按状态名猜 tfsId**。必须从同任务类型（同 `sfcId`）的存量任务上抄实际在用的那个；分「未完成」「已完成」两组各取样 |

**另外两条环境类坑**（非 Teambition 接口问题，但会重复踩）：

- `/tmp` 会被沙箱不定期清空 → 辅助脚本与中间数据放 `<workdir>/.tbtmp/`
- 时间字段一律存 **UTC**，界面显示当地时间 → 读写时要显式换算
  （例：北京时间当日 18:00 = `T10:00:00.000Z`），报告里必须写明用的是哪个时区

---

## 5. 工时体系

**两类工时**：`CreatePlanTime`（计划/排期）、`CreateWorkTime`（实际/填报）。
**考核看实际工时**，计划工时只是排期，两者必须分开统计、分开出表。

**单位是毫秒**：`1h = 3600000`，`8h = 28800000`。

**`CreatePlanTime` 必填字段**：

| 字段 | 含义 | 备注 |
|---|---|---|
| `userId` | **工时执行人**（工时算给谁） | 关键字段，见下方结论 1 |
| `submitterId` | 工时提交人 | |
| `objectId` | 任务 ID | |
| `objectType` | 固定 `"task"` | |
| `plantime` | 计划工时数 | 毫秒 |
| `startDate` / `endDate` | 工时时间段 | **日期串 `'YYYY-MM-DD'` 实测可用**；落库后记录的 `date` 变为当天 `T00:00:00.000Z` |
| `isDuration` | 跨天填 1，当天填 0 | |
| `includesHolidays` | 1=周六日也填，0=系统跳过周末顺延周一 | |

`CreateWorkTime` 在上述基础上：`worktime`（毫秒）+ **`description`（工作进展，必填）**，另有可选 `tagIds`。

### 三条已验证的关键结论

1. **一条任务可以给多人分别记工时**，因为工时记录自带 `userId`。
   → **不要为了区分人而拆子任务**。拆子任务的正确粒度是「工作项 / 交付物」；
   按人拆会让工时数据碎片化、任务树膨胀。
2. **汇总字段会向上累加，且不重复计数**：
   实测父任务「计划工时(含子任务)」= 自身工时 + 所有子孙工时（父 8h + 子 8h = 16h）。
   → 父任务上直接记工时是安全的，不会造成重复统计。
3. **`ListTaskPlanTimes` / `SumTaskPlanTime` 只统计直接挂在本任务上的记录**，
   在父任务上返回空数组。
   → **做报表必须读自定义字段**（`计划工时(含子任务)` / `实际工时(含子任务)`）。
   读那两个接口会把所有父任务算成 0，这是最容易出的报表错误。

### 工时相关自定义字段怎么找

这些字段的 `cfId` **因企业/项目而异，不可硬编码**。用下面命令现场查：

```bash
bash run.sh call-tool SearchProjectCustomFiledsV3 --project-id $P --page-size 100
```

按字段名识别，通常是这 6 个的一组：

| 字段名 | 类型 | 值的单位 |
|---|---|---|
| 计划工时（含子任务） | lookup2 | 毫秒 |
| 实际工时（含子任务） | lookup2 | 毫秒 |
| 计划工时 | lookup2 | 分钟 |
| 实际工时 | lookup2 | 分钟 |
| 计划工时（number 变体） | number | 分钟 |
| 实际工时（number 变体） | number | 分钟 |

值都存在 `customfields[].value[0].title` 里（字符串数字），单位看 `metaString` 里的 `unit`。
另外常有 `共同执行人`（type=lookup，值是 userId）—— 它是**独立字段，不会从
`involveMembers` 推导**，要协作就得显式写。

### 建议的填报规范

- 工时**只记在最底层叶子任务**，上层只看「（含子任务）」汇总字段；
- 每天收工前记一次**实际工时** + `description`；
- 按员工出报表 = 遍历任务调 `ListTaskWorkTimes` / `ListTaskPlanTimes`，
  按 `userId` 聚合（成本约 2 次调用/任务）。`AggregateUserPlanTime` 用不了，见陷阱 #11。

### ⚠️ 出报表前先量一次覆盖率

工时报表最常见的失败不是技术问题，而是**没人填**。动手做汇总前，先抽样统计：
项目内有多少比例的任务带非零工时字段、实际工时是否全为 0。
如果覆盖率极低，先告诉我实际情况并建议「立填报规范 + 巡检催填」，
不要把一张接近空白的表当成成果交付。

---

## 6. 现场探测：如何拿到本环境的 ID

**不要硬编码任何 ID。** 每次会话开始时按下面顺序探测，并把结果填进这张表报给我，
本会话后续所有操作都基于这张表：

```
企业 orgId          =
我的 userId         =
目标项目 projectId   =            编号前缀 =
任务分组 tasklistId  =            分组名   =
目标阶段 stageId     =            阶段名   =
任务类型 sfcId(任务)  =
状态 tfsId(未完成)    =            状态名 =
状态 tfsId(已完成)    =            状态名 =
工时字段 cfId        = 计划工时(含子任务) =
                     实际工时(含子任务) =
共同执行人 cfId      =
```

### 探测顺序

1. **企业与我**：`GetOrgInfo`、`GetUsersMe`（注意 #12：`accessCode` 别外泄）
2. **项目全集**：`SearchProjectsTQL --tql 'isArchived = false'` 拿 ID 数组
   → `QueryProjectsV3 --project-ids` 补名称与前缀
   （不要只用 `QueryProjectsV3` 裸查，见陷阱 #1）
3. **项目成员**：`ListProjectMembersV3 --project-id $P --page-size 200` 拿 userId
   → `PostV3MemberQuery --user-ids "…"` 换姓名（该接口按姓名搜索也可用 `--q`）
4. **项目结构**：
   - `SearchTaskGroupsV3` → 任务分组 / tasklistId
   - `SearchStagesV3` → 阶段 / stageId（团队名在这里，见陷阱 #14）
   - `GetScenarioFieldsV3` → 任务类型 / sfcId
   - `SearchTaskflowsV3` → 工作流
   - `SearchTaskflowStatusesV3` → 状态 / tfsId（**重名多，配合第 5 步定夺**）
   - `SearchProjectCustomFiledsV3` → 工时、共同执行人等自定义字段 cfId
5. **取样定夺字段惯例**（关键，别跳过）：
   `SearchTasksByTQLV2` 取该阶段全部 taskId → `QueryTaskV3 --task-id` 批量取详情，
   分别找 **未完成** 和 **已完成** 各若干条，抄它们实际在用的 `sfcId`、`tfsId`、
   `priority`、`visible`，并观察起止时间、参与人、工时的填写习惯。
   `tfsId` 必须这样定，不能按状态名猜（陷阱 #16）。

### 补充能力探测（按需）

- 迭代：`SearchSprintsV3`（很多项目根本没用迭代，返回空属正常）
- 项目集：`GetProgramsV3`
- 权限受限项会直接报 `Forbidden`，记录后绕开，不要反复重试

---

## 7. 常用命令配方（变量占位，可直接复制）

先设好变量（值来自第 6 节的探测表），后续命令即可直接复制：

```bash
cd <skill 目录>          # 下列命令都在 skill 目录下执行
P=<projectId>            # 项目
TL=<tasklistId>          # 任务分组
STAGE=<stageId>          # 阶段
SFC=<sfcId 任务类型>
TFS_OPEN=<tfsId 未完成>
TFS_DONE=<tfsId 已完成>
ME=<我的 userId>
```

**① 建任务**

```bash
bash run.sh call-tool CreateTaskV3 --request-body '{
  "content":"<标题>","note":"<正文，支持 \n 换行>",
  "projectId":"'$P'","tasklistId":"'$TL'","stageId":"'$STAGE'",
  "sfcId":"'$SFC'","tfsId":"'$TFS_OPEN'",
  "executorId":"'$ME'","involveMembers":["'$ME'"],
  "priority":-10,"visible":"projectMembers"}'
```

**② 建子任务** —— 同样调 `CreateTaskV3`，body 里加 `"parentTaskId":"<父taskId>"`。
建完回读 `ancestorIds` 确认层级（数组长度 = 祖先层数，0 为根任务）。
项目内已存在多层嵌套任务时，继续往下加是允许的。

**③ 改挂父任务**（字段名是 `targetId`，**不是** `parentTaskId`，见陷阱 #9）

```bash
bash run.sh call-tool UpdateTaskParentV3 --task-id <子taskId> \
  --request-body '{"targetId":"<新父taskId>"}'
```

**④ 改状态 / 置完成**

```bash
bash run.sh call-tool UpdateTaskStatusV3 --task-id <taskId> \
  --request-body '{"taskflowstatusId":"'$TFS_DONE'","tfsUpdateNote":"<流转说明>"}'
```

**⑤ 改时间、执行人、参与人**

```bash
bash run.sh call-tool UpdateTaskStartDateV3 --task-id <taskId> --request-body '{"startDate":"<ISO8601 UTC>"}'
bash run.sh call-tool UpdateTaskDueDateV3   --task-id <taskId> --request-body '{"dueDate":"<ISO8601 UTC>"}'
bash run.sh call-tool UpdateTaskExecutorV3  --task-id <taskId> --request-body '{"executorId":"<userId>"}'
bash run.sh call-tool UpdateTaskMembersV3   --task-id <taskId> --request-body '{"involveMembers":["<userId>"]}'
bash run.sh call-tool UpdateTaskContentV3   --task-id <taskId> --request-body '{"content":"<新标题>"}'
bash run.sh call-tool UpdateTaskNoteV3      --task-id <taskId> --request-body '{"note":"<新正文>","renderMode":"markdown"}'
```

> `UpdateTaskNoteV3` 是**整体覆盖**。合并备注时必须先读原文、在内存里拼接、
> 再整体写回，且保留原文的特殊字符（实测遇到过不换行空格 `\xa0`，手写会丢）。

**⑥ 写自定义字段**（必须用带 V3 的那个，见陷阱 #10）

```bash
bash run.sh call-tool UpdateTaskCusomFieldV3 --task-id <taskId> \
  --request-body '{"customfieldId":"<cfId>","type":"lookup","value":[{"id":"<userId>"}]}'
```

**⑦ 记工时**

```bash
bash run.sh call-tool CreatePlanTime --request-body '{
  "userId":"<工时归属人>","submitterId":"'$ME'","objectId":"<taskId>","objectType":"task",
  "plantime":28800000,"startDate":"<YYYY-MM-DD>","endDate":"<YYYY-MM-DD>",
  "isDuration":0,"includesHolidays":0}'

bash run.sh call-tool CreateWorkTime --request-body '{
  "userId":"<工时归属人>","submitterId":"'$ME'","objectId":"<taskId>","objectType":"task",
  "worktime":28800000,"startDate":"<YYYY-MM-DD>","endDate":"<YYYY-MM-DD>",
  "isDuration":0,"includesHolidays":0,"description":"<工作进展，必填>"}'

bash run.sh call-tool ListTaskPlanTimes --task-id <taskId>   # 只含本任务，不含子任务
bash run.sh call-tool ListTaskWorkTimes --task-id <taskId>
bash run.sh call-tool DeletePlanTime    --plan-time-id <plantimeId>   # 撤销误记
```

**⑧ 查任务树与改动流水**

```bash
bash run.sh call-tool QueryTaskV3 --task-id "<id1>,<id2>,<id3>"   # 批量详情
bash run.sh call-tool QueryTaskV3 --parent-task-id <taskId>       # 直接子任务
bash run.sh call-tool SearchTasksByTQLV2 --tql 'projectId = "'$P'" AND stageId = "'$STAGE'"' --page-size 300
bash run.sh call-tool ListTaskActivitiesV3 --task-id <taskId> --order-by created --page-size 30
```

**⑨ 删除与恢复**（`ArchiveTaskV3` 服务端已坏，见陷阱 #8）

```bash
# 删前先存快照：note / stageId / sfcId / tfsId
bash run.sh call-tool DeleteTaskV3  --task-id <taskId> --request-body '{}'
bash run.sh call-tool RestoreTaskV3 --task-id <taskId> \
  --request-body '{"stageId":"'$STAGE'","sfcId":"'$SFC'","tfsId":"'$TFS_OPEN'"}'
```

---

## 8. 交付要求与自检清单

**开工自检清单**（按序执行，把结果报给我）：

- [ ] `GetOrgInfo` → 当前是哪个企业
- [ ] `GetUsersMe` → 我是谁、`userId` 是什么（`accessCode` 不要外泄）
- [ ] `SearchProjectsTQL` → 该企业有几个项目，分别是什么
- [ ] 目标项目的阶段 / 任务类型 / 状态 / 工时字段 ID → 填第 6 节那张表
- [ ] 目标阶段取 3~5 条存量任务 → 抄出字段惯例
- [ ] 目标主题的存量任务扫一遍 → 是否已有撞题或重复项（有就先问我，别急着自己建）

**每次写入后，报告里必须包含：**

1. 任务编号（`uniqueId`，界面显示形如 `<项目前缀>-<编号>`）＋ `taskId`
2. 落位：项目 / 任务分组 / **阶段** / 父子层级
3. 关键字段：执行人、参与人、任务类型、状态、优先级、起止时间
4. **回读校验结果** —— 不是「接口返回 200」，而是重新查一遍确认字段真的变了
5. 表格或树形展示，不要贴原始 JSON

**主动汇报，不用等我问：**

- 内容重复 / 与同事撞题的任务
- 逾期未完成、或即将到期（48 小时内）的任务
- 字段缺失：无截止时间、无工时、无共同执行人 —— 这些会让巡检和报表失效
- 结构与我预期不符：例如我把某个名字当成「任务分组」，实际它在「阶段」里；
  或某批任务其实是模板示例数据、不是真实工作项
- 数据覆盖率过低导致目标无法达成（如工时报表全空）——**先说清现状，再给建议**，
  不要把空表当成果交付

**出错时**：给出原始 `code` / `errorCode` / `errorMessage`，并说明你判断是
调用问题还是服务端问题（可读 `cli.py` 确认自己发出的参数格式是否正确），
再提替代方案。**不要静默换路径，也不要假装成功。**

**统计类输出**必须写清口径：是否含子任务、时区、时间范围、去重规则。

**判断有歧义时先问**，特别是这几类：
目标落在哪个项目/阶段、某条任务该指派给谁、截止日期填哪一天、
归档还是删除、是否要动别人名下的任务。这些写错会影响整个团队，
问一句的成本远低于改回来。
