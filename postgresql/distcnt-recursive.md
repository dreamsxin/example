# 使用 WITH RECURSIVE 优化 distcnt/group

```sql
WITH RECURSIVE skip AS (  
  (  
    SELECT MIN(t.field) as field FROM test t WHERE t.field IS NOT NULL
  )  
  UNION ALL  
  (  
    SELECT (SELECT MIN(t.field) FROM test t WHERE t.field > s.field AND t.field IS NOT NULL) FROM skip s WHERE s.field IS NOT NULL  -- 这里的where s.field is not null 一定要加,否则就死循环了. 
  )   
)   
SELECT * FROM skip;
```

## 向上查询（查询所有父亲节点）

```sql
WITH RECURSIVE res AS (
  SELECT t1.* FROM t_code as t1  
  WHERE t1.enable = 'true' and t1.id = '4'

  UNION 
    SELECT t2.* from t_code as t2
    INNER JOIN res as t3 ON t2.id = t3.parent_id
    WHERE t2.enable = 'true'
)SELECT res.* from res
```

## 向下查询（查询所有孩子节点）

```sql
WITH RECURSIVE res AS (
  SELECT t1.* FROM t_code as t1  
  WHERE t1.enable = 'true' and t1.id = '1'

  UNION 
        SELECT t2.* from t_code as t2
    INNER JOIN res as t3 ON t3.id = t2.parent_id
    WHERE t2.enable = 'true'
)SELECT res.* from res
```