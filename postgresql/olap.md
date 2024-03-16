# OLAP (On-line Analytical Processing)
联机分析处理是一种用于分析和查询大规模数据集的计算机处理技术。

帮助用户从不同角度和层次上对数据进行分析和查询，侧重分析决策。

**BI模块**
数据可视化组件。为用户提供拖拽式的数据分析操作页面，以及各种报表、图表的展示。本质上它就类似一个客户端，通过配置连接到各种OLAP引擎上（可以直白的理解为各种DB）来完成数据的分析和查询。通常一种BI组件都支持若干种OLAP引擎，比如superset可以连接clickhouse、druid、mysql等。

**OLAP引擎**
大数据存储引擎。可以直白的理解为就是存储数据的地方，但是不同引擎的存储数据原理、支持数据量级、查询性能、适用场景等都有很大区别。引擎的选择也决定着底层数仓的建设选型，比如你选择了clickhouse，那数仓最终也一定是基于clickhouse的建表原则去建若干ck表。

**数据仓库**
数据建设。初级的理解就是建表，将业务数据、日志数据、消息队列数据等，通过各种调度任务写入到表里供OLAP引擎使用。但要想建好数仓也是一个复杂、庞大的工程，比如要考虑：数据清洗、数据建模（星型模型、雪花模型、宽表模型、主题、维度、指标）、数据时效性（实时、T+1）、延迟容错、机器资源等。

## 常见操作

### 上卷（Roll Up）/聚合
选定某些维度，根据这些维度来聚合事实，如果用SQL来表达就是 `select dim_a, aggs_func(fact_b) from fact_table group by dim_a`.
e.g. 风控场景：点赞场景，决策REJECT，UV多少

### 下钻（Drill Down）
上卷和下钻是相反的操作。它是选定某些维度，将这些维度拆解出小的维度（如年拆解为月，省份拆解为城市），之后聚合事实。
e.g. 风控场景：点赞场景，命中规则A，决策REJECT，UV多少

### 切片（Slicing、Dicing）：选定某些维度，并根据特定值过滤这些维度的值，将原来的大 Cube 切成小cube。如`dim_a in ('CN', 'USA')`
e.g. 风控场景：点赞场景，命中规则A、规则B，规则C，决策REJECT，UV是多少

### 旋转（Pivot/Rotate）：
维度位置的互换

## OLAP分类

### a. Relational OLAP
ROLAP提倡存储明细数据，适用于查询模式不固定，查询灵活性较高的场景。ROLAP的代表：Clickhosue、Presto、Doris、Hive、SpartSQL、FlinkSQL、Impala、GreenPlum等。
- 优势：
ROLAP收到Query请求时，会先解析Query，生成执行计划，扫描数据，执行关系型算子，在原始数据上做过滤(Where)、聚合(Sum, Avg, Count)、关联(Join)，分组（Group By)、排序（Order By）等，最后将结算结果返回给用户，整个过程都是即时计算，没有预先聚合好的数据可供优化查询速度，拼的都是资源和算力的大小。
ROLAP不需要进行预聚合，因此查询的灵活性、可扩展性更好。
使用成本低，与关系型数据库一脉相承，”群众基础“好。
- 劣势：
当数据量较大（时间跨度大）、查询较复杂（多表join）时，查询性能较差，因为所有的计算都是即时触发（没有预处理），会消耗更多的计算资源。

### b. Multi-dimensional OLAP
MOLAP 提倡存储预聚合数据，适用于查询模式固定并且对查询性能要求较高的场景。MOLAP 的代表主要有 Druid、Kylin。MOLAP一般会根据用户事先定义的数据维度、指标在数据写入时就计算好预聚合数据，这样在Qeury到来时，直接查预聚合数据即可，无需基于明细数据实时计算，性能提升明显。
- 优势：
查询性能极好：数据写入时即完成了预聚合，避免了查询过程中的大量计算，极大提升查询性能。
- 劣势：
灵活性较差：因为预先定义了聚合维度和指标，所以只能用于较固定的查询模式，不支持明细查询。
存储成本较高：不同维度、指标的预聚合会产生冗余数据，增大存储成本。

