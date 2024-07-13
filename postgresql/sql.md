
```psql
UPDATE test c
SET ext = jsonb_set(c.ext, '{time}', r.ext->'time')
FROM test r
WHERE c.jobid=r.jobid AND r.ext->'time' IS NOT NULL AND r.ext->'time'<>c.ext->'time';
```

```sql
DELETE FROM test c USING test r WHERE c.jobid=r.jobid
```
