#

- https://github.com/paradedb/paradedb
- https://github.com/quickwit-oss/tantivy

使用搜索引擎 Tantivy，Tantivy 比 Lucene 更快.

## Docker

This will start a ParadeDB instance with default user postgres and password postgres

```shell
docker run --name paradedb -e POSTGRES_USER=myuser -e POSTGRES_PASSWORD=mypassword -e POSTGRES_DB=mydatabase -v paradedb_data:/var/lib/postgresql/data/ -p 5432:5432 -d paradedb/paradedb:latest
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

```shell
docker exec -it paradedb psql -U postgres
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
**创建索引**
```sql
CREATE INDEX search_idx ON mock_items
USING bm25 (id, description, category, rating, in_stock, created_at, metadata, weight_range)
WITH (key_field='id');
```

**查看指定索引模式**
```sql
SELECT name, field_type FROM paradedb.schema('search_idx');
```
**索引大小**
```sql
SELECT index_size FROM paradedb.index_size('search_idx');
```
**索引段**
```sql
SELECT * FROM paradedb.index_info('search_idx');
```

**搜索**
```sql
SELECT description, rating, category
FROM mock_items
WHERE description @@@ 'shoes' OR category @@@ 'footwear' AND rating @@@ '>2'
ORDER BY description
LIMIT 5;

SELECT description, rating, category, paradedb.score(id)
FROM mock_items
WHERE description @@@ 'shoes' OR category @@@ 'footwear' AND rating @@@ '>2'
ORDER BY score DESC, description
LIMIT 5;

SELECT description, rating, category
FROM mock_items
WHERE description @@@ '"white shoes"~1'
LIMIT 5;
```

**join**
```sql
CALL paradedb.create_bm25_test_table(
  schema_name => 'public',
  table_name => 'orders',
  table_type => 'Orders'
);

ALTER TABLE orders
ADD CONSTRAINT foreign_key_product_id
FOREIGN KEY (product_id)
REFERENCES mock_items(id);

SELECT * FROM orders LIMIT 3;
```
**创建索引**
```sql
CREATE INDEX orders_idx ON orders
USING bm25 (order_id, customer_name)
WITH (key_field='order_id');
```

**关联查询**
```sql
SELECT o.order_id, o.customer_name, m.description
FROM orders o
JOIN mock_items m ON o.product_id = m.id
WHERE o.customer_name @@@ 'Johnson' AND m.description @@@ 'shoes'
ORDER BY order_id
LIMIT 5;
```

**相似性搜索**
```sql
ALTER TABLE mock_items ADD COLUMN embedding vector(3);

UPDATE mock_items m
SET embedding = ('[' ||
    ((m.id + 1) % 10 + 1)::integer || ',' ||
    ((m.id + 2) % 10 + 1)::integer || ',' ||
    ((m.id + 3) % 10 + 1)::integer || ']')::vector;

SELECT description, rating, category, embedding
FROM mock_items
LIMIT 3;

CREATE INDEX on mock_items
USING hnsw (embedding vector_cosine_ops);

SELECT description, category, rating, embedding
FROM mock_items
ORDER BY embedding <=> '[1,2,3]', description
LIMIT 3;
```

## 创建 BM25 索引

**Syntax**
```sql
CREATE INDEX <index_name> ON <schema_name>.<table_name>
USING bm25 (<columns>)
WITH (key_field='<key_field>');
```
目标列表中的第一列。
- key_field
必须为唯一字段，整数类型性能最佳。
**Basic Usage**
```sql
CREATE INDEX search_idx ON mock_items
USING bm25 (id, description, category, rating, in_stock, created_at, metadata, weight_range)
WITH (key_field='id');
```
### WITH 选项
支持 `text_fields`、`json_fields`、`numeric_fields`、`numeric_fields`、`boolean_fields`、`datetime_fields`、`range_fields`、`range_fields`。

### 标记器 Tokenizers
指定文本的拆分方式，允许对同一个字段使用多个标记器。
`text_fields`、`json_fields` 支持 `tokenizer`。
**可用的 tokenizer 类型**
```sql
SELECT * FROM paradedb.tokenizers();
```

