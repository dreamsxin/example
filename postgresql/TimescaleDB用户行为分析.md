# 使用TimescaleDB进行用户行为链路统计分析

TimescaleDB作为PostgreSQL的时序数据库扩展，非常适合用于用户行为链路分析。以下是实现方法：

## 1. 数据模型设计

首先设计适合用户行为分析的数据模型：

```sql
-- 创建用户行为事件表
CREATE TABLE user_events (
    time TIMESTAMPTZ NOT NULL,
    user_id TEXT NOT NULL,
    session_id TEXT NOT NULL,
    event_type TEXT NOT NULL,
    page_url TEXT,
    referrer_url TEXT,
    device_info JSONB,
    -- 其他相关字段
    metadata JSONB
);

-- 转换为超表
SELECT create_hypertable('user_events', 'time');
```

## 2. 常用分析查询

### 用户路径分析

```sql
-- 查询特定用户的完整路径
WITH user_path AS (
    SELECT 
        time,
        event_type,
        page_url,
        LEAD(page_url) OVER (PARTITION BY user_id, session_id ORDER BY time) AS next_page
    FROM user_events
    WHERE user_id = 'user123'
    ORDER BY time
)
SELECT 
    time,
    event_type,
    page_url AS current_page,
    next_page
FROM user_path;
```

### 漏斗分析

```sql
-- 计算转化漏斗
WITH funnel_steps AS (
    SELECT
        COUNT(DISTINCT CASE WHEN event_type = 'view_home' THEN session_id END) AS step1,
        COUNT(DISTINCT CASE WHEN event_type = 'view_product' THEN session_id END) AS step2,
        COUNT(DISTINCT CASE WHEN event_type = 'add_to_cart' THEN session_id END) AS step3,
        COUNT(DISTINCT CASE WHEN event_type = 'checkout' THEN session_id END) AS step4
    FROM user_events
    WHERE time > NOW() - INTERVAL '7 days'
)
SELECT
    step1 AS "访问首页",
    step2 AS "查看产品",
    step3 AS "加入购物车",
    step4 AS "完成结算",
    ROUND(step2::numeric / step1 * 100, 2) AS "首页到产品转化率(%)",
    ROUND(step3::numeric / step2 * 100, 2) AS "产品到购物车转化率(%)",
    ROUND(step4::numeric / step3 * 100, 2) AS "购物车到结算转化率(%)"
FROM funnel_steps;
```

### 留存分析

```sql
-- 计算7日留存率
WITH first_visits AS (
    SELECT 
        user_id,
        DATE(MIN(time)) AS first_day
    FROM user_events
    WHERE time > NOW() - INTERVAL '30 days'
    GROUP BY user_id
),
retention AS (
    SELECT
        first_day AS cohort_day,
        COUNT(DISTINCT user_id) AS cohort_size,
        COUNT(DISTINCT CASE WHEN DATE(time) = first_day + INTERVAL '1 day' THEN user_id END) AS day1,
        COUNT(DISTINCT CASE WHEN DATE(time) = first_day + INTERVAL '7 day' THEN user_id END) AS day7
    FROM first_visits
    JOIN user_events USING (user_id)
    GROUP BY first_day
    ORDER BY first_day
)
SELECT
    cohort_day,
    cohort_size,
    ROUND(day1::numeric / cohort_size * 100, 2) AS day1_retention_rate,
    ROUND(day7::numeric / cohort_size * 100, 2) AS day7_retention_rate
FROM retention;
```

## 3. 性能优化技巧

1. **创建索引**：
   ```sql
   CREATE INDEX idx_user_events_user_id ON user_events(user_id);
   CREATE INDEX idx_user_events_session_id ON user_events(session_id);
   CREATE INDEX idx_user_events_event_type ON user_events(event_type);
   ```

