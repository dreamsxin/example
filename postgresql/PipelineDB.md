# PipelineDB

下载地址 https://www.pipelinedb.com/download

## 安装

See http://docs.pipelinedb.com/installation.html


```shell
useradd pipeline
password pipeline


# 下载安装包之后安装
sudo rpm -ivh --prefix=/path/to/pipelinedb pipelinedb-<version>.rpm
sudo dpkg -i pipelinedb-<version>.deb

# or

docker run -v /dev/shm:/dev/shm pipelinedb/pipelinedb
```

## 初始化数据库

```shell
mkdir data
chown pipeline:pipeline data
sudo -u pipeline pipeline-init -D data
```

## 修改配置

```shell
vi data/pipelinedb.conf
```

## 运行数据库

```shell
sudo -u pipeline pipeline-ctl -D data -l pipelinedb.log start
# 运行在前台
sudo -u pipeline pipelinedb -D data
```

## Debug 模式下运行

```shell
pipeline-ctl -d -D ... start
pipeline-ctl --debug -D ... start
```

## 停止数据库

```shell
pipeline-ctl -D <data directory> stop
```

## 连接到数据库

```shell
# sudo pipeline -p 5434 [dbname [username]]
pipeline pipeline

psql -p 5432 -h localhost pipeline
# 激活 continuous query
psql -h localhost -p 5432 -d pipeline -c "ACTIVATE"
```

## 查看 Continuous Views

> Continuous view 自带属性 arrival_timestamp 表示数据接受时间

```sql
SELECT * FROM pipeline_views();
```

查看最近一分钟数据：
```sql
CREATE CONTINUOUS VIEW recent_users WITH (sw = '1 minute') AS
   SELECT user_id::integer FROM stream;

# 等价于
CREATE CONTINUOUS VIEW recent_users AS
   SELECT user_id::integer FROM stream WHERE (arrival_timestamp > clock_timestamp() - interval '1 minute');
```

* 子级 Continuous view

```sql
CREATE CONTINUOUS VIEW sw0 WITH (sw = '1 hour') AS SELECT COUNT(*) FROM event_stream;
CREATE VIEW sw1 WITH (sw = '5 minutes') AS SELECT * FROM sw0;
CREATE VIEW sw2 WITH (sw = '10 minutes') AS SELECT * FROM sw0;
```

* 多级处理

- old	旧值，上一次的值
- new	新值，本次值
- delta 新旧之间的差异值

```sql
CREATE STREAM stream (x integer, y integer);

CREATE CONTINUOUS VIEW v_sum AS SELECT sum(x) FROM stream;

CREATE CONTINUOUS VIEW v_deltas AS SELECT abs((new).sum - (old).sum) AS delta
  FROM output_of('v_sum')
  WHERE abs((new).sum - (old).sum) > 10;

CREATE CONTINUOUS VIEW v AS SELECT COUNT(*) FROM stream;

CREATE CONTINUOUS VIEW v_real_deltas AS SELECT (delta).sum FROM output_of('v');

CREATE CONTINUOUS VIEW uniques_1m AS
  SELECT minute(arrival_timestamp) AS ts, COUNT(DISTINCT user_id) AS uniques
FROM s GROUP BY ts;

CREATE CONTINUOUS VIEW uniques_hourly AS
  SELECT hour((new).ts) AS ts, combine((delta).uniques) AS uniques
FROM output_of('uniques_1m') GROUP BY ts;
```

## 备份与恢复

```shell
pipeline-dump -t <CV name> -t <CV name>_mrel # <-- Note the "_mrel" suffix

pipeline-dump > backup.sql
pipeline -f backup.sql
```

## 例子

本例是关于 Wikipedia页面访问数据的统计。每一条访问记录，包括以下字段，以英文逗号分割。
`hour,project,title,view_count,size`

### 创建数据表 Stream

http://docs.pipelinedb.com/streams.html

```shell
psql -h localhost -p 5432 -d pipeline -c "CREATE STREAM wiki_stream (hour timestamp, project text, title text, view_count bigint, size bigint);"
```

### 创建 Continuous view

