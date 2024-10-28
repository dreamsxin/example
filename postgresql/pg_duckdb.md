#

pg_duckdb is a Postgres extension that embeds DuckDB's columnar-vectorized analytics engine and features into Postgres. We recommend using pg_duckdb to build high performance analytics and data-intensive applications.

提供了列存表和向量化执行能力，显著提升复杂查询的执行速度。

## Object storage bucket (AWS S3, Cloudflare R2, or Google GCS)

### Add a credential to enable DuckDB's httpfs support.
```psql
-- Session Token is Optional
INSERT INTO duckdb.secrets
(type, key_id, secret, session_token, region)
VALUES ('S3', 'access_key_id', 'secret_access_key', 'session_token', 'us-east-1')
```

### Copy data directly to your bucket - no ETL pipeline!
```psql
COPY (SELECT user_id, item_id, price, purchased_at FROM purchases)
TO 's3://your-bucket/purchases.parquet;
```
### Perform analytics on your data.
```psql
SELECT SUM(price) AS total, item_id
FROM read_parquet('s3://your-bucket/purchases.parquet')
  AS (price float, item_id int)
GROUP BY item_id
ORDER BY total DESC
LIMIT 100;
```