2. **使用连续聚合**加速常用查询：
   ```sql
   CREATE MATERIALIZED VIEW user_funnel_daily
   WITH (timescaledb.continuous) AS
   SELECT
       time_bucket('1 day', time) AS day,
       COUNT(DISTINCT CASE WHEN event_type = 'view_home' THEN session_id END) AS step1,
       COUNT(DISTINCT CASE WHEN event_type = 'view_product' THEN session_id END) AS step2,
       COUNT(DISTINCT CASE WHEN event_type = 'add_to_cart' THEN session_id END) AS step3,
       COUNT(DISTINCT CASE WHEN event_type = 'checkout' THEN session_id END) AS step4
   FROM user_events
   GROUP BY day;
   ```

3. **数据保留策略**：
   ```sql
   -- 设置数据自动保留90天
   SELECT add_retention_policy('user_events', INTERVAL '90 days');
   ```

## 4. 高级分析功能

### 路径模式匹配

```sql
-- 查找从首页到结算的完整路径
WITH user_journeys AS (
    SELECT
        user_id,
        session_id,
        array_agg(event_type ORDER BY time) AS journey
    FROM user_events
    GROUP BY user_id, session_id
)
SELECT 
    COUNT(*) AS journey_count
FROM user_journeys
WHERE journey @> ARRAY['view_home', 'view_product', 'add_to_cart', 'checkout'];
```

### 时间序列分析

```sql
-- 计算每日活跃用户(DAU)和每周活跃用户(WAU)
SELECT
    time_bucket('1 day', time) AS day,
    COUNT(DISTINCT user_id) AS dau,
    COUNT(DISTINCT user_id) * 1.0 / 
        LAG(COUNT(DISTINCT user_id), 7) OVER (ORDER BY time_bucket('1 day', time)) AS wow_growth
FROM user_events
GROUP BY day
ORDER BY day DESC;
```

## 5. 可视化集成

TimescaleDB可以与多种BI工具集成：
- Grafana：用于创建实时监控仪表盘
https://github.com/grafana/grafana
- Superset：用于高级分析和可视化
https://github.com/apache/superset
- Metabase：用于业务团队的自助分析
https://github.com/metabase/metabase

通过合理设计数据模型和利用TimescaleDB的特性，您可以高效地进行用户行为链路分析，洞察用户行为模式并优化产品体验。

# TimescaleDB存储1亿条用户行为数据的内存与存储规划

对于1亿条用户行为数据保留一年的需求，以下是详细的内存和存储规划分析：

## 存储空间估算

### 基础数据存储需求
| 字段          | 类型         | 平均大小 | 1亿条数据总量 |
|---------------|-------------|----------|--------------|
| time          | TIMESTAMPTZ | 8字节    | 800 MB       |
| user_id       | TEXT        | 32字节   | 3.2 GB       |
| session_id    | TEXT        | 36字节   | 3.6 GB       |
| event_type    | TEXT        | 16字节   | 1.6 GB       |
| page_url      | TEXT        | 64字节   | 6.4 GB       |
| referrer_url  | TEXT        | 64字节   | 6.4 GB       |
| device_info   | JSONB       | 128字节  | 12.8 GB      |
| metadata      | JSONB       | 128字节  | 12.8 GB      |
| **原始数据小计** |            |          | **47.6 GB**  |

### 额外开销
- **行开销**：每行约24字节 → 2.4 GB
- **索引开销**：约占总数据的20-30% → 9.5-14.3 GB
- **压缩收益**：TimescaleDB压缩率通常为70-90% → 节省33-43GB
- **WAL日志**：约占总数据的20% → 9.5 GB
- **临时空间**：查询操作需要额外5-10% → 2.4-4.8 GB

### 总存储需求
| 组件             | 大小估算       |
|------------------|---------------|
| 原始数据         | 47.6 GB       |
| + 行开销         | +2.4 GB       |
| + 索引           | +12 GB (平均) |
| **未压缩小计**   | **62 GB**     |
| - 压缩节省       | -37 GB (60%)  |
| + WAL日志        | +9.5 GB       |
| + 临时空间       | +3 GB         |
| **总存储需求**   | **~37.5 GB**  |

**建议存储配置**：至少准备 **50-60 GB** 存储空间（考虑安全边际）

## 内存需求分析