首先，我们创建一个 Continuous view，使用psql工具。从sql里，我们能够看到统计方法和访问记录的对应关系。
```shell
psql -h localhost -p 5432 -d pipeline -c "
CREATE CONTINUOUS VIEW wiki_stats AS
SELECT hour, project,
        count(*) AS total_pages,
        sum(view_count) AS total_views,
        min(view_count) AS min_views,
        max(view_count) AS max_views,
        avg(view_count) AS avg_views,
        percentile_cont(0.99) WITHIN GROUP (ORDER BY view_count) AS p99_views,
        sum(size) AS total_bytes_served
FROM wiki_stream
GROUP BY hour, project;"
```

### 插入 Stream

我们通过curl工具，获取wiki的数据集，并压缩数据，作为一个Stream写入到stdin。因为数据集比较大，当我们执行了几秒钟之后，可以使用ctrl+c中断curl操作。
```shell
curl -sL http://pipelinedb.com/data/wiki-pagecounts | gunzip | \
        psql -h localhost -p 5432 -d pipeline -c "
        COPY wiki_stream (hour, project, title, view_count, size) FROM STDIN"
```

### 查看结果

```shell
psql -h localhost -p 5432 -d pipeline -c "SELECT * FROM wiki_stats ORDER BY total_views DESC";
```


## PipelineDB 和 kafka的集成

### 安装 pipeline_kafka 组件

PipelineDB 默认是没有`pipeline_kafka`扩展组件的，需要我们自己安装。

* 安装依赖 librdkafka

```shell
git clone -b 0.9.1 https://github.com/edenhill/librdkafka.git ~/librdkafka
cd ~/librdkafka
./configure --prefix=/usr
make
sudo make install
```

* 安装 pipeline_kafka

```shell
./configure
make
make install
```

*配置 pipeline_kafka

编辑配置文件 `/opt/pipelinedb/dbdata/pipelinedb.conf`

```conf
shared_preload_libraries = 'pipeline_kafka'
```

重启数据库，使得扩展组件生效
```shell
# pipeline-ctl -D <data directory> start|stop|restart
pipeline-ctl -D /opt/pipelinedb/dbdata restart
```

### Stream SQL开发过程

```shell
# 连接数据库
psql -h localhost -p 5432 -d pipeline
```

```sql
# 创建pipeline_kafka
CREATE EXTENSION pipeline_kafka;

# 配置kafka broker
SELECT pipeline_kafka.add_broker('192.168.1.170:9092');

# 创建Stream，从kafka里接受三个参数
CREATE STREAM msg_stream (sjc varchar, thread_name varchar, msg varchar);

# 创建CONTINUOUS VIEW
CREATE CONTINUOUS VIEW msg_result AS SELECT sjc,thread_name,msg FROM msg_stream;
# 开始消费kafka消息
# topic是my-topic，连接PipelineDB Stream名是msg_stream，消息类型是text，消息以英文逗号分割。
SELECT pipeline_kafka.consume_begin ( 'my-topic', 'msg_stream', format := 'text', 
            delimiter := ',', quote := NULL, escape := NULL, batchsize := 1000,
            maxbytes := 32000000, parallelism := 1, start_offset := NULL );


# 如果要停止Stream，请使用以下命令。
SELECT pipeline_kafka.consume_end('my-topic', 'msg_stream');
```

### 验证

* 向kafka发送消息

登录kafka节点的服务器，进入到kafka home路径，使用以下命令发送消息
```shell
# 启动producer
bin/kafka-console-producer.sh --broker-list 192.168.1.90:9092 --topic my-topic
```
输入以下数据
```text
a,b,c
```

* 在PipelineDB中查询收到的消息

从CONTINUOUS VIEW中 查询数据，可以看到有一条记录，即[a,b,c]。
```shell
psql -h localhost -p 5432 -d pipeline -c "SELECT * FROM msg_result";
```
ps: 当我们连接到PipelineDB，我们可以使用PostgreSQL的命令，来查看有那些数据库对象生成。例如通过 \d 可以查看到，当我们创建CONTINUOUS VIEW的时候，额外创建了msg_result_mrel、msg_result_seq和msg_result_osrel，实际的数据就存储在msg_result_mrel中。

