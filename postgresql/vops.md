# 向量计算插件

https://github.com/postgrespro/vops

## 使用 VOPS

总共分三步：

1. 创建基础表的VOPS表（使用对应的瓦片类型）。
2. 调用VOPS提供的转换函数，将基础表的数据转移到VOPS表（即转换、规整为瓦片存储的过程）。
3. 使用正常的SQL，访问VOPS表即可，但是需要注意，使用VOPS提供的操作符哦。

* 操作符

因为是瓦片类型，VOPS重写了对应的操作符来支持向量计算。

1. `+、-、/、*`保持原样不变。
2. 比较操作符`=、<>、>、>=、<、<=`保持原样不变。
3. 逻辑操作符`and, or, not`是SQL PARSER部分，没有开放重写，VOPS定义了`&、|、!`来对应他们的功能（注意优先级）。
4. 由于BETWEEN AND无法重写，使用`betwixt(x, a , b)`函数来代替这个语法。
5. 逻辑操作返回的是`vops_boolean`，所以使用`filter`函数来处理断言 `where filter( (x=1) | (x=2) )`，filter()会对传入参数的内容设置`filter_mask`，任何情况下都返回`boolean:true`
6. 除了函数调用，所有的向量化计算操作符，在传入字符串常量时，必须显示转换。
```sql
select sum(price) from trades where filter(day >= '2017-01-01'::date);  
select sum(price) from trades where filter(betwixt(day, '2017-01-01', '2017-02-01')); 
```
7. 对于char, int2, int4类型，VOPS提供了一个连接符||，用于如下合并`(char || char) -> int2, (int2 || int2) -> int4, (int4 || int4) -> int8`
8. 以及 `is_null`、`is_not_null`、`ifnull`

* 聚合函数

1. 目前VOPS只支持INT类型的聚合操作（int2, int4, int8）
2. 由于map函数是vardic参数函数，所以末尾的所有expr类型必须一致，所有的expr类型必须一致，即 `sum(expr1),avg(expr2),...`，这些expr类型必须一致。
3. 
`count, min, max, sum, avg`

聚合分为两种，一种是非分组聚合，一种是分组聚合。

* 非分组聚合和原有用法一样
```sql
select sum(l_extendedpricel_discount) as revenue  
from vops_lineitem  
where filter(betwixt(l_shipdate, '1996-01-01', '1997-01-01')  
        & betwixt(l_discount, 0.08, 0.1)  
        & (l_quantity < 24));
```

* 带有group by的分组聚合稍微复杂一些，需要用到map和reduce函数
`map`函数实现聚合操作，并且返回一个`hash table`，`reduce`函数则将`hash table`的数据一条条解析并返回。
`map`函数是运算，`reduce`函数是返回结果。
- map(group_by_expression, aggregate_list, expr {, expr })
`expr`的数据类型必须一致，`map(c1||c2, 'count,max,avg,max', c3, c4, c5, c6)` 表示  `count(*),  count(c3),  max(c4).  avg(c5),  max(c6)  group by c1, c2; 
- reduce(map(...))

```sql
select   
    sum(l_quantity),  
    sum(l_extendedprice),  
    sum(l_extendedprice(1-l_discount)),  
    sum(l_extendedprice(1-l_discount)(1+l_tax)),  
    avg(l_quantity),  
    avg(l_extendedprice),  
    avg(l_discount)  
  from vops_lineitem   
  where l_shipdate <= '1998-12-01'::date  
  group by l_returnflag, l_linestatus;  
 
-- 改写为  
 
select reduce(map(l_returnflag||l_linestatus, 'sum,sum,sum,sum,avg,avg,avg',  
    l_quantity,  
    l_extendedprice,  
    l_extendedprice(1-l_discount),  
    l_extendedprice(1-l_discount)(1+l_tax),  
    l_quantity,  
    l_extendedprice,  
    l_discount))   
  from vops_lineitem   
  where filter(l_shipdate <= '1998-12-01'::date);
```

* 索引

```sql
create index low_boundary on trades using brin(first(day)); -- trades table is ordered by day  
create index high_boundary on trades using brin(last(day)); -- trades table is ordered by day 

-- first, last, high, low返回的是PostgreSQL原生boolean，而不是vops_boolean，所以不需要加filter，可以直接写在where表达式中。
-- 那么下面这个QUERY可以用到以上索引