### 最小运行内存
- **共享缓冲区**：至少分配4GB (shared_buffers)
- **工作内存**：256MB (work_mem)
- **维护内存**：1GB (maintenance_work_mem)
- **操作系统开销**：2GB
- **基础服务**：1GB

**最低要求**：8GB RAM

### 推荐生产配置
| 内存组件          | 推荐大小 | 说明 |
|-------------------|----------|------|
| 共享缓冲区        | 8-12 GB  | 总内存25-40% |
| 工作内存          | 512 MB   | 处理复杂查询 |
| 维护内存          | 2 GB     | 索引重建等操作 |
| TimescaleDB缓存   | 4 GB     | 时序数据专用缓存 |
| 操作系统缓存      | 4 GB     | 文件系统缓存 |
| 安全缓冲          | 2 GB     | 应对峰值负载 |
| **总内存推荐**    | **24-32 GB** | |

**推荐配置**：32GB RAM（支持高效处理热数据）

## 性能优化关键策略

### 1. 压缩配置
```sql
-- 启用压缩并优化设置
ALTER TABLE user_events SET (
    timescaledb.compress,
    timescaledb.compress_segmentby = 'user_id, session_id',
    timescaledb.compress_orderby = 'time DESC'
);

-- 配置压缩策略
SELECT add_compression_policy('user_events', INTERVAL '7 days');
```

### 2. 分区策略优化
```sql
-- 创建超表时优化分区
SELECT create_hypertable(
    'user_events', 
    'time', 
    chunk_time_interval => INTERVAL '1 week',
    partitioning_column => 'user_id',
    number_partitions => 16
);
```

### 3. 分层存储方案
```sql
-- 配置数据分层存储
CREATE TABLESPACE fast_ssd LOCATION '/ssd_data';
CREATE TABLESPACE archive_hdd LOCATION '/hdd_data';

-- 设置存储策略
SELECT add_tiering_policy('user_events', 
    tier_tablespaces => array['fast_ssd', 'archive_hdd'],
    tier_intervals => array['30 days', '180 days']
);
```

### 4. 索引优化
```sql
-- 创建复合索引
CREATE INDEX idx_user_session_time ON user_events(user_id, session_id, time DESC);

-- 创建BRIN索引加速时间范围查询
CREATE INDEX idx_time_brin ON user_events USING BRIN(time);
```

## 运维建议

1. **硬件配置**：
   - CPU：4核以上（推荐8核）
   - 存储：SSD NVMe优先
   - 网络：10GbE

2. **监控指标**：
   ```sql
   -- 检查压缩率
   SELECT * FROM timescaledb_information.compression_stats;
   
   -- 监控块性能
   SELECT * FROM timescaledb_information.hypertable;
   ```

3. **备份策略**：
   - 每日增量备份
   - 每周全量备份
   - 启用PITR（时间点恢复）

4. **查询优化**：
   ```sql
   -- 使用连续聚合加速报表
   CREATE MATERIALIZED VIEW user_behavior_daily
   WITH (timescaledb.continuous) AS
   SELECT 
     time_bucket('1 day', time) AS day,
     user_id,
     COUNT(*) FILTER (WHERE event_type = 'page_view') AS views,
     COUNT(DISTINCT session_id) AS sessions
   FROM user_events
   GROUP BY day, user_id;
   ```

## 成本估算示例

| 资源        | 规格              | 月成本估算 |
|------------|-------------------|------------|
| 云服务器    | 8 vCPU, 32GB RAM  | $200-300   |
| SSD存储     | 500GB (IOPS优化)  | $100-150   |
| 备份存储    | 100GB             | $20-30     |
| **月总成本**|                   | **$320-480** |

> 注：实际成本因云服务商和地区而异

## 结论

对于1亿条/年的用户行为数据：
1. **存储需求**：约50GB（启用压缩后）
2. **内存需求**：最小8GB，**推荐32GB**
3. **优化关键**：启用压缩、合理分区、SSD存储
4. **硬件建议**：8核CPU + 32GB RAM + 500GB SSD

