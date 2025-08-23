# 使用Go和TimescaleDB开发用户行为分析系统

下面我将详细介绍如何使用Go语言和TimescaleDB构建用户行为分析系统的完整步骤，包括数据库设计、系统架构、部署方案和分析查询。

## 系统架构概述

```
用户客户端 → Go数据收集服务 → TimescaleDB存储 → Go数据分析API → 前端展示
```

## TimescaleDB vs ClickHouse 比较

### 选择TimescaleDB的理由
1. **完整的SQL支持**：TimescaleDB基于PostgreSQL，提供完整的SQL支持
2. **事务支持**：支持ACID事务，保证数据一致性
3. **生态系统**：可以充分利用PostgreSQL丰富的扩展和工具
4. **操作友好**：管理工具和监控工具更成熟
5. **实时分析**：适合需要实时查询和分析的场景

## 环境准备与部署

### 1. 部署TimescaleDB

**使用Docker部署TimescaleDB：**

```bash
# 创建数据目录
mkdir -p ~/timescaledb/data
mkdir -p ~/timescaledb/config

# 拉取TimescaleDB镜像
docker pull timescale/timescaledb:latest-pg14

# 运行TimescaleDB容器
docker run -d \
  --name timescaledb \
  -p 5432:5432 \
  -e POSTGRES_PASSWORD=password \
  -e POSTGRES_DB=user_analytics \
  -v ~/timescaledb/data:/var/lib/postgresql/data \
  -v ~/timescaledb/config:/etc/postgresql \
  timescale/timescaledb:latest-pg14
```

**验证安装：**
```bash
# 进入容器
docker exec -it timescaledb psql -U postgres -d user_analytics

# 或从外部连接
psql -h localhost -U postgres -d user_analytics
```

### 2. 安装Go环境

```bash
# 下载并安装Go
wget https://golang.org/dl/go1.19.linux-amd64.tar.gz
sudo tar -C /usr/local -xzf go1.19.linux-amd64.tar.gz

# 设置环境变量
echo 'export PATH=$PATH:/usr/local/go/bin' >> ~/.bashrc
echo 'export GOPATH=$HOME/go' >> ~/.bashrc
source ~/.bashrc

# 验证安装
go version
```

## 数据库设计与建表

### 1. 创建数据库和表

在TimescaleDB中执行以下SQL语句：

```sql
-- 启用TimescaleDB扩展
CREATE EXTENSION IF NOT EXISTS timescaledb;

-- 创建用户事件表
CREATE TABLE user_events (
    event_id UUID DEFAULT gen_random_uuid(),
    user_id BIGINT,
    session_id TEXT,
    event_type TEXT,
    event_name TEXT,
    page_url TEXT,
    referrer TEXT,
    device_type TEXT CHECK (device_type IN ('desktop', 'mobile', 'tablet', 'other')),
    browser TEXT,
    os TEXT,
    country_code CHAR(2),
    region TEXT,
    city TEXT,
    event_time TIMESTAMPTZ NOT NULL,
    properties JSONB,
    created_at TIMESTAMPTZ DEFAULT NOW()
);

-- 将表转换为超表（Hypertable）
SELECT create_hypertable('user_events', 'event_time');

-- 创建索引
CREATE INDEX idx_user_events_user_id ON user_events(user_id);
CREATE INDEX idx_user_events_event_time ON user_events(event_time);
CREATE INDEX idx_user_events_event_type ON user_events(event_type);
CREATE INDEX idx_user_events_event_name ON user_events(event_name);
CREATE INDEX idx_user_events_properties ON user_events USING GIN (properties);

-- 创建用于快速查询的物化视图
CREATE MATERIALIZED VIEW user_events_daily
WITH (timescaledb.continuous) AS
SELECT
    time_bucket('1 day', event_time) as bucket,
    event_type,
    country_code,
    COUNT(*) as event_count,
    COUNT(DISTINCT user_id) as unique_users
FROM user_events
GROUP BY bucket, event_type, country_code;

-- 添加自动刷新策略
SELECT add_continuous_aggregate_policy('user_events_daily',
  start_offset => INTERVAL '1 month',
  end_offset => INTERVAL '1 hour',
  schedule_interval => INTERVAL '1 hour');
```

## Go应用开发

### 1. 项目初始化

```bash
# 创建项目目录
mkdir user-analytics-system
cd user-analytics-system

# 初始化Go模块
go mod init github.com/your-username/user-analytics-system

# 安装依赖
go get github.com/jackc/pgx/v4
go get github.com/gin-gonic/gin
go get github.com/rs/zerolog/log
```

### 2. 项目结构

```
user-analytics-system/
├── cmd/
│   ├── collector/          # 数据收集服务
│   └── api/               # 数据分析API服务
├── internal/
│   ├── models/            # 数据模型
│   ├── repository/        # 数据访问层
│   └── service/           # 业务逻辑层
├── pkg/
│   └── postgres/          # PostgreSQL/TimescaleDB客户端配置
└── configs/               # 配置文件
```

### 3. TimescaleDB连接配置

创建 `pkg/postgres/client.go`:

```go
package postgres

import (
	"context"
	"fmt"
	"log"
	"time"

	"github.com/jackc/pgx/v4/pgxpool"
)

type Config struct {
	Host     string
	Port     int
	Database string
	Username string
	Password string
}

func NewConnection(cfg Config) (*pgxpool.Pool, error) {
	dsn := fmt.Sprintf("host=%s port=%d dbname=%s user=%s password=%s sslmode=disable",
		cfg.Host, cfg.Port, cfg.Database, cfg.Username, cfg.Password)

	config, err := pgxpool.ParseConfig(dsn)
	if err != nil {
		return nil, err
	}

	config.MaxConns = 25
	config.MinConns = 5
	config.HealthCheckPeriod = 1 * time.Minute
	config.MaxConnLifetime = 5 * time.Minute

	conn, err := pgxpool.ConnectConfig(context.Background(), config)
	if err != nil {
		return nil, err
	}

	// 测试连接
	if err := conn.Ping(context.Background()); err != nil {
		return nil, err
	}

	log.Println("Connected to TimescaleDB successfully")
	return conn, nil
}
```

### 4. 数据模型

创建 `internal/models/event.go`:

```go
package models

import (
	"encoding/json"
	"time"
)

type DeviceType string

const (
	Desktop DeviceType = "desktop"
	Mobile  DeviceType = "mobile"
	Tablet  DeviceType = "tablet"
	Other   DeviceType = "other"
)

type UserEvent struct {
	EventID     string                 `json:"event_id"`
	UserID      int64                  `json:"user_id"`
	SessionID   string                 `json:"session_id"`
	EventType   string                 `json:"event_type"`
	EventName   string                 `json:"event_name"`
	PageURL     string                 `json:"page_url"`
	Referrer    string                 `json:"referrer"`
	DeviceType  DeviceType             `json:"device_type"`
	Browser     string                 `json:"browser"`
	OS          string                 `json:"os"`
	CountryCode string                 `json:"country_code"`
	Region      string                 `json:"region"`
	City        string                 `json:"city"`
	EventTime   time.Time              `json:"event_time"`
	Properties  map[string]interface{} `json:"properties"`
	CreatedAt   time.Time              `json:"created_at"`
}

func (e *UserEvent) MarshalProperties() ([]byte, error) {
	return json.Marshal(e.Properties)
}

func (e *UserEvent) UnmarshalProperties(data []byte) error {
	return json.Unmarshal(data, &e.Properties)
}
```

### 5. 数据访问层

创建 `internal/repository/event_repository.go`:

