# WITH

See http://www.postgres.cn/docs/9.4/queries-with.html

WITH 语句通常被称为通用表表达式（Common Table Expressions）或者CTEs。
WITH 语句最基本的功能是把复杂查询语句拆分成多个简单的部分，如下例所示：

```sql
WITH regional_sales AS (
  SELECT region, SUM(amount) AS total_sales
  FROM orders
  GROUP BY region
), top_regions AS (
  SELECT region
  FROM regional_sales
  WHERE total_sales > (SELECT SUM(total_sales)/10 FROM regional_sales
)
SELECT
  region,
  product,
  SUM(quantity) AS product_units,
  SUM(amount) AS product_sales
FROM orders
WHERE region IN (SELECT region FROM top_regions)
GROUP BY region, product;
```
该例中，定义了两个WITH辅助语句，regional_sales和top_regions。前者算出每个区域的总销售量，后者了查出所有销售量占所有地区总销售里10%以上的区域。主语句通过将这个CTEs及订单表关联，算出了顶级区域每件商品的销售量和销售额。

当然，本例也可以不使用CTEs而使用两层嵌套子查询来实现，但使用CTEs更简单，更清晰，可读性更强。
在WITH中使用数据修改语句

文章开头处提到，WITH中可以不仅可以使用 SELECT 语句，同时还能使用 DELETE，UPDATE，INSERT 语句。因此，可以使用 WITH，在一条SQL语句中进行不同的操作，如下例所示。
```sql
WITH moved_rows AS (
  DELETE FROM products
  WHERE
    "date" >= '2010-10-01'
  AND "date" < '2010-11-01'
  RETURNING *
)
INSERT INTO products_log
SELECT * FROM moved_rows;
```
本例通过WITH中的DELETE语句从products表中删除了一个月的数据，并通过RETURNING子句将删除的数据集赋给moved_rows这一CTE，最后在主语句中通过INSERT将删除的商品插入products_log中。

如果 WITH 里面使用的不是 SELECT 语句，并且没有通过 RETURNING 子句返回结果集，则主查询中不可以引用该 CTE，但主查询和 WITH 语句仍然可以继续执行。这种情况可以实现将多个不相关的语句放在一个SQL语句里，实现了在不显式使用事务的情况下保证WITH语句和主语句的事务性，如下例所示。

```sql
WITH d AS (
  DELETE FROM foo
),
u as (
  UPDATE foo SET a = 1
  WHERE b = 2
)
DELETE FROM bar;
```

* WITH 使用注意事项

WITH中 的数据修改语句会被执行一次，并且肯定会完全执行，无论主语句是否读取或者是否读取所有其输出。而 WITH 中的SELECT语句则只输出主语句中所需要记录数。
WITH中使用多个子句时，这些子句和主语句会并行执行，所以当存在多个修改子语句修改相同的记录时，它们的结果不可预测。
 所有的子句所能“看”到的数据集是一样的，所以它们看不到其它语句对目标数据集的影响。这也缓解了多子句执行顺序的不可预测性造成的影响。
如果在一条SQL语句中，更新同一记录多次，只有其中一条会生效，并且很难预测哪一个会生效。
如果在一条SQL语句中，同时更新和删除某条记录，则只有更新会生效。
目前，任何一个被数据修改 CTE 的表，不允许使用条件规则，和 ALSO 规则以及 INSTEAD 规则。


* 用到的表

```sql
CREATE TABLE place
(
  id serial NOT NULL,                    --主键
  name character varying(200),           --地区名
  parent_id bigint,                      --外键  父地区
  CONSTRAINT place_pkey PRIMARY KEY (id) --主键约束
)

CREATE TABLE COMPANY(
   ID INT PRIMARY KEY     NOT NULL,
   NAME           TEXT    NOT NULL,
   AGE            INT     NOT NULL,
   ADDRESS        CHAR(50),
   SALARY         REAL
);

CREATE TABLE COMPANY1(
   ID INT PRIMARY KEY     NOT NULL,
   NAME           TEXT    NOT NULL,
   AGE            INT     NOT NULL,
   ADDRESS        CHAR(50),
   SALARY         REAL
);
```