此配置可确保高效查询（毫秒级响应）同时保持合理成本。实际部署前建议用1%样本数据进行压力测试，优化分区策略和索引设计。

# TimescaleDB 用户行为分析：漏斗与留存实现方案

## 漏斗分析实现

### 基础漏斗查询（不考虑事件顺序）

```sql
-- 基础漏斗分析（不考虑事件顺序）
SELECT
  COUNT(DISTINCT CASE WHEN event_type = 'view_home' THEN user_id END) AS step1,
  COUNT(DISTINCT CASE WHEN event_type = 'view_product' THEN user_id END) AS step2,
  COUNT(DISTINCT CASE WHEN event_type = 'add_to_cart' THEN user_id END) AS step3,
  COUNT(DISTINCT CASE WHEN event_type = 'checkout' THEN user_id END) AS step4,
  
  ROUND(
    COUNT(DISTINCT CASE WHEN event_type = 'view_product' THEN user_id END)::NUMERIC * 100 /
    NULLIF(COUNT(DISTINCT CASE WHEN event_type = 'view_home' THEN user_id END), 0)
  , 2) AS step1_to_step2_conversion,
  
  ROUND(
    COUNT(DISTINCT CASE WHEN event_type = 'add_to_cart' THEN user_id END)::NUMERIC * 100 /
    NULLIF(COUNT(DISTINCT CASE WHEN event_type = 'view_product' THEN user_id END), 0)
  , 2) AS step2_to_step3_conversion,
  
  ROUND(
    COUNT(DISTINCT CASE WHEN event_type = 'checkout' THEN user_id END)::NUMERIC * 100 /
    NULLIF(COUNT(DISTINCT CASE WHEN event_type = 'add_to_cart' THEN user_id END), 0)
  , 2) AS step3_to_step4_conversion
FROM user_events
WHERE time > NOW() - INTERVAL '7 days';
```

### 带时间顺序的漏斗分析

```sql
-- 带事件顺序的漏斗分析
WITH user_journeys AS (
  SELECT
    user_id,
    session_id,
    array_agg(event_type ORDER BY time) AS journey
  FROM user_events
  WHERE time > NOW() - INTERVAL '7 days'
    AND event_type IN ('view_home', 'view_product', 'add_to_cart', 'checkout')
  GROUP BY user_id, session_id
),
funnel_counts AS (
  SELECT
    COUNT(*) FILTER (WHERE journey @> ARRAY['view_home']) AS step1,
    COUNT(*) FILTER (WHERE journey @> ARRAY['view_home','view_product']) AS step2,
    COUNT(*) FILTER (WHERE journey @> ARRAY['view_home','view_product','add_to_cart']) AS step3,
    COUNT(*) FILTER (WHERE journey @> ARRAY['view_home','view_product','add_to_cart','checkout']) AS step4
  FROM user_journeys
)
SELECT
  step1, step2, step3, step4,
  ROUND(step2 * 100.0 / NULLIF(step1, 0), 2) AS step1_to_step2_conversion,
  ROUND(step3 * 100.0 / NULLIF(step2, 0), 2) AS step2_to_step3_conversion,
  ROUND(step4 * 100.0 / NULLIF(step3, 0), 2) AS step3_to_step4_conversion
FROM funnel_counts;
```

### 带时间窗口的漏斗分析（30分钟内完成）

```sql
-- 30分钟内完成的漏斗分析
WITH user_sequences AS (
  SELECT
    user_id,
    session_id,
    bool_and(event_type = 'view_home') OVER w AS has_step1,
    bool_and(event_type = 'view_product') OVER w AS has_step2,
    bool_and(event_type = 'add_to_cart') OVER w AS has_step3,
    bool_and(event_type = 'checkout') OVER w AS has_step4,
    MAX(time) FILTER (WHERE event_type = 'view_home') OVER w AS step1_time,
    MAX(time) FILTER (WHERE event_type = 'checkout') OVER w AS step4_time
  FROM user_events
  WHERE time > NOW() - INTERVAL '7 days'
    AND event_type IN ('view_home', 'view_product', 'add_to_cart', 'checkout')
  WINDOW w AS (PARTITION BY user_id, session_id)
),
qualified_sessions AS (
  SELECT DISTINCT session_id
  FROM user_sequences
  WHERE has_step1 AND has_step2 AND has_step3 AND has_step4
    AND step4_time - step1_time <= INTERVAL '30 minutes'
)
SELECT
  COUNT(DISTINCT session_id) AS completed_funnel_count
FROM qualified_sessions;
```