```go
package repository

import (
	"context"
	"encoding/json"
	"fmt"
	"time"

	"github.com/your-username/user-analytics-system/internal/models"
	"github.com/jackc/pgx/v4/pgxpool"
)

type EventRepository struct {
	pool *pgxpool.Pool
}

func NewEventRepository(pool *pgxpool.Pool) *EventRepository {
	return &EventRepository{pool: pool}
}

func (r *EventRepository) Insert(event *models.UserEvent) error {
	query := `
		INSERT INTO user_events (
			user_id, session_id, event_type, event_name, 
			page_url, referrer, device_type, browser, os, 
			country_code, region, city, event_time, properties
		) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14)
	`
	
	propertiesJSON, err := event.MarshalProperties()
	if err != nil {
		return fmt.Errorf("failed to marshal properties: %v", err)
	}
	
	_, err = r.pool.Exec(context.Background(), query,
		event.UserID,
		event.SessionID,
		event.EventType,
		event.EventName,
		event.PageURL,
		event.Referrer,
		string(event.DeviceType),
		event.Browser,
		event.OS,
		event.CountryCode,
		event.Region,
		event.City,
		event.EventTime,
		propertiesJSON,
	)
	
	return err
}

func (r *EventRepository) GetEventsByTimeRange(start, end time.Time, limit int) ([]models.UserEvent, error) {
	query := `
		SELECT 
			event_id, user_id, session_id, event_type, event_name,
			page_url, referrer, device_type, browser, os,
			country_code, region, city, event_time, properties, created_at
		FROM user_events 
		WHERE event_time >= $1 AND event_time <= $2
		ORDER BY event_time DESC
		LIMIT $3
	`
	
	rows, err := r.pool.Query(context.Background(), query, start, end, limit)
	if err != nil {
		return nil, err
	}
	defer rows.Close()
	
	var events []models.UserEvent
	for rows.Next() {
		var event models.UserEvent
		var deviceTypeStr string
		var propertiesData []byte
		
		if err := rows.Scan(
			&event.EventID,
			&event.UserID,
			&event.SessionID,
			&event.EventType,
			&event.EventName,
			&event.PageURL,
			&event.Referrer,
			&deviceTypeStr,
			&event.Browser,
			&event.OS,
			&event.CountryCode,
			&event.Region,
			&event.City,
			&event.EventTime,
			&propertiesData,
			&event.CreatedAt,
		); err != nil {
			return nil, err
		}
		
		event.DeviceType = models.DeviceType(deviceTypeStr)
		
		// 解析JSON属性
		if err := event.UnmarshalProperties(propertiesData); err != nil {
			return nil, fmt.Errorf("failed to unmarshal properties: %v", err)
		}
		
		events = append(events, event)
	}
	
	return events, nil
}

// 批量插入方法
func (r *EventRepository) BatchInsert(events []models.UserEvent) error {
	tx, err := r.pool.Begin(context.Background())
	if err != nil {
		return err
	}
	defer tx.Rollback(context.Background())
	
	query := `
		INSERT INTO user_events (
			user_id, session_id, event_type, event_name, 
			page_url, referrer, device_type, browser, os, 
			country_code, region, city, event_time, properties
		) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14)
	`
	
	for _, event := range events {
		propertiesJSON, err := event.MarshalProperties()
		if err != nil {
			return fmt.Errorf("failed to marshal properties: %v", err)
		}
		
		_, err = tx.Exec(context.Background(), query,
			event.UserID,
			event.SessionID,
			event.EventType,
			event.EventName,
			event.PageURL,
			event.Referrer,
			string(event.DeviceType),
			event.Browser,
			event.OS,
			event.CountryCode,
			event.Region,
			event.City,
			event.EventTime,
			propertiesJSON,
		)
		
		if err != nil {
			return err
		}
	}
	
	return tx.Commit(context.Background())
}
```

### 6. 数据收集服务

创建 `cmd/collector/main.go`:

```go
package main

import (
	"encoding/json"
	"net/http"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/rs/zerolog/log"
	"github.com/your-username/user-analytics-system/internal/models"
	"github.com/your-username/user-analytics-system/internal/repository"
	"github.com/your-username/user-analytics-system/pkg/postgres"
)

func main() {
	// 初始化TimescaleDB连接
	cfg := postgres.Config{
		Host:     "localhost",
		Port:     5432,
		Database: "user_analytics",
		Username: "postgres",
		Password: "password",
	}
	
	pool, err := postgres.NewConnection(cfg)
	if err != nil {
		log.Fatal().Err(err).Msg("Failed to connect to TimescaleDB")
	}
	defer pool.Close()
	
	// 初始化仓库
	eventRepo := repository.NewEventRepository(pool)
	
	// 初始化Gin路由
	router := gin.Default()
	
	// 健康检查端点
	router.GET("/health", func(c *gin.Context) {
		c.JSON(http.StatusOK, gin.H{"status": "ok"})
	})
	
	// 事件收集端点
	router.POST("/track", func(c *gin.Context) {
		var event models.UserEvent
		if err := c.BindJSON(&event); err != nil {
			c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
			return
		}
		
		// 设置事件时间（如果未提供）
		if event.EventTime.IsZero() {
			event.EventTime = time.Now().UTC()
		}
		
		// 存储事件
		if err := eventRepo.Insert(&event); err != nil {
			log.Error().Err(err).Msg("Failed to insert event")
			c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to track event"})
			return
		}
		
		c.JSON(http.StatusOK, gin.H{"status": "success"})
	})
	
	// 批量事件收集端点
	router.POST("/batch", func(c *gin.Context) {
		var events []models.UserEvent
		if err := c.BindJSON(&events); err != nil {
			c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
			return
		}
		
		if err := eventRepo.BatchInsert(events); err != nil {
			log.Error().Err(err).Msg("Failed to insert events in batch")
			c.JSON(http.StatusInternalServerError, gin.H{"error": "Failed to track events"})
			return
		}
		
		c.JSON(http.StatusOK, gin.H{"status": "success", "processed": len(events)})
	})
	
	// 启动服务器
	log.Info().Msg("Starting collector server on :8080")
	if err := router.Run(":8080"); err != nil {
		log.Fatal().Err(err).Msg("Failed to start server")
	}
}
```

## TimescaleDB 分析查询示例

### 1. 漏斗分析SQL

```sql
-- 使用TimescaleDB的窗口函数进行漏斗分析
WITH user_journey AS (
  SELECT
    user_id,
    session_id,
    event_name,
    event_time,
    ROW_NUMBER() OVER (PARTITION BY user_id, session_id ORDER BY event_time) as step
  FROM user_events
  WHERE event_time >= NOW() - INTERVAL '7 days'
    AND event_name IN ('page_view', 'add_to_cart', 'checkout_start', 'purchase')
),
funnel_data AS (
  SELECT
    user_id,
    session_id,
    MAX(CASE WHEN event_name = 'page_view' THEN 1 ELSE 0 END) as viewed_page,
    MAX(CASE WHEN event_name = 'add_to_cart' THEN 1 ELSE 0 END) as added_to_cart,
    MAX(CASE WHEN event_name = 'checkout_start' THEN 1 ELSE 0 END) as started_checkout,
    MAX(CASE WHEN event_name = 'purchase' THEN 1 ELSE 0 END) as made_purchase
  FROM user_journey
  GROUP BY user_id, session_id
)
SELECT
  COUNT(*) as total_sessions,
  SUM(viewed_page) as page_views,
  SUM(added_to_cart) as add_to_carts,
  SUM(started_checkout) as checkouts_started,
  SUM(made_purchase) as purchases,
  ROUND(100.0 * SUM(added_to_cart) / SUM(viewed_page), 2) as view_to_cart_rate,
  ROUND(100.0 * SUM(started_checkout) / SUM(added_to_cart), 2) as cart_to_checkout_rate,
  ROUND(100.0 * SUM(made_purchase) / SUM(started_checkout), 2) as checkout_to_purchase_rate,
  ROUND(100.0 * SUM(made_purchase) / SUM(viewed_page), 2) as overall_conversion_rate
FROM funnel_data;
```

### 2. 留存分析SQL