### c. Hybrid OLAP
HOLAP 即混合OLAP，是MOLAP和ROLAP的一种融合，兼顾查询性能与灵活性。当查询固定模式的聚合数据时使用MOLAP技术，当查询明细数据或较灵活场景使用ROLAP技术。
- 优势：
兼顾查询性能、灵活性、存储空间
- 劣势：
系统结构复杂：要同时支持ROLAP和MOLAP，BI组件、OLAP引擎、数仓建设都要做好融合，尤其是底层数据的建模也比较复杂。
适用场景少：与其提高系统复杂度，很多用户宁愿选择拆分为两类（ROLAP和MOLAP）单独实现，使用HOLAP的场景较少。

### d. Other
除此之外，还包含一些其他分类，包括启用Web的OLAP（WOLAP），桌面OLAP（DOLAP），移动OLAP（MOLAP）和空间OLAP（SOLAP）。但总体上不太流行，故此不再进行介绍。

## OLA P主流框架对比

### Clickhouse
ClickHouse由俄罗斯第一大搜索引擎Yandex于2016年6月发布，开发语言为C++，是一个面向联机分析处理（OLAP）的开源的面向列式存储的DBMS，简称CK，与Hadoop、Spark这些巨无霸组件相比，ClickHouse很轻量级，查询性能非常好。目前国内社区火热，各个大厂纷纷跟进大规模使用。

### Presto
Presto 是 Facebook 推出分布式SQL交互式查询引擎，采用MPP架构，完全基于内存的并行计算。Presto比Hive快的原因就在于不在落盘，而是全内存操作。Presto在支持的SQL计算上更加通用，更适合ad-hoc查询场景，然而这些通用系统往往比专用系统更难做性能优化，所以不太适合做对查询QPS(参考值QPS > 1000)、延迟要求比较高(参考值search latency < 500ms)的在线服务，更适合做公司内部的查询服务和加速Hive查询的服务。

### Doris/StarRocks
Doris是由百度开源的一款MPP数据库，实现了MySQL协议，集成Google Mesa 和Apache Impala 的技术。DorisDB是基于 Apache Doris 做的闭源商业化产品，后该产品又基于Elastic License 2.0开源并更名为StarRocks。

### Spark SQL
Spark是UC Berkeley AMP lab开源的类MapReduce的通用的并行计算框架。SparkSQL 是 Spark 处理结构化数据的模块。本质上也是基于 DAG (有向无环图，Directed Acyclic Graph的缩写，常用于建模) 的 MPP。

### Kylin
Kylin 是2014年由 eBay 中国研发中心开源的OLAP引擎，提供 Hadoop/Spark 之上的 SQL 查询接口及多维分析能力以支持超大规模数据，它能在亚秒内查询巨大的Hive表。其核心技术点在于预计算和Cube（立方体模型）的设置：首先， 对需要分析的数据进行建模，框定需要分析的维度字段；然后通过预处理的形式，对各种维度进行组合并事先聚合；最后，将聚合结果以某种索引或者缓存的形式保存起来（通常只保留聚合后的结果，不存储明细数据）。这样一来，在随后的查询过程中，就可以直接利用结果返回数据。

### Druid
Druid是由广告公司 MetaMarkets 于2012年开源的实时大数据分析引擎。Druid 作为MOLAP引擎，也是对数据进行预聚合。只不过预聚合的方式与Kylin不同，Kylin是Cube化，Druid的预聚合方式只是全维度进行Group-by，相当于是Kylin Cube 的 base cuboid。Druid 支持低延时的数据摄取，灵活的数据探索分析，高性能的数据聚合，简便的水平扩展。Druid支持更大的数据规模，具备一定的预聚合能力，通过倒排索引和位图索引进一步优化查询性能，在广告分析场景、监控报警等时序类应用均有广泛使用

## 关于MPP、Map Reduce、MPPDB
MPP (Massively Parallel Processing)、MapReduce 和 MPPDB（Massively Parallel Processing Database）是三个不同的概念和技术，三者定义如下：

MPP（Massively Parallel Processing）：MPP是一种并行计算架构，用于处理大规模数据。MPP系统将数据分割成多个分片，每个分片由独立的计算节点处理。每个节点负责处理自己的数据分片，然后将结果合并。这种并行计算架构可以提高数据处理的速度和效率。

