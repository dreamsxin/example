# Hydra
https://github.com/hydradatabase/hydra

Hydra Fork 自 Citus 的列存插件 columnar，但进行了许多改进优化，例如矢量化执行，查询并性化，并进行了一系列针对性的调优。hydra 目前已经发布1.0.1 Release版，目前还不支持最新的PostgreSQL 16。

Hydra 是一个列式存储扩展，旨在为 PostgreSQL 提供高性能的向量化列存储扩展。PostgreSQL 生态其实已经有一些列式存储扩展，例如 Citus 自带的 columnar，以及 TimescaleDB 针对时序数据的压缩列存引擎。不过看起来 hydra 在这个领域又达到了新的高度：在它给出的样例场景中（500G count），它可以达到令人震惊的加速比：从四五分钟到亚秒级。兼容性也还不错，支持分区表，可以与pg_hint_plan、pg_ivm等扩展配合使用。

## 使用
```sql
-- 创建扩展，需要超级用户权限
CREATE EXTENSION IF NOT EXISTS columnar;

-- Hydra支持列存表(columnar)和传统的PostgreSQL堆表(heap)，默认创建时是columnar表。可在建表时指定要创建的类型：
CREATE TABLE heap_table (...) USING heap;
CREATE TABLE columnar_table (...) USING columnar;

-- Hydra支持列存、行存相互转换
CREATE TABLE my_table (i INT8) USING heap;

-- convert to columnar
SELECT columnar.alter_table_set_access_method('my_table', 'columnar');

-- convert back to row (heap)
SELECT columnar.alter_table_set_access_method('my_table', 'heap');

--也支持通过拷贝数据手动转换
CREATE TABLE table_heap (i INT8) USING heap;
CREATE TABLE table_columnar (LIKE table_heap) USING columnar;
INSERT INTO table_columnar SELECT * FROM table_heap;

--支持分区表，分区表可以是heap表，也可以是columna表
CREATE TABLE parent(ts timestamptz, i int, n numeric, s text)
PARTITION BY RANGE (ts);
-- columnar partition
CREATE TABLE p0 PARTITION OF parent
FOR VALUES FROM ('2020-01-01') TO ('2020-02-01')
USING columnar;
-- columnar partition
CREATE TABLE p1 PARTITION OF parent
FOR VALUES FROM ('2020-02-01') TO ('2020-03-01')
USING columnar;

-- row partition
CREATE TABLE p2 PARTITION OF parent
FOR VALUES FROM ('2020-03-01') TO ('2020-04-01')
USING heap;
INSERT INTO parent VALUES ('2020-01-15', 10, 100, 'one thousand'); -- columnar
INSERT INTO parent VALUES ('2020-02-15', 20, 200, 'two thousand'); -- columnar
INSERT INTO parent VALUES ('2020-03-15', 30, 300, 'three thousand'); -- row
```
