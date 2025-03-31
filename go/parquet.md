#

- https://github.com/parquet-go/parquet-go
- https://github.com/hangxie/parquet-tools
- https://github.com/apache/arrow

```go
package main

import (
	"fmt"
	"log"
	"os"

	"github.com/apache/arrow/go/v12/arrow"
	"github.com/apache/arrow/go/v12/arrow/array"
	"github.com/apache/arrow/go/v12/arrow/memory"
	"github.com/apache/arrow/go/v12/parquet"
	"github.com/apache/arrow/go/v12/parquet/compress"
	"github.com/apache/arrow/go/v12/parquet/pqarrow"
)

func main() {
	// 创建内存池（使用默认Go分配器）
	pool := memory.NewGoAllocator()

	// 定义Schema（包含多种数据类型）
	schema := arrow.NewSchema(
		[]arrow.Field{
			{
				Name: "id", 
				Type: arrow.PrimitiveTypes.Int64,
				Nullable: false,
			},
			{
				Name: "product_name", 
				Type: arrow.BinaryTypes.String,
				Nullable: true,
			},
			{
				Name: "price",
				Type: arrow.PrimitiveTypes.Float64,
				Nullable: false,
			},
			{
				Name: "in_stock",
				Type: arrow.FixedWidthTypes.Boolean,
				Nullable: true,
			},
			{
				Name: "timestamp",
				Type: arrow.FixedWidthTypes.Timestamp_ms,
				Nullable: false,
			},
		},
		nil,
	)

	// 创建列构建器（注意及时释放资源）
	idBuilder := array.NewInt64Builder(pool)
	defer idBuilder.Release()

	nameBuilder := array.NewStringBuilder(pool)
	defer nameBuilder.Release()

	priceBuilder := array.NewFloat64Builder(pool)
	defer priceBuilder.Release()

	stockBuilder := array.NewBooleanBuilder(pool)
	defer stockBuilder.Release()

	tsBuilder := array.NewTimestampBuilder(pool, arrow.FixedWidthTypes.Timestamp_ms)
	defer tsBuilder.Release()

	// 填充测试数据
	idBuilder.AppendValues([]int64{1, 2, 3, 4}, nil)
	
	nameBuilder.AppendValues([]string{"Laptop", "Phone", nil, "Tablet"}, 
		[]bool{true, true, false, true}) // 第三个元素为null
	
	priceBuilder.AppendValues([]float64{1299.99, 699.99, 899.00, 299.99}, nil)
	
	stockBuilder.AppendValues([]bool{true, false, true, true}, 
		[]bool{true, true, true, true}) // 全部非null
	
	tsBuilder.AppendValues([]arrow.Timestamp{
		arrow.Timestamp(1672531200000), // 2023-01-01
		arrow.Timestamp(1672617600000), // 2023-01-02
		arrow.Timestamp(1672704000000), // 2023-01-03
		arrow.Timestamp(1672790400000), // 2023-01-04
	}, nil)

	// 创建Record Batch
	rec := array.NewRecord(
		schema,
		[]arrow.Array{
			idBuilder.NewArray(),
			nameBuilder.NewArray(),
			priceBuilder.NewArray(),
			stockBuilder.NewArray(),
			tsBuilder.NewArray(),
		},
		-1, // 自动计算行数
	)
	defer rec.Release()

	// 创建输出文件
	f, err := os.Create("products.parquet")
	if err != nil {
		log.Fatal("创建文件失败:", err)
	}
	defer f.Close()

	// 配置Parquet写入参数
	props := parquet.NewWriterProperties(
		parquet.WithCompression(compress.Codecs.Snappy), // 使用Snappy压缩
		parquet.WithDictionaryDefault(true),           // 启用字典编码
		parquet.WithDataPageVersion(parquet.DataPageV2), // 使用V2数据页格式
		parquet.WithMaxRowGroupLength(128*1024),       // 128KB行组大小
	)

	// 创建Parquet写入器
	writer, err := pqarrow.NewFileWriter(
		rec.Schema(), 
		f, 
		props,                  // 写入属性
		pqarrow.DefaultWriterProps(), // 默认Arrow属性
	)
	if err != nil {
		log.Fatal("创建Parquet写入器失败:", err)
	}
	defer writer.Close()

	// 写入数据（可多次写入多个Record Batch）
	if err := writer.Write(rec); err != nil {
		log.Fatal("写入数据失败:", err)
	}

	// 显式关闭以确保数据刷新到磁盘
	if err := writer.Close(); err != nil {
		log.Fatal("关闭写入器失败:", err)
	}

	fmt.Println("Parquet文件已成功生成: products.parquet")

	// 可选：验证文件内容
	if err := verifyParquetFile("products.parquet"); err != nil {
		log.Fatal("验证失败:", err)
	}
}

// 验证Parquet文件内容
func verifyParquetFile(path string) error {
	f, err := os.Open(path)
	if err != nil {
		return err
	}
	defer f.Close()

	// 创建Parquet读取器
	reader, err := pqarrow.NewFileReader(
		f, 
		pqarrow.ArrowReadProperties{},
		memory.NewGoAllocator(),
	)
	if err != nil {
		return err
	}

	// 获取文件元数据
	meta := reader.MetaData()
	fmt.Printf("\n文件元数据:\n")
	fmt.Printf("版本: %s\n", meta.Version())
	fmt.Printf("创建者: %s\n", meta.CreatedBy())
	fmt.Printf("行数: %d\n", meta.NumRows())
	fmt.Printf("行组数: %d\n", meta.NumRowGroups())

	// 读取第一个行组
	arrowReadProps := pqarrow.ArrowReadProperties{BatchSize: 1024}
	tbl, err := reader.ReadTable(&arrowReadProps)
	if err != nil {
		return err
	}
	defer tbl.Release()

	fmt.Printf("\nSchema验证:\n%s\n", tbl.Schema())

	fmt.Println("\n前10行数据样例:")
	tr := array.NewTableReader(tbl, 10)
	defer tr.Release()

	for tr.Next() {
		rec := tr.Record()
		fmt.Println(rec)
	}

	return nil
}

// 1. 每次追加写入新文件
func appendData(newRec arrow.Record) error {
    filename := fmt.Sprintf("data_%d.parquet", time.Now().Unix())
    
    writer, err := newParquetWriter(filename, newRec.Schema())
    if err != nil {
        return err
    }
    defer writer.Close()
    
    return writer.Write(newRec)
}

// 2. 查询时合并读取
func readAllData() (arrow.Table, error) {
    dataset, _ := ds.NewDataset(
        []string{"data_*.parquet"},
        nil, // 自动推断Schema
        parquet.NewFileFormat(),
    )
    
    scanner := ds.NewScanner(dataset, ds.WithBatchSize(1024))
    return scanner.ToTable()
}

// 合并多个Parquet文件
func mergeFiles(inputs []string, output string) error {
    // 创建合并后的写入器
    writer := newParquetWriter(output, schema)
    defer writer.Close()
    
    // 逐个读取并合并
    for _, f := range inputs {
        tbl := readParquetFile(f)
        defer tbl.Release()
        
        if err := writer.WriteTable(tbl); err != nil {
            return err
        }
    }
    return nil
}

// 示例：每日合并
func dailyMerge() {
    files := listFiles("data_*.parquet")
    mergeFiles(files, fmt.Sprintf("merged_%s.parquet", time.Now().Format("20060102")))
    deleteFiles(files) // 可选删除原文件
}
```