```sql
-- 使用TimescaleDB的时间桶功能进行留存分析
WITH first_events AS (
  SELECT
    user_id,
    DATE(MIN(event_time)) as first_date
  FROM user_events
  WHERE event_time >= NOW() - INTERVAL '30 days'
  GROUP BY user_id
),
daily_activity AS (
  SELECT
    user_id,
    DATE(event_time) as activity_date
  FROM user_events
  WHERE event_time >= NOW() - INTERVAL '30 days'
  GROUP BY user_id, DATE(event_time)
)
SELECT
  first_date as cohort_date,
  COUNT(DISTINCT user_id) as cohort_size,
  COUNT(DISTINCT CASE WHEN activity_date = first_date + INTERVAL '1 day' THEN user_id END) as day_1_retained,
  COUNT(DISTINCT CASE WHEN activity_date = first_date + INTERVAL '7 day' THEN user_id END) as day_7_retained,
  COUNT(DISTINCT CASE WHEN activity_date = first_date + INTERVAL '30 day' THEN user_id END) as day_30_retained,
  ROUND(100.0 * COUNT(DISTINCT CASE WHEN activity_date = first_date + INTERVAL '1 day' THEN user_id END) / COUNT(DISTINCT user_id), 2) as day_1_retention_rate,
  ROUND(100.0 * COUNT(DISTINCT CASE WHEN activity_date = first_date + INTERVAL '7 day' THEN user_id END) / COUNT(DISTINCT user_id), 2) as day_7_retention_rate,
  ROUND(100.0 * COUNT(DISTINCT CASE WHEN activity_date = first_date + INTERVAL '30 day' THEN user_id END) / COUNT(DISTINCT user_id), 2) as day_30_retention_rate
FROM first_events
JOIN daily_activity USING (user_id)
WHERE first_date >= NOW() - INTERVAL '30 days'
GROUP BY first_date
ORDER BY first_date;
```

### 3. 用户行为路径分析

```sql
-- 使用TimescaleDB的窗口函数分析用户行为路径
WITH user_paths AS (
  SELECT
    user_id,
    session_id,
    event_name,
    event_time,
    LEAD(event_name) OVER (PARTITION BY user_id, session_id ORDER BY event_time) as next_event,
    LEAD(event_time) OVER (PARTITION BY user_id, session_id ORDER BY event_time) as next_event_time
  FROM user_events
  WHERE event_time >= NOW() - INTERVAL '7 days'
)
SELECT
  event_name,
  next_event,
  COUNT(*) as transition_count,
  AVG(EXTRACT(EPOCH FROM (next_event_time - event_time))) as avg_time_between_events
FROM user_paths
WHERE next_event IS NOT NULL
GROUP BY event_name, next_event
ORDER BY transition_count DESC
LIMIT 20;
```

### 4. 实时活跃用户分析

```sql
-- 使用TimescaleDB的连续聚合进行实时分析
SELECT
  bucket as date,
  event_type,
  country_code,
  event_count,
  unique_users
FROM user_events_daily
WHERE bucket >= NOW() - INTERVAL '7 days'
ORDER BY bucket DESC, event_count DESC;

-- 按小时统计活跃用户
SELECT
  time_bucket('1 hour', event_time) as hour,
  COUNT(DISTINCT user_id) as active_users
FROM user_events
WHERE event_time >= NOW() - INTERVAL '24 hours'
GROUP BY hour
ORDER BY hour DESC;
```

## 部署和运行

### 1. 使用Docker Compose部署

创建 `docker-compose.yml`:

```yaml
version: '3.8'

services:
  timescaledb:
    image: timescale/timescaledb:latest-pg14
    ports:
      - "5432:5432"
    environment:
      - POSTGRES_DB=user_analytics
      - POSTGRES_USER=admin
      - POSTGRES_PASSWORD=password
    volumes:
      - timescaledb_data:/var/lib/postgresql/data
    healthcheck:
      test: ["CMD-SHELL", "pg_isready -U admin -d user_analytics"]
      interval: 30s
      timeout: 10s
      retries: 5

  collector:
    build:
      context: .
      dockerfile: Dockerfile.collector
    ports:
      - "8080:8080"
    depends_on:
      timescaledb:
        condition: service_healthy
    environment:
      - DB_HOST=timescaledb
      - DB_PORT=5432
      - DB_NAME=user_analytics
      - DB_USER=admin
      - DB_PASSWORD=password

  api:
    build:
      context: .
      dockerfile: Dockerfile.api
    ports:
      - "8081:8081"
    depends_on:
      timescaledb:
        condition: service_healthy
    environment:
      - DB_HOST=timescaledb
      - DB_PORT=5432
      - DB_NAME=user_analytics
      - DB_USER=admin
      - DB_PASSWORD=password

volumes:
  timescaledb_data:
```

创建 `Dockerfile.collector`:

```dockerfile
FROM golang:1.19-alpine

WORKDIR /app

COPY go.mod go.sum ./
RUN go mod download

COPY . .

RUN go build -o collector ./cmd/collector

EXPOSE 8080

CMD ["./collector"]
```

创建 `Dockerfile.api`:

```dockerfile
FROM golang:1.19-alpine

WORKDIR /app

COPY go.mod go.sum ./
RUN go mod download

COPY . .

RUN go build -o api ./cmd/api

EXPOSE 8081

CMD ["./api"]
```

### 2. 启动服务

```bash
# 启动所有服务
docker-compose up -d

# 查看服务状态
docker-compose ps

# 查看日志
docker-compose logs -f
```

## 性能优化建议

1. **使用连续聚合**：利用TimescaleDB的连续聚合功能预计算常用指标
2. **合理设置分区间隔**：根据数据量和查询模式调整超表的分区间隔
3. **使用适当的索引**：为常用查询字段创建索引
4. **数据压缩**：启用TimescaleDB的压缩功能减少存储空间
5. **查询优化**：使用EXPLAIN分析查询性能

```sql
-- 启用压缩
ALTER TABLE user_events SET (
  timescaledb.compress,
  timescaledb.compress_orderby = 'event_time DESC',
  timescaledb.compress_segmentby = 'user_id, event_type'
);

-- 添加压缩策略
SELECT add_compression_policy('user_events', INTERVAL '7 days');
```

## 监控和维护

1. **监控数据库性能**：使用TimescaleDB的监控工具
2. **定期维护**：执行VACUUM和ANALYZE操作
3. **备份策略**：设置定期备份策略
4. **性能调优**：根据查询模式调整数据库配置

```sql
-- 查看超表信息
SELECT * FROM timescaledb_information.hypertables;

-- 查看压缩统计信息
SELECT * FROM timescaledb_information.compression_settings;

-- 查看连续聚合信息
SELECT * FROM timescaledb_information.continuous_aggregates;
```

这个基于TimescaleDB的用户行为分析系统提供了完整的数据收集、存储和分析功能。TimescaleDB的完整SQL支持和PostgreSQL生态系统使得开发和维护更加方便，同时提供了优秀的时序数据处理性能。

## 连续聚合

# 使用TimescaleDB连续聚合优化用户行为分析

下面我将详细介绍如何使用TimescaleDB的连续聚合(Continuous Aggregates)功能来优化漏斗分析、留存分析和用户行为路径分析的性能。

## 连续聚合概述

TimescaleDB的连续聚合功能可以自动、增量地维护预聚合查询结果，显著提高复杂分析查询的性能。与物化视图不同，连续聚合支持增量更新，无需完全重新计算。

## 1. 漏斗分析优化

### 创建漏斗分析连续聚合

