# InfluxDB

InfluxDB（时序数据库）（influx，[ˈɪnflʌks]，流入，涌入），常用的一种使用场景：监控数据统计。
每毫秒记录一下电脑内存的使用情况，然后就可以根据统计的数据，利用图形化界面（InfluxDB V1 一般配合 Grafana）制作内存使用情况的折线图；可以理解为按时间记录一些数据（常用的监控数据、埋点统计数据等），然后制作图表做统计；

InfluxDB自带的各种特殊函数如求标准差，随机取样数据，统计数据变化比等，使数据统计和实时分析变得十分方便，适合用于包括 DevOps 监控，应用程序指标，物联网传感器数据和实时分析的后端存储。类似的数据库有Elasticsearch、Graphite等。

## 特性

- Time Series （时间序列）：你可以使用与时间有关的相关函数（如最大，最小，求和等）
- Metrics（度量）：你可以实时对大量数据进行计算
- Eevents（事件）：它支持任意的事件数据

## 特点

- 为时间序列数据专门编写的自定义高性能数据存储。 TSM引擎具有高性能的写入和数据压缩
- Golang编写，没有其它的依赖
- 提供简单、高性能的写入、查询 http api，Native HTTP API, 内置http支持，使用http读写
- 插件支持其它数据写入协议，例如 graphite、collectd、OpenTSDB
- 支持类sql查询语句
- tags可以索引序列化，提供快速有效的查询
- Retention policies自动处理过期数据
- Continuous queries自动聚合，提高查询效率
- schemaless(无结构)，可以是任意数量的列
- Scalable可拓展
- min, max, sum, count, mean,median 一系列函数，方便统计
- Built-in Explorer 自带管理工具

## 基础概念

- database：数据库；
- measurement：数据库中的表；
- points：表里面的一行数据。

Point 由时间戳（time）、数据（field）和标签（tags）组成。
field 必须存在。因为 field 是没有索引的。如果使用字段作为查询条件，会扫描符合查询条件的所有字段值，性能不及tag。
tags 是可选的，但是强烈建议你用上它，因为 tag 是有索引的，tags 相当于SQL中的有索引的列。tag value 只能是 string 类型。

|   MySQL  | InfluxDB            |
| -------- | ------------------- |
| database | database            |
| table    | measurement         |
| column   | timestemp/field/tag |

## 常用命令

* 显示所有数据库
`show databases`

* 创建数据库
`create database test`

* 使用某个数据库
`use test`

* 显示所有表
`show measurements`

* 新建表和插入数据
语法：`insert + measurement + "," + tag=value,tag=value + 空格 + field=value,field=value timestamp`
增加第一条数据时，会自动建立表
`insert test_test,tag1=index1,tag12=index2 field1=1,field2=2`
`insert test_test,tag1=index1,tag12=index2 field1=1,field2=2 1564150279123000000`

* 查看表字段类型
field 有四种类型，int, float, string, boolean
`show field keys from test_test`

* 设置一下时间显示格式
`precision rfc3339`

* 查询数据
表名有点号时，输入双引号
`select * from test_test"

* 显示所有用户
`show users`

* 新增通用户
`create user "user" with password 'user'`

* 新增管理员用户
`create user "admin" with password 'admin' with all privileges`

* 删除用户
`drop user "user"`