- default
默认，按空格和标点符号分割文本来标记文本，过滤掉大于 255 字节的标记，并转换为小写
```sql
CREATE INDEX search_idx ON mock_items
USING bm25 (id, description)
WITH (
    key_field = 'id',
    text_fields = '{
        "description": {
          "tokenizer": {"type": "default"}
        }
    }'
);
```
- whitespace
与 类似default，但仅基于空格进行拆分。过滤掉大于 255 字节的标记并转换为小写。
- raw
将整个文本视为单个标记。过滤掉大于 255 个字节的标记并转换为小写。
- regex
使用正则表达式对文本进行标记。可以使用参数指定正则表达式pattern。例如，\\W+按非单词字符进行拆分。
```sql
CREATE INDEX search_idx ON mock_items
USING bm25 (id, description)
WITH (
    key_field = 'id',
    text_fields = '{
        "description": {
          "tokenizer": {"type": "regex", "pattern": "\\W+"}
        }
    }'
);
```
- ngram
根据指定参数将单词拆分为重叠子字符串，从而对文本进行标记。例如，`3-gram` 标记器将单词拆分 `cheese` 为 `che`、`hee`、`ees`和`ese`。
```sql
CREATE INDEX search_idx ON mock_items
USING bm25 (id, description)
WITH (
    key_field = 'id',
    text_fields = '{
        "description": {
          "tokenizer": {"type": "ngram", "min_gram": 2, "max_gram": 3, "prefix_only": false}
        }
    }'
);
```
- source_code
根据代码中常用的大小写约定（如 camelCase 或 PascalCase）拆分文本，从而对文本进行标记。过滤掉超过 255 个字节的标记，并使用 ASCII 折叠将其转换为小写。
```sql
CREATE INDEX search_idx ON mock_items
USING bm25 (id, description)
WITH (
    key_field = 'id',
    text_fields = '{
        "description": {
          "tokenizer": {"type": "source_code"}
        }
    }'
);
```
- chinese_compatible
```sql
CREATE INDEX search_idx ON mock_items
USING bm25 (id, description)
WITH (
    key_field = 'id',
    text_fields = '{
        "description": {
          "tokenizer": {"type": "chinese_compatible"}
        }
    }'
);
```
- chinese_lindera
Lindera 标记器是一种更高级的 CJK 标记器，它使用预建的中文、日语或韩语词典将文本分解为有意义的标记（单词或短语），而不是单个字符。
```sql
CREATE INDEX search_idx ON mock_items
USING bm25 (id, description)
WITH (
    key_field = 'id',
    text_fields = '{
        "description": {
          "tokenizer": {"type": "chinese_lindera"}
        }
    }'
);
```
- icu
ICU（Unicode 国际化组件）标记器根据 Unicode 标准分解文本。它可用于标记大多数语言，并可识别不同语言之间单词边界的细微差别。

### 标记过滤器 Token Filters

- stemmer
- remove_long
默认为 255
- lowercase
默认为 true
### 例子
```sql
CREATE INDEX search_idx ON public.mock_items
USING bm25 (id, description, description_ngram, description_stem)
WITH (
    key_field='id',
    text_fields='{
        "description": {"tokenizer": {"type": "whitespace"}},
        "description_ngram": {"tokenizer": {"type": "ngram", "min_gram": 3, "max_gram": 3, "prefix_only": false} "column": "description"},
        "description_stem": {"tokenizer": {"type": "default", "stemmer": "English"}, "column": "description"}}
    }'
);

-- Example queries
SELECT * FROM mock_items WHERE id @@@ paradedb.parse('description_ngram:cam AND description_stem:digitally');
SELECT * FROM mock_items WHERE id @@@ paradedb.parse('description:"Soft cotton" OR description_stem:shirts');

CREATE INDEX search_idx ON mock_items
USING bm25 (id, description)
WITH (
    key_field = 'id',
    text_fields = '{
        "description": {
          "tokenizer": {"type": "ngram", "min_gram": 2, "max_gram": 3, "prefix_only": false} -- 定义分词器 en-stem，category 精确匹配
        }
    }'
);

CREATE INDEX search_idx ON mock_items
USING bm25 (id, description, category)
WITH (
    key_field = 'id',
    text_fields = '{
        "description": {
          "tokenizer": {"type": "ngram", "min_gram": 2, "max_gram": 3, "prefix_only": false} -- 定义分词器 en-stem，category 精确匹配
        },
        "category": {
            "tokenizer": {"type": "ngram", "min_gram": 2, "max_gram": 3, "prefix_only": false} -- 定义分词器 en-stem，category 精确匹配
        }
    }'
);

CREATE INDEX search_idx ON mock_items
USING bm25 (id, description)
WITH (
    key_field = 'id',
    text_fields = '{
        "description": {
          "fast": true,
          "tokenizer": {"type": "ngram", "min_gram": 2, "max_gram": 3, "prefix_only": false}
        }
    }'
);

CREATE INDEX search_idx ON mock_items
USING bm25 (id, metadata)
WITH (
  key_field = 'id',
  json_fields = '{
    "metadata": {
      "fast": true
    }
  }'
);

SELECT * FROM paradedb.format_create_bm25(
  index_name => 'search_idx',
  table_name => 'mock_items',
  key_field => 'id',
  text_fields => paradedb.field('description') || paradedb.field('category'),
  numeric_fields => paradedb.field('rating'),
  boolean_fields => paradedb.field('in_stock'),
  datetime_fields => paradedb.field('created_at'),
  json_fields => paradedb.field('metadata'),
  range_fields => paradedb.field('weight_range')
);

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

### ParadeDB provides the following tokenizer options:

- default: Splits text on whitespace and punctuation, removes long tokens, and converts to lowercase.
- raw: Does not process or tokenize text.
- en_stem: Similar to default, but also applies stemming to the tokens.
- whitespace: Tokenizes text by splitting on whitespaces only.
- ngram: Creates overlapping substrings based on specified parameters.
- chinese_compatible: Tokenizes text considering Chinese character nuances.
- chinese_lindera: Uses the Lindera tokenizer with the CC-CEDICT dictionary for Chinese text.
- korean_lindera: Uses the Lindera tokenizer with the KoDic dictionary for Korean text.
- japanese_lindera: Uses the Lindera tokenizer with the IPADIC dictionary for Japanese text.
- icu: Uses Unicode Text Segmentation, suitable for tokenizing most languages.

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

CREATE TABLE my_trips AS SELECT * FROM trips;

-- Success! Now you can query the remote Parquet file like a regular Postgres table
SELECT COUNT(*) FROM trips;
  count
---------
 2964624
(1 row)
```