假设有数据：
```text
id | name  | age | address   | salary
----+-------+-----+-----------+--------
  1 | Paul  |  32 | California|  20000
  2 | Allen |  25 | Texas     |  15000
  3 | Teddy |  23 | Norway    |  20000
  4 | Mark  |  25 | Rich-Mond |  65000
  5 | David |  27 | Texas     |  85000
  6 | Kim   |  22 | South-Hall|  45000
  7 | James |  24 | Houston   |  10000
```

查处所有记录：
```sql
WITH cte AS ( 
	SELECT * FROM COMPANY
) 
SELECT * FROM cte;
```

## WITH RECURSIVE

遞歸或譜係查詢，是一種熱膨脹係數（CTE）的CTE可以引用本身，即一個具有查詢，可以參考自己的輸出。因此名稱遞歸。
从上面的例子可以看出，WITH RECURSIVE语句包含了两个部分：

- non-recursive term（非递归部分），即上例中的union all前面部分
- recursive term（递归部分），即上例中union all后面部分

执行步骤如下：

- 执行non-recursive term。（如果使用的是union而非union all，则需对结果去重）其结果作为recursive term中对result的引用，同时将这部分结果放入临时的working table中
- 重复执行如下步骤，直到working table为空：用working table的内容替换递归的自引用，执行recursive term，（如果使用union而非union all，去除重复数据），并用该结果（如果使用union而非union all，则是去重后的结果）替换working table


* 查询某个地区以及子集下的所有记录

```sql
WITH RECURSIVE r AS ( 
	SELECT * FROM place WHERE id = 2
	UNION ALL 
	SELECT place.* FROM place, r WHERE place.parent_id = r.id 
) 
SELECT * FROM r ORDER BY id;
```

* 查询某个地区以及所有父集合的所有记录

```sql
WITH RECURSIVE r AS ( 
	SELECT * FROM place WHERE id = 8
	UNION ALL 
	SELECT place.* FROM place, r WHERE place.id = r.parent_id 
) 
SELECT * FROM r ORDER BY id;
```

这里 `r` 将会是对 `UNION ALL` 上面一个语句结构的引用

現在，讓我們寫一個查詢使用遞歸關鍵字WITH子句，找到小於20000的薪金總和，如下：

```sql
WITH RECURSIVE t(n) AS (
    VALUES (0)
    UNION ALL
    SELECT SALARY FROM COMPANY WHERE SALARY < 20000
)
SELECT sum(n) FROM t;
```

以上PostgreSQL表會產生以下結果：
```text
  sum
-------
 25000
(1 row)
```

* 将指定数据移动到另外一个表`COMPANY1`，并从原表`COMPANY`中删除

讓我們使用數據修改語句，以及WITH子句編寫一個查詢，如下所示。首先創建一個類似於表公司表COMPANY1。查詢在這個例子中，有效地移動行，從COMPANY到COMPANY1。刪除刪除指定的公司行RETURNING子句返回它們的內容;然後主查詢讀取輸出，並將其插入到COMPANY1 表：

```sql
WITH moved_rows AS (
    DELETE FROM COMPANY WHERE SALARY >= 30000
    RETURNING *
)
INSERT INTO COMPANY1 (SELECT * FROM moved_rows);
```
 以上PostgreSQL的表會產生以下結果：

INSERT 0 3

現在的記錄表COMPANY與COMPANY1 如下： 
```text
testdb=# SELECT * FROM COMPANY;
 id | name  | age |  address   | salary
----+-------+-----+------------+--------
  1 | Paul  |  32 | California |  20000
  2 | Allen |  25 | Texas      |  15000
  3 | Teddy |  23 | Norway     |  20000
  7 | James |  24 | Houston    |  10000
(4 rows)
```
```text
testdb=# SELECT * FROM COMPANY1;
 id | name  | age | address | salary
----+-------+-----+-------------+--------
  4 | Mark  |  25 | Rich-Mond   |  65000
  5 | David |  27 | Texas       |  85000
  6 | Kim   |  22 | South-Hall  |  45000
(3 rows)
```

## 取最新数据，去重
```sql

with cte as
(
    SELECT *,row_number() over(PARTITION by user_id order by trace_time desc) rn FROM public.tg_user_event_reports t1 
) SELECT * FROM cte WHERE rn=1 limit 100 ;
```