select sum(price) from trades where first(day) >= '2015-01-01' and last(day) <= '2016-01-01' and filter(betwixt(day, '2015-01-01', '2016-01-01'));
```

* 创建普通表
```sql
create table t(c1 int,c2 int);
```

* 创建vops表
```sql
create table t_vops(c1 vops_int4,c2 vops_int4);
```

* 向表t中插入数据
```sql
insert into t select random()*100,random()*1000 from generate_series(1,10000000);
```

* 将普通表t中的数据转换到vops表中
```sql
select populate('t_vops'::regclass, 't'::regclass);
```

`populate(destination regclass, source regclass, predicate cstring default null, sort cstring default null);`

- destination
表示VOPS表的 `regclass`
- source
原始表的 `regclass`
- predicate
参数用于合并数据，将最近接收的数据合并到已有数据的VOPS表。
- sort
是告诉populate，按什么字段排序，前面讲索引时有讲到为什么需要排序，就是要线性相关性，好创建first, last的brin索引。

* 查询

```sql
select * from t_vops limit 1;
select unnest(l.*) from t_vops l limit 5;
```
## VOPS瓦片式存储表10亿

使用 dblink 实现并发查询。
已使用DBLINK异步调用，防止再度并行，设置参数
```sql
alter role postgres set max_parallel_workers_per_gather=0;
alter role postgres set work_mem='2GB';
```

```sql
-- 连接函数
create or replace function conn(        
  name,   -- dblink名字        
  text    -- 连接串,URL        
) returns void as $$          
declare          
begin          
  perform dblink_connect($1, $2);         
  return;          
exception when others then          
  return;          
end;          

-- 创建表模板
create unlogged table a(id int, c1 int);    

-- 创建56个子表
do language plpgsql $$    
declare    
begin    
  for i in 0..55 loop    
    execute format('create unlogged table a%s (like a) inherits (a)', i);    
  end loop;    
end;    
$$;

-- 生成10亿数据

do language plpgsql $$    
declare    
begin    
  for i in 0..55 loop           
    perform conn('link'||i,  'hostaddr=127.0.0.1 port=1921 user=postgres dbname=postgres');           
    perform dblink_send_query('link'||i, 
        format('insert into a%s select generate_series(%s, %s), random()*99', i, i*17857142+1, (i+1)*17857142)
    );          
  end loop;       
end;    
$$; 
```

1、创建VOPS表

```sql
create unlogged table b(id vops_int4, c1 vops_int4);    
  
do language plpgsql $$    
declare    
begin    
  for i in 0..55 loop    
    execute format('create unlogged table b%s (like b) inherits (b)', i);    
  end loop;    
end;    
$$;
```

2、将10亿原始数据转换为vops

```sql
do language plpgsql $$    
declare    
begin    
  for i in 0..55 loop    
    perform conn('link'||i,  'hostaddr=127.0.0.1 port=1921 user=postgres dbname=postgres');           
    perform dblink_send_query('link'||i, format('select populate(''b%s''::regclass, ''a%s''::regclass)', i, i));         
  end loop;    
end;    
$$;
```

3、数据样例

```output
postgres=# select * from b0 limit 2;  
-[ RECORD 1 ]----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------  
id | {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64}  
c1 | {91,59,72,18,53,26,86,83,44,82,66,42,7,87,66,29,10,85,49,6,89,98,9,46,93,7,42,61,47,3,5,39,62,77,58,16,4,45,99,49,27,66,90,35,55,58,64,65,44,14,71,34,13,80,80,7,87,23,68,36,26,73,75,88}  
-[ RECORD 2 ]----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------  
id | {65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128}  
c1 | {51,33,6,55,78,5,5,7,72,95,41,27,54,6,92,98,19,64,34,32,45,15,39,33,38,9,69,64,82,45,53,34,78,59,90,57,64,94,64,37,91,6,64,46,12,58,45,31,23,79,64,68,94,4,2,33,13,71,97,95,17,51,30,95}  
 
postgres=# select * from a0 limit 2;  
-[ RECORD 1 ]  
id | 1  
c1 | 91  
-[ RECORD 2 ]  
id | 2  
c1 | 59  
```

4、空间占用，8.7GB

```output
postgres=# \dt+ b*  
                    List of relations  
 Schema | Name | Type  |  Owner   |  Size   | Description   
--------+------+-------+----------+---------+-------------  
 public | b    | table | postgres | 0 bytes |   
 public | b0   | table | postgres | 156 MB  |   
 public | b1   | table | postgres | 156 MB  |   
 public | b10  | table | postgres | 156 MB  |   
 public | b11  | table | postgres | 156 MB  |   
 public | b12  | table | postgres | 156 MB  |   
 public | b13  | table | postgres | 156 MB  |   
 public | b14  | table | postgres | 156 MB  |   
 public | b15  | table | postgres | 156 MB  |   
 public | b16  | table | postgres | 156 MB  |   
 public | b17  | table | postgres | 156 MB  |   
 public | b18  | table | postgres | 156 MB  |   
 public | b19  | table | postgres | 156 MB  |   
 public | b2   | table | postgres | 156 MB  |   
 public | b20  | table | postgres | 156 MB  |
```

5、vops（瓦片式存储）分片表并行聚合

```sql
create or replace function get_res_vops() returns setof record as $$    
declare    
begin    
  for i in 0..55 loop           
    perform conn('link'||i,  'hostaddr=127.0.0.1 port=1921 user=postgres dbname=postgres');           
    perform 1 from dblink_get_result('link'||i) as t(c1 int, cnt int8, aggs int8[]);          
    perform dblink_send_query('link'||i, format('select (reduce(map(c1, ''min,max,sum'', id,id,id))).* from b%s', i));          
  end loop;       
  for i in 0..55 loop    
    return query select * from dblink_get_result('link'||i) as t(c1 int, cnt int8, aggs int8[]);    
  end loop;    
