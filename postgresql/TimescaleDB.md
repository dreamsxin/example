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
方法一
https://docs.timescale.com/self-hosted/latest/install/installation-linux/
```shell
echo "deb https://packagecloud.io/timescale/timescaledb/ubuntu/ $(lsb_release -c -s) main" | sudo tee /etc/apt/sources.list.d/timescaledb.list
wget --quiet -O - https://packagecloud.io/timescale/timescaledb/gpgkey | sudo apt-key add -
# ubuntu 21
wget --quiet -O - https://packagecloud.io/timescale/timescaledb/gpgkey | sudo gpg --dearmor -o /etc/apt/trusted.gpg.d/timescaledb.gpg
apt update
apt install timescaledb-2-postgresql-16
```
方法二
http://docs.timescale.com/latest/getting-started/installation/linux/installation-apt

```shell
# Add our PPA
sudo add-apt-repository ppa:timescale/timescaledb-ppa
sudo apt-get update
# To install
sudo apt install timescaledb-postgresql-10
```

### 配置扩展
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

**查看授权协议**
```sql
show timescaledb.license;
ALTER SYSTEM SET timescaledb.license = 'timescale';
```

## 创建库

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

## 性能测试

https://github.com/timescale/tsbs.git
```shell
# Fetch TSBS and its dependencies
#go get github.com/timescale/tsbs
git clone https://github.com/timescale/tsbs.git

cd tsbs/cmd/tsbs_generate_data
go install
cd $GOPATH/src/github.com/timescale/tsbs
make

```

### 数据生成

生成超过100M行（1B指标）
```shell
tsbs_generate_data --use-case="cpu-only" --seed=123 --scale=4000 --timestamp-start="2016-01-01T00:00:00Z" --timestamp-end="2016-01-07T00:00:00Z" --log-interval="10s" --format="timescaledb" | gzip > ./timescaledb-data.gz
```
插入测试
```shell
#tsbs_load config --target=timescaledb --data-source=FILE
./scripts/load/load_timescaledb.sh

# 使用 tsbs_load_timescaledb 工具向远程数据库实例写入数据

#会自动创建一个名为 benchmark 的新数据库
tsbs_load_timescaledb --postgres="sslmode=disable" --host=localhost --port=5432 --pass="123456" --user="postgres" --admin-db-name=postgres --workers=1 --in-table-partition-tag=false --field-index-count=0 --do-create-db=true --force-text-format=false --do-abort-on-exist=false --file="timescaledb-data" --partitions=1 --use-insert=true --batch-size=8000

tsbs_load_timescaledb --postgres="sslmode=disable" \
                                --db-name=${DATABASE_NAME} \
                                --host=${DATABASE_HOST} \
                                --port=${DATABASE_PORT} \
                                --pass=${DATABASE_PWD} \
                                --user=${DATABASE_USER} \
                                --workers=${NUM_WORKERS} \
                                --batch-size=${BATCH_SIZE} \
                                --reporting-period=${REPORTING_PERIOD} \
                                --use-hypertable=${USE_HYPERTABLE} \
                                --use-jsonb-tags=${JSON_TAGS} \
                                --in-table-partition-tag=${IN_TABLE_PARTITION_TAG} \
                                --hash-workers=${HASH_WORKERS} \
                                --time-partition-index=${TIME_PARTITION_INDEX} \
                                --partition-on-hostname=${PARTITION_ON_HOSTNAME} \
                                --partitions=${PARTITIONS} \
                                --chunk-time=${CHUNK_TIME} \
                                --write-profile=${PERF_OUTPUT} \
                                --field-index-count=1 \
                                --do-create-db=${DO_CREATE_DB} \
                                --force-text-format=${FORCE_TEXT_FORMAT} \
                                --use-copy=${USE_COPY} \
                                --replication-factor=${REPLICATION_FACTOR} \
                                --create-metrics-table=${CREATE_METRICS_TABLE}

$ cat /tmp/timescaledb-data.gz | gunzip | tsbs_load_timescaledb \
--postgres="sslmode=require" --host="my.tsdb.host" --port=5432 --pass="password" \
--user="benchmarkuser" --admin-db-name=defaultdb --workers=8  \
--in-table-partition-tag=true --chunk-time=8h --write-profile= \
--field-index-count=1 --do-create-db=true --force-text-format=false \
--do-abort-on-exist=false

# 使用 load_timescaledb.sh 脚本向本地 timescaledb 实例写入数据
$ cd /home/randy/go/src/github.com/timescale/tsbs
$ NUM_WORKERS=2 BATCH_SIZE=10000 BULK_DATA_DIR=/tmp DATABASE_PORT=port \
DATABASE_USER=user DATABASE_NAME=dbname DATABASE_PWD=passwd \
    scripts/load/load_timescaledb.sh
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
CREATE UNIQUE INDEX ON conditions(time, device_id); 
-- 分区索引
CREATE INDEX ON conditions(time, device_id)
    WITH (timescaledb.transaction_per_chunk);
CREATE UNIQUE INDEX ON conditions(time, device_id)
    WITH (timescaledb.transaction_per_chunk); 
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

打开服务管理器，重启服务 `services.msc`

### 创建数据库

```sql
CREATE DATABASE logs;
\c 
CREATE EXTENSION IF NOT EXISTS timescaledb CASCADE;

