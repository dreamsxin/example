
## 安装

```shell
# Add our PPA
sudo add-apt-repository ppa:timescale/timescaledb-ppa
sudo apt-get update
# To install
sudo apt install timescaledb
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

```sql
CREATE database tutorial;
\c tutorial
CREATE EXTENSION IF NOT EXISTS timescaledb CASCADE;
select setup_timescaledb();
```

```sql
CREATE TABLE new_table (LIKE old_table INCLUDING DEFAULTS INCLUDING CONSTRAINTS INCLUDING INDEXES);
CREATE TABLE new_table (LIKE old_table INCLUDING DEFAULTS INCLUDING CONSTRAINTS EXCLUDING INDEXES);

-- Assuming 'time' is the time column for the dataset
SELECT create_hypertable('new_table', 'time');

-- Insert everything from old_table
INSERT INTO new_table SELECT * FROM old_table;

CREATE INDEX on new_table (column_name, <options>)
```

```shell
pg_dump --schema-only -f old_db.bak old_db
psql -d new_db < old_db.bak
psql -d new_db
```

```sql
SELECT create_hypertable('foo', 'time');
```

```shell
# The following ensures 'foo' outputs to a comma-separated .csv file
psql -d old_db -c "\COPY (SELECT * FROM foo) TO old_db.csv DELIMITER ',' CSV"
psql -d new_db -c "\COPY foo FROM old_db.csv CSV"
```


## 例子

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
SELECT create_hypertable  
('sensor_data', 'time', 'device_id', 16);  
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
SELECT date_trunc('hour', time) as hour, firmware,  
COUNT(error_msg) as errno FROM data  
WHERE firmware > 50  
AND time > now() - interval '7 day'  
GROUP BY hour, firmware  
ORDER BY hour DESC, errno DESC;  
```

6. 计算巴士的每小时平均速度

```text
# Find average bus speed in last hour  
# for each NYC borough  
SELECT loc.region, AVG(bus.speed) FROM bus  
INNER JOIN loc ON (bus.bus_id = loc.bus_id)  
WHERE loc.city = 'nyc'  
AND bus.time > now() - interval '1 hour'  
GROUP BY loc.region;  
```

7. 展示最近12小时，每小时的平均值

```text
=#  SELECT date_trunc('hour', time) AS hour, AVG(weight)  
    FROM logs  
    WHERE device_type = 'pressure-sensor' AND customer_id = 440  
      AND time > now() - interval '12 hours'  
    GROUP BY hour;  
  
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
=#  SELECT date_trunc('minute', time) AS minute, COUNT(device_id)  
    FROM logs  
    WHERE cpu_level > 0.9 AND free_mem < 1024  
      AND time > now() - interval '24 hours'  
    GROUP BY minute  
    ORDER BY COUNT(device_id) DESC LIMIT 25;  
  
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
=#  SELECT firmware_version, SUM(error_count) FROM logs  
    WHERE time > now() - interval '7 days'  
    GROUP BY firmware_version  
    ORDER BY SUM(error_count) DESC LIMIT 10;  
  
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
=#  SELECT date_trunc('hour', time) AS hour, COUNT(logs.device_id)  
    FROM logs  
    JOIN devices ON logs.device_id = devices.id  
    WHERE logs.temperature > 90 AND devices.location = 'SITE-1'  
    GROUP BY hour;  
  
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