```sql
-- 创建用户会话级别的漏斗步骤记录
CREATE MATERIALIZED VIEW user_session_funnel
WITH (timescaledb.continuous) AS
SELECT
  user_id,
  session_id,
  time_bucket('1 hour', event_time) as bucket,
  -- 记录每个步骤的最早发生时间
  min(event_time) FILTER (WHERE event_name = 'page_view') as page_view_time,
  min(event_time) FILTER (WHERE event_name = 'add_to_cart') as add_to_cart_time,
  min(event_time) FILTER (WHERE event_name = 'checkout_start') as checkout_start_time,
  min(event_time) FILTER (WHERE event_name = 'purchase') as purchase_time,
  -- 记录是否完成每个步骤
  bool_and(event_name = 'page_view') as has_page_view,
  bool_and(event_name = 'add_to_cart') as has_add_to_cart,
  bool_and(event_name = 'checkout_start') as has_checkout_start,
  bool_and(event_name = 'purchase') as has_purchase
FROM user_events
WHERE event_name IN ('page_view', 'add_to_cart', 'checkout_start', 'purchase')
GROUP BY user_id, session_id, bucket;

-- 添加刷新策略
SELECT add_continuous_aggregate_policy('user_session_funnel',
  start_offset => INTERVAL '7 days',
  end_offset => INTERVAL '1 hour',
  schedule_interval => INTERVAL '1 hour');
```

### 创建漏斗汇总连续聚合

```sql
-- 创建按小时汇总的漏斗数据
CREATE MATERIALIZED VIEW funnel_summary_hourly
WITH (timescaledb.continuous) AS
SELECT
  bucket,
  COUNT(*) as total_sessions,
  COUNT(has_page_view) as step1_page_view,
  COUNT(has_add_to_cart) as step2_add_to_cart,
  COUNT(has_checkout_start) as step3_checkout_start,
  COUNT(has_purchase) as step4_purchase,
  -- 计算按时序完成的转化（后一步骤在前一步骤之后发生）
  COUNT(CASE WHEN add_to_cart_time > page_view_time THEN 1 END) as step1_to_step2,
  COUNT(CASE WHEN checkout_start_time > add_to_cart_time THEN 1 END) as step2_to_step3,
  COUNT(CASE WHEN purchase_time > checkout_start_time THEN 1 END) as step3_to_step4
FROM user_session_funnel
GROUP BY bucket;

-- 添加刷新策略
SELECT add_continuous_aggregate_policy('funnel_summary_hourly',
  start_offset => INTERVAL '7 days',
  end_offset => INTERVAL '1 hour',
  schedule_interval => INTERVAL '1 hour');
```

### 优化后的漏斗分析查询

```sql
-- 查询最近24小时的漏斗数据
SELECT
  time_bucket('1 day', bucket) as day,
  SUM(total_sessions) as total_sessions,
  SUM(step1_page_view) as page_views,
  SUM(step2_add_to_cart) as add_to_carts,
  SUM(step3_checkout_start) as checkouts_started,
  SUM(step4_purchase) as purchases,
  ROUND(100.0 * SUM(step1_to_step2) / SUM(step1_page_view), 2) as view_to_cart_rate,
  ROUND(100.0 * SUM(step2_to_step3) / SUM(step2_add_to_cart), 2) as cart_to_checkout_rate,
  ROUND(100.0 * SUM(step3_to_step4) / SUM(step3_checkout_start), 2) as checkout_to_purchase_rate,
  ROUND(100.0 * SUM(step4_purchase) / SUM(step1_page_view), 2) as overall_conversion_rate
FROM funnel_summary_hourly
WHERE bucket >= NOW() - INTERVAL '24 hours'
GROUP BY day
ORDER BY day;
```

## 2. 留存分析优化

### 创建用户首次活动连续聚合

```sql
-- 创建用户首次活动时间的连续聚合
CREATE MATERIALIZED VIEW user_first_activity
WITH (timescaledb.continuous) AS
SELECT
  user_id,
  min(time_bucket('1 day', event_time)) as first_activity_date
FROM user_events
GROUP BY user_id;

-- 添加刷新策略
SELECT add_continuous_aggregate_policy('user_first_activity',
  start_offset => INTERVAL '90 days',
  end_offset => INTERVAL '1 day',
  schedule_interval => INTERVAL '1 day');
```

### 创建用户每日活动连续聚合

```sql
-- 创建用户每日活动状态的连续聚合
CREATE MATERIALIZED VIEW user_daily_activity
WITH (timescaledb.continuous) AS
SELECT
  user_id,
  time_bucket('1 day', event_time) as activity_date,
  COUNT(*) as event_count
FROM user_events
GROUP BY user_id, activity_date;

-- 添加刷新策略
SELECT add_continuous_aggregate_policy('user_daily_activity',
  start_offset => INTERVAL '90 days',
  end_offset => INTERVAL '1 day',
  schedule_interval => INTERVAL '1 day');
```

### 创建留存分析连续聚合

```sql
-- 创建留存分析专用连续聚合
CREATE MATERIALIZED VIEW retention_analysis
WITH (timescaledb.continuous) AS
SELECT
  ufa.first_activity_date as cohort_date,
  uda.activity_date,
  COUNT(DISTINCT uda.user_id) as active_users,
  COUNT(DISTINCT CASE WHEN uda.activity_date = ufa.first_activity_date THEN uda.user_id END) as cohort_size
FROM user_daily_activity uda
JOIN user_first_activity ufa ON uda.user_id = ufa.user_id
WHERE uda.activity_date >= ufa.first_activity_date
GROUP BY ufa.first_activity_date, uda.activity_date;

-- 添加刷新策略
SELECT add_continuous_aggregate_policy('retention_analysis',
  start_offset => INTERVAL '90 days',
  end_offset => INTERVAL '1 day',
  schedule_interval => INTERVAL '1 day');
```

### 优化后的留存分析查询

```sql
-- 查询最近30天的留存情况
SELECT
  cohort_date,
  cohort_size,
  MAX(CASE WHEN activity_date = cohort_date + INTERVAL '1 day' THEN active_users END) as day_1_retained,
  MAX(CASE WHEN activity_date = cohort_date + INTERVAL '7 days' THEN active_users END) as day_7_retained,
  MAX(CASE WHEN activity_date = cohort_date + INTERVAL '30 days' THEN active_users END) as day_30_retained,
  ROUND(100.0 * MAX(CASE WHEN activity_date = cohort_date + INTERVAL '1 day' THEN active_users END) / cohort_size, 2) as day_1_retention_rate,
  ROUND(100.0 * MAX(CASE WHEN activity_date = cohort_date + INTERVAL '7 days' THEN active_users END) / cohort_size, 2) as day_7_retention_rate,
  ROUND(100.0 * MAX(CASE WHEN activity_date = cohort_date + INTERVAL '30 days' THEN active_users END) / cohort_size, 2) as day_30_retention_rate
FROM retention_analysis
WHERE cohort_date >= NOW() - INTERVAL '30 days'
GROUP BY cohort_date, cohort_size
ORDER BY cohort_date;
```

## 3. 用户行为路径分析优化

### 创建用户会话路径连续聚合

```sql
-- 创建用户会话路径的连续聚合
CREATE MATERIALIZED VIEW user_session_paths
WITH (timescaledb.continuous) AS
SELECT
  user_id,
  session_id,
  time_bucket('1 hour', event_time) as bucket,
  -- 记录事件序列
  array_agg(event_name ORDER BY event_time) as event_sequence,
  -- 记录事件时间序列
  array_agg(event_time ORDER BY event_time) as event_times,
  -- 记录页面URL序列
  array_agg(page_url ORDER BY event_time) as page_urls,
  COUNT(*) as event_count
FROM user_events
GROUP BY user_id, session_id, bucket;

-- 添加刷新策略
SELECT add_continuous_aggregate_policy('user_session_paths',
  start_offset => INTERVAL '7 days',
  end_offset => INTERVAL '1 hour',
  schedule_interval => INTERVAL '1 hour');
```

### 创建常见路径分析连续聚合

```sql
-- 创建常见路径分析的连续聚合
CREATE MATERIALIZED VIEW common_paths_analysis
WITH (timescaledb.continuous) AS
SELECT
  time_bucket('1 day', bucket) as day,
  -- 获取最常见的前10条路径
  (array_agg(event_sequence ORDER BY COUNT(*) DESC))[1:10] as top_paths,
  -- 获取最常见的前10条路径的出现次数
  (array_agg(COUNT(*) ORDER BY COUNT(*) DESC))[1:10] as path_counts,
  -- 计算平均会话长度
  AVG(event_count) as avg_session_length,
  -- 计算唯一会话数量
  COUNT(DISTINCT session_id) as unique_sessions
FROM user_session_paths
GROUP BY day;

-- 添加刷新策略
SELECT add_continuous_aggregate_policy('common_paths_analysis',
  start_offset => INTERVAL '7 days',
  end_offset => INTERVAL '1 day',
  schedule_interval => INTERVAL '1 day');
```

