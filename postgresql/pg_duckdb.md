#

- https://github.com/duckdb/pg_duckdb
- https://github.com/euiko/duckdb-pgwire

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

## 安装和配置

### 1. 安装 pg_duckdb

```bash
# 从源码编译安装
git clone https://github.com/pgspider/pg_duckdb.git
cd pg_duckdb
make
sudo make install

# 或使用包管理器（如果可用）
```

### 2. PostgreSQL 配置

```sql
-- 在 PostgreSQL 中加载扩展
CREATE EXTENSION pg_duckdb;

-- 创建外部数据包装器
CREATE FOREIGN DATA WRAPPER duckdb_wrapper
  HANDLER duckdb_fdw_handler
  VALIDATOR duckdb_fdw_validator;
```

## 基本使用示例

### 示例 1：直接读取单个 Parquet 文件

```sql
-- 创建服务器连接
CREATE SERVER duckdb_server
  FOREIGN DATA WRAPPER duckdb_wrapper
  OPTIONS (database ':memory:');

-- 创建用户映射
CREATE USER MAPPING FOR CURRENT_USER
  SERVER duckdb_server;

-- 创建外部表指向 Parquet 文件
CREATE FOREIGN TABLE sales_data (
    order_id INTEGER,
    product_id INTEGER,
    customer_id INTEGER,
    amount DECIMAL(10,2),
    order_date DATE
)
SERVER duckdb_server
OPTIONS (
    table 'SELECT * FROM read_parquet(''/path/to/sales.parquet'')'
);

-- 直接查询 Parquet 数据
SELECT * FROM sales_data WHERE amount > 1000;

-- 聚合查询
SELECT 
    customer_id,
    COUNT(*) as order_count,
    SUM(amount) as total_spent
FROM sales_data
GROUP BY customer_id
ORDER BY total_spent DESC;
```

### 示例 2：读取多个 Parquet 文件

```sql
-- 使用 glob 模式读取多个文件
CREATE FOREIGN TABLE monthly_sales (
    month VARCHAR(7),
    product_id INTEGER,
    revenue DECIMAL(12,2)
)
SERVER duckdb_server
OPTIONS (
    table 'SELECT * FROM read_parquet(''/path/to/sales_*.parquet'')'
);

-- 或者读取整个目录
CREATE FOREIGN TABLE all_sales_data (
    order_id INTEGER,
    product_id INTEGER,
    order_date DATE,
    amount DECIMAL(10,2)
)
SERVER duckdb_server
OPTIONS (
    table 'SELECT * FROM read_parquet(''/path/to/sales_data/*.parquet'')'
);
```

### 示例 3：使用通配符和模式匹配

```sql
-- 使用 DuckDB 的文件读取函数
CREATE FOREIGN TABLE partitioned_data (
    year INTEGER,
    month INTEGER,
    region VARCHAR(50),
    sales_amount DECIMAL(12,2)
)
SERVER duckdb_server
OPTIONS (
    table $$
        SELECT 
            *,
            filename as source_file
        FROM read_parquet(
            '/data/sales/year=*/month=*/region=*.parquet',
            hive_partitioning = true
        )
    $$
);

-- 查询时可以利用分区信息
SELECT 
    year,
    month,
    region,
    SUM(sales_amount) as total_sales
FROM partitioned_data
WHERE year = 2024 AND month IN (1, 2, 3)
GROUP BY year, month, region;
```

## 高级用法

### 1. 连接多个 Parquet 文件

```sql
CREATE FOREIGN TABLE combined_data AS
SERVER duckdb_server
OPTIONS (
    table $$
        WITH sales_2023 AS (
            SELECT *, 2023 as year FROM read_parquet('/data/sales_2023.parquet')
        ),
        sales_2024 AS (
            SELECT *, 2024 as year FROM read_parquet('/data/sales_2024.parquet')
        )
        SELECT * FROM sales_2023
        UNION ALL
        SELECT * FROM sales_2024
    $$
);
```

### 2. 指定列和数据类型

```sql
CREATE FOREIGN TABLE typed_parquet_data (
    id BIGINT,
    name TEXT,
    price DECIMAL(10,2),
    created_at TIMESTAMP,
    metadata JSONB
)
SERVER duckdb_server
OPTIONS (
    table $$
        SELECT 
            id::BIGINT,
            name::TEXT,
            price::DECIMAL(10,2),
            created_at::TIMESTAMP,
            metadata::JSON
        FROM read_parquet('/path/to/data.parquet')
    $$
);
```

### 3. 与 PostgreSQL 表 JOIN