-- We start by creating a regular SQL table
CREATE TABLE conditions (
  time        TIMESTAMPTZ       NOT NULL,
  location    varchar(255)      NOT NULL,
  temperature DOUBLE PRECISION  NULL,
  humidity    DOUBLE PRECISION  NULL
);

-- Then we convert it into a hypertable that is partitioned by time
SELECT create_hypertable('conditions', 'time');
SELECT create_hypertable('conditions', 'time', 'location', 4);
SELECT create_hypertable(
  'conditions',
  by_range('time', INTERVAL '1 day')
);
SELECT set_chunk_time_interval('conditions', INTERVAL '24 hours');
SELECT * FROM add_dimension('hypertable_example', by_hash('device_id', 4));

INSERT INTO conditions(time, location, temperature, humidity)
  VALUES (NOW(), 'office', 70.0, 50.0);

SELECT * FROM conditions ORDER BY time DESC LIMIT 100;

SELECT time_bucket('15 minutes', time) AS fifteen_min,
    location, COUNT(*),
    MAX(temperature) AS max_temp,
    MAX(humidity) AS max_hum
  FROM conditions
  WHERE time > NOW() - interval '3 hours'
  GROUP BY fifteen_min, location
  ORDER BY fifteen_min DESC, max_temp DESC;
```

### 压缩策略（暂时不需要使用）

设置压缩分组
```sql
ALTER TABLE example SET (
  timescaledb.compress,
  timescaledb.compress_segmentby = 'device_id'
);
```
超过7天进行压缩处理
```sql
SELECT add_compression_policy('example', INTERVAL '7 days');
```
### 数据保留策略

保留24H数据
```sql
SELECT add_retention_policy('conditions', INTERVAL '24 hours');

//清除数据保留策略
SELECT remove_retention_policy('conditions');

// 查看计划作业
SELECT * FROM timescaledb_information.job_stats;
```

```sql
// 查看超表
select show_chunks('tablename');
SELECT drop_chunks('tablename', older_than >= id); 删除当前id之前的块

// 若要重建策略，先删除原来的策略
SELECT remove_retention_policy('cigarette_single');
// 查看策略任务详细信息
select * from timescaledb_information.jobs
// 查看策略任务执行情况
SELECT * FROM timescaledb_information.job_stats
// 修改策略执行时间
SELECT alter_job(1015, schedule_interval => INTERVAL '6 minutes');
// 注：1015是策略任务信息中的 job_id，schedule_interval 表示任务执行周期
// 其他：开启dbug调试，并查看任务运行情况
SET client_min_messages TO DEBUG1;
CALL run_job(1015);
```

## 搭建集群

### 节点划分
访问节点AN、数据节点DN

### 访问节点AN 配置
`/var/lib/pgsql/14/data/postgresql.conf`
```
# 访问节点AN
max_prepared_transactions = 500
enable_partitionwise_aggregate = on
jit = off
```
### 数据节点DN 配置

```conf
# 数据节点DN
max_prepared_transactions = 500
wal_level = logical
```

### 添加数据节点
# 连接访问节点数据库
psql -U postgres -h localhost -d mydatabase
 
### 添加数据节点
```sql
SELECT add_data_node('dn1','xxx.xx.xx.xx1','mydatabase',5432,false,true,'postgres');
SELECT add_data_node('dn2','xxx.xx.xx.xx2','mydatabase',5432,false,true,'postgres');
```
此时Timescaledb集群就搭建成功了

### 创建分布式超表（AN访问节点）
```sql
# 创建表
CREATE TABLE test2 (
                       time        TIMESTAMPTZ       NOT NULL,
                       location    TEXT              NOT NULL,
                       temperature DOUBLE PRECISION  NULL,
                       humidity    DOUBLE PRECISION  NULL
);
# 创建分布式超表，默认使用所有数据节点
SELECT create_distributed_hypertable('test2', 'time', 'location');
 
