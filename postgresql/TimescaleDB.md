# TimescaleDB

TimescaleDB 是 PostgreSQL 数据库的一个插件，通过 `hypertable` 实现对时间序列数据库进行分块，通过把大表分为多个小表的方式，新的数据总是插入到最新的块上，由于进行了分块，使得索引数据变小，保持索引一直处于>内存当中，不用去磁盘交换数据，因此加快了数据的插入速度。
官方提供了数据库性能测试工具和数据，可参考 https://github.com/timescale/benchmark-postgres。

TimescaleDB 是一款针对快速获取和复杂查询而优化的开源时间序列数据库。它使用“标准的SQL”语句，并且像传统的关系数据库那样容易使用，像nosql那样可扩展。
与这两种替代品（关系数据库与NoSQL）相比,TimescaleDB为时间序列数据集合了两种数据库的优点。

* 易用

- PostgreSQL原生支持的所有SQL,包含完整SQL接口（包括辅助索引，非时间聚合，子查询，JOIN，窗口函数）
- 任何使用PostgreSQL的客户端或工具，可以直接应用到该数据库，不需要更改
- 时间为导向的特性，API功能和相应的优化
- 可靠的数据存储

* 可扩展

- 透明时间/空间分区，用于放大（单个节点）和扩展（即将出现）
- 高数据写入速率（包括批量提交，内存中索引，事务支持，数据备份支持)
- 单个节点上的大小合适的块（二维数据分区），以确保即使在大数据量时即可快速读取
- 块之间和服务器之间的并行操作

* 可靠性

- 作为PostgreSQL的扩展。
- 受益于20多年的PostgreSQL研究的成功经验（包括流式复制，备份）
- 灵活的管理选项（与现有的PostgreSQL生态系统和工具兼容）

本节的其余部分描述了TimescaleDB架构的设计和动机,包括为什么时间序列数据不同，以及在构建 TimescaleDB 时如何利用其特性。

## 安装

http://docs.timescale.com/latest/getting-started/installation/linux/installation-apt

```shell
# Add our PPA
sudo add-apt-repository ppa:timescale/timescaledb-ppa
sudo apt-get update
# To install
sudo apt install timescaledb-postgresql-10
```

`postgresql.conf`

```conf
shared_preload_libraries = 'timescaledb'
```

```shell
# Restart PostgreSQL instance
sudo service postgresql restart

# Add a superuser postgres:
createuser postgres -s
```

## 配置

```shell
# Connect to PostgreSQL, using a superuser named 'postgres'
psql -U postgres -h localhost
```

```sql
-- Create the database, let's call it 'tutorial'
CREATE database tutorial;

-- Connect to the database
\c tutorial

-- Extend the database with TimescaleDB
CREATE EXTENSION IF NOT EXISTS timescaledb CASCADE;
```

## 创建新表 Creating a (Hyper)table

```sql
-- We start by creating a regular SQL table

CREATE TABLE conditions (
  time        TIMESTAMPTZ       NOT NULL,
  location    TEXT              NOT NULL,
  temperature DOUBLE PRECISION  NULL,
  humidity    DOUBLE PRECISION  NULL
);
```

使用 `create_hypertable` 转换为(Hyper)table：
```sql
-- This creates a hypertable that is partitioned by time
--   using the values in the `time` column.

SELECT create_hypertable('conditions', 'time');

-- OR you can additionally partition the data on another
--   dimension (what we call 'space partitioning').
-- E.g., to partition `location` into 4 partitions:

SELECT create_hypertable('conditions', 'time', 'location', 4);
```

必选参数：
- ​main_table​
Identifier of table to convert to hypertable

- ​time_column_name​
Name of the column containing time values

其他选项：
- ​​partitioning_column​
Name of an additional column to partition by. If provided, ​number_partitions​ must be set.

- ​​number_partitions
Number of hash partitions to use for ​partitioning_column​ when this optional argument is supplied. Must be > 0.
​
- ​chunk_time_interval​
Interval in event time that each chunk covers. Must be > 0. Default is 1 month (​units​).

​- ​create_default_indexes​
Boolean whether to create default indexes on time/partitioning columns. Default is TRUE.
​
- ​if_not_exists​
Boolean whether to print warning if table already converted to hypertable or raise exception. Default is FALSE.

例子：

只使用时间分区：
```sql
SELECT create_hypertable('conditions', 'time');
```

