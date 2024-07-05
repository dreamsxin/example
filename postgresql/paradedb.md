#

- https://github.com/paradedb/paradedb
- https://github.com/quickwit-oss/tantivy

使用搜索引擎 Tantivy，Tantivy 比 Lucene 更快.

## Docker

This will start a ParadeDB instance with default user postgres and password postgres
```shell
docker exec -it paradedb psql -U postgres
```

```shell
docker run \
  --name paradedb \
  -e POSTGRESQL_USERNAME=<user> \
  -e POSTGRESQL_PASSWORD=<password> \
  -e POSTGRESQL_DATABASE=<dbname> \
  -e POSTGRESQL_POSTGRES_PASSWORD=<superuser_password> \
  -v paradedb_data:/bitnami/postgresql \
  -p 5432:5432 \
  -d \
  paradedb/paradedb:latest
```

## 创建测试数据

```sql
CALL paradedb.create_bm25_test_table(
  schema_name => 'public',
  table_name => 'mock_items'
);

SELECT description, rating, category
FROM mock_items
LIMIT 3;
```

## 创建 BM25  索引

```shell
CALL paradedb.create_bm25(
        index_name => 'search_idx',
        schema_name => 'public',
        table_name => 'mock_items',
        key_field => 'id', -- 唯一键
        text_fields => '{description: {tokenizer: {type: "en_stem"}}, category: {}}', -- 定义分词器 en-stem，category 精确匹配
        numeric_fields => '{rating: {}}'
);

CALL paradedb.create_bm25(
        index_name => 'ngrams_idx',
        schema_name => 'public',
        table_name => 'mock_items',
        key_field => 'id',
        text_fields => '{description: {tokenizer: {type: "ngram", min_gram: 4, max_gram: 4, prefix_only: false}}, category: {}}'
);
```

## 搜索

```shell
SELECT * FROM search_idx.search('description:shoes'); <-等效-> SELECT * FROM search_idx.search(query => paradedb.parse('description:shoes'));

SELECT description, rating, category
FROM search_idx.search(
  '(description:keyboard OR category:electronics) AND rating:>2',
  limit_rows => 5
);

-- 操作符 ~1 表示按照单个词分割词组，should 匹配任意一个词，词组必须包含在双引号内，
SELECT description, rating, category
FROM search_idx.search('description:"bluetooth speaker"~1');

SELECT description, rating, category
FROM ngrams_idx.search('description:blue');

-- 高亮显示
SELECT id, description, paradedb.highlight(id, field => 'description'), paradedb.rank_bm25(id), rating, category
FROM search_idx.search(
  '(description:keyboard OR category:electroni) AND rating:>2',
  limit_rows => 5
);

SELECT description, paradedb.highlight(id, field => 'description'), paradedb.rank_bm25(id)
FROM ngrams_idx.search('description:blue');
```

## 相似搜索

使用 L2 distance 欧式距离
```sql
ALTER TABLE mock_items ADD COLUMN embedding vector(3);
CREATE INDEX on mock_items
USING hnsw (embedding vector_l2_ops);

UPDATE mock_items m
SET embedding = ('[' ||
    ((m.id + 1) % 10 + 1)::integer || ',' ||
    ((m.id + 2) % 10 + 1)::integer || ',' ||
    ((m.id + 3) % 10 + 1)::integer || ']')::vector;

SELECT description, rating, category, embedding
FROM mock_items
LIMIT 3;

SELECT description, category, rating, embedding
FROM mock_items
ORDER BY embedding <-> '[1,2,3]'
LIMIT 3;
```

## 混合搜索 Hybrid Search
```sql
SELECT * FROM search_idx.rank_hybrid(
    bm25_query => 'description:keyboard OR category:electronics',
    similarity_query => '''[1,2,3]'' <-> embedding',
    bm25_weight => 0.9,
    similarity_weight => 0.1
) LIMIT 5;
```

## 搜索类型

```sql
-- Equality
'description:keyboard AND rating:4'

-- Simple range
'description:keyboard AND rating:>4'

-- Inclusive range
'description:keyboard AND rating:[2 TO 5]'

-- Exclusive range
'description:keyboard AND rating:{2 TO 5}'

-- Boolean
'description:keyboard AND is_available:false'
```

https://docs.paradedb.com/search/full-text/complex

### All
```sql
SELECT * FROM search_idx.search(
    query => paradedb.all()
);
```
### Boolean
```sql
SELECT * FROM search_idx.search(
    query => paradedb.boolean(
	    should => ARRAY[
		    paradedb.parse('description:shoes'),
		    paradedb.phrase_prefix(field => 'description', phrases => ARRAY['book']),
		    paradedb.term(field => 'description', value => 'speaker'),
		    paradedb.fuzzy_term(field => 'description', value => 'wolo')
	    ]
    )
);
```

**must**
A query object or an ARRAY of query objects as conditions which must be matched.

**must_not**
A query object or an ARRAY of query objects as conditions which must not be matched.

**should**
A query object or an ARRAY of query objects as conditions of which at least one must be matched.

## 外部数据包装器 Foreign Data Wrapper
支持格式： CSV, Parquet, Delta, and Iceberg。

## 接入 300 外部数据

```psql
# 创建外部数据包装器
CREATE FOREIGN DATA WRAPPER parquet_wrapper
HANDLER parquet_fdw_handler VALIDATOR parquet_fdw_validator;

# 创建外部服务
-- Provide S3 credentials
CREATE SERVER parquet_server FOREIGN DATA WRAPPER parquet_wrapper;

# 创建外部表
-- Create foreign table
CREATE FOREIGN TABLE trips (
    VendorID              INT,
    tpep_pickup_datetime  TIMESTAMP,
    tpep_dropoff_datetime TIMESTAMP,
    passenger_count       BIGINT,
    trip_distance         DOUBLE PRECISION,
    RatecodeID            DOUBLE PRECISION,
    store_and_fwd_flag    TEXT,
    PULocationID          REAL,
    DOLocationID          REAL,
    payment_type          DOUBLE PRECISION,
    fare_amount           DOUBLE PRECISION,
    extra                 DOUBLE PRECISION,
    mta_tax               DOUBLE PRECISION,
    tip_amount            DOUBLE PRECISION,
    tolls_amount          DOUBLE PRECISION,
    improvement_surcharge DOUBLE PRECISION,
    total_amount          DOUBLE PRECISION
)
SERVER parquet_server
OPTIONS (files 's3://paradedb-benchmarks/yellow_tripdata_2024-01.parquet');

-- Success! Now you can query the remote Parquet file like a regular Postgres table
SELECT COUNT(*) FROM trips;
  count
---------
 2964624
(1 row)
```