## 留存分析实现

### 基础留存分析

```sql
-- 基础留存分析（7日留存）
WITH first_events AS (
  SELECT
    user_id,
    MIN(time)::DATE AS first_event_date
  FROM user_events
  GROUP BY user_id
),
retention_data AS (
  SELECT
    fe.first_event_date AS cohort,
    ue.time::DATE AS activity_date,
    COUNT(DISTINCT fe.user_id) AS cohort_size,
    COUNT(DISTINCT ue.user_id) AS retained_users
  FROM first_events fe
  JOIN user_events ue ON fe.user_id = ue.user_id
    AND ue.time::DATE BETWEEN fe.first_event_date AND fe.first_event_date + INTERVAL '30 days'
  WHERE fe.first_event_date > NOW() - INTERVAL '90 days'
  GROUP BY fe.first_event_date, ue.time::DATE
)
SELECT
  cohort,
  activity_date - cohort AS day_number,
  retained_users,
  cohort_size,
  ROUND(retained_users * 100.0 / cohort_size, 2) AS retention_rate
FROM retention_data
ORDER BY cohort, day_number;
```

### 分群留存分析（按设备类型）

```sql
-- 分群留存分析（按设备类型）
WITH first_events AS (
  SELECT
    user_id,
    device->>'type' AS device_type,
    MIN(time)::DATE AS first_event_date
  FROM user_events
  GROUP BY user_id, device->>'type'
),
retention_data AS (
  SELECT
    fe.first_event_date AS cohort,
    fe.device_type,
    ue.time::DATE AS activity_date,
    COUNT(DISTINCT fe.user_id) AS cohort_size,
    COUNT(DISTINCT ue.user_id) AS retained_users
  FROM first_events fe
  JOIN user_events ue ON fe.user_id = ue.user_id
    AND ue.time::DATE BETWEEN fe.first_event_date AND fe.first_event_date + INTERVAL '30 days'
  WHERE fe.first_event_date > NOW() - INTERVAL '90 days'
  GROUP BY fe.first_event_date, fe.device_type, ue.time::DATE
)
SELECT
  cohort,
  device_type,
  activity_date - cohort AS day_number,
  retained_users,
  cohort_size,
  ROUND(retained_users * 100.0 / cohort_size, 2) AS retention_rate
FROM retention_data
ORDER BY cohort, device_type, day_number;
```

### 滚动留存分析

```sql
-- 滚动留存分析（每日活跃用户）
SELECT
  date,
  COUNT(DISTINCT user_id) AS dau,
  COUNT(DISTINCT CASE WHEN prev_7d THEN user_id END) AS rolling_7d,
  COUNT(DISTINCT CASE WHEN prev_30d THEN user_id END) AS rolling_30d,
  ROUND(COUNT(DISTINCT CASE WHEN prev_7d THEN user_id END) * 100.0 / COUNT(DISTINCT user_id), 2) AS retention_7d,
  ROUND(COUNT(DISTINCT CASE WHEN prev_30d THEN user_id END) * 100.0 / COUNT(DISTINCT user_id), 2) AS retention_30d
FROM (
  SELECT
    time::DATE AS date,
    user_id,
    EXISTS (SELECT 1 FROM user_events ue2 
            WHERE ue2.user_id = ue.user_id 
            AND ue2.time::DATE = ue.time::DATE - INTERVAL '7 days') AS prev_7d,
    EXISTS (SELECT 1 FROM user_events ue2 
            WHERE ue2.user_id = ue.user_id 
            AND ue2.time::DATE = ue.time::DATE - INTERVAL '30 days') AS prev_30d
  FROM user_events ue
  WHERE time > NOW() - INTERVAL '60 days'
) AS activity
GROUP BY date
ORDER BY date DESC;
```