块覆盖的间隔时间设置为 1 天：
```sql
SELECT create_hypertable('conditions', 'time', chunk_time_interval => 86400000000);
SELECT create_hypertable('conditions', 'time', chunk_time_interval => interval '1 day');
```

使用 4 个分区，时间分区使用 `time` 字段，空间分区使用 `location` 字段。
```sql
SELECT create_hypertable('conditions', 'time', 'location', 4);
```

转换时如果已经是 hypertable 不发出警告：
```sql
SELECT create_hypertable('conditions', 'time', if_not_exists => TRUE);
```

## 查询 timescale 的区块信息

```sql
select * from chunk_relation_size('conditions');
```

## 转换已存在的表和数据

假定有一个名叫 `​old_table​` 表要迁移到一个新的表 `​new_table​`。

创建 `new_table`，从旧表复制包括索引：
```sql
CREATE TABLE new_table (LIKE old_table INCLUDING DEFAULTS INCLUDING CONSTRAINTS INCLUDING INDEXES);
```
排除索引：
```sql
CREATE TABLE new_table (LIKE old_table INCLUDING DEFAULTS INCLUDING CONSTRAINTS EXCLUDING INDEXES);
```
转换新表为 Hypertable，并插入数据：
```sql
-- Assuming 'time' is the time column for the dataset
SELECT create_hypertable('new_table', 'time');

-- Insert everything from old_table
INSERT INTO new_table SELECT * FROM old_table;
```

假如在创建新表时排除了索引，插入数据之后加上索引：
```sql
CREATE INDEX on new_table (column_name, <options>)
```

## 从不同的库转换已存在的表和数据

将 `old_db` 中所有的表结构都导出到 `old_db.bak`：
```shell
pg_dump --schema-only -f old_db.bak old_db
```

在 `new_db` 创建表：
```shell
psql -d new_db < old_db.bak
```

将 `new_db` 中的表 `conditions` 转为 Hypertable：
```sql
SELECT create_hypertable('foo', 'time');
```

将 `old_db` 数据导出到 `old_db.csv` 文件：
```shell
# The following ensures 'conditions' outputs to a comma-separated .csv file
psql -d old_db -c "\COPY (SELECT * FROM conditions) TO old_db.csv DELIMITER ',' CSV"
```

* 导入数据
```shell
psql -d new_db -c "\COPY conditions FROM old_db.csv CSV"
```

## 基本操作

* 插入
```sql
INSERT INTO conditions(time, location, temperature, humidity) VALUES (NOW(), 'office', 70.0, 50.0);
```

* 查询
```sql
SELECT * FROM conditions ORDER BY time DESC LIMIT 100;
```

* 清除

```sql
SELECT drop_chunks(interval '24 hours', 'conditions');
```

* 索引数据

```sql
CREATE INDEX ON conditions (location, time DESC);
```

* 索引建议

离散值（limited-cardinality 有限的基数）的列，例如，您最可能使用“等于”或“不等于”比较器。我们建议使用这样的索引：
```sql
CREATE INDEX ON conditions (location, time DESC);
```

对于所有其他类型的列，例如，具有连续值的列，例如，您最有可能使用“小于”或“大于”比较器，则索引类似这样：
```sql
CREATE INDEX ON conditions (time DESC, temperature);
```
定义 `time DESC` 能有效的查询列值和 `time`，例如下面的查询：
```sql
SELECT * FROM conditions WHERE location = 'garage' ORDER BY time DESC LIMIT 10
```

要理解为什么复合索引需要以这样的方式定义，我们来假设有两种位置（“办公室office”和“车库garage”），他们有不同的时间戳和温度。
如果我们的索引为 `(location, time DESC)` 数据将会像下面的形式存储和排列：

```text
garage-4
garage-3
garage-2
garage-1
office-3
office-2
office-1
```

如果我们的索引为 `​(time DESC, location)​` 数据将会像下面的形式存储和排列：
```text
4-garage
3-garage
3-office
2-garage
2-office
1-garage
1-office
```

可以认为索引 `btrees` 是从最高位向下构建的，所以先匹配的第一个字符，然后第二个，以此类推，在上面的例子中我可以很容易的看出他们是作为两个单独的元组。

现在基于条件 `​WHERE location = 'garage' and time >= 1 and time < 4`，最好使索引 `(location, time DESC)`，因为 `location` 是连续的，所以索引的第一位精确地从车库 garage 找到所有的度量值，然后我们可以使用 `time` 进一步缩小所选的集合。