# 插入数据
INSERT INTO test2 VALUES ('2020-12-14 13:45', 1, '1.2');
 
#给表增加数据节点
SELECT detach_data_node('dn1', 'test2');
```


## 股票数据

创建普通表
```sql
CREATE TABLE stocks_real_time (
  time TIMESTAMPTZ NOT NULL,
  symbol TEXT NOT NULL,
  price DOUBLE PRECISION NULL,
  day_volume INT NULL
);
```
创建超表
```sql
SELECT create_hypertable('stocks_real_time','time');
```

查看子表 `\d+ stocks_real_time`

创建索引
```sql
CREATE INDEX ix_symbol_time ON stocks_real_time (symbol, time DESC);
```
添加一个普通表来存储股票交易数据的公司名称和代码
```sql
CREATE TABLE company (
  symbol TEXT NOT NULL,
  name TEXT NOT NULL
);
```
psql 里输入 `\dt` 可以看到目前已有的表。
下载数据
`https://assets.timescale.com/docs/downloads/get-started/real_time_stock_data.zip`

psql 命令下，将数据导入
```sql
\COPY stocks_real_time from './tutorial_sample_tick.csv' DELIMITER ',' CSV HEADER;
\COPY company from './tutorial_sample_company.csv' DELIMITER ',' CSV HEADER;
```

使用简单查询语句检索一下刚导入的数据
```sql
SELECT * FROM stocks_real_time srt WHERE symbol='TSLA' and day_volume is not null ORDER BY time DESC, day_volume desc LIMIT 10;
```

TimescaleDB 也具有自定义SQL函数，可以帮助简化和快速地进行时间序列分析。

### `first()`和`last()`函数
可以检索一列的第一个和最后一个值。例如，股票数据有一个时间戳列time和一个数字列price。可以使用first(price, time)来获取价格列中随着时间列的增加而排序的第一个值。

在这个查询中，首先选择`stocks_real_time srt`表中每只股票最近四天的`first()`和`last()`交易价格:
```sql
SELECT symbol, first(price,time), last(price, time) FROM stocks_real_time srt WHERE time > now() - INTERVAL '4 days' GROUP BY symbol ORDER BY symbol LIMIT 10;
```

### `time_bucket`函数

时间桶用于对数据进行分组，以便可以在不同的时间段内执行计算。时间桶代表特定的时间点，因此单个时间桶中的数据的所有时间戳都使用桶的时间戳。我们使用与上述相同的查询来查找第一个和最后一个值，但首先将数据组织成1小时的时间桶。之前检索了列的第一个和最后一个值，而这次将检索1小时时间桶的第一个和最后一个值。
```sql
SELECT time_bucket('1 hour', time) AS bucket, first(price,time), last(price, time) FROM stocks_real_time srt WHERE time > now() - INTERVAL '4 days' GROUP BY bucket;
```

### 连续聚合

连续聚合是一种特殊的超级表，它会自动在后台刷新，随着新数据的添加或旧数据的修改。对数据集的更改会被跟踪，并且在连续聚合背后的超级表会在后台自动更新。用户不需要手动刷新连续聚合，它们会在后台持续和增量更新。与常规的PostgreSQL物化视图相比，连续聚合的维护负担要小得多，因为不必在每次刷新时从头开始创建整个视图。这意味着可以继续处理数据，而不必维护数据库。
由于连续聚合基于超级表，你可以以与其他表完全相同的方式查询它们，并在连续聚合上启用压缩或数据分层，甚至可以在连续聚合之上创建连续聚合。默认情况下，查询连续聚合可以为你提供实时数据，物化视图中的预先聚合数据与尚未进行聚合的最新数据相结合，这使你可以在每次查询时获取最新的结果。

简化聚合有三种主要方法：物化视图、连续聚合和实时聚合。

### 创建连续聚合

先试下聚合查询，我们使用SELECT语句查找min和max函数的最高值和最低值，以及first和last函数的打开值和关闭值。然后将数据汇总到1天的桶中，如下所示:
```sql
SELECT time_bucket('1 day', time) AS bucket, symbol, max(price) AS high, first(price, time) AS open, last(price, time) AS close,  min(price) AS low FROM stocks_real_time srt WHERE time > now() - INTERVAL '1 week' GROUP BY bucket, symbol ORDER BY bucket, symbol LIMIT 10;
```
有了聚合查询，就可以使用它来创建连续聚合。

