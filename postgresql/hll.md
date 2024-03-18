#
HLL是 HyperLogLog数据结构的简称。PostgresSQL通过插件的方式引入了这种新的数据类型hll。HyperLogLog是一个具有固定大小，类似于集合结构，用于可调精度的不同值计数。例如，在1280字节的hll数据结构中，它可以在很小的误差范围内估算出数百亿的不同值计数。

## 安装
```shell
sudo apt-get install postgresql-16-hll
CREATE EXTENSION IF NOT EXISTS hll CASCADE;
```

##

构建数据
```psql
CREATE TABLE helloworld
  (
    id integer,
    set hll
  )
;
--- Insert an empty HLL
INSERT INTO helloworld
  (id,
    set
  )
  VALUES
  (1,
    hll_empty()
  )
;

--- Add a hashed integer to the HLL
UPDATE
  helloworld
SET
set = hll_add(set, hll_hash_integer(12345))
WHERE
  id = 1
;

--- Or add a hashed string to the HLL
UPDATE
  helloworld
SET
set = hll_add(set, hll_hash_text('hello world'))
WHERE
  id = 1
;

--- Get the cardinality of the HLL
SELECT
  hll_cardinality(set)
FROM
  helloworld
WHERE
  id = 1
;
postgres=# SELECT
postgres-#   hll_cardinality(set)
postgres-# FROM
postgres-#   helloworld
postgres-# WHERE
postgres-#   id = 1
postgres-# ;
 hll_cardinality 
-----------------
               2
(1 row)

postgres=# SELECT * FROM helloworld ;
 id |                   set                    
----+------------------------------------------
  1 | x128b7faaebcf97601e5541533f6046eb7f610e
```
从上面的示例中得到，首先构建了一个空的hll集合，然后向该集合中添加了两个值，那么得到的该hll的基数计数就是2。

##

更加实用的用例：

创建网站访问事实表和用户日访问表
```psql
CREATE TABLE facts (
    date            date,
    user_id         integer,
    activity_type   smallint,
    referrer        varchar(255)
);

CREATE TABLE daily_uniques (
    date            date UNIQUE,
    users           hll
);
```
然后给网站访问表中插入过去1000天的访问数据(此处由于没有实际的数据，只能模拟过去1000天的数据)
```psql
[postgres@pgserver ~]$ psql -d postgres -f ~/test.sql
```
查看表
```psql
postgres=# SELECT count(*) FROM facts ;
  count   
----------
 50000000
```
5000万数据

然后根据日期，对user_id进行hash处理，聚合每天用户访问网站的数据到 hll结构中。
```psql
INSERT INTO daily_uniques(date, users)
    SELECT date, hll_add_agg(hll_hash_integer(user_id))
    FROM facts
    GROUP BY 1;
```
查看表数据
```psql
postgres=# SELECT count(*) FROM daily_uniques ;
 count 
-------
  1000
(1 row)
```
只有1000行数据

现在查找一下每天hll的基数计数值
```psql
postgres=# SELECT date, hll_cardinality(users) FROM daily_uniques LIMIT 10;
    date    |  hll_cardinality  
------------+-------------------
 2021-02-06 | 9725.852733707077
 2021-02-21 | 9725.852733707077
 2021-02-02 | 9725.852733707077
 2021-02-08 | 9725.852733707077
 2021-02-10 | 9725.852733707077
 2021-02-03 | 9725.852733707077
 2021-02-14 | 9725.852733707077
 2021-02-22 | 9725.852733707077
 2021-02-11 | 9725.852733707077
 2021-02-20 | 9725.852733707077
```
此刻，可能会想，可以用 COUNT DISTINCT做到基数统计。但是这里只能看到每天多少个唯一身份的用户访问了网站。

### 查看每一周的唯一值呢

HLL可以这样处理
```psql
postgres=# SELECT hll_cardinality(hll_union_agg(users)) FROM daily_uniques WHERE date >= '2018-10-02'::date AND date <= '2018-10-08'::date;
  hll_cardinality  
-------------------
 9725.852733707077
(1 row)
```
### 查看一年中的每个月访问情况
```psql
postgres=# SELECT EXTRACT(MONTH FROM date) AS month, hll_cardinality(hll_union_agg(users))
postgres-# FROM daily_uniques
postgres-# WHERE date >= '2019-01-01' AND
postgres-#       date <  '2020-01-01'
postgres-# GROUP BY 1;
 month |  hll_cardinality  
-------+-------------------
     3 | 9725.852733707077
     7 | 9725.852733707077
     8 | 9725.852733707077
    12 | 9725.852733707077
     5 | 9725.852733707077
    10 | 9725.852733707077
    11 | 9725.852733707077
     9 | 9725.852733707077
     4 | 9725.852733707077
     1 | 9725.852733707077
     2 | 9725.852733707077
     6 | 9725.852733707077
```
等等。因此，可以得到hll可以很好的来计算DV值，并且很快。