另一方面，如果条件是 `temperature > 80`，特别是条件匹配较大的数值，您仍然需要搜索与谓词匹配的所有 `time` 值，但在每一个值中，也只获取值的一个（可能是大的）子集，而不仅仅是一个不同的值。

对于一个值通常为 NULL 的列，我们建议在索引中添加一个列不是 NULL 子句（除非您经常搜索丢失的数据），这将创建一个更紧凑、更有效的索引。。例如：
```sql
CREATE INDEX ON conditions (time DESC, humidity) WHERE humidity IS NOT NULL;
```

默认情况下，TimescaleDB 自动创建时间索引，在 Hypertable 创建的时候。
```sql
CREATE INDEX ON conditions (time DESC);
```

此外，如果​ `create_hypertable​` 命令指定了 `space partition`，会自动创建以下索引：
```sql
CREATE INDEX ON conditions (location, time DESC);
```

这个默认的行为，可以在执行 `create_hypertable` 是使用参数 `create_default_indexes​` 覆盖。

## 使用例子

1. 创建时序表(hypertable)

```text
# Create a schema for a new hypertable  
CREATE TABLE sensor_data (  
	"time" timestamp with time zone NOT NULL,  
	device_id TEXT NOT NULL,  
	location TEXT NULL,  
	temperature NUMERIC NULL,  
	humidity NUMERIC NULL,  
	pm25 NUMERIC  
);  
  
# Create a hypertable from this data  
SELECT create_hypertable ('sensor_data', 'time', 'device_id', 16);  
```

2. 迁移数据到hyper table

```text
# Migrate data from existing Postgres table into  
# a TimescaleDB hypertable  
INSERT INTO sensor_data (SELECT * FROM old_data);  
```

3. 查询hyper table

```text
# Query hypertable like any SQL table  
SELECT device_id, AVG(temperature) from sensor_data  
WHERE temperature IS NOT NULL AND humidity > 0.5  
AND time > now() - interval '7 day'  
GROUP BY device_id;  
```

4. 查询最近异常的数据

```text
# Metrics about resource-constrained devices  
SELECT time, cpu, freemem, battery FROM devops  
WHERE device_id='foo'  
AND cpu > 0.7 AND freemem < 0.2  
ORDER BY time DESC  
LIMIT 100;  
```

5. 计算最近7天，每小时的异常次数

```text
# Calculate total errors by latest firmware versions  
# per hour over the last 7 days  
SELECT date_trunc('hour', time) as hour, firmware,  COUNT(error_msg) as errno FROM data  WHERE firmware > 50  AND time > now() - interval '7 day' GROUP BY hour, firmware ORDER BY hour DESC, errno DESC;  
```

6. 计算巴士的每小时平均速度

```text
# Find average bus speed in last hour  
# for each NYC borough  
SELECT loc.region, AVG(bus.speed) FROM bus INNER JOIN loc ON (bus.bus_id = loc.bus_id) WHERE loc.city = 'nyc' AND bus.time > now() - interval '1 hour'  
GROUP BY loc.region;  
```

7. 展示最近12小时，每小时的平均值

```text
=#  SELECT date_trunc('hour', time) AS hour, AVG(weight) FROM logs WHERE device_type = 'pressure-sensor' AND customer_id = 440 AND time > now() - interval '12 hours' GROUP BY hour;  
  
 hour               | AVG(weight)  
--------------------+--------------  
 2017-01-04 12:00   | 170.0  
 2017-01-04 13:00   | 174.2  
 2017-01-04 14:00   | 174.0  
 2017-01-04 15:00   | 178.6  
 2017-01-04 16:00   | 173.0  
 2017-01-04 17:00   | 169.9  
 2017-01-04 18:00   | 168.1  
 2017-01-04 19:00   | 170.2  
 2017-01-04 20:00   | 167.4  
 2017-01-04 21:00   | 168.6  
```

8. 监控每分钟过载的设备数量

```text
=#  SELECT date_trunc('minute', time) AS minute, COUNT(device_id) FROM logs WHERE cpu_level > 0.9 AND free_mem < 1024 AND time > now() - interval '24 hours' GROUP BY minute ORDER BY COUNT(device_id) DESC LIMIT 25;  
  
 minute             | heavy_load_devices  
--------------------+---------------------  
 2017-01-04 14:59   | 1653  
 2017-01-04 15:01   | 1650  
 2017-01-04 15:00   | 1605  
 2017-01-04 15:02   | 1594  
 2017-01-04 15:03   | 1594  
 2017-01-04 15:04   | 1561  
 2017-01-04 15:06   | 1499  
 2017-01-04 15:05   | 1460  
 2017-01-04 15:08   | 1459  
```