首先创建一个名为 `stock_candlestick_daily` 的实体化视图，使用 `WITH (timescaledb.continuous)` 然后将其转换为一个连续聚合：
```sql
CREATE MATERIALIZED VIEW stock_candlestick_daily
WITH (timescaledb.continuous) AS
SELECT
  time_bucket('1 day', "time") AS day,
  symbol,
  max(price) AS high,
  first(price, time) AS open,
  last(price, time) AS close,
  min(price) AS low
FROM stocks_real_time srt
GROUP BY day, symbol;
```
使用联合聚合来查询聚合结果
```sql
SELECT * FROM stock_candlestick_daily ORDER BY day DESC, symbol LIMIT 10;
```
## add_retention_policy()

对 `stocks_real_time` 表进行自动保留策略：

`SELECT add_retention_policy('stocks_real_time', INTERVAL '3 weeks');`
运行此命令后，所有超过 3 周的数据都将从中删除 stocks_real_time，并创建定期保留策略。不会从您的连续聚合中删除任何数据.

查看创建保留策略后的任务
```sql
SELECT * FROM timescaledb_information.jobs;
SELECT * FROM timescaledb_information.job_stats;
```

### drop_chunks
`SELECT drop_chunks('stocks_real_time', INTERVAL '3 weeks');`
要删除超过两周但新于三周的所有数据：
```sql
SELECT drop_chunks(
  'stocks_real_time',
  older_than => INTERVAL '2 weeks',
  newer_than => INTERVAL '3 weeks'
)
```

### 修改聚合

```sql
--修改视图参数
ALTER VIEW device_summary SET (timescaledb.refresh_interval = '10 min');

--删除视图
DROP VIEW device_summary CASCADE;

ALTER VIEW conditions_summary_hourly SET (  
  timescaledb.refresh_lag = '1 hour'  
);  
  
  
ALTER VIEW conditions_summary_daily SET (  
  timescaledb.ignore_invalidation_older_than = '30 days'  
);  

```
### `timescaledb.ignore_invalidation_older_than`
该参数接受时间间隔（例如 1 个月），如果设置，它将限制处理无效的时间。因此，如果为 `timescaledb.ignore_invalidation_older_than = '1 month'`，则对自修改时间到当前时间戳的 1 个月以上的数据的任何修改都不会导致连续聚合被更新。

```sql
--timescaledb.refresh_interval参数控制视图的刷新时间，间隔越短，后台进程越频繁，当然进程是需要消耗资源的。
--默认情况下，查询该聚合视图的数据中，包含了已经聚合的数据以及基础表中未聚合的数据，如果你想要的结果只是聚合后的数据，不需要基础表中最新的数据，那么可以设置timescaledb.materialized_only改参数为true

ALTER VIEW conditions_summary_hourly SET (
    timescaledb.materialized_only = true
);


-- 可以查询timescaledb_information.continuous_aggregates获取所有的聚合视图，如果要查询相关聚合处理进程的处理状态，可以查询 timescaledb_information.continuous_aggregate_stats视图。

SELECT view_name, materialization_hypertable
    FROM timescaledb_information.continuous_aggregates;
         view_name         |            materialization_hypertable             
---------------------------+---------------------------------------------------
 conditions_summary_hourly | _timescaledb_internal._materialized_hypertable_30
 conditions_summary_daily  | _timescaledb_internal._materialized_hypertable_31
(2 rows)

--timescaledb.refresh_lag参数控制延迟聚合的时间，如下，conditions_summary_hourly视图的bucket_width为1小时，如果设置timescaledb.refresh_lag为1小时，那么就是bucket_width+timescaledb.refresh_lag=2，也就是雾化时间比当前时间晚了两个小时，也就是聚合的是2小时之前的数据。

ALTER VIEW conditions_summary_hourly SET (
  timescaledb.refresh_lag = '1 hour'
);
--越低的refresh_lag值，表示聚合的数据和基础数据的时间更接近，但是可能会导致写放大，导致插入性能变差。一般情况下，该参数不需要修改。

--timescaledb.max_interval_per_job参数决定一个job聚合的最大量，当一个job处理的数据后，还有剩下要处理的数据时，会自己启动一个新的job进行处理。

--timescaledb.ignore_invalidation_older_than参数控制修改（插入，更新和删除）如何触发连续聚合的更新。如果对基础表进行了修改，则它将使聚合中已计算的部分无效，并且必须更新聚合。默认情况下，所有数据的改变都会触发聚合的更新，如果设置了改参数，则改时间段之前的数据更改将不会触发聚合更新。
--一个常用的实例，删除基础表中30天以外的数据，但是保留持续聚合的数据在视图中。
ALTER VIEW device_readings SET (
  timescaledb.ignore_invalidation_older_than = '30 days'
);
SELECT drop_chunks(INTERVAL '30 days', 'device_readings')
```