## 性能优化方案

### 1. 创建连续聚合物化视图

```sql
-- 创建漏斗分析的连续聚合
CREATE MATERIALIZED VIEW funnel_daily
WITH (timescaledb.continuous) AS
SELECT
  time_bucket('1 day', time) AS day,
  COUNT(DISTINCT user_id) FILTER (WHERE event_type = 'view_home') AS step1,
  COUNT(DISTINCT user_id) FILTER (WHERE event_type = 'view_product') AS step2,
  COUNT(DISTINCT user_id) FILTER (WHERE event_type = 'add_to_cart') AS step3,
  COUNT(DISTINCT user_id) FILTER (WHERE event_type = 'checkout') AS step4
FROM user_events
GROUP BY day;

-- 创建留存分析的连续聚合
CREATE MATERIALIZED VIEW retention_weekly
WITH (timescaledb.continuous) AS
SELECT
  time_bucket('1 week', first_event_date) AS cohort_week,
  time_bucket('1 week', activity_date) AS activity_week,
  COUNT(DISTINCT user_id) AS retained_users
FROM (
  SELECT
    user_id,
    MIN(time)::DATE AS first_event_date,
    time::DATE AS activity_date
  FROM user_events
  GROUP BY user_id, time::DATE
) AS user_activity
GROUP BY cohort_week, activity_week;
```

### 2. 索引优化策略

```sql
-- 为漏斗分析优化索引
CREATE INDEX idx_user_events_type_time ON user_events(event_type, time);
CREATE INDEX idx_user_events_user_session ON user_events(user_id, session_id);

-- 为留存分析优化索引
CREATE INDEX idx_user_events_user_time ON user_events(user_id, time);
```

### 3. 数据分区策略

```sql
-- 优化超表分区
SELECT alter_hypertable(
  'user_events',
  chunk_time_interval => INTERVAL '1 month',
  partitioning_column => 'user_id',
  number_partitions => 16
);

-- 添加保留策略
SELECT add_retention_policy('user_events', INTERVAL '365 days');
```

### 4. 压缩策略

```sql
-- 启用高级压缩
ALTER TABLE user_events SET (
  timescaledb.compress,
  timescaledb.compress_segmentby = 'user_id, session_id',
  timescaledb.compress_orderby = 'time DESC'
);

-- 设置压缩策略
SELECT add_compression_policy('user_events', INTERVAL '7 days');
```

## 可视化与报表

### Grafana 仪表板配置

1. **漏斗分析仪表板**：
   ```sql
   -- 漏斗数据查询
   SELECT
     $__timeGroupAlias(day, '1d'),
     step1,
     step2,
     step3,
     step4,
     ROUND(step2 * 100.0 / NULLIF(step1, 0), 2) AS step1_to_step2,
     ROUND(step3 * 100.0 / NULLIF(step2, 0), 2) AS step2_to_step3,
     ROUND(step4 * 100.0 / NULLIF(step3, 0), 2) AS step3_to_step4
   FROM funnel_daily
   WHERE $__timeFilter(day)
   ORDER BY day
   ```

2. **留存曲线仪表板**：
   ```sql
   -- 留存曲线查询
   SELECT
     cohort_week AS "time",
     EXTRACT('day' FROM activity_week - cohort_week)/7 AS week_number,
     ROUND(retained_users * 100.0 / 
         FIRST_VALUE(retained_users) OVER (PARTITION BY cohort_week ORDER BY activity_week), 2) AS retention_rate
   FROM retention_weekly
   WHERE cohort_week > $__timeFrom()::TIMESTAMPTZ
     AND activity_week <= $__timeTo()::TIMESTAMPTZ
   ORDER BY cohort_week, week_number
   ```

### Superset 高级分析