### 创建页面流转分析连续聚合

```sql
-- 创建页面流转分析的连续聚合
CREATE MATERIALIZED VIEW page_transition_analysis
WITH (timescaledb.continuous) AS
WITH transitions AS (
  SELECT
    user_id,
    session_id,
    time_bucket('1 hour', event_time) as bucket,
    page_url as from_page,
    LEAD(page_url) OVER (PARTITION BY session_id ORDER BY event_time) as to_page
  FROM user_events
  WHERE event_name = 'page_view'
)
SELECT
  bucket,
  from_page,
  to_page,
  COUNT(*) as transition_count
FROM transitions
WHERE to_page IS NOT NULL
GROUP BY bucket, from_page, to_page;

-- 添加刷新策略
SELECT add_continuous_aggregate_policy('page_transition_analysis',
  start_offset => INTERVAL '7 days',
  end_offset => INTERVAL '1 hour',
  schedule_interval => INTERVAL '1 hour');
```

### 优化后的路径分析查询

```sql
-- 查询最近7天最常见的用户路径
SELECT
  day,
  unnest(top_paths) as common_path,
  unnest(path_counts) as path_count,
  avg_session_length,
  unique_sessions
FROM common_paths_analysis
WHERE day >= NOW() - INTERVAL '7 days'
ORDER BY day, path_count DESC;

-- 查询最近24小时最频繁的页面流转
SELECT
  time_bucket('1 day', bucket) as day,
  from_page,
  to_page,
  SUM(transition_count) as total_transitions
FROM page_transition_analysis
WHERE bucket >= NOW() - INTERVAL '24 hours'
GROUP BY day, from_page, to_page
ORDER BY day, total_transitions DESC
LIMIT 20;
```

## 4. 综合性能优化策略

### 创建多维度分析连续聚合

```sql
-- 创建用户行为的多维度分析连续聚合
CREATE MATERIALIZED VIEW user_behavior_multi_dimension
WITH (timescaledb.continuous) AS
SELECT
  time_bucket('1 hour', event_time) as bucket,
  -- 维度分组
  device_type,
  country_code,
  -- 指标计算
  COUNT(*) as total_events,
  COUNT(DISTINCT user_id) as unique_users,
  COUNT(DISTINCT session_id) as unique_sessions,
  -- 关键事件计数
  COUNT(*) FILTER (WHERE event_name = 'page_view') as page_views,
  COUNT(*) FILTER (WHERE event_name = 'add_to_cart') as add_to_carts,
  COUNT(*) FILTER (WHERE event_name = 'purchase') as purchases,
  -- 转化率计算
  CASE 
    WHEN COUNT(*) FILTER (WHERE event_name = 'page_view') > 0 
    THEN 100.0 * COUNT(*) FILTER (WHERE event_name = 'purchase') / COUNT(*) FILTER (WHERE event_name = 'page_view')
    ELSE 0 
  END as conversion_rate
FROM user_events
GROUP BY bucket, device_type, country_code;

-- 添加刷新策略
SELECT add_continuous_aggregate_policy('user_behavior_multi_dimension',
  start_offset => INTERVAL '7 days',
  end_offset => INTERVAL '1 hour',
  schedule_interval => INTERVAL '1 hour');
```

### 创建实时监控连续聚合

```sql
-- 创建实时监控用的连续聚合（更短的刷新间隔）
CREATE MATERIALIZED VIEW real_time_monitoring
WITH (timescaledb.continuous) AS
SELECT
  time_bucket('1 minute', event_time) as bucket,
  event_name,
  COUNT(*) as event_count,
  COUNT(DISTINCT user_id) as unique_users
FROM user_events
WHERE event_time >= NOW() - INTERVAL '1 hour'
GROUP BY bucket, event_name;

-- 添加刷新策略（更频繁的刷新）
SELECT add_continuous_aggregate_policy('real_time_monitoring',
  start_offset => INTERVAL '1 hour',
  end_offset => INTERVAL '1 minute',
  schedule_interval => INTERVAL '1 minute');
```

## 5. Go代码集成

### 更新数据访问层以利用连续聚合

```go
// 在repository中添加专门的方法来查询连续聚合
func (r *EventRepository) GetFunnelAnalysis(start, end time.Time) ([]FunnelResult, error) {
    query := `
        SELECT
            bucket as time_period,
            SUM(total_sessions) as total_sessions,
            SUM(step1_page_view) as page_views,
            SUM(step2_add_to_cart) as add_to_carts,
            SUM(step3_checkout_start) as checkouts_started,
            SUM(step4_purchase) as purchases
        FROM funnel_summary_hourly
        WHERE bucket >= $1 AND bucket <= $2
        GROUP BY time_bucket('1 day', bucket)
        ORDER BY time_period
    `
    
    rows, err := r.pool.Query(context.Background(), query, start, end)
    if err != nil {
        return nil, err
    }
    defer rows.Close()
    
    var results []FunnelResult
    for rows.Next() {
        var result FunnelResult
        if err := rows.Scan(
            &result.TimePeriod,
            &result.TotalSessions,
            &result.PageViews,
            &result.AddToCarts,
            &result.CheckoutsStarted,
            &result.Purchases,
        ); err != nil {
            return nil, err
        }
        results = append(results, result)
    }
    
    return results, nil
}

// 添加留存分析查询方法
func (r *EventRepository) GetRetentionAnalysis(cohortDate time.Time) ([]RetentionResult, error) {
    query := `
        SELECT
            activity_date,
            active_users,
            cohort_size,
            ROUND(100.0 * active_users / cohort_size, 2) as retention_rate
        FROM retention_analysis
        WHERE cohort_date = $1
        ORDER BY activity_date
    `
    
    rows, err := r.pool.Query(context.Background(), query, cohortDate)
    if err != nil {
        return nil, err
    }
    defer rows.Close()
    
    var results []RetentionResult
    for rows.Next() {
        var result RetentionResult
        if err := rows.Scan(
            &result.ActivityDate,
            &result.ActiveUsers,
            &result.CohortSize,
            &result.RetentionRate,
        ); err != nil {
            return nil, err
        }
        results = append(results, result)
    }
    
    return results, nil
}
```

## 6. 监控和维护连续聚合

### 监控连续聚合状态

```sql
-- 查看所有连续聚合的信息
SELECT * FROM timescaledb_information.continuous_aggregates;

-- 查看连续聚合的作业状态
SELECT * FROM timescaledb_information.jobs WHERE proc_name LIKE '%continuous_aggregate%';

-- 查看连续聚合的详细统计信息
SELECT * FROM timescaledb_information.continuous_aggregate_stats;
```

### 手动刷新连续聚合

```sql
-- 手动刷新连续聚合（如果需要立即更新）
CALL refresh_continuous_aggregate('funnel_summary_hourly', NULL, NULL);

-- 刷新特定时间范围的连续聚合
CALL refresh_continuous_aggregate(
  'funnel_summary_hourly', 
  '2023-01-01 00:00:00'::timestamptz, 
  '2023-01-02 00:00:00'::timestamptz
);
```

### 调整连续聚合策略

```sql
-- 调整连续聚合的刷新策略
SELECT remove_continuous_aggregate_policy('funnel_summary_hourly');
SELECT add_continuous_aggregate_policy('funnel_summary_hourly',
  start_offset => INTERVAL '14 days',
  end_offset => INTERVAL '30 minutes',
  schedule_interval => INTERVAL '30 minutes');
```

## 7. 性能优化建议