从上面得知，删除基础表的数据可以使用drop_chunks函数，cascade_to_materializations参数可以控制是否在聚合的视图中保留在基础表中删除的数据，如果为true,则聚合视图中的数据也将删除，如果为false,则只删除基础表中的数据，保留聚合视图中的历史聚合数据。另外需要注意的是drop_chunks函数中的older_than参数应该长于timescaledb.ignore_invalidation_older_than，因为基础数据备删除了，无法处理删除区域的数据。

```sql
-- 创建普通数据表
CREATE TABLE stocks_real_time (
  time TIMESTAMPTZ NOT NULL,
  symbol TEXT NOT NULL,
  price DOUBLE PRECISION NULL,
  day_volume INT NULL
);

-- 通过 time 进行分区将普通表转换为超表
SELECT create_hypertable('stocks_real_time','time');

-- 创建索引以高效查询 symbol 和 time 列
CREATE INDEX ix_symbol_time ON stocks_real_time (symbol, time DESC);

-- timescaleDB 仍然是一个 postgrepSQL 库，可以新建普通的数据表
CREATE TABLE company (
  symbol TEXT NOT NULL,
  name TEXT NOT NULL
);

-- 执行两个文件数据的插入
-- 若无交易数量人为补充
UPDATE stocks_real_time SET day_volume=(random()*(100000-1)+1)
-- 查询近 4 天的股票数据
SELECT * FROM stocks_real_time srt
WHERE time > now() - INTERVAL '4 days';

-- 查询 Amazon 公司最近 10 条股票交易信息
SELECT * FROM stocks_real_time srt
WHERE symbol = 'AMZN'
ORDER BY time DESC, day_volume desc
LIMIT 10;

-- 计算近 4 天 Apple 公司的股票平均交易价格
SELECT
    avg(price)
FROM stocks_real_time srt
JOIN company c ON c.symbol = srt.symbol
WHERE c.name = 'Apple' AND time > now() - INTERVAL '4 days';

-- first() and last()
-- 获取所有公司近三天来的第一笔成交价和最后一笔成交价
SELECT symbol, first(price,time), last(price, time)
FROM stocks_real_time srt
WHERE time > now() - INTERVAL '3 days'
GROUP BY symbol
ORDER BY symbol;

-- 查询一周内每个公司每天的股票平均交易额
SELECT
  time_bucket('1 day', time) AS bucket,
  symbol,
  avg(price)
FROM stocks_real_time srt
WHERE time > now() - INTERVAL '1 week'
GROUP BY bucket, symbol
ORDER BY bucket, symbol;

-- 普通聚合查询：查询各个股票每天的开盘价格、闭盘价格、以及最高、最低交易价格
SELECT
  time_bucket('1 day', "time") AS day,
  symbol,
  max(price) AS high,
  first(price, time) AS open,
  last(price, time) AS close,
  min(price) AS low
FROM stocks_real_time srt
GROUP BY day, symbol
ORDER BY day DESC, symbol;

-- 连续聚合查询
CREATE MATERIALIZED VIEW stock_candlestick_daily
WITH (timescaledb.continuous) AS
SELECT
  time_bucket('1 day', "time") AS day,
  symbol,
  max(price) AS high,
  first(price, time) AS open,
  last(price, time) AS close,
  min(price) AS low
FROM stocks_real_time srt
GROUP BY day, symbol;

-- 注意：当连续聚合被创建时，其默认开启实时聚合功能，即未被聚合到视图中的数据将被实时聚合并记录到视图当中。若实时插入的数据量非常庞大，则会影响查询的性能
-- 可以通过下列语句关闭实时聚合功能
ALTER MATERIALIZED VIEW stock_candlestick_daily SET (timescaledb.materialized_only = true);

-- 查询聚合结果
SELECT * FROM stock_candlestick_daily
  ORDER BY day DESC, symbol;

-- 设置自动更新策略
-- 时间段为 3天前 到 1小时前，以当前时间为基准
-- 策略每天执行 1 次，由 schedule_interval 设置
-- 策略执行的查询是连续聚合 stock_candlestick_daily 中定义的查询
SELECT add_continuous_aggregate_policy('stock_candlestick_daily',
  start_offset => INTERVAL '3 days',
  end_offset => INTERVAL '1 hour',
  schedule_interval => INTERVAL '1 days');
	
-- 设置手动更新策略
-- 用于更新自动更新时间范围外的聚合数据，如掉线重连后发送的历史数据
-- 下列策略将更新 1周前 到 现在 的连续聚合
-- 手动刷新仅会更新一次连续聚合
CALL refresh_continuous_aggregate(
  'stock_candlestick_daily',
  now() - INTERVAL '1 week',
  now()
);

-- 连续聚合的详细信息查询
SELECT * FROM timescaledb_information.continuous_aggregate
```

## add_continuous_aggregate_policy 连续聚合策略

```sql
CREATE MATERIALIZED VIEW conditions_summary_daily (day, device, temp)
WITH (timescaledb.continuous) AS
  SELECT time_bucket('1 day', time), device, avg(temperature)
  FROM conditions
  GROUP BY (1, 2);

#默认实时聚合，如果插入数据非常多，将影响查询性能，改成定时聚合，聚合7天前到1天前的数据，每天聚合一次
SELECT add_continuous_aggregate_policy('conditions_summary_daily', '7 days', '1 day', '1 day');
```

### 查看计划作业
```sql
SELECT * FROM timescaledb_information.job_stats;
```

## 高可用 High availability

### 配置主库 Configuring the primary database

默认密码加密级别，如需设置
```shell
SET password_encryption = 'scram-sha-256';
```
**创建新用户**
```shell
CREATE ROLE repuser WITH REPLICATION PASSWORD '<PASSWORD>' LOGIN;
```
**配置复制参数**
- synchronous_commit 设置为off
用来控制事务提交的方式。如果此参数设置为 ON，则所有事务的提交将会等待数据同步到磁盘上才会返回完成结果，这样可以保证提交的数据不会丢失。如果此参数设置为 OFF，则事务提交后不会等待数据同步到磁盘上，这样速度会更快，但可能会导致数据丢失。
- max_wal_senders 设置为副本总数
每一个 slot 要使用一个 `wal sender`，每一个流式物理复制也要使用一个 `wal sender`。
- wal_level 默认值即可
wal_level = replica （pg13默认已经开启replica） 该参数的可选的值有minimal，replica和logical，wal的级别依次增高
- max_replication_slots 可以支持的复制插槽总数
每一个订阅需要消耗一个slot，每一个指定了slot的流式物理复制也要消耗一个slot。
- listen_addresses

```conf
listen_addresses = '*'
wal_level = replica
max_wal_senders = 2
max_replication_slots = 2
synchronous_commit = off
```

**配置认证**
将前面创建的用户配置上认证方式
`pg_hba.conf`
```conf
TYPE  DATABASE    USER    ADDRESS METHOD            AUTH_METHOD
host  replication repuser <REPLICATION_HOST_IP>/32  scram-sha-256
```
配置完参数后，重启服务
**创建复制插槽 Create replication slots**
创建名为 `replica_1_slot` 插槽
`SELECT * FROM pg_create_physical_replication_slot('replica_1_slot');`

### 配置从库 Create a base backup on the replica

**停止从库服务**
如果已经有数据，删除数据
```shell
rm -rf <DATA_DIRECTORY>/*
```
数据目录可以通过下令命令查看
```shell
psql
-- 查看配置文件
show config_file;
-- 查看数据目录
show data_directory;
-- 查看日志目录
show log_directory;
```
**使用主数据库基本备份恢复**
可以使用 `-W` 手动输入密码，也可以配置 `pgpass` 实现命令行免输口令登陆 (Linux:/home/username/.pgpass, Windows:%APPDATA%\postgresql\pgpass.conf，通过设置 PGPASSFILE 环境变量来指定 .pgpass 文件的位置。)
```shell
pg_basebackup -h <PRIMARY_IP> \
-D <DATA_DIRECTORY> \
-U repuser -vP -W
```

**恢复模式启动**
备份完成后，在数据目录中创建一个备用.signal文件。当PostgreSQL在其数据目录中找到一个 `standby.signal` 文件时，它会以恢复模式启动，并通过复制协议流式传输WAL：
```shell
touch <DATA_DIRECTORY>/standby.signal
```
**配置复制和恢复设置**
添加与主服务器通信的详细信息，以及插槽名
`postgresql.conf`
```conf
primary_conninfo = 'host=<PRIMARY_IP> port=5432 user=repuser password=<POSTGRES_USER_PASSWORD> application_name=r1'
primary_slot_name = 'replica_1_slot'

hot_standby = on # 必须设置为on，才能允许对复制副本进行只读查询。在PostgreSQL 10及更高版本中，默认情况下会启用此设置。
wal_level = replica
max_wal_senders = 2
max_replication_slots = 2
synchronous_commit = off
```
重启从库

### 主库上查看复制状态
```psql
SELECT * FROM pg_stat_replication;
```

## 故障转移 Failover

### patroni
https://github.com/zalando/patroni
### 主要特点：
1. 自动故障检测和恢复：Patroni 监视 PostgreSQL 集群的健康状态，一旦检测到主节点故障，它将自动执行故障恢复操作，将其中一个从节点晋升为主节点。
2. 自动故障转移：一旦 Patroni 定义了新的主节点，它将协调所有从节点和客户端，以确保它们正确地切换到新的主节点，从而实现快速、无缝的故障转移。
3. 一致性和数据完整性：Patroni 高度关注数据一致性和完整性。在故障切换过程中，它会确保在新主节点接管之前，数据不会丢失或受损。
4. 外部共享配置存储：Patroni 使用外部键值存储（如 ZooKeeper、etcd 或 Consul）来存储配置和集群状态信息。这确保了配置的一致性和可访问性，并支持多个 Patroni 实例之间的协作。
5. 支持多种云环境和物理硬件：Patroni 不仅可以在云环境中运行，还可以部署在物理硬件上，提供了广泛的部署选项。

### 使用
```text
postgres:5432
patroni:8008
etcd:2379/2380

# PostgreSQL
node1：192.168.234.201
node2：192.168.234.202
node3：192.168.234.203

# etcd
node4：192.168.234.204

# VIP
读写VIP：192.168.234.2
只读VIP：192.168.234.211
```
时钟同步
```shell
yum install -y ntpdate
ntpdate time.windows.com && hwclock -w
```
**安装**
```shell
yum install -y gcc epel-release
yum install -y python-pip python-psycopg2 python-devel
 
pip install --upgrade pip
pip install --upgrade setuptools
pip install patroni[etcd]
```
**配置**
```yml
scope: pgsql
namespace: /service/
name: pg1 # 根据不同节点设置不同名字pg1-pg3
 
restapi:
  listen: 0.0.0.0:8008
  connect_address: 192.168.234.201:8008 # 根据各自节点IP设置
 
etcd:
  host: 192.168.234.204:2379
 
bootstrap:
  dcs:
    ttl: 30
    loop_wait: 10
    retry_timeout: 10
    maximum_lag_on_failover: 1048576
    master_start_timeout: 300
    synchronous_mode: false
    postgresql:
      use_pg_rewind: true
      use_slots: true
      parameters:
        listen_addresses: "0.0.0.0"
        port: 5432
        wal_level: logical
        hot_standby: "on"
        wal_keep_segments: 100
        max_wal_senders: 10
        max_replication_slots: 10
        wal_log_hints: "on"
 
  initdb:
  - encoding: UTF8
  - locale: C
  - lc-ctype: zh_CN.UTF-8
  - data-checksums
 
  pg_hba:
  - host replication repl 0.0.0.0/0 md5
  - host all all 0.0.0.0/0 md5
 
postgresql:
  listen: 0.0.0.0:5432
  connect_address: 192.168.234.201:5432 # 根据各自节点IP设置
  data_dir: /pgsql/data
  bin_dir: /usr/pgsql-12/bin
 
  authentication:
    replication:
      username: repl
      password: "123456"
    superuser:
      username: postgres
      password: "123456"
 
  basebackup:
    max-rate: 100M
    checkpoint: fast
 
tags:
    nofailover: false
    noloadbalance: false
    clonefrom: false
    nosync: false
```

## 查看集群状态

`patronictl -c /etc/patroni.yml list`

