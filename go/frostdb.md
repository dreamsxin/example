# frostdb

- https://github.com/polarsignals/frostdb
- https://github.com/parca-dev/parca

嵌入式列存储解决方案

## 特点

不可变数据集：FrostDB采用不可变数据集的设计，一旦数据被写入，就不能被修改，这有助于保证数据的稳定性和一致性。
动态列扩展：FrostDB支持动态列扩展，这意味着在运行时可以根据需要添加新的列，使得系统更加灵活。
基于Apache Parquet的存储：在底层存储方面，FrostDB采用了Apache Parquet格式，这是一种高效的列式存储格式，被广泛用于大数据领域。
基于Apache Arrow的查询：在查询方面，FrostDB利用了Apache Arrow的高效内存布局，使得数据在内存中的处理速度大大加快。

## 列式存储的优势

列式存储与常见的行式存储相比，具有显著的优势。首先，列式存储更适合于数据分析场景，因为它能够针对某一列进行高效的压缩和编码，从而大幅度减少存储空间。其次，列式存储在进行聚合查询时，只需扫描涉及的相关列，而不是整行数据，从而显著提高了查询效率。

## 使用

```shell
go get github.com/polarsignals/frostdb
```

### 创建库

```go
package main

import (
	"context"
	"fmt"

	"github.com/apache/arrow/go/v15/arrow"
	"github.com/apache/arrow/go/v15/arrow/memory"

	"github.com/dreamsxin/frostdb"
	"github.com/dreamsxin/frostdb/query"
	"github.com/dreamsxin/frostdb/query/logicalplan"
)

func main() {
	// Create a new column store
	columnstore, _ := frostdb.New()
	defer columnstore.Close()

	// Open up a database in the column store
	database, _ := columnstore.DB(context.Background(), "simple_db")

	type Simple struct {
		Names map[string]string `frostdb:",asc"`
		Value int64
	}
	table, _ := frostdb.NewGenericTable[Simple](
		database, "simple_table", memory.DefaultAllocator,
	)
	// Create values to insert into the database these first rows havel dynamic label names of 'firstname' and 'surname'
	frederic := Simple{
		Names: map[string]string{
			"first_name": "Frederic",
			"surname":    "Brancz",
		},
		Value: 100,
	}

	thor := Simple{
		Names: map[string]string{
			"first_name": "Thor",
			"surname":    "Hansen",
		},
		Value: 99,
	}
	table.Write(context.Background(), frederic, thor)

	// Now we can insert rows that have middle names into our dynamic column
	matthias := Simple{
		Names: map[string]string{
			"first_name":  "Matthias",
			"middle_name": "Oliver Rainer",
			"surname":     "Loibl",
		},
		Value: 101,
	}
	table.Write(context.Background(), matthias)

	// Create a new query engine to retrieve data and print the results
	engine := query.NewEngine(memory.DefaultAllocator, database.TableProvider())
	_ = engine.ScanTable("simple_table").
		Project(logicalplan.DynCol("names")). // We don't know all dynamic columns at query time, but we want all of them to be returned.
		Filter(
			logicalplan.Col("names.first_name").Eq(logicalplan.Literal("Frederic")),
		).Execute(context.Background(), func(ctx context.Context, r arrow.Record) error {
		fmt.Println(r)
		return nil
	})
}
```

### 持久化

```go
package main

import (
	"context"
	stdlog "log"
	"os"

	kitlog "github.com/go-kit/log"

	"github.com/apache/arrow/go/v15/arrow"
	"github.com/apache/arrow/go/v15/arrow/memory"
	"github.com/thanos-io/objstore"

	"github.com/polarsignals/frostdb"
	"github.com/polarsignals/frostdb/dynparquet"
	"github.com/polarsignals/frostdb/query/logicalplan"
)

func main() {
	logger := kitlog.NewLogfmtLogger(kitlog.NewSyncWriter(os.Stdout))                              //NewJSONLogger
	logger = kitlog.With(logger, "ts", kitlog.DefaultTimestampUTC, "caller", kitlog.DefaultCaller) //kitlog.Caller
	stdlog.SetOutput(kitlog.NewStdlibAdapter(logger))
	stdlog.Print("I sure like pie")

	// dir, err := os.MkdirTemp("", "frostdb*")
	dir := "frostdb"
	_, err := os.Stat(dir)
	if err != nil {
		err := os.Mkdir(dir, 0755)
		if err != nil {
			stdlog.Panicln("MkdirTemp", err)
		}
	}
	bucket := objstore.NewInMemBucket()

	sinksource := frostdb.NewDefaultObjstoreBucket(bucket)

	// 创建 ColumnStore 对象
	c, err := frostdb.New(
		frostdb.WithLogger(logger),
		frostdb.WithWAL(),
		frostdb.WithStoragePath(dir),
		frostdb.WithReadWriteStorage(sinksource),
		frostdb.WithActiveMemorySize(100*frostdb.KiB),
	)
	if err != nil {
		stdlog.Panicln("frostdb.New", err)
	}

	// 获取或创建数据库
	db, err := c.DB(context.Background(), "test")
	if err != nil {
		stdlog.Panicln("frostdb.DB", err)
	}
	// 获取或创建表
	testTable, err := db.Table("test", frostdb.NewTableConfig(
		dynparquet.SampleDefinition(), //  schemapb.Schema 使用示例里的表定义
	))
	if err != nil {
		stdlog.Panicln("frostdb.Table", err)
	}

	// 示例数据集
	samples := dynparquet.NewTestSamples()

	ctx := context.Background()
	for i := 0; i < 100; i++ {
		r, err := samples.ToRecord()
		if err != nil {
			stdlog.Panicln("samples.ToRecord", err)
		}
		_, err = testTable.InsertRecord(ctx, r)
		if err != nil {
			stdlog.Panicln("testTable.InsertRecord", err)
		}
	}
	// 强制压缩表
	err = testTable.EnsureCompaction()
	if err != nil {
		stdlog.Panicln("testTable.EnsureCompaction", err)
	}
	// 关闭 ColumnStore
	err = c.Close()
	if err != nil {
		stdlog.Panicln("frostdb.Close", err)
	}

	// 重新打开 ColumnStore
	c, err = frostdb.New(
		frostdb.WithLogger(logger),
		frostdb.WithWAL(),
		frostdb.WithStoragePath(dir),
		frostdb.WithReadWriteStorage(sinksource),
		frostdb.WithActiveMemorySize(100*frostdb.KiB),
	)
	if err != nil {
		stdlog.Panicln("frostdb.New", err)
	}
	defer c.Close()

	db, err = c.DB(context.Background(), "test")
	if err != nil {
		stdlog.Panicln("frostdb.DB", err)
	}
	testTable, err = db.Table("test", frostdb.NewTableConfig(
		dynparquet.SampleDefinition(), //  schemapb.Schema 使用示例里的表定义
	))
	if err != nil {
		stdlog.Panicln("frostdb.Table", err)
	}

	// 验证数据是否保存成功
	pool := memory.NewCheckedAllocator(memory.DefaultAllocator)

	rows := int64(0)
	err = testTable.View(ctx, func(ctx context.Context, tx uint64) error {
		return testTable.Iterator(
			ctx,
			tx,
			pool, // 允许为 nil
			[]logicalplan.Callback{func(ctx context.Context, ar arrow.Record) error {
				rows += ar.NumRows()
				return nil
			}},
		)
	})
	if err != nil {
		stdlog.Panicln("testTable.View", err)
	}
	stdlog.Println("pool.CurrentAlloc()", pool.CurrentAlloc(), "rows", rows)
}
```