1. **合理设置时间桶大小**：根据数据量和查询需求选择合适的时间桶大小
2. **调整刷新策略**：根据数据更新频率和查询实时性需求调整刷新间隔
3. **使用适当的分区键**：确保连续聚合的分区键与查询模式匹配
4. **监控资源使用**：定期监控连续聚合的存储使用和刷新性能
5. **定期维护**：定期检查并优化连续聚合的配置

```sql
-- 查看连续聚合的空间使用情况
SELECT 
  hypertable_name,
  pg_size_pretty(hypertable_size) as hypertable_size,
  pg_size_pretty(total_bytes) as total_size
FROM timescaledb_information.continuous_aggregates
JOIN hypertable_size ON hypertable_name = view_name;
```

通过使用TimescaleDB的连续聚合功能，我们可以显著提高复杂分析查询的性能，同时保持数据的近实时更新。这些优化特别适合用户行为分析这种需要处理大量时序数据并执行复杂聚合的场景。

## 优化

# 优化TimescaleDB数据库结构以去除冗余数据

下面我将详细介绍如何优化TimescaleDB数据库结构，去除冗余数据，提高存储效率和查询性能。

## 数据库结构优化策略

### 1. 规范化设计

#### 创建维度表以减少冗余

```sql
-- 创建设备类型维度表
CREATE TABLE dim_device_types (
    device_type_id SERIAL PRIMARY KEY,
    device_type_name VARCHAR(20) NOT NULL UNIQUE,
    description TEXT
);

INSERT INTO dim_device_types (device_type_name, description) VALUES
('desktop', '台式电脑'),
('mobile', '移动手机'),
('tablet', '平板电脑'),
('other', '其他设备');

-- 创建浏览器维度表
CREATE TABLE dim_browsers (
    browser_id SERIAL PRIMARY KEY,
    browser_name VARCHAR(50) NOT NULL UNIQUE,
    browser_family VARCHAR(50)
);

-- 创建操作系统维度表
CREATE TABLE dim_operating_systems (
    os_id SERIAL PRIMARY KEY,
    os_name VARCHAR(50) NOT NULL UNIQUE,
    os_family VARCHAR(50)
);

-- 创建国家维度表
CREATE TABLE dim_countries (
    country_code CHAR(2) PRIMARY KEY,
    country_name VARCHAR(100) NOT NULL,
    region VARCHAR(100),
    continent VARCHAR(50)
);

-- 创建事件类型维度表
CREATE TABLE dim_event_types (
    event_type_id SERIAL PRIMARY KEY,
    event_type_name VARCHAR(50) NOT NULL UNIQUE,
    category VARCHAR(50)
);

-- 创建事件名称维度表
CREATE TABLE dim_event_names (
    event_name_id SERIAL PRIMARY KEY,
    event_name VARCHAR(100) NOT NULL UNIQUE,
    event_type_id INT REFERENCES dim_event_types(event_type_id)
);
```

### 2. 优化用户事件表结构

```sql
-- 删除原有的user_events表（如果存在）
DROP TABLE IF EXISTS user_events CASCADE;

-- 创建优化后的用户事件表
CREATE TABLE user_events (
    event_id UUID DEFAULT gen_random_uuid() PRIMARY KEY,
    user_id BIGINT NOT NULL,
    session_id VARCHAR(64) NOT NULL,
    event_name_id INT NOT NULL REFERENCES dim_event_names(event_name_id),
    page_url VARCHAR(500),
    referrer VARCHAR(500),
    device_type_id INT NOT NULL REFERENCES dim_device_types(device_type_id),
    browser_id INT REFERENCES dim_browsers(browser_id),
    os_id INT REFERENCES dim_operating_systems(os_id),
    country_code CHAR(2) REFERENCES dim_countries(country_code),
    event_time TIMESTAMPTZ NOT NULL,
    properties JSONB,
    created_at TIMESTAMPTZ DEFAULT NOW()
);

-- 将表转换为超表（Hypertable）
SELECT create_hypertable('user_events', 'event_time');

-- 创建索引
CREATE INDEX idx_user_events_user_id ON user_events(user_id);
CREATE INDEX idx_user_events_session_id ON user_events(session_id);
CREATE INDEX idx_user_events_event_time ON user_events(event_time);
CREATE INDEX idx_user_events_event_name_id ON user_events(event_name_id);
CREATE INDEX idx_user_events_properties ON user_events USING GIN (properties);
CREATE INDEX idx_user_events_composite ON user_events(user_id, event_time);

-- 启用压缩
ALTER TABLE user_events SET (
  timescaledb.compress,
  timescaledb.compress_orderby = 'event_time DESC',
  timescaledb.compress_segmentby = 'user_id, event_name_id'
);

-- 添加压缩策略
SELECT add_compression_policy('user_events', INTERVAL '7 days');
```

### 3. 创建会话摘要表

```sql
-- 创建用户会话摘要表
CREATE TABLE user_sessions (
    session_id VARCHAR(64) PRIMARY KEY,
    user_id BIGINT NOT NULL,
    start_time TIMESTAMPTZ NOT NULL,
    end_time TIMESTAMPTZ NOT NULL,
    duration INTERVAL NOT NULL,
    page_count INT NOT NULL DEFAULT 0,
    device_type_id INT REFERENCES dim_device_types(device_type_id),
    browser_id INT REFERENCES dim_browsers(browser_id),
    os_id INT REFERENCES dim_operating_systems(os_id),
    country_code CHAR(2) REFERENCES dim_countries(country_code),
    entry_page VARCHAR(500),
    exit_page VARCHAR(500),
    created_at TIMESTAMPTZ DEFAULT NOW(),
    updated_at TIMESTAMPTZ DEFAULT NOW()
);

-- 将会话表也转换为超表
SELECT create_hypertable('user_sessions', 'start_time');

-- 创建索引
CREATE INDEX idx_user_sessions_user_id ON user_sessions(user_id);
CREATE INDEX idx_user_sessions_start_time ON user_sessions(start_time);
CREATE INDEX idx_user_sessions_duration ON user_sessions(duration);
```

### 4. 创建用户属性表

```sql
-- 创建用户属性表（存储不经常变化的用户属性）
CREATE TABLE user_attributes (
    user_id BIGINT PRIMARY KEY,
    first_seen TIMESTAMPTZ NOT NULL,
    last_seen TIMESTAMPTZ NOT NULL,
    total_events INT DEFAULT 0,
    total_sessions INT DEFAULT 0,
    preferred_device_type_id INT REFERENCES dim_device_types(device_type_id),
    preferred_browser_id INT REFERENCES dim_browsers(browser_id),
    preferred_os_id INT REFERENCES dim_operating_systems(os_id),
    country_code CHAR(2) REFERENCES dim_countries(country_code),
    acquisition_source VARCHAR(100),
    acquisition_campaign VARCHAR(100),
    created_at TIMESTAMPTZ DEFAULT NOW(),
    updated_at TIMESTAMPTZ DEFAULT NOW()
);

-- 创建更新用户属性的函数
CREATE OR REPLACE FUNCTION update_user_attributes()
RETURNS TRIGGER AS $$
BEGIN
    INSERT INTO user_attributes (user_id, first_seen, last_seen, total_events, total_sessions)
    VALUES (NEW.user_id, NEW.event_time, NEW.event_time, 1, 0)
    ON CONFLICT (user_id) DO UPDATE
    SET 
        last_seen = GREATEST(user_attributes.last_seen, NEW.event_time),
        total_events = user_attributes.total_events + 1,
        updated_at = NOW();
    
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- 创建触发器
CREATE TRIGGER trg_update_user_attributes
AFTER INSERT ON user_events
FOR EACH ROW EXECUTE FUNCTION update_user_attributes();
```

## 数据填充和迁移

### 1. 填充维度表

