# 使用 width recursive 优化 distcnt/group

```sql
WITH RECURSIVE skip AS (  
  (  
    SELECT MIN(t.field) as field FROM test t WHERE t.field IS NOT NULL
  )  
  UNION ALL  
  (  
    SELECT (SELECT MIN(t.field) FROM test t WHERE t.field > s.field AND t.field IS NOT NULL) FROM skip s WHERE s.field IS NOT NULL  -- 这里的where s.otherinfo is not null 一定要加,否则就死循环了. 
  )   
)   
SELECT * FROM skip;
```