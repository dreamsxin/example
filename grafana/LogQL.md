# LogQL 入门教程

## 1. LogQL 是什么？

LogQL 是 Loki 的查询语言，类似 PromQL，但针对日志数据。分为两类查询：

- **日志查询（Log Query）**：返回日志行本身。
- **指标查询（Metric Query）**：对日志行进行计数、速率等聚合，返回时间序列。

---

## 2. 日志流选择器（Stream Selector）

**每个查询必须从流选择器开始**，用 `{}` 包裹标签匹配。

| 符号 | 含义 | 示例 |
|------|------|------|
| `=` | 精确相等 | `{app="nginx"}` |
| `!=` | 不等于 | `{app!="nginx"}` |
| `=~` | 正则匹配 | `{job=~".+-server"}` |
| `!~` | 正则不匹配 | `{job!~"test-.*"}` |

```logql
{namespace="prod", app="api"}        # 精确两个标签
{job=~"sdk-pro/.+"}                  # 正则匹配前缀
```

---

## 3. 行过滤器（Line Filter）

在流选择器后使用 `|~`、`|=` 等对日志**行内容**进行过滤。

| 符号 | 含义 | 示例（匹配日志行包含字符串） |
|------|------|------------------------------|
| `|=` | 子串存在 | `{app="api"} \|= "error"` |
| `!=` | 子串不存在 | `\|= "error"` 改为 `!= "error"` |
| `\|~` | 正则匹配 | `\|~ "5[0-9]{2}"`（匹配 5xx 状态码） |
| `!\~` | 正则不匹配 | `!\~ "debug"` |

```logql
{app="api"} |= "timeout" |~ "status=[45].."
```

---

## 4. 解析器（Parser）

从非结构化的日志行中提取字段，供后续过滤或指标聚合使用。

### 4.1 `| logfmt` — 解析 key=value 格式

日志行示例：`level=info msg="user login" user_id=123`

```logql
{app="api"} | logfmt | level = "error"
```

### 4.2 `| json` — 解析 JSON 格式

日志行示例：`{"level":"info","msg":"hello"}`

```logql
{app="api"} | json | level="info"
```

### 4.3 `| unpack` — 自动解包

自动识别 JSON / logfmt。

---

## 5. 标签过滤器（Label Filter）

解析后生成的字段可以像标签一样过滤：

```logql
{app="api"} | logfmt | level = "error" | duration_ms > 100
```

支持 `=`、`!=`、`>`、`<`、`>=`、`<=`。

---

## 6. 指标查询（Metric Query）

使用 `count_over_time`、`rate` 等函数将日志转换为指标。

### 常见函数

| 函数 | 含义 |
|------|------|
| `count_over_time(range)` | 日志行数量 |
| `rate(range)` | 每秒日志行速率 |
| `sum by(...) (rate(...))` | 分组聚合 |

### 示例

```logql
# 过去 5 分钟 API 的错误日志条数
count_over_time({app="api"} |= "error" [5m])

# 各 level 的日志速率
sum by (level) (rate({app="api"} | logfmt [1m]))
```

---

## 7. 常用操作符和范围

- **时间范围**：`[5m]`、`[1h]`、`[24h]` 等，必须用在 `rate`、`count_over_time` 等函数中。
- **算术/比较**：在指标查询中可使用 `+`、`-`、`*`、`/`、`>`、`<` 等。

---

## 8. 完整示例

假设日志行（logfmt）：
```
level=info method=GET path=/api/user status=200 duration_ms=45
level=error method=POST path=/api/login status=500 duration_ms=320
```

查询：返回 status>=400 且 method=POST 的日志行

```logql
{job="api-server"} | logfmt | status >= 400 | method = "POST"
```

查询：最近 5 分钟每分钟的错误日志速率

```logql
rate({job="api-server"} | logfmt | level="error" [5m])
```

---

## 9. 备注与最佳实践

- 始终先用**日志查询**验证过滤条件，再转成**指标查询**。
- 解析器（`logfmt`/`json`）会略微增加查询开销，必要时再使用。
- 正则行过滤器（`|~`）比子串过滤器（`|=`）慢，尽量优先使用子串。
- 在 Grafana Explore 中可交互式编写 LogQL。

---

掌握上述内容，你已经能用 LogQL 完成大部分日常日志查询和分析任务。如需更高级的语法（如 `| pattern`、`| regexp`、`label_format` 等），可以查阅 [Loki 官方文档](https://grafana.com/docs/loki/latest/logql/)。