MapReduce：MapReduce是一种编程模型和分布式计算框架，用于处理大规模数据集。它由两个主要阶段组成：Map阶段和Reduce阶段。在Map阶段，数据被分割成多个小块，每个小块由独立的计算节点进行处理，并生成中间结果。在Reduce阶段，中间结果被合并和聚合，生成最终的结果。MapReduce适用于批量处理大规模数据，如日志分析、离线数据处理等。

MPPDB（Massively Parallel Processing Database）：MPPDB是一种基于MPP架构的分布式数据库系统。它将数据分割成多个分片存储在不同的节点上，并利用并行计算能力进行查询和分析。MPPDB具有高度可扩展性和并行处理能力，可以处理大规模数据集，并提供高性能的查询和分析功能。

## 使用 PostgreSQL 做 OLAP

### 聚簇表
### 在建立索引时允许添加数据
```sql
CREATE INDEX CONCURRENTLY idx_name2 ON t_test (name);
```
### 操作符类
一个操作符类将会告诉一个索引应该怎么运转。让我们看一个标准的二叉树。它可以执行 5 种操作：<、<=、=、>=、>。
```pgsql
\h CREATE OPERATOR
Command: CREATE OPERATOR
Description: define a new operator
Syntax:
CREATE OPERATOR name (
PROCEDURE = function_name
    [, LEFTARG = left_type ] [, RIGHTARG = right_type ]
    [, COMMUTATOR = com_op ] [, NEGATOR = neg_op ]
    [, RESTRICT = res_proc ] [, JOIN = join_proc ]
    [, HASHES ] [, MERGES ]
)
```
基本上，其中的概念是这样的：一个操作符调用一个函数，函数会得到一个或者两个参数，一个是操作符的左参数而另一个是操作符的右参数。

对于一些特殊数组，需要固定排序顺序，就需要自定义操作符。
```pgsql
CREATE TABLE t_sva (sva text);
INSERT INTO t_sva VALUES ('1118090878');
INSERT INTO t_sva VALUES ('2345010477');

CREATE OR REPLACE FUNCTION normalize_si(text) RETURNS text AS $$
    BEGIN
        RETURN substring($1, 9, 2) ||
        substring($1, 7, 2) ||
        substring($1, 5, 2) ||
        substring($1, 1, 4);
    END; $$
LANGUAGE 'plpgsql' IMMUTABLE;

SELECT normalize_si('1118090878');
normalize_si
--------------
7808091118
(1 row)

-- lower equals
CREATE OR REPLACE FUNCTION si_le(text, text) RETURNS boolean AS $$
    BEGIN
        RETURN normalize_si($1) <= normalize_si($2);
    END;
$$ LANGUAGE 'plpgsql' IMMUTABLE;

-- greater equal
CREATE OR REPLACE FUNCTION si_ge(text, text) RETURNS boolean AS $$
    BEGIN
        RETURN normalize_si($1) >= normalize_si($2);
    END;
$$ LANGUAGE 'plpgsql' IMMUTABLE;

-- greater
CREATE OR REPLACE FUNCTION si_gt(text, text) RETURNS boolean AS $$
    BEGIN
        RETURN normalize_si($1) > normalize_si($2);
    END;
$$ LANGUAGE 'plpgsql' IMMUTABLE;

-- define operators
CREATE OPERATOR <# ( PROCEDURE=si_lt, LEFTARG=text, RIGHTARG=text);
CREATE OPERATOR <=# ( PROCEDURE=si_le, LEFTARG=text, RIGHTARG=text);
CREATE OPERATOR >=# ( PROCEDURE=si_ge, LEFTARG=text, RIGHTARG=text);
CREATE OPERATOR ># ( PROCEDURE=si_gt, LEFTARG=text, RIGHTARG=text);

CREATE OR REPLACE FUNCTION si_same(text, text) RETURNS int AS $$
    BEGIN
        IF normalize_si($1) < normalize_si($2) THEN
            RETURN -1;
        ELSIF normalize_si($1) > normalize_si($2) THEN
            RETURN +1;
        ELSE
            RETURN 0;
    END IF;
END;
$$ LANGUAGE 'plpgsql' IMMUTABLE;

-- define class
CREATE OPERATOR CLASS sva_special_ops
FOR TYPE text USING btree
AS
    OPERATOR 1 <# ,
    OPERATOR 2 <=# ,
    OPERATOR 3 = ,
    OPERATOR 4 >=# ,
    OPERATOR 5 ># ,

    FUNCTION 1 si_same(text, text);
CREATE INDEX idx_special ON t_sva (sva sva_special_ops);
```
上面的代码所做的也不过是交换了一些数位，现在就可以使用普通的字符串排序顺序了。

