# MATERIALIZED VIEW 物化视图

物化视图的查询和普通表是一样的，相当于一个缓存机制，将复杂的查询结果存放在物化视图中，需要refresh操作更新数据。

PostgreSQL物化视图（
1、创建视图
```sql
CREATE MATERIALIZED VIEW [ IF NOT EXISTS ] table_name
[ (column_name [, ...] ) ]
[ WITH ( storage_parameter [= value] [, ... ] ) ]
[ TABLESPACE tablespace_name ]
AS query
[ WITH [ NO ] DATA ]
```
2、删除视图
```sql
DROP MATERIALIZED VIEW table_name
```
3、创建索引
```sql
CREATE INDEX idx_index_name ON table_name USING index(column);
```
4、手动刷新视图
```sql
REFRESH MATERIALIZED VIEW [ CONCURRENTLY ] name
  [ WITH [ NO ] DATA ]
```

-- 不带CONCURRENTLY即为全量刷新，带CONCURRENTLY即为增量刷新
-- 全量刷新速度较快，但是刷新时会阻塞对book_mv的查询，增量刷新相反
-- 实际业务中选增量刷新，定时执行refresh即