```sql
-- 从现有数据中提取并填充浏览器维度表
INSERT INTO dim_browsers (browser_name, browser_family)
SELECT DISTINCT 
    browser,
    CASE 
        WHEN browser ILIKE '%chrome%' THEN 'Chrome'
        WHEN browser ILIKE '%firefox%' THEN 'Firefox'
        WHEN browser ILIKE '%safari%' THEN 'Safari'
        WHEN browser ILIKE '%edge%' THEN 'Edge'
        WHEN browser ILIKE '%opera%' THEN 'Opera'
        ELSE 'Other'
    END
FROM user_events_old
WHERE browser IS NOT NULL;

-- 从现有数据中提取并填充操作系统维度表
INSERT INTO dim_operating_systems (os_name, os_family)
SELECT DISTINCT 
    os,
    CASE 
        WHEN os ILIKE '%windows%' THEN 'Windows'
        WHEN os ILIKE '%mac%' OR os ILIKE '%os x%' THEN 'macOS'
        WHEN os ILIKE '%linux%' THEN 'Linux'
        WHEN os ILIKE '%android%' THEN 'Android'
        WHEN os ILIKE '%ios%' THEN 'iOS'
        ELSE 'Other'
    END
FROM user_events_old
WHERE os IS NOT NULL;

-- 从现有数据中提取并填充国家维度表
INSERT INTO dim_countries (country_code, country_name)
SELECT DISTINCT 
    country_code,
    CASE 
        WHEN country_code = 'US' THEN 'United States'
        WHEN country_code = 'GB' THEN 'United Kingdom'
        WHEN country_code = 'CN' THEN 'China'
        WHEN country_code = 'JP' THEN 'Japan'
        WHEN country_code = 'DE' THEN 'Germany'
        ELSE 'Unknown'
    END
FROM user_events_old
WHERE country_code IS NOT NULL;

-- 从现有数据中提取并填充事件类型和名称维度表
INSERT INTO dim_event_types (event_type_name, category)
SELECT DISTINCT 
    event_type,
    CASE 
        WHEN event_type = 'page_view' THEN 'Navigation'
        WHEN event_type = 'click' THEN 'Interaction'
        WHEN event_type = 'form_submit' THEN 'Conversion'
        WHEN event_type = 'purchase' THEN 'Conversion'
        ELSE 'Other'
    END
FROM user_events_old
WHERE event_type IS NOT NULL;

INSERT INTO dim_event_names (event_name, event_type_id)
SELECT DISTINCT 
    e.event_name,
    et.event_type_id
FROM user_events_old e
JOIN dim_event_types et ON e.event_type = et.event_type_name
WHERE e.event_name IS NOT NULL;
```

### 2. 迁移用户事件数据

```sql
-- 迁移用户事件数据到新结构
INSERT INTO user_events (
    user_id,
    session_id,
    event_name_id,
    page_url,
    referrer,
    device_type_id,
    browser_id,
    os_id,
    country_code,
    event_time,
    properties
)
SELECT 
    e.user_id,
    e.session_id,
    en.event_name_id,
    e.page_url,
    e.referrer,
    dt.device_type_id,
    b.browser_id,
    os.os_id,
    e.country_code,
    e.event_time,
    e.properties
FROM user_events_old e
JOIN dim_event_names en ON e.event_name = en.event_name
JOIN dim_device_types dt ON e.device_type = dt.device_type_name
LEFT JOIN dim_browsers b ON e.browser = b.browser_name
LEFT JOIN dim_operating_systems os ON e.os = os.os_name;
```

### 3. 创建会话摘要数据

```sql
-- 从用户事件中提取会话摘要
INSERT INTO user_sessions (
    session_id,
    user_id,
    start_time,
    end_time,
    duration,
    page_count,
    device_type_id,
    browser_id,
    os_id,
    country_code,
    entry_page,
    exit_page
)
SELECT 
    session_id,
    user_id,
    MIN(event_time) as start_time,
    MAX(event_time) as end_time,
    MAX(event_time) - MIN(event_time) as duration,
    COUNT(*) FILTER (WHERE en.event_name = 'page_view') as page_count,
    MIN(device_type_id) as device_type_id, -- 假设会话中设备类型不变
    MIN(browser_id) as browser_id, -- 假设会话中浏览器不变
    MIN(os_id) as os_id, -- 假设会话中操作系统不变
    MIN(country_code) as country_code, -- 假设会话中国家不变
    FIRST(page_url ORDER BY event_time) as entry_page,
    FIRST(page_url ORDER BY event_time DESC) as exit_page
FROM user_events ue
JOIN dim_event_names en ON ue.event_name_id = en.event_name_id
GROUP BY session_id, user_id;

-- 更新用户属性表中的会话计数
UPDATE user_attributes ua
SET total_sessions = sub.session_count
FROM (
    SELECT user_id, COUNT(DISTINCT session_id) as session_count
    FROM user_events
    GROUP BY user_id
) sub
WHERE ua.user_id = sub.user_id;
```

## 优化后的连续聚合

### 1. 优化漏斗分析连续聚合

```sql
-- 删除旧的连续聚合（如果存在）
DROP MATERIALIZED VIEW IF EXISTS user_session_funnel CASCADE;
DROP MATERIALIZED VIEW IF EXISTS funnel_summary_hourly CASCADE;

-- 创建优化后的漏斗分析连续聚合
CREATE MATERIALIZED VIEW user_session_funnel
WITH (timescaledb.continuous) AS
SELECT
  ue.user_id,
  ue.session_id,
  time_bucket('1 hour', ue.event_time) as bucket,
  -- 使用事件ID而不是名称进行比较，减少存储
  MIN(ue.event_time) FILTER (WHERE en.event_name = 'page_view') as page_view_time,
  MIN(ue.event_time) FILTER (WHERE en.event_name = 'add_to_cart') as add_to_cart_time,
  MIN(ue.event_time) FILTER (WHERE en.event_name = 'checkout_start') as checkout_start_time,
  MIN(ue.event_time) FILTER (WHERE en.event_name = 'purchase') as purchase_time,
  -- 使用布尔值而不是文本，减少存储
  BOOL_OR(en.event_name = 'page_view') as has_page_view,
  BOOL_OR(en.event_name = 'add_to_cart') as has_add_to_cart,
  BOOL_OR(en.event_name = 'checkout_start') as has_checkout_start,
  BOOL_OR(en.event_name = 'purchase') as has_purchase
FROM user_events ue
JOIN dim_event_names en ON ue.event_name_id = en.event_name_id
WHERE en.event_name IN ('page_view', 'add_to_cart', 'checkout_start', 'purchase')
GROUP BY ue.user_id, ue.session_id, bucket;

-- 添加刷新策略
SELECT add_continuous_aggregate_policy('user_session_funnel',
  start_offset => INTERVAL '7 days',
  end_offset => INTERVAL '1 hour',
  schedule_interval => INTERVAL '1 hour');

-- 创建优化后的漏斗汇总连续聚合
CREATE MATERIALIZED VIEW funnel_summary_hourly
WITH (timescaledb.continuous) AS
SELECT
  bucket,
  COUNT(*) as total_sessions,
  COUNT(has_page_view) as step1_page_view,
  COUNT(has_add_to_cart) as step2_add_to_cart,
  COUNT(has_checkout_start) as step3_checkout_start,
  COUNT(has_purchase) as step4_purchase,
  -- 计算按时序完成的转化
  COUNT(CASE WHEN add_to_cart_time > page_view_time THEN 1 END) as step1_to_step2,
  COUNT(CASE WHEN checkout_start_time > add_to_cart_time THEN 1 END) as step2_to_step3,
  COUNT(CASE WHEN purchase_time > checkout_start_time THEN 1 END) as step3_to_step4
FROM user_session_funnel
GROUP BY bucket;

-- 添加刷新策略
SELECT add_continuous_aggregate_policy('funnel_summary_hourly',
  start_offset => INTERVAL '7 days',
  end_offset => INTERVAL '1 hour',
  schedule_interval => INTERVAL '1 hour');
```

### 2. 优化留存分析连续聚合

