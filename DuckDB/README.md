## DuckDB

### 简介
DuckDB是一个嵌入式分析型数据库，专为OLAP工作负载设计，类似于SQLite但针对分析查询优化。

### Golang使用

#### 安装驱动
```bash
go get github.com/marcboeker/go-duckdb
```

#### 基本使用
```go
package main

import (
    "database/sql"
    "fmt"
    "log"

    _ "github.com/marcboeker/go-duckdb"
)

func main() {
    // 打开数据库连接（内存数据库）
    db, err := sql.Open("duckdb", "")
    if err != nil {
        log.Fatal(err)
    }
    defer db.Close()

    // 创建表
    _, err = db.Exec(`
        CREATE TABLE users (
            id INTEGER PRIMARY KEY,
            name VARCHAR(50),
            age INTEGER,
            created_at TIMESTAMP
        )
    `)
    if err != nil {
        log.Fatal(err)
    }

    // 插入数据
    _, err = db.Exec(`
        INSERT INTO users VALUES 
        (1, 'Alice', 30, CURRENT_TIMESTAMP),
        (2, 'Bob', 25, CURRENT_TIMESTAMP),
        (3, 'Charlie', 35, CURRENT_TIMESTAMP)
    `)
    if err != nil {
        log.Fatal(err)
    }

    // 查询数据
    rows, err := db.Query("SELECT id, name, age FROM users WHERE age > ?", 25)
    if err != nil {
        log.Fatal(err)
    }
    defer rows.Close()

    for rows.Next() {
        var id int
        var name string
        var age int
        err = rows.Scan(&id, &name, &age)
        if err != nil {
            log.Fatal(err)
        }
        fmt.Printf("ID: %d, Name: %s, Age: %d\n", id, name, age)
    }
}
```

#### 文件数据库
```go
// 使用文件数据库
db, err := sql.Open("duckdb", "/path/to/database.db")
if err != nil {
    log.Fatal(err)
}

// 或者使用连接字符串
db, err := sql.Open("duckdb", "motherduck:my_db?token=your_token")
```

#### 高级功能
```go
// 读取Parquet文件
_, err = db.Exec(`
    CREATE TABLE sales AS 
    SELECT * FROM read_parquet('sales_*.parquet')
`)

// 读取CSV文件
_, err = db.Exec(`
    CREATE TABLE customers AS 
    SELECT * FROM read_csv_auto('customers.csv')
`)

// 使用窗口函数
rows, err := db.Query(`
    SELECT 
        name,
        age,
        RANK() OVER (ORDER BY age DESC) as age_rank
    FROM users
`)
```
