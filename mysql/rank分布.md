

## 方法一：按天统计排名分布

```sql
SELECT 
    created_date AS `日期`,
    COUNT(CASE WHEN rank_position BETWEEN 1 AND 3 THEN 1 END) AS `1-3名数量`,
    COUNT(CASE WHEN rank_position BETWEEN 4 AND 10 THEN 1 END) AS `4-10名数量`,
    COUNT(CASE WHEN rank_position BETWEEN 11 AND 20 THEN 1 END) AS `11-20名数量`,
    COUNT(*) AS `总数量`,
    ROUND(COUNT(CASE WHEN rank_position BETWEEN 1 AND 3 THEN 1 END) * 100.0 / COUNT(*), 2) AS `1-3名占比(%)`,
    ROUND(COUNT(CASE WHEN rank_position BETWEEN 4 AND 10 THEN 1 END) * 100.0 / COUNT(*), 2) AS `4-10名占比(%)`,
    ROUND(COUNT(CASE WHEN rank_position BETWEEN 11 AND 20 THEN 1 END) * 100.0 / COUNT(*), 2) AS `11-20名占比(%)`
FROM ranking_history
WHERE deleted_at IS NULL
    AND rank_position > 0  -- 排除未收录的情况（排名为0）
    AND created_date IS NOT NULL
GROUP BY created_date
ORDER BY created_date DESC;
```

## 方法二：按天统计排名分布

```sql
SELECT 
    created_date AS `日期`,
    SUM(IF(rank_position BETWEEN 1 AND 3, 1, 0)) AS `1-3名`,
    SUM(IF(rank_position BETWEEN 4 AND 10, 1, 0)) AS `4-10名`,
    SUM(IF(rank_position BETWEEN 11 AND 20, 1, 0)) AS `11-20名`
FROM ranking_history
WHERE deleted_at IS NULL
    AND rank_position BETWEEN 1 AND 20  -- 只统计1-20名
    AND created_date IS NOT NULL
GROUP BY created_date
ORDER BY created_date DESC;
```

## 方法三：按项目分组统计排名分布

```sql
SELECT 
    created_date AS `日期`,
    project_id AS `项目ID`,
    COUNT(CASE WHEN rank_position BETWEEN 1 AND 3 THEN 1 END) AS `1-3名`,
    COUNT(CASE WHEN rank_position BETWEEN 4 AND 10 THEN 1 END) AS `4-10名`,
    COUNT(CASE WHEN rank_position BETWEEN 11 AND 20 THEN 1 END) AS `11-20名`,
    COUNT(*) AS `该范围总数`
FROM ranking_history
WHERE deleted_at IS NULL
    AND rank_position BETWEEN 1 AND 20
    AND created_date IS NOT NULL
GROUP BY created_date, project_id
ORDER BY created_date DESC, project_id;
```

## 方法四：统计最近30天的排名分布趋势

```sql
SELECT 
    created_date AS `日期`,
    COUNT(CASE WHEN rank_position BETWEEN 1 AND 3 THEN 1 END) AS `top3`,
    COUNT(CASE WHEN rank_position BETWEEN 4 AND 10 THEN 1 END) AS `top10`,
    COUNT(CASE WHEN rank_position BETWEEN 11 AND 20 THEN 1 END) AS `top20`,
    COUNT(*) AS `total_in_range`
FROM ranking_history
WHERE deleted_at IS NULL
    AND rank_position BETWEEN 1 AND 20
    AND created_date >= DATE_SUB(CURDATE(), INTERVAL 30 DAY)
    AND created_date IS NOT NULL
GROUP BY created_date
ORDER BY created_date DESC;
```

## 方法五：创建视图便于重复查询

```sql
CREATE VIEW daily_ranking_distribution AS
SELECT 
    created_date,
    COUNT(CASE WHEN rank_position BETWEEN 1 AND 3 THEN 1 END) AS rank_1_3,
    COUNT(CASE WHEN rank_position BETWEEN 4 AND 10 THEN 1 END) AS rank_4_10,
    COUNT(CASE WHEN rank_position BETWEEN 11 AND 20 THEN 1 END) AS rank_11_20,
    COUNT(CASE WHEN rank_position = 0 THEN 1 END) AS not_indexed,
    COUNT(*) AS total_keywords
FROM ranking_history
WHERE deleted_at IS NULL
    AND created_date IS NOT NULL
GROUP BY created_date;
```

## 使用示例：
```sql
-- 查询昨天的排名分布
SELECT * FROM daily_ranking_distribution 
WHERE created_date = DATE_SUB(CURDATE(), INTERVAL 1 DAY);

-- 查询最近7天的趋势
SELECT 
    created_date,
    rank_1_3,
    rank_4_10,
    rank_11_20,
    ROUND(rank_1_3 * 100.0 / total_keywords, 2) AS top3_percentage
FROM daily_ranking_distribution
WHERE created_date >= DATE_SUB(CURDATE(), INTERVAL 7 DAY)
ORDER BY created_date DESC;
```