```sql
-- 删除旧的连续聚合（如果存在）
DROP MATERIALIZED VIEW IF EXISTS user_first_activity CASCADE;
DROP MATERIALIZED VIEW IF EXISTS user_daily_activity CASCADE;
DROP MATERIALIZED VIEW IF EXISTS retention_analysis CASCADE;

-- 创建优化后的留存分析连续聚合
CREATE MATERIALIZED VIEW user_first_activity
WITH (timescaledb.continuous) AS
SELECT
  user_id,
  MIN(time_bucket('1 day', event_time)) as first_activity_date
FROM user_events
GROUP BY user_id;

-- 添加刷新策略
SELECT add_continuous_aggregate_policy('user_first_activity',
  start_offset => INTERVAL '90 days',
  end_offset => INTERVAL '1 day',
  schedule_interval => INTERVAL '1 day');

-- 创建优化后的用户每日活动连续聚合
CREATE MATERIALIZED VIEW user_daily_activity
WITH (timescaledb.continuous) AS
SELECT
  user_id,
  time_bucket('1 day', event_time) as activity_date,
  COUNT(*) as event_count
FROM user_events
GROUP BY user_id, activity_date;

-- 添加刷新策略
SELECT add_continuous_aggregate_policy('user_daily_activity',
  start_offset => INTERVAL '90 days',
  end_offset => INTERVAL '1 day',
  schedule_interval => INTERVAL '1 day');

-- 创建优化后的留存分析专用连续聚合
CREATE MATERIALIZED VIEW retention_analysis
WITH (timescaledb.continuous) AS
SELECT
  ufa.first_activity_date as cohort_date,
  uda.activity_date,
  COUNT(DISTINCT uda.user_id) as active_users,
  COUNT(DISTINCT CASE WHEN uda.activity_date = ufa.first_activity_date THEN uda.user_id END) as cohort_size
FROM user_daily_activity uda
JOIN user_first_activity ufa ON uda.user_id = ufa.user_id
WHERE uda.activity_date >= ufa.first_activity_date
GROUP BY ufa.first_activity_date, uda.activity_date;

-- 添加刷新策略
SELECT add_continuous_aggregate_policy('retention_analysis',
  start_offset => INTERVAL '90 days',
  end_offset => INTERVAL '1 day',
  schedule_interval => INTERVAL '1 day');
```

## Go代码适配

### 1. 更新数据模型

```go
// 更新用户事件模型
type UserEvent struct {
    EventID     string                 `json:"event_id"`
    UserID      int64                  `json:"user_id"`
    SessionID   string                 `json:"session_id"`
    EventNameID int                    `json:"event_name_id"` // 使用ID而不是名称
    PageURL     string                 `json:"page_url"`
    Referrer    string                 `json:"referrer"`
    DeviceTypeID int                   `json:"device_type_id"` // 使用ID而不是名称
    BrowserID   int                    `json:"browser_id"`     // 使用ID而不是名称
    OSID        int                    `json:"os_id"`          // 使用ID而不是名称
    CountryCode string                 `json:"country_code"`
    EventTime   time.Time              `json:"event_time"`
    Properties  map[string]interface{} `json:"properties"`
    CreatedAt   time.Time              `json:"created_at"`
}

// 添加维度查询方法
func (r *EventRepository) GetOrCreateBrowser(browserName string) (int, error) {
    var browserID int
    query := `
        INSERT INTO dim_browsers (browser_name)
        VALUES ($1)
        ON CONFLICT (browser_name) DO UPDATE SET browser_name = EXCLUDED.browser_name
        RETURNING browser_id
    `
    
    err := r.pool.QueryRow(context.Background(), query, browserName).Scan(&browserID)
    if err != nil {
        return 0, err
    }
    
    return browserID, nil
}

// 类似的方法用于操作系统、设备类型等维度
```

### 2. 更新数据插入逻辑

```go
func (r *EventRepository) Insert(event *models.UserEvent) error {
    // 首先获取或创建维度ID
    browserID, err := r.GetOrCreateBrowser(event.Browser)
    if err != nil {
        return err
    }
    
    osID, err := r.GetOrCreateOS(event.OS)
    if err != nil {
        return err
    }
    
    deviceTypeID, err := r.GetOrCreateDeviceType(string(event.DeviceType))
    if err != nil {
        return err
    }
    
    eventNameID, err := r.GetOrCreateEventName(event.EventName, event.EventType)
    if err != nil {
        return err
    }
    
    // 使用维度ID插入事件
    query := `
        INSERT INTO user_events (
            user_id, session_id, event_name_id, page_url, referrer,
            device_type_id, browser_id, os_id, country_code, event_time, properties
        ) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11)
    `
    
    propertiesJSON, err := event.MarshalProperties()
    if err != nil {
        return fmt.Errorf("failed to marshal properties: %v", err)
    }
    
    _, err = r.pool.Exec(context.Background(), query,
        event.UserID,
        event.SessionID,
        eventNameID,
        event.PageURL,
        event.Referrer,
        deviceTypeID,
        browserID,
        osID,
        event.CountryCode,
        event.EventTime,
        propertiesJSON,
    )
    
    return err
}
```

## 性能优化和监控

### 1. 添加数据库监控

```sql
-- 创建监控表
CREATE TABLE database_metrics (
    metric_time TIMESTAMPTZ DEFAULT NOW(),
    metric_name VARCHAR(100),
    metric_value DOUBLE PRECISION,
    PRIMARY KEY (metric_time, metric_name)
);

SELECT create_hypertable('database_metrics', 'metric_time');

-- 创建收集数据库指标的函数
CREATE OR REPLACE FUNCTION collect_database_metrics()
RETURNS void AS $$
BEGIN
    -- 表大小指标
    INSERT INTO database_metrics (metric_name, metric_value)
    SELECT 
        'user_events_table_size',
        pg_total_relation_size('user_events') / 1024.0 / 1024.0
    UNION ALL
    SELECT 
        'user_sessions_table_size',
        pg_total_relation_size('user_sessions') / 1024.0 / 1024.0
    UNION ALL
    SELECT 
        'user_attributes_table_size',
        pg_total_relation_size('user_attributes') / 1024.0 / 1024.0;
    
    -- 行数指标
    INSERT INTO database_metrics (metric_name, metric_value)
    SELECT 'user_events_row_count', COUNT(*) FROM user_events
    UNION ALL
    SELECT 'user_sessions_row_count', COUNT(*) FROM user_sessions
    UNION ALL
    SELECT 'user_attributes_row_count', COUNT(*) FROM user_attributes;
    
    -- 压缩率指标
    INSERT INTO database_metrics (metric_name, metric_value)
    SELECT 
        'user_events_compression_ratio',
        pg_total_relation_size('user_events')::float / 
        (SELECT COUNT(*) FROM user_events WHERE event_time > NOW() - INTERVAL '30 days');
END;
$$ LANGUAGE plpgsql;

-- 创建定期收集指标的作业
SELECT add_job('collect_database_metrics', INTERVAL '1 hour');
```

### 2. 添加数据清理策略

```sql
-- 添加数据保留策略
SELECT add_retention_policy('user_events', INTERVAL '6 months');
SELECT add_retention_policy('user_sessions', INTERVAL '1 year');
SELECT add_retention_policy('database_metrics', INTERVAL '2 years');

-- 添加压缩策略
SELECT add_compression_policy('user_sessions', INTERVAL '30 days');
```

## 总结

通过以上优化，我们实现了以下改进：

1. **规范化设计**：使用维度表减少数据冗余，提高数据一致性
2. **减少存储空间**：使用整数ID代替文本，启用TimescaleDB压缩功能
3. **提高查询性能**：优化索引设计，使用连续聚合预计算常用查询
4. **简化数据模型**：分离用户属性、会话摘要和事件明细数据
5. **增强可维护性**：添加监控和自动化维护任务

这些优化不仅减少了数据库的存储需求，还提高了查询性能，使系统能够更好地处理大规模用户行为数据。同时，规范化设计使数据结构更加清晰，便于后续扩展和维护。