### 高级 SQL

**装载一些案例数据**
```sql
CREATE TABLE t_oil (
    region text,
    country text,
    year int,
    production int,
    consumption int
);
```

用 curl 可以直接从我们的网站下载这一测试数据：
```psql
COPY t_oil FROM PROGRAM ' curl www.cybertec.at/secret/oil_ext.txt ';
```

**应用分组集 GROUP BY**
这份数据中包含世界上两个地区共计 14 个国家在 1965 年到 2010 年间的数据：
```psql
SELECT region, avg(production) FROM t_oil GROUP BY region;
region | avg
---------------+-----------------------
Middle East   | 1992.6036866359447005
North America | 4541.3623188405797101
(2 rows)
```
`GROUP BY` 子句将把很多行转变成每一组一行。

**附加总体的平均值 ROLLUP**
不过，如果是在做现实生活中的报表，用户可能还对总体的平均值感兴趣。因此有时可能需要一个附加的行。
可以这样来做：
```psql
SELECT region, avg(production) FROM t_oil GROUP BY ROLLUP (region);
region         | avg
---------------+-----------------------
Middle East    | 1992.6036866359447005
North America  | 4541.3623188405797101
               | 2607.5139860139860140      -- 总体的平均值
(3 rows)

SELECT region, country, avg(production) FROM t_oil WHERE country
IN ('USA', 'Canada', 'Iran', 'Oman') GROUP BY ROLLUP (region, country);
region | country | avg
---------------+---------+-----------------------
Middle East    | Iran   | 3631.6956521739130435
Middle East    | Oman   | 586.4545454545454545
Middle East    |        | 2142.9111111111111111      -- 针对 Middle East 总体的平均值
North America  | Canada | 2123.2173913043478261
North America  | USA    | 9141.3478260869565217
North America  |        | 5632.2826086956521739      -- 针对 North America 总体的平均值
               |        | 3906.7692307692307692      -- 总体的平均值
(7 rows)
```

**附加总体的平均值 CUBE**
CUBE 创建的数据是：GROUP BY region, country + GROUP BY region + GROUP BY country + 总体均值。
```psql
SELECT region, country, avg(production) FROM t_oil WHERE country IN ('USA', 'Canada', 'Iran', 'Oman') GROUP BY CUBE (region, country);
region         | country | avg
---------------+---------+-----------------------
Middle East    | Iran    | 3631.6956521739130435
Middle East    | Oman    | 586.4545454545454545
Middle East    |         | 2142.9111111111111111      -- 针对 Middle East 总体的平均值
North America  | Canada  | 2123.2173913043478261
North America  | USA     | 9141.3478260869565217
North America  |         | 5632.2826086956521739      -- 针对 North America 总体的平均值
               |         | 3906.7692307692307692      -- 总体的平均值
               | Canada  | 2123.2173913043478261      -- 针对 Canada 总体的平均值
               | Iran    | 3631.6956521739130435      -- 针对 Iran 总体的平均值
               | Oman    | 586.4545454545454545       -- 针对 Oman 总体的平均值
               | USA     | 9141.3478260869565217      -- 针对 USA 总体的平均值
(11 rows)
```

**GROUPING SETS**
ROLLUP 和 CUBE 实际只是在 `GROUPING SETS` 子句之上的便利特性。

通过 `GROUPING SETS` 子句，读者可以明确地列出想要的聚集：
```psql
SELECT region, country, avg(production) FROM t_oil WHERE country IN ('USA', 'Canada', 'Iran', 'Oman') GROUP BY GROUPING SETS ( (), region, country);
region         | country | avg
---------------+---------+-----------------------
Middle East    |         | 2142.9111111111111111
North America  |         | 5632.2826086956521739
               |         | 3906.7692307692307692
               | Canada  | 2123.2173913043478261
               | Iran    | 3631.6956521739130435
               | Oman    | 586.4545454545454545
               | USA     | 9141.3478260869565217
(7 rows)
```
在这里，我们得到了 3 个分组集：总体均值、GROUP BY region 和 GROUP BY country。如果读者想要组合 region 和 country，可以使用(region, country)。

