# WITH

WITH 语句通常被称为通用表表达式（Common Table Expressions）或者CTEs。

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