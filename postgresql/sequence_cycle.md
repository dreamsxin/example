# 序列

可以多个表共用一个序列发生器，实现多表唯一值，还可以是否可循环来实现重用。

```sql
-- CREATE SEQUENCE name INCREMENT BY 1 MAXVALUE 10 MINVALUE 1 START 1 CYCLE;
CREATE SEQUENCE log.clients_id_seq INCREMENT 1 CYCLE;
-- ALTER SEQUENCE log.clients_id_seq CYCLE;
CREATE TABLE log.clients ( id INT4 DEFAULT NEXTVAL('clients_id_seq'));
```

## 操作

- NEXTVAL('clients_id_seq') 下一个值
- CURRVAL('clients_id_seq') 当前值
- SETVAL('clients_id_seq') 设置值

## 序列结构

```text
postgres=# \d+ test5_id_seq 
         Sequence "public.clients_id_seq"
Column     |  Type   |        Value        | Storage 
---------------+---------+---------------------+---------
 sequence_name | name    | clients_id_seq      | plain
 last_value    | bigint  | 1                   | plain
 start_value   | bigint  | 1                   | plain
 increment_by  | bigint  | 1                   | plain
 max_value     | bigint  | 9223372036854775807 | plain
 min_value     | bigint  | 1                   | plain
 cache_value   | bigint  | 1                   | plain     
 log_cnt       | bigint  | 0                   | plain 
 is_cycled     | boolean | f                   | plain
 is_called     | boolean | f                   | plain
```