**FILTER 子句**
```psql
SELECT region, avg(production) AS all, avg(production) FILTER (WHERE year < 1990) AS old, avg(production) FILTER (WHERE year >= 1990) AS new FROM t_oil GROUP BY ROLLUP (region);
region         | all            | old            | new
---------------+----------------+----------------+----------------
Middle East    | 1992.603686635 | 1747.325892857 | 2254.233333333
North America  | 4541.362318840 | 4471.653333333 | 4624.349206349
               | 2607.513986013 | 2430.685618729 | 2801.183150183
(3 rows)
```

**有序集WITHIN GROUP**

percentile_disc 将跳过该组的 50% 并且返回想要的值。
percentile_cont 将在找不到精确匹配时插值。

```psql
SELECT region, percentile_disc(0.5) WITHIN GROUP (ORDER BY production) FROM t_oil GROUP BY 1;
region         | percentile_disc
---------------+-----------------
Middle East    | 1082
North America  | 3054
(2 rows)

SELECT region,
percentile_disc(0.5) WITHIN GROUP (ORDER BY production) FROM t_oil GROUP BY ROLLUP (1);
region         | percentile_disc
---------------+-----------------
Middle East    | 1082
North America  | 3054
               | 1696
(3 rows)
```
查询最经常出现的产量：
```psql
SELECT country, mode() WITHIN GROUP (ORDER BY production) FROM t_oil WHERE country = 'Other Middle East' GROUP BY 1;
country            | mode
-------------------+------
Other Middle East  | 48
```
假想产量为 9000 数据排名：
```psql
SELECT region, rank(9000) WITHIN GROUP (ORDER BY production DESC NULLS LAST) FROM t_oil GROUP BY ROLLUP (1);
region         | rank
---------------+------
Middle East    | 21
North America  | 27
               | 47
(3 rows)
```

### 窗口函数 OVER
OVER 子句定义我们正在查看的窗口。
**总体均值**
```psql
SELECT country, year, production, consumption, avg(production) OVER() FROM t_oil LIMIT 4;
country  | year | production | consumption | avg
---------+------+------------+-------------+----------
USA      | 1965 | 9014       | 11522       | 2607.5139
USA      | 1966 | 9579       | 12100       | 2607.5139
USA      | 1967 | 10219      | 12567       | 2607.5139
USA      | 1968 | 10600      | 13405       | 2607.5139
(4 rows)
```
**划分数据**
在这个例子中窗口是行所属的国家，就是按照国家来划分数据。换句话说，该查询返回行与其所在国家的所有行的对比。
```psql
SELECT country, year, production, consumption, avg(production) OVER (PARTITION BY country) FROM t_oil;
country          | year | production | consumption | avg
-----------------+------+------------+-------------+-----------
Canada           | 1965 | 920        | 1108        | 2123.2173
Canada           | 2010 | 3332       | 2316        | 2123.2173
Canada           | 2009 | 3202       | 2190        | 2123.2173
...
Iran             | 1966 | 2132       | 148         | 3631.6956
Iran             | 2010 | 4352       | 1874        | 3631.6956
Iran             | 2009 | 4249       | 2012        | 3631.6956
```
PARTITION BY 子句可以接受任何表达式。通常大部分人会使用一个列来划分数据，例如按照 year < 1990 划分为两种值：true 和 false：
```psql
SELECT year, production, avg(production) OVER (PARTITION BY year < 1990) FROM t_oil WHERE country = 'Canada' ORDER BY year;
year  | production | avg
------+------------+-----------------------
1965  | 920        | 1631.6000000000000000
1966  | 1012       | 1631.6000000000000000
...
1990  | 1967       | 2708.4761904761904762
1991  | 1983       | 2708.4761904761904762
1992  | 2065       | 2708.4761904761904762
...
从这里可以看到 PostgreSQL 确实很灵活。使用函数来判断分组成员关系在实际应用中并不鲜见。

**在窗口中排序数据**
```psql
SELECT country, year, production, min(production) OVER (PARTITION BY country ORDER BY year) FROM t_oil WHERE year BETWEEN 1978 AND 1983 AND country IN ('Iran', 'Oman');
country | year | production | min
---------+------+------------+------
Iran     | 1978 | 5302       | 5302
Iran     | 1979 | 3218       | 3218
Iran     | 1980 | 1479       | 1479
Iran     | 1981 | 1321       | 1321
Iran     | 1982 | 2397       | 1321
Iran     | 1983 | 2454       | 1321
Oman     | 1978 | 314        | 314
Oman     | 1979 | 295        | 295
Oman     | 1980 | 285        | 285
Oman     | 1981 | 330        | 285
```
如果使用的聚集不带 ORDER BY，它将自动地取窗口中整个数据集的最小值。而有ORDER BY 时就不是这样：它将总是给定顺序中到目前为止的最小值。

```psql
SELECT country, year, production, min(production) OVER (), min(production) OVER (ORDER BY year) FROM t_oil WHERE year BETWEEN 1978 AND 1983 AND country = 'Iran';
country  | year | production | min  | min
---------+------+------------+------+------
Iran     | 1978 | 5302       | 1321 | 5302
Iran     | 1979 | 3218       | 1321 | 3218
Iran     | 1980 | 1479       | 1321 | 1479
Iran     | 1981 | 1321       | 1321 | 1321
Iran     | 1982 | 2397       | 1321 | 1321
Iran     | 1983 | 2454       | 1321 | 1321
(6 rows)
```
**滑动窗口**
到目前为止，我们所使用的窗口都是静态的。然而，对于移动平均这类计算来说这还不够。移动聚集需要一个滑动窗口，滑动窗口会随着数据的处理而移动。
最重要的事情是移动窗口应该与 ORDER BY 子句一起使用，否则就会有大量问题。
PostgreSQL 实际上会接受那样的查询，但是其结果会是彻头彻尾的垃圾。记住，将事先没有排序的数据交给一个滑动窗口只会导致随机数据。
“ROWS BETWEEN 1 PRECEDING and 1 FOLLOWING 1”定义了窗口。在笔者的例子中，被使用的最多有 3 行：当前行、当前行的前一行以及当前行的后一行。
```psql
SELECT country, year, production, min(production) OVER (PARTITION BY country ORDER BY year ROWS BETWEEN 1 PRECEDING AND 1 FOLLOWING) FROM t_oil WHERE year BETWEEN 1978 AND 1983 AND country IN ('Iran', 'Oman');
country  | year | production | min
---------+------+------------+------
Iran     | 1978 | 5302       | 3218
Iran     | 1979 | 3218       | 1479
Iran     | 1980 | 1479       | 1321
Iran     | 1981 | 1321       | 1321
Iran     | 1982 | 2397       | 1321
Iran     | 1983 | 2454       | 2397
Oman     | 1978 | 314        | 295
Oman     | 1979 | 295        | 285
Oman     | 1980 | 285        | 285
Oman     | 1981 | 330        | 285
Oman     | 1982 | 338        | 330
Oman     | 1983 | 391        | 338
(12 rows)
```
为了展示滑动窗口如何工作，试试下面的例子：
```psql
SELECT *, array_agg(id) OVER (ORDER BY id ROWS BETWEEN 1 PRECEDING AND 1 FOLLOWING) FROM generate_series(1, 5) AS id;
id  | array_agg
----+-----------
1   | {1,2}
2   | {1,2,3}
3   | {2,3,4}
4   | {3,4,5}
5   | {4,5}
(5 rows)
```
- rank 和 dense_rank 函数
rank 列将会对那些行编号。有可能会有相等的。如果想避免这样的事情，可以使用 dense_rank 函数。
- ntile 函数
一些应用要求把数据划分成完美相等的分组，ntile 函数可以做到这一点。
下面的例子展示了把数据划分成 4 个分组：
```psql
SELECT year, production, ntile(4) OVER (ORDER BY production) FROM t_oil WHERE country = 'Iraq' AND year BETWEEN 2000 AND 2006;
year  | production | ntile
------+------------+-------
2003  | 1344       | 1
2005  | 1833       | 1
2006  | 1999       | 2
2004  | 2030       | 2
2002  | 2116       | 3
2001  | 2522       | 3
2000  | 2613       | 4
(7 rows)
```
- lead 和 lag 函数
lead 和 lag 函数可以在结果集内移动行。一种典型的用例是计算一年和接下来一年的产量差：
```sql
SELECT year, production, lag(production, 1) OVER (ORDER BY year) FROM t_oil WHERE country = 'Mexico' LIMIT 5;
year  | production | lag
------+------------+-----
1965  | 362        |
1966  | 370        | 362
1967  | 411        | 370
1968  | 439        | 411
1969  | 461        | 439
(5 rows)

SELECT year, production, production - lag(production, 1) OVER (ORDER BY year) FROM t_oil WHERE country = 'Mexico' LIMIT 3;
year  | production | ?column?
------+------------+----------
1965  | 362        |
1966  | 370        | 8
1967  | 411        | 41
(3 rows)
```

**编写聚集 CREATE AGGREGATE**
```psql
CREATE AGGREGATE name ( input_data_type [ , ... ] ) (
    SFUNC = sfunc,
    STYPE = state_data_type
    [ , FINALFUNC = ffunc ]
    [ , INITCOND = initial_condition ]
    [ , SORTOP = sort_operator ]
)

or the old syntax

CREATE AGGREGATE name (
    BASETYPE = base_type,
    SFUNC = sfunc,
    STYPE = state_data_type
    [ , FINALFUNC = ffunc ]
    [ , INITCOND = initial_condition ]
    [ , SORTOP = sort_operator ]
)
```
参数
- name
要创建的聚合函数名(可以有模式修饰) 。

- input_data_type
该聚合函数要处理的输入数据类型。要创建一个零参数聚合函数，可以使用*代替输入数据类型列表。 （count(*)就是这种聚合函数的一个实例。 ）

- base_type
在以前的CREATE AGGREGATE语法中，输入数据类型是通过basetype参数指定的，而不是写在聚合的名称之后。 需要注意的是这种以前语法仅允许一个输入参数。 要创建一个零参数聚合函数，可以将basetype指定为"ANY"(而不是*)。

- sfunc
将在每一个输入行上调用的状态转换函数的名称。 对于有N个参数的聚合函数，sfunc必须有 +1 个参数，其中的第一个参数类型为state_data_type，其余的匹配已声明的输入数据类型。 函数必须返回一个state_data_type类型的值。 这个函数接受当前状态值和当前输入数据，并返回下个状态值。

- state_data_type
聚合的状态值的数据类型。

- ffunc
在转换完所有输入行后调用的最终处理函数，它计算聚合的结果。 此函数必须接受一个类型为state_data_type的参数。 聚合的输出数据 类型被定义为此函数的返回类型。 如果没有声明ffunc则使用聚合结果的状态值作为聚合的结果，且输出类型为state_data_type。

- initial_condition
状态值的初始设置(值)。 它必须是一个state_data_type类型可以接受的文本常量值。 如果没有声明，状态值初始为 NULL 。

- sort_operator
用于MIN或MAX类型聚合的排序操作符。 这个只是一个操作符名 (可以有模式修饰)。这个操作符假设接受和聚合一样的输入数据类型。

编写一个聚集所需要的第一个部分是一个函数，每一行都会调用它。该函数将接受一个中间值和取自被处理行的数据。这里是一个例子：
```psql
CREATE FUNCTION taxi_per_line (numeric, numeric)
RETURNS numeric AS
$$
BEGIN
    RAISE NOTICE 'intermediate: %, per row: %', $1, $2;
    RETURN $1 + $2*2.2;
END;
$$ LANGUAGE 'plpgsql';

