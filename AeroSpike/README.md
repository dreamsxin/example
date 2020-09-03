# Aerospike

Aerospike是一个分布式，高可用的 K-V类型的Nosql数据库。提供类似传统数据库的ACID操作。
AS最大的卖点就是可以存储在SSD上，并且保证和redis相同的查询性能。AS内部在访问SSD屏蔽了文件系统层级，直接访问地址，保证了数据的读取速度。 AS同时支持二级索引与聚合，支持简单的sql操作，相比于其他nosql数据库，有一定优势。

## 基本概念       

* Namespaces                       

AS数据存储的最高层级，类比于传统的数据库的库层级，一个namespace包含记录（records），索引（indexes ）及策略（policies）。

其中策略决定namespace的行为，包括：
1.数据的存储位置是内存还是SSD。
2.一条记录存储的副本个数。
3.过期时间（TTL）：不同redis的针对key设置TTL，AS可以在库的层级进行全局设置，并且支持对于已存在的数据进行TTL的设置，方便了使用。

* Set   
存储于namespace，是一个逻辑分区，类比于传统数据库的表。set的存储策略继承自namespace，也可以为set设置单独的存储策略

* Records
类比于传统数据库的行，包含key，Bins（value）,和Metadata（元数据）。key全局唯一，作为K-V数据库一般也是通过key去查询。Bins相当于列，存储具体的数据。元数据存储一些基本信息，例如TTL等。

* Key
提到key，有一个和key伴生的概念是摘要（Digests），当key被存入数据库，key与set信息一起被哈希化成一个160位的摘要。数据库中，摘要为所有操作定位记录。key主要用于应用程序访问，而摘要主要用于数据库内部查找记录.
Metadata

每一条记录包含以下几条元数据

1.generation（代）：表示记录被修改的次数。该数字在程序度数据时返回，用来确认正在写入的数据从最后一次读开始未被修改过。
2.time-to-live（TTL）：AS会自动根据记录的TTL使其过期。每次在对象上执行写操作TTL就会增加。3.10.1版本以上，可以通过设置策略，使更新记录时不刷新TTL。
3.last-update-time （LUT）：上次更新时间，这是一个数据库内部的元数据，不会返回给客户端。

* Bins
在一条记录里，数据被存储在一个或多个bins里，bins由名称和值组成。bins不需要指定数据类型，数据类型由bins中的值决定。动态的数据类型提供了很好的灵活性。AS中每条记录可以由完全不同的bins组成。记录无模式，你可以记录的任何生命周期增加或删除bins。
在一个库中bins的名称最多包含32k，这是由内部字符串优化所致。（相比于HBase支持几百万列还是有一定差距，如果想直接将HBase表迁移到AS可能需要重新设计存储结构）

## 常用操作命令

通过 `aql --help` 可以查看系统提供的帮助文档，很详细，里面还有很多DEMO语句。然后通过`aql -h ip -p port` 连上服务器（默认端口 3000）

* 查询 namespace
```shell
aql> show namespaces ;
```

* 插入一条测试数据，demo为as中的set,系统会帮我们创建；类似于mysql中的table
```shell
aql> INSERT INTO testMemory.demo (PK, pass, name) VALUES ('key1', 123, 'abc')
```

* bins列相当于mysql table中的column
```shell
aql> show bins ;
```

* sets 相当于mysql中的show tables
```shell
aql> show sets ;
```

* 查询该ns下的所有sets
```shell
aql> select * from testMemory 
```

* 对上面的相比，该查询明确指定了set，查询数据
```shell
aql> select * from testMemory.demo
```

* 根据PK查询，AS还支持索引查询，但不支持不带索引的bin查询
```shell
aql> select * from testMemory.demo where PK='key1'
```

* 根据PK删除记录，AS中的更新其实就是覆盖
```shell
aql> delete from testMemory.demo where PK='key1'
```

* 创建索引
```shell
aql>  CREATE INDEX idx_demo1_name ON testMemory.demo1 (name) STRING
aql> select * from testMemory.demo1 where name='abc'
```
