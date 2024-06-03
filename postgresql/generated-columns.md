# 

## 类型
- stored
存储的生成列在写入（插入或更新）并占用存储时进行计算
- virtual
不占用任何存储空间，并且在读取时进行计算

### 增加列
```sql
-- DROP TABLE IF EXISTS test

CREATE TABLE IF NOT EXISTS test
(
    xxx ltree NOT NULL,
    xxx_gen text COLLATE pg_catalog."default" GENERATED ALWAYS AS (ltree2text(xxx)) STORED
)
```

### 增加列
```sql
ALTER TABLE test ADD COLUMN xxx_gen text GENERATED ALWAYS AS (ltree2text(xxx)) stored;
```
