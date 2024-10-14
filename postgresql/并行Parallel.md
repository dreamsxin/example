## 影响wokers数量的参数权重依次顺序：

- max_parallel_workers_per_gather ：每次sql操作workers数量的最大值。
- max_parallel_workers：其次，查询执行器从max_parallel_workers池中可以获取workers的最大数。
- max_worker_processes：这个是workers的顶级限制后台进程的总数(此参数谨慎修改，根据系统实际的cpu个数(核数)来设置)。
- max_parallel_workers_per_gather：理解为每个用户去银行取钱金额。
- max_parallel_workers：理解为用户存在银行中的总存款金额。
- max_worker_processes：理解为某个银行支点可用现金总数。

```psql

-- 查看 max_parallel_workers 参数
SHOW max_parallel_workers;
 
-- 查看 max_parallel_workers_per_gather 参数
SHOW max_parallel_workers_per_gather;

SHOW min_parallel_table_scan_size;
SHOW min_parallel_index_scan_size;
```

## 查看索引大小

```psql
SELECT pg_size_pretty(pg_relation_size(oid)) AS index_size
FROM pg_class
WHERE relname = '_hyper_139_777_chunk_browser_logs_created_at_idx';
```

