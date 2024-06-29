
## 查询

```sql
SELECT * FROM test WHERE JSON_CONTAINS(field, JSON_OBJECT('keyname', ?))
```

## 修改

```sql
UPDATE test SET account=JSON_REMOVE(account, JSON_UNQUOTE(REPLACE( JSON_SEARCH(field, \"one\", ?, NULL, '$'), '.keyname', ''))) WHERE JSON_SEARCH(field, \"one\", ?, NULL, '$') IS NOT NULL
```
