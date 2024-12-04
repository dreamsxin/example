https://www.sqlite.org/json1.html

## The json_each() and json_tree() table-valued functions
`json_tree` 将 `json` 数据转为表数据
```sql
SELECT s.*,t.* FROM "test" s, json_tree(s.data) as t where  t.key='income' AND t.value>0 AND t.value != '-'
```