CREATE AGGREGATE taxi_price (numeric)
(
    INITCOND = 2.5,
    SFUNC = taxi_per_line,
    STYPE = numeric
);
```
每一次行程从踏进出租车开始价格就是 2.50 欧元，这由 INITCOND（初始条件）定义，它表示每个分组的起始值。然后对分组中的每一行都调用一个函数，在笔者的例子中这个函数是已经定义好的 taxi_per_line。如你所见，它需要两个参数。第一个参数是一个中间值。那些额外的参数（可能有很多）是用户传递给函数的。

avg(平均)是聚合更复杂一点的例子。它需要两个运行时状态： 输入的总和以及输入的数量。最终结果是通过把两者相除得到的。 平均的典型实现是用一个数组做状态值。比如，内建的avg(float8)实现是这样的：
```psql
CREATE AGGREGATE avg (float8)
(
    sfunc = float8_accum,
    stype = float8[],
    finalfunc = float8_avg,
    initcond = '{0,0,0}'
);
```
float8_accum 要求一个三元素数组，而不是两元素， 因为它累积平方和和输入的总和和计数。这样它就可以在一些除了avg 之外的聚合中使用了。
```psql
SELECT a.aggfnoid::regprocedure AS aggregate,
       a.agginitval AS initial_value,
       a.aggtransfn::regprocedure AS state_transition_function,
       a.aggfinalfn::regprocedure AS final_function
FROM pg_aggregate AS a
   JOIN pg_proc AS p
      ON a.aggfnoid = p.oid
WHERE p.prokind = 'a'
  AND p.proname = 'stddev_pop';

          aggregate           │ initial_value │             state_transition_function             │            final_function             
══════════════════════════════╪═══════════════╪═══════════════════════════════════════════════════╪═══════════════════════════════════════
 stddev_pop(bigint)           │ ∅             │ int8_accum(internal,bigint)                       │ numeric_stddev_pop(internal)
 stddev_pop(integer)          │ ∅             │ int4_accum(internal,integer)                      │ numeric_poly_stddev_pop(internal)
 stddev_pop(smallint)         │ ∅             │ int2_accum(internal,smallint)                     │ numeric_poly_stddev_pop(internal)
 stddev_pop(real)             │ {0,0,0}       │ float4_accum(double precision[],real)             │ float8_stddev_pop(double precision[])
 stddev_pop(double precision) │ {0,0,0}       │ float8_accum(double precision[],double precision) │ float8_stddev_pop(double precision[])
 stddev_pop(numeric)          │ ∅             │ numeric_accum(internal,numeric)                   │ numeric_stddev_pop(internal)
(6 rows)

select rank() over (partition by 1 order by aggtransfn,agginitval),
           row_number() over (partition by aggtransfn,agginitval order by aggfnoid) rn,
           aggfnoid,aggtransfn,agginitval from pg_aggregate ;
```
```c
Datum
float8_stddev_pop(PG_FUNCTION_ARGS)
{
    ArrayType  *transarray = PG_GETARG_ARRAYTYPE_P(0);
    float8     *transvalues;
    float8      N,
                Sxx;

    transvalues = check_float8_array(transarray, "float8_stddev_pop", 3);
    N = transvalues[0];
    /* ignore Sx */
    Sxx = transvalues[2];

    /* Population stddev is undefined when N is 0, so return NULL */
    if (N == 0.0)
        PG_RETURN_NULL();

    /* Note that Sxx is guaranteed to be non-negative */

    PG_RETURN_FLOAT8(sqrt(Sxx / N));
}
```
**滑动窗口聚集的改进**
```psql
-- msfunc 函数将把窗口中的下一行加入中间结果上：
CREATE FUNCTION taxi_msfunc(numeric, numeric)
RETURNS numeric AS
$$
BEGIN
    RAISE NOTICE 'taxi_msfunc called with % and %', $1, $2;
    RETURN $1 + $2;
END;
$$ LANGUAGE 'plpgsql' STRICT;

-- minvfunc 函数将从中间结果中去掉刚移出窗口之外的值：
CREATE FUNCTION taxi_minvfunc(numeric, numeric)
RETURNS numeric AS
$$
BEGIN
    RAISE NOTICE 'taxi_minvfunc called with % and %', $1, $2;
    RETURN $1 - $2;
END;
$$ LANGUAGE 'plpgsql' STRICT;

CREATE AGGREGATE taxi_price (numeric)
(
    INITCOND = 0,
    STYPE = numeric,
    SFUNC = taxi_per_line,
    MSFUNC = taxi_msfunc,
    MINVFUNC = taxi_minvfunc,
    MSTYPE = numeric
);
```