```sql
-- PostgreSQL 本地表
CREATE TABLE customers (
    customer_id INTEGER PRIMARY KEY,
    customer_name VARCHAR(100),
    segment VARCHAR(50)
);

-- JOIN Parquet 数据与本地表
SELECT 
    c.customer_name,
    c.segment,
    s.total_spent,
    s.order_count
FROM customers c
JOIN (
    SELECT 
        customer_id,
        SUM(amount) as total_spent,
        COUNT(*) as order_count
    FROM sales_data  -- 这是我们的 Parquet 外部表
    GROUP BY customer_id
) s ON c.customer_id = s.customer_id;
```

## 性能优化技巧

### 1. 使用 WHERE 子句下推

```sql
-- DuckDB 会将过滤条件下推到 Parquet 文件读取层
CREATE FOREIGN TABLE optimized_query
SERVER duckckdb_server
OPTIONS (
    table $$
        SELECT * 
        FROM read_parquet('/path/to/large_data.parquet')
        WHERE date >= '2024-01-01'
          AND category = 'Electronics'
    $$
);
```

### 2. 列投影优化

```sql
-- 只读取需要的列
CREATE FOREIGN TABLE minimal_columns
SERVER duckdb_server
OPTIONS (
    table $$
        SELECT 
            customer_id,
            order_date,
            amount
        FROM read_parquet('/path/to/large_file.parquet')
    $$
);
```

### 3. 使用 Parquet 统计信息

```sql
-- DuckDB 会自动利用 Parquet 文件统计信息
-- 确保 Parquet 文件有正确的统计信息
CREATE FOREIGN TABLE with_stats
SERVER duckdb_server
OPTIONS (
    table $$
        SELECT *
        FROM read_parquet('/path/to/data.parquet')
        WHERE amount BETWEEN 100 AND 1000  -- 范围过滤可以利用统计信息
    $$
);
```

## 实用函数

### 1. 检查 Parquet 文件结构

```sql
-- 查看 Parquet 文件架构
CREATE FOREIGN TABLE parquet_schema
SERVER duckdb_server
OPTIONS (
    table $$
        DESCRIBE SELECT * FROM read_parquet('/path/to/file.parquet')
    $$
);

SELECT * FROM parquet_schema;
```

### 2. 获取文件信息

```sql
-- 查看文件元数据
CREATE FOREIGN TABLE file_info
SERVER duckdb_server
OPTIONS (
    table $$
        SELECT 
            filename,
            count(*) as row_count,
            min(amount) as min_amount,
            max(amount) as max_amount
        FROM read_parquet('/path/to/sales_*.parquet')
        GROUP BY filename
    $$
);
```

## 注意事项

### 文件路径权限
```bash
# 确保 PostgreSQL 用户有权限读取文件
sudo chmod +r /path/to/parquet/files
sudo chown postgres:postgres /path/to/parquet/files  # 如果使用 postgres 用户
```

### 内存使用
- 大文件可能需要调整 `shared_buffers` 和 `work_mem`
- 考虑使用 `LIMIT` 子句进行分页查询

### 数据类型映射
- Parquet 类型会自动映射到 PostgreSQL 类型
- 复杂类型（如 LIST、MAP）可能需要特殊处理

## 完整示例工作流

```sql
-- 1. 创建扩展和服务器
CREATE EXTENSION IF NOT EXISTS pg_duckdb;

CREATE SERVER IF NOT EXISTS duckdb_server
  FOREIGN DATA WRAPPER duckdb_wrapper
  OPTIONS (database ':memory:');

-- 2. 创建用户映射
CREATE USER MAPPING IF NOT EXISTS FOR CURRENT_USER
  SERVER duckdb_server;

-- 3. 创建外部表读取 Parquet
CREATE FOREIGN TABLE daily_sales_parquet (
    sale_date DATE,
    product_category VARCHAR(50),
    units_sold INTEGER,
    revenue DECIMAL(12,2),
    region VARCHAR(20)
)
SERVER duckdb_server
OPTIONS (
    table 'SELECT * FROM read_parquet(''/data/sales/daily_*.parquet'')'
);

-- 4. 创建物化视图加速查询
CREATE MATERIALIZED VIEW sales_summary AS
SELECT 
    DATE_TRUNC('month', sale_date) as month,
    product_category,
    region,
    SUM(units_sold) as total_units,
    SUM(revenue) as total_revenue,
    COUNT(*) as transaction_count
FROM daily_sales_parquet
GROUP BY DATE_TRUNC('month', sale_date), product_category, region;

-- 5. 查询物化视图
SELECT * FROM sales_summary 
WHERE month >= '2024-01-01'
ORDER BY total_revenue DESC;
```

使用 `pg_duckdb` 的主要优势是可以在 PostgreSQL 环境中直接利用 DuckDB 强大的 Parquet 读取能力，无需数据导入，特别适合临时分析和对大量 Parquet 文件的即席查询。