### 老的方式
当主库 crush 时，备库就需要启动 failover，此时备库就成为新主库。pg 没有提供可以识别failure的方法，但是pg提供了激活主库的方法。
pg 提供了2种方法将备库激活为主库：
- trigger_file文件(pg 12以后trigger_file变成promote_trigger_file）
- pg_ctl promote命令

**trigger_file**
`cat recovery.conf|grep trigger`
找到 trigger 配置，指定文件
`trigger_file = '/pg/pg96data_sla/trigger.kenyon'`

**备库修改配置**
postgres.conf 
```conf
max_wal_senders = 6
```
查看从库是否从主库接收了所有 wal 日志
```psql
SELECT pg_last_xlog_receive_location() AS last_wal_received;
```
```shell
psql -c "SHOW replication;"
cd pg_xlog
cd pg_wal
# 查看日志里是否可以看到主库 关键词shutdown
ls -ltr|tail -n 1 |awk '{print $NF}'|while read xlog;do pg_xlogdump $xlog;done
```
**激活从库**
```shell
pg_ctl promote -D /pg/pg96data_sla
#or
touch /pg/pg96data_sla/trigger.kenyon
```


## 超函数 Hyperfunctions

https://docs.timescale.com/use-timescale/latest/hyperfunctions/

### 与前一行的差
```sql
SELECT device id, sum(abs_delta) as volatility FROM (
 SELECT device_id, abs(val - lag(val) OVER last_day) as abs_delta FROM measurements WHERE ts >= now()-'1 day'::interval

) calc_delta
GROUP BY device_id;
```
TimescaleDB 写法（timevector 会一次性载入所有数据，不建议在大数据表使用）
```sql

SELECT device_id,
    toolkit_experimental.timevector(ts, val)
        -> toolkit_experimental.sort()
        -> toolkit_experimental.delta()
        -> toolkit_experimental.abs()
        -> toolkit_experimental.sum() as volatility
FROM measurements
WHERE ts >= now()-'1 day'::interval
GROUP BY device_id;
```
## Fill gaps in time-series data

```psql
SELECT
  time_bucket_gapfill('1 day', time) AS date,
  sum(volume) AS volume
FROM trades
WHERE asset_code = 'TIMS'
  AND time >= '2021-09-01' AND time < '2021-10-01'
GROUP BY date
ORDER BY date DESC;
```
```txt
btime          | volume
------------------------+----------
 2021-03-09 17:28:00+00 |  1085.25
 2021-03-09 17:46:40+00 |  1020.42
 2021-03-09 18:05:20+00 |
 2021-03-09 18:24:00+00 |  1031.25
 2021-03-09 18:42:40+00 |  1049.09
 2021-03-09 19:01:20+00 |  1083.80
 2021-03-09 19:20:00+00 |  1092.66
 2021-03-09 19:38:40+00 |
 2021-03-09 19:57:20+00 |  1048.42
 2021-03-09 20:16:00+00 |  1063.17
 2021-03-09 20:34:40+00 |  1054.10
 2021-03-09 20:53:20+00 |  1037.78
```
### Fill gaps by carrying the last observation forward
```pgsql

SELECT
  time_bucket_gapfill(INTERVAL '5 min', time, now() - INTERVAL '2 weeks', now()) as 5min,
  meter_id,
  locf(avg(data_value)) AS data_value
FROM my_hypertable
WHERE
  time > now() - INTERVAL '2 weeks'
  AND meter_id IN (1,2,3,4)
GROUP BY 5min, meter_id
```

## job

https://docs.timescale.com/api/latest/actions/add_job/

任务信息存在 模式 `timescaledb_information` 下的 `jobs` 表
```sql
SELECT * FROM timescaledb_information.jobs;

CREATE OR REPLACE PROCEDURE user_defined_action(job_id int, config jsonb) LANGUAGE PLPGSQL AS
$$
BEGIN
  RAISE NOTICE 'Executing action % with config %', job_id, config;
END
$$;

SELECT add_job('user_defined_action','1h');
SELECT add_job('user_defined_action','1h', fixed_schedule => false);

```

## chunk

```psql
SELECT show_chunks('tablename')
SELECT pg_size_pretty(hypertable_size('tablename'));
SELECT * FROM timescaledb_information.hypertables WHERE hypertable_name = 'tablename';

# 显示关联表的所有数据块（输出内容见下图）:
SELECT show_chunks('表名');

# 显示早于3个月的所有数据块：
SELECT show_chunks('表名', older_than => interval '3 months');

# 显示未来三个月的所有数据块。当时间出错时非常有用：
SELECT show_chunks('表名', newer_than => now() + interval '3 months');

# 显示指定表 时序元数据表上早于2021-10-06年的数据块:
SELECT show_chunks('表名', older_than => '2021-10-06'::date);

# 显示新于3个月的数据块：
SELECT show_chunks('表名', newer_than => interval '3 months');

# 显示早于3个月新于4个月的所有数据块：
SELECT show_chunks('表名', older_than => interval '3 months', newer_than => interval '4 months');

# 获取超表的大小信息
SELECT * FROM hypertable_detailed_size('表名') ORDER BY node_name;
```

## memory

- shared_buffers
- effective_cache_size
- work_mem
- maintenance_work_mem
- max_connections

- max_locks_per_transaction
  
