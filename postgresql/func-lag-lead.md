## lag 获取记录前一行记录的数据

```sql
SELECT coin, bank, ctime
  FROM (
    SELECT coin, bank, ctime, LAG(bank) OVER (ORDER BY ctime) AS prev_bank
    FROM tp_analytics.account_logs_202210 WHERE uid = 10016874
  ) x
  WHERE bank <> COALESCE(prev_bank, bank)
  ORDER BY ctime;
```

## lead 获取记录后一行的数据
