# ZomboDB

https://github.com/zombodb/zombodb

ZomboDB 提供了一套全方位的查询语言，可以供您自由地查询关系型数据。此外，ZomboDB 允许您创建 ZomboDB 类型的索引，此时 ZomboDB 完全接管远程的 Elasticsearch，并负责文本搜索的事务正确性。

ZomboDB 的优势在于允许您直接使用 Elasticsearch 的强大功能而不用处理同步、通信等问题。

## 创建插件

```shell
CREATE EXTENSION zombodb;
SELECT * FROM pg_extension;
```

## 创建表
请注意 long_description 字段的数据类型为 `zdb.fulltext`。
```psql
CREATE TABLE products (
    id SERIAL8 NOT NULL PRIMARY KEY,
    name text NOT NULL,
    keywords varchar(64)[],
    short_summary text,
    long_description zdb.fulltext,
    price bigint,
    inventory_count integer,
    discontinued boolean default false,
    availability_date date
);
```

## 导入数据

```psql
COPY products FROM PROGRAM 'curl https://raw.githubusercontent.com/zombodb/zombodb/master/TUTORIAL-data.dmp';
```

```text
 id |      name      |                     keywords                      |                  short_summary                  |                                              long_description                                              | price | inventory_count | discontinued | availability_date 
----+----------------+---------------------------------------------------+-------------------------------------------------+------------------------------------------------------------------------------------------------------------+-------+-----------------+--------------+-------------------
  1 | Magical Widget | {magical,widget,round}                            | A widget that is quite magical                  | Magical Widgets come from the land of Magicville and are capable of things you can't imagine               |  9900 |              42 | f            | 2015-08-31
  2 | Baseball       | {baseball,sports,round}                           | It's a baseball                                 | Throw it at a person with a big wooden stick and hope they don't hit it                                    |  1249 |               2 | f            | 2015-08-21
  3 | Telephone      | {communication,primitive,"alexander graham bell"} | A device to enable long-distance communications | Use this to call your friends and family and be annoyed by telemarketers.  Long-distance charges may apply |  1899 |             200 | f            | 2015-08-11
  4 | Box            | {wooden,box,"negative space",square}              | Just an empty box made of wood                  | A wooden container that will eventually rot away.  Put stuff it in (but not a cat).                        | 17000 |               0 | t            | 2015-07-01
(4 rows)
```

## 添加 ZomboDB 类型的索引
ZomboDB 索引本质上是一个包含所有列的“covering index”，也就是不需要回表操作的带有所有列数据的索引。
ZomboDB 会自动将被索引的行转换为 Elasticsearch 要的格式（JSON）。

```psql
CREATE INDEX idxproducts ON products
       USING zombodb ((products.*))
        WITH (url='localhost:9200/');
```
还有一些其他索引选项来控制 Elasticsearch 分片和副本的数量。创建ZomboDB 索引时，不仅在 Postgres 中创建了一个索引，还在 Elasticsearch 中创建一个索引。


**说明**
WITH语句后跟随了 Elasticsearch 的地址，该地址指向了一个正在服务的Elasticsearch实例。

## 使用 ZomboDB 格式的查询语句进行查询。

```psql
SELECT *
  FROM products
 WHERE products ==> '(keywords:(sports OR box) OR long_description:"wooden away"~5) AND price:[1000 TO 20000]';
```

```psql
EXPLAIN SELECT * FROM products WHERE products ==> 'sports, box';
```