1. **漏斗桑基图**：
   ```sql
   WITH funnel_data AS (
     SELECT
       event_type,
       LEAD(event_type) OVER (PARTITION BY session_id ORDER BY time) AS next_event,
       COUNT(*) AS count
     FROM user_events
     WHERE time > NOW() - INTERVAL '7 days'
       AND event_type IN ('view_home', 'view_product', 'add_to_cart', 'checkout')
     GROUP BY session_id, event_type, time
   )
   SELECT
     event_type AS source,
     next_event AS target,
     SUM(count) AS value
   FROM funnel_data
   WHERE next_event IS NOT NULL
   GROUP BY source, target
   ```

2. **留存热力图**：
   ```sql
   SELECT
     TO_CHAR(first_event_date, 'YYYY-MM') AS cohort,
     day_number,
     AVG(retention_rate) AS retention
   FROM (
     SELECT
       first_event_date,
       activity_date - first_event_date AS day_number,
       COUNT(DISTINCT ue.user_id) * 100.0 / COUNT(DISTINCT fe.user_id) AS retention_rate
     FROM first_events fe
     LEFT JOIN user_events ue ON fe.user_id = ue.user_id
       AND ue.time::DATE BETWEEN fe.first_event_date AND fe.first_event_date + 30
     GROUP BY first_event_date, activity_date
   ) AS retention_data
   GROUP BY cohort, day_number
   ORDER BY cohort, day_number
   ```

## 高级分析技巧

### 混合漏斗留存分析

```sql
-- 完成漏斗用户的留存分析
WITH funnel_completers AS (
  SELECT DISTINCT user_id
  FROM (
    SELECT
      user_id,
      session_id,
      bool_and(event_type = 'view_home') OVER w AS step1,
      bool_and(event_type = 'view_product') OVER w AS step2,
      bool_and(event_type = 'add_to_cart') OVER w AS step3,
      bool_and(event_type = 'checkout') OVER w AS step4
    FROM user_events
    WINDOW w AS (PARTITION BY user_id, session_id)
  ) AS journey
  WHERE step1 AND step2 AND step3 AND step4
),
retention AS (
  SELECT
    fc.user_id,
    MIN(ue.time)::DATE AS first_funnel_date,
    ue.time::DATE AS activity_date
  FROM funnel_completers fc
  JOIN user_events ue ON fc.user_id = ue.user_id
  GROUP BY fc.user_id, ue.time::DATE
)
SELECT
  first_funnel_date AS cohort,
  activity_date - first_funnel_date AS day_number,
  COUNT(DISTINCT user_id) AS retained_users,
  COUNT(DISTINCT user_id) FILTER (WHERE day_number = 0) OVER (PARTITION BY first_funnel_date) AS cohort_size,
  ROUND(COUNT(DISTINCT user_id) * 100.0 / 
        COUNT(DISTINCT user_id) FILTER (WHERE day_number = 0) OVER (PARTITION BY first_funnel_date), 2) AS retention_rate
FROM retention
GROUP BY cohort, day_number
ORDER BY cohort, day_number;
```

### 用户分群分析

```sql
-- 高留存用户特征分析
WITH user_retention AS (
  SELECT
    user_id,
    COUNT(DISTINCT time::DATE) > 5 AS high_retention_user
  FROM user_events
  WHERE time > NOW() - INTERVAL '30 days'
  GROUP BY user_id
)
SELECT
  device->>'type' AS device_type,
  COUNT(*) FILTER (WHERE high_retention_user) AS high_retention_count,
  COUNT(*) FILTER (WHERE NOT high_retention_user) AS low_retention_count,
  ROUND(COUNT(*) FILTER (WHERE high_retention_user) * 100.0 / COUNT(*), 2) AS retention_rate
FROM user_events ue
JOIN user_retention ur ON ue.user_id = ur.user_id
WHERE time > NOW() - INTERVAL '30 days'
GROUP BY device->>'type';
```

这些 TimescaleDB 实现方案提供了从基础到高级的用户行为分析方法，结合 TimescaleDB 的时序优化特性，能够在亿级数据量下保持高性能查询。实际应用中可根据具体业务需求调整事件类型、时间窗口和分析维度。

