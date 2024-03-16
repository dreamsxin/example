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

```sql
CREATE TABLE t_oil (
region
text,
country
text,
year
int,
production
int,
consumption int
);
```
