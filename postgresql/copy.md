
```psql
# 导出到本地
\copy (select clientip, count(1) as num from xxxx group by clientip) to 'ip.csv' WITH CSV
# 服务端导出
copy (select clientip, count(1) as num from xxxx group by clientip) to 'ip.csv' WITH CSV
\copy (SELECT to_json(created_time)#>>'{}' AS created, to_char(created_time, 'YYYY-MM-DD"T"HH24:MI:SS"Z"')::cstring FROM xxx ORDER BY created_time ASC) to 'active.csv' with csv HEADER ENCODING 'UTF-8';
```
