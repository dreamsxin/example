## pg_advisory_lock

https://www.postgresql.org/docs/9.6/explicit-locking.html#ADVISORY-LOCKS

`advisory lock` 锁住的 ID，是库级冲突的。

```sql
SELECT pg_advisory_lock(id) FROM foo WHERE id = 12345; -- ok
SELECT pg_advisory_lock(id) FROM foo WHERE id > 12345 LIMIT 100; -- danger!
SELECT pg_advisory_lock(q.id) FROM
(
  SELECT id FROM foo WHERE id > 12345 LIMIT 100
) q; -- ok
```

* 读不到被锁的记录
```sql
UPDATE test SET name='test' WHERE id=1 returning pg_try_advisory_xact_lock(id);
SELECT* FROM test WHERE id=1 ANDpg_try_advisory_xact_lock(id);
```

## skip locked
