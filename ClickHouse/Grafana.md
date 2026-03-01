## 实现“按 username 分成多条曲线”的视图

### 确保 ClickHouse 数据源已配置

首先，需要在 Grafana 中拥有一个指向 ClickHouse 集群的数据源。
- **插件**：Grafana 原生不支持 ClickHouse，需要安装插件。最常用的是 `grafana-clickhouse-datasource` 或 `vertamedia-clickhouse-datasource` 。
- **连接**：在 Grafana 的 “Data Sources” 页面添加 ClickHouse 数据源，填写的集群地址（例如 `10.0.128.7`）、端口（默认 `9000` 或 `8123`）、数据库名 `default` 。

### 创建 Panel 并编写 Query

在 Dashboard 中添加一个新的 Panel，选择的 ClickHouse 数据源，并在 SQL 编辑器中编写查询。**您的原始 SQL 非常接近，但需要为 Grafana 的时间序列格式做一点调整。**

**推荐使用的 SQL：**
```sql
SELECT
    $__timeInterval(timestamp) as time,  -- ① 使用 Grafana 宏处理时间轴
    c.username,
    avg(c.duration_ms) AS duration_ms
FROM crawl_logs c
GLOBAL LEFT JOIN proxy_host AS p ON (c.username = p.proxy_user)
WHERE
    p.provider = 'sea'
    AND c.timestamp >= $__fromTime     -- ② 使用 Grafana 时间范围宏
    AND c.timestamp <= $__toTime
GROUP BY
    time,                              -- 按 Grafana 自动调整的时间间隔分组
    c.username
ORDER BY time;
```

### 配置 Panel 实现多条曲线

写好 SQL 后，需要告诉 Grafana 如何绘制这些曲线。

1.  **Format As**：在 Query 选项区域，有一个 **Format as** 下拉菜单，请选择 **`Table`** 。
2.  **Visualization**：在右侧面板设置中，选择 **`Time series`** 可视化类型。
3.  **生成多条曲线**：当您返回的数据中包含 `username` 和 `duration_ms` 两列时，Grafana 会自动将 `username` 列识别为 **“系列名称”（Series name）**或**“分组依据”（Group by）**的标签。它会对每一个唯一的 `username` 自动生成一条曲线 。
    - 如果自动生成的图例是 `duration_ms {username: "userA"}` 这种格式，您可以在 Query 的 **Legend format** 选项中自定义，例如直接写成 `{{username}}`，图例就会只显示用户名。
