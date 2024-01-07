# Catalogs

目录

https://www.postgresql.org/docs/15/catalogs.html

https://www.postgresql.org/docs/15/information-schema.html

每个 database 创建好后，

默认会有 3 个 `Schema`。

- pg_catalog:
用于存储Postgresql系统自带的函数、表、系统视图、数据类型转换器、数据类型定义等元数据
- information_schema
用于存储所需求提供的元数据查询视图, 目的是以符合ANSI SQL规范,可单独删除
- public
用于存储用户创建的数据表。

## 使用

```psql
\?
-- 查看模式
\dn
-- 设置默认搜索模式
SET search_path TO myschema, public;
```

通过 pgAdmin4，可以在每个数据库下看到一个名叫 `catalogs` 的对象，目录结构如下。
- Catalogs
  - ANSI (information_schema)
  - PostgreSQL Catalog (pg_catalog)

```sql
select table_schema from information_schema.tables group by table_schema;
```
### 查出所有字段注释
```shell
SELECT col.*, pgd.description AS comment FROM information_schema.columns AS col
LEFT JOIN pg_catalog.pg_description AS pgd ON (col.table_name::regclass = pgd.objoid AND col.ordinal_position = pgd.objsubid)
WHERE table_name = 'tg_event_reports' ORDER BY ordinal_position;
```
