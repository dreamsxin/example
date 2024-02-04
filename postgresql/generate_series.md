# generate_series 函数

可以按不同的规则产生一系列的填充数据。

http://www.postgres.cn/docs/9.6/functions-srf.html

```sql
-- 使用日期格式按周递增，然后分别获取其年和周，用于后续的排序
select 
    extract (year from date_series.date) as year,
    extract (week from date_series.date) as week
from (
      select generate_series(
          timestamp '2015-12-24', 
          timestamp '2016-01-08',  '1 weeks') as date
) date_series
```
`generate_series` 的时间递增包含：

- 1 seconds
- 1 minutes
- 1 hours
- 1 days
- 1 weeks
- 1 months
- 1 years

## 时间类型

```text
david=# select generate_series(now(), now() + '7 days', '1 day');
        generate_series        
-------------------------------
 2013-04-03 14:22:26.391852+08
 2013-04-04 14:22:26.391852+08
 2013-04-05 14:22:26.391852+08
 2013-04-06 14:22:26.391852+08
 2013-04-07 14:22:26.391852+08
 2013-04-08 14:22:26.391852+08
 2013-04-09 14:22:26.391852+08
 2013-04-10 14:22:26.391852+08
(8 rows)
```

```text
david=# select generate_series(to_date('20130403','yyyymmdd'), to_date('20130404','yyyymmdd'), '3 hours');  
    generate_series     
------------------------
 2013-04-03 00:00:00+08
 2013-04-03 03:00:00+08
 2013-04-03 06:00:00+08
 2013-04-03 09:00:00+08
 2013-04-03 12:00:00+08
 2013-04-03 15:00:00+08
 2013-04-03 18:00:00+08
 2013-04-03 21:00:00+08
 2013-04-04 00:00:00+08
(9 rows)
```

## IP 类型

IP类型

a. 建表

```sql
create table tbl_david(id int, ip_start inet, ip_stop inet);
```

b. 插入数据
```sql
insert into tbl_david values (1, '192.168.1.6', '192.168.1.10');
insert into tbl_david values (2, '192.168.2.16', '192.168.2.20');
insert into tbl_david values (3, '192.168.3.116', '192.168.3.120'); 
```

c. 查看数据
```text
# select * from tbl_david ;
 id |   ip_start    |    ip_stop    
----+---------------+---------------
  1 | 192.168.1.6   | 192.168.1.10
  2 | 192.168.2.16  | 192.168.2.20
  3 | 192.168.3.116 | 192.168.3.120
(3 rows)
```

d. generate_series 生成序列
```text
# select id, generate_series(0, ip_stop-ip_start)+ip_start as ip_new from tbl_david ;
 id |    ip_new     
----+---------------
  1 | 192.168.1.6
  1 | 192.168.1.7
  1 | 192.168.1.8
  1 | 192.168.1.9
  1 | 192.168.1.10
  2 | 192.168.2.16
  2 | 192.168.2.17
  2 | 192.168.2.18
  2 | 192.168.2.19
  2 | 192.168.2.20
  3 | 192.168.3.116
  3 | 192.168.3.117
  3 | 192.168.3.118
  3 | 192.168.3.119
  3 | 192.168.3.120
(15 rows)
```

## 补齐时间

```sql
SELECT lt.label, (case when rt.num is null then 0 else rt.num end) AS num
	FROM   (
	   SELECT d::TIMESTAMPTZ AS label FROM generate_series(?::timestamp, ?::timestamp, interval '1 hour') d
	) lt
	LEFT JOIN (
		SELECT time_bucket('1 hour', "time") AS label, COUNT(id) AS num FROM "event_reports" 
		WHERE event_reports.time >= ?::timestamp AND event_reports.time <= ?::timestamp GROUP BY time_bucket('1 hour', "time") ORDER BY time_bucket('1 hour', "time") DESC
	) rt ON (lt.label = rt.label)
```