9. 最近7天，按固件版本，输出每个固件版本的报错次数

```text
=#  SELECT firmware_version, SUM(error_count) FROM logs WHERE time > now() - interval '7 days' GROUP BY firmware_version ORDER BY SUM(error_count) DESC LIMIT 10;  
  
 firmware_version  | SUM(error_count)  
-------------------+-------------------  
 1.0.10            | 191  
 1.1.0             | 180  
 1.1.1             | 179  
 1.0.8             | 164  
 1.1.3             | 161  
 1.1.2             | 152  
 1.2.1             | 144  
 1.2.0             | 137  
 1.0.7             | 130  
 1.0.5             | 112  
 1.2.2             | 110
 ```

10. 某个范围，每小时，温度高于90度的设备数量。

```text
=#  SELECT date_trunc('hour', time) AS hour, COUNT(logs.device_id) FROM logs JOIN devices ON logs.device_id = devices.id WHERE logs.temperature > 90 AND devices.location = 'SITE-1' GROUP BY hour;  
  
 hour               | COUNT(logs.device_id)  
--------------------+------------------------  
 2017-01-04 12:00   | 994  
 2017-01-04 13:00   | 905  
 2017-01-04 14:00   | 875  
 2017-01-04 15:00   | 910  
 2017-01-04 16:00   | 905  
 2017-01-04 17:00   | 840  
 2017-01-04 18:00   | 801  
 2017-01-04 19:00   | 813  
 2017-01-04 20:00   | 798  
```

## 传感器例子

假设传感器数据不断的上报，用户需要查询当前最新的，每个传感器上报的值。

创建测试表如下，
```sql
create unlogged table sort_test(
  id serial8 primary key,  -- 主键
  c2 int,  -- 传感器ID
  c3 int  -- 传感器值
);  
```

写入1000万传感器测试数据：
```sql
insert into sort_test (c2,c3) select random()*100000, random()*100 from generate_series(1,10000000);
```

优化手段，新增复合索引，避免SORT，注意，id需要desc

```sql
create index sort_test_1 on sort_test(c2,id desc); 
```
如果被取出的数据需要后续的处理，可以使用游标，分批获取，因为不需要显示sort，所以分批获取速度很快，从而加快整个的处理速度。
```text
begin;
declare c1 cursor for select id,c2,c3 from (select id,c2,c3,row_number() over(partition by c2 order by id desc) rn from sort_test) t where rn=1;
fetch 100 from c1;
```

## 增量合并数据同步例子

类似Oracle的物化视图，apply时，对于同一条记录的update并不需要每次update的中间过程都需要执行，只需要执行最后一次的。

因此，也可以利用类似的操作手段，分组取最后一条，

```sql
create extension hstore;

create unlogged table sort_test1(
  id serial8 primary key,  -- 主键
  c2 int,  -- 目标表PK
  c3 text,  -- insert or update or delete
  c4 hstore -- row
); 

create index idx_sort_test1_1 on sort_test1(c2,id desc);

select c2,c3,c4 from (select c2,c3,c4,row_number() over(partition by c2 order by id desc) rn from sort_test1)
```

## 稀疏列的变态优化方法

```sql
create type r as (c2 int, c3 int);

declare cur cursor for with recursive skip as (  
  (  
    select (c2,c3)::r as r from sort_test where id in (select id from sort_test where c2 is not null order by c2,id desc limit 1) 
  )  
  union all  
  (  
    select (
      select (c2,c3)::r as r from sort_test where id in (select id from sort_test t where t.c2>(s.r).c2 and t.c2 is not null order by c2,id desc limit 1) 
    ) from skip s where (s.r).c2 is not null
  )    -- 这里的where (s.r).c2 is not null 一定要加, 否则就死循环了. 
)   
select (t.r).c2, (t.r).c3 from skip t where t.* is not null; 
```

## windows 安装

**下载软件**
- postgresql-16.1-1-windows-x64.exe
- timescaledb-postgresql-16-windows-amd64.zip

**环境变量**环境
`D:\Program Files\PostgreSQL\16\bin`

运行 timescaledb `setup.exe`
输入配置文件路径，然后一路 yes
`D:\Program Files\PostgreSQL\16\data\postgresql.conf`

