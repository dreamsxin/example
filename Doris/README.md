
## Doris

### 简介
Apache Doris是一个高性能、实时的MPP分析型数据库，适合在线分析处理(OLAP)场景。

### Golang使用

#### 安装驱动
```bash
go get github.com/go-sql-driver/mysql
```

#### 基本使用
```go
package main

import (
    "database/sql"
    "fmt"
    "log"
    "time"

    _ "github.com/go-sql-driver/mysql"
)

type Config struct {
    Host     string
    Port     int
    User     string
    Password string
    Database string
}

func main() {
    config := Config{
        Host:     "localhost",
        Port:     9030,
        User:     "root",
        Password: "",
        Database: "test_db",
    }

    dsn := fmt.Sprintf("%s:%s@tcp(%s:%d)/%s", 
        config.User, config.Password, config.Host, config.Port, config.Database)

    db, err := sql.Open("mysql", dsn)
    if err != nil {
        log.Fatal("连接失败:", err)
    }
    defer db.Close()

    // 设置连接池参数
    db.SetMaxOpenConns(20)
    db.SetMaxIdleConns(10)
    db.SetConnMaxLifetime(time.Hour)

    // 测试连接
    err = db.Ping()
    if err != nil {
        log.Fatal("ping失败:", err)
    }

    // 创建表
    err = createTable(db)
    if err != nil {
        log.Fatal(err)
    }

    // 插入数据
    err = insertData(db)
    if err != nil {
        log.Fatal(err)
    }

    // 查询数据
    err = queryData(db)
    if err != nil {
        log.Fatal(err)
    }
}

func createTable(db *sql.DB) error {
    _, err := db.Exec(`
        CREATE TABLE IF NOT EXISTS user_behavior (
            user_id INT,
            item_id INT,
            category_id INT,
            behavior_type VARCHAR(10),
            ts DATETIME
        )
        DISTRIBUTED BY HASH(user_id) BUCKETS 10
        PROPERTIES ("replication_num" = "1")
    `)
    return err
}

func insertData(db *sql.DB) error {
    // 批量插入数据
    stmt, err := db.Prepare(`
        INSERT INTO user_behavior 
        (user_id, item_id, category_id, behavior_type, ts) 
        VALUES (?, ?, ?, ?, ?)
    `)
    if err != nil {
        return err
    }
    defer stmt.Close()

    // 模拟数据
    behaviors := []struct {
        userID     int
        itemID     int
        categoryID int
        behavior   string
        timestamp  time.Time
    }{
        {1001, 2001, 1, "pv", time.Now()},
        {1001, 2002, 1, "buy", time.Now()},
        {1002, 2001, 1, "pv", time.Now()},
        {1002, 2003, 2, "cart", time.Now()},
    }

    for _, b := range behaviors {
        _, err = stmt.Exec(b.userID, b.itemID, b.categoryID, b.behavior, b.timestamp)
        if err != nil {
            return err
        }
    }
    return nil
}

func queryData(db *sql.DB) error {
    // 复杂查询示例
    query := `
        SELECT 
            user_id,
            COUNT(*) as total_actions,
            SUM(CASE WHEN behavior_type = 'buy' THEN 1 ELSE 0 END) as buy_count,
            COUNT(DISTINCT item_id) as unique_items
        FROM user_behavior 
        WHERE ts >= DATE_SUB(NOW(), INTERVAL 1 DAY)
        GROUP BY user_id
        HAVING buy_count > 0
        ORDER BY total_actions DESC
    `

    rows, err := db.Query(query)
    if err != nil {
        return err
    }
    defer rows.Close()

    fmt.Println("用户行为统计:")
    for rows.Next() {
        var userID int
        var totalActions int
        var buyCount int
        var uniqueItems int

        err = rows.Scan(&userID, &totalActions, &buyCount, &uniqueItems)
        if err != nil {
            return err
        }

        fmt.Printf("用户ID: %d, 总行为: %d, 购买次数: %d, 浏览商品数: %d\n", 
            userID, totalActions, buyCount, uniqueItems)
    }

    return rows.Err()
}
```

#### 批量插入优化
```go
func batchInsert(db *sql.DB, data [][]interface{}) error {
    // 开始事务
    tx, err := db.Begin()
    if err != nil {
        return err
    }

    stmt, err := tx.Prepare(`
        INSERT INTO user_behavior 
        (user_id, item_id, category_id, behavior_type, ts) 
        VALUES (?, ?, ?, ?, ?)
    `)
    if err != nil {
        tx.Rollback()
        return err
    }
    defer stmt.Close()

    for _, row := range data {
        _, err = stmt.Exec(row...)
        if err != nil {
            tx.Rollback()
            return err
        }
    }

    return tx.Commit()
}
```

#### 监控和性能分析
```go
func showQueryStats(db *sql.DB) {
    // 获取数据库状态
    rows, err := db.Query("SHOW PROC '/dbs'")
    if err != nil {
        log.Println("获取数据库状态失败:", err)
        return
    }
    defer rows.Close()

    // 处理状态信息
    // ...
}
```

## 选择建议

### 使用DuckDB的场景：
- 嵌入式分析应用
- 数据科学和探索性分析
- 小型到中型数据集
- 需要快速原型开发
- 本地文件分析（Parquet、CSV等）

### 使用Doris的场景：
- 大规模数据分析
- 企业级OLAP应用
- 高并发查询需求
- 实时数据分析
- 需要分布式架构