end;    
$$ language plpgsql strict;    
```

6、查询耗时，1.97秒

```output
select c1, sum(cnt), sum(aggs[3])/sum(cnt)::float8, min(aggs[1]), max(aggs[2]), sum(aggs[3]) from ( select * from get_res_vops() as t(c1 int, cnt int8, aggs int8[])) t group by c1;  

postgres=# select c1, sum(cnt), sum(aggs[3])/sum(cnt)::float8, min(aggs[1]), max(aggs[2]), sum(aggs[3]) from ( select * from get_res_vops() as t(c1 int, cnt int8, aggs int8[])) t group by c1;  
 c1 |   sum    |     ?column?     | min |    max    |       sum          
----+----------+------------------+-----+-----------+------------------  
  7 | 10104223 |  500196670.66755 |  13 | 999999922 | 5054098704282486  
 41 | 10098079 | 500105983.312797 |  75 | 999999899 | 5050109727865311  
 72 | 10099035 | 499987853.746656 |   3 | 999999917 | 5049394834562362  
  2 | 10098707 | 500027259.278005 | 119 | 999999881 | 5049628783461600  
 29 | 10101931 | 500172863.741008 |  16 | 999999911 | 5052711757584067  
...
(100 rows)  
  
Time: 1972.137 ms (00:01.972)  
```

7、CPU资源使用

```output
top - 16:27:44 up 30 days, 21:15,  2 users,  load average: 7.49, 6.21, 6.41  
Tasks: 522 total,  58 running, 464 sleeping,   0 stopped,   0 zombie  
%Cpu(s): 99.9 us,  0.1 sy,  0.0 ni,  0.0 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st  
KiB Mem : 23094336+total,  7251336 free,  7786940 used, 21590508+buff/cache  
KiB Swap:        0 total,        0 free,        0 used. 10075729+avail Mem  
```

## 普通单表10亿，HASH并行聚合

1、普通表并行聚合。

```sql
create unlogged table c(id int, c1 int);  
  
insert into c select * from a; 
```

2、空间占用，34GB

```output
postgres=# \dt+ c  
                   List of relations  
 Schema | Name | Type  |  Owner   | Size  | Description   
--------+------+-------+----------+-------+-------------  
 public | c    | table | postgres | 34 GB |   
(1 row)  
```

3、并行度设置

```output
set max_parallel_workers_per_gather =56;  
set min_parallel_table_scan_size =0;  
set min_parallel_index_scan_size =0;  
set parallel_tuple_cost =0;  
set parallel_setup_cost =0;  
  
alter table c set (parallel_workers =56);  
  
explain select c1, count(*), avg(id), min(id), max(id), sum(id) from c group by c1;  
  
postgres=# explain select c1, count(*), avg(id), min(id), max(id), sum(id) from c group by c1;  
                                          QUERY PLAN                                             
-----------------------------------------------------------------------------------------------  
 Finalize GroupAggregate  (cost=4871557.23..4871656.48 rows=100 width=60)  
   Group Key: c1  
   ->  Sort  (cost=4871557.23..4871571.23 rows=5600 width=60)  
         Sort Key: c1  
         ->  Gather  (cost=4871207.60..4871208.60 rows=5600 width=60)  
               Workers Planned: 56  
               ->  Partial HashAggregate  (cost=4871207.60..4871208.60 rows=100 width=60)  
                     Group Key: c1  
                     ->  Parallel Seq Scan on c  (cost=0.00..4603350.44 rows=17857144 width=8)  
(9 rows)  
```

4、聚合耗时，11.6秒

```output
select c1, count(*), avg(id), min(id), max(id), sum(id) from c group by c1;  
  
postgres=# select c1, count(*), avg(id), min(id), max(id), sum(id) from c group by c1;  
 c1 |  count   |        avg         | min |    max    |       sum          
----+----------+--------------------+-----+-----------+------------------  
  0 |  5048622 | 500110433.47564959 | 263 | 999999912 | 2524868536874701  
  1 | 10095409 | 500001476.91771596 | 206 | 999999776 | 5047719410088402  
  2 | 10098707 | 500027259.27800460 | 119 | 999999881 | 5049628783461600  
  3 | 10101198 | 499940146.38158266 |  30 | 999999883 | 5049994406749350  
  4 | 10094812 | 499884810.73452760 |  37 | 999999815 | 5046243186020638  
  5 | 10100558 | 499863342.32527777 |  31 | 999999940 | 5048898681230323   
...
(100 rows)  
  
Time: 11585.315 ms (00:11.585)  
```

5、CPU消耗

```output
top - 17:25:46 up 30 days, 22:13,  2 users,  load average: 4.78, 1.92, 2.38  
Tasks: 522 total,  58 running, 464 sleeping,   0 stopped,   0 zombie  
%Cpu(s): 96.1 us,  3.6 sy,  0.0 ni,  0.0 id,  0.3 wa,  0.0 hi,  0.0 si,  0.0 st  
KiB Mem : 23094336+total, 79267856 free,  2570612 used, 14910489+buff/cache  
KiB Swap:        0 total,        0 free,        0 used. 15125537+avail Mem  
```

