#

- https://github.com/parquet-go/parquet-go
- https://github.com/hangxie/parquet-tools
- https://github.com/apache/arrow

```go
package main

import (
	"context"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"sort"
	"sync"
	"time"

	"github.com/apache/arrow/go/v12/arrow"
	"github.com/apache/arrow/go/v12/arrow/array"
	"github.com/apache/arrow/go/v12/arrow/memory"
	"github.com/apache/arrow/go/v12/parquet"
	"github.com/apache/arrow/go/v12/parquet/pqarrow"
)

// 系统配置
const (
	MaxFileRecords = 100000    // 单个文件最大记录数
	MergeThreshold = 10        // 触发合并的文件数量阈值
	DataDir        = "./data"  // 数据存储目录
	MergeInterval  = 5 * time.Minute // 合并检查间隔
)

// 流式写入器
type StreamWriter struct {
	mu         sync.Mutex
	currentRec int               // 当前文件记录数
	writer     *pqarrow.FileWriter // Parquet写入器
	schema     *arrow.Schema     // 数据模式
	filePath   string            // 当前文件路径
}

// 初始化流式写入器
func NewStreamWriter(schema *arrow.Schema) *StreamWriter {
	return &StreamWriter{
		schema: schema,
	}
}

// 追加写入记录
func (sw *StreamWriter) Write(rec arrow.Record) error {
	sw.mu.Lock()
	defer sw.mu.Unlock()

	// 创建新文件（首次写入或达到阈值）
	if sw.writer == nil || sw.currentRec >= MaxFileRecords {
		if err := sw.rotateFile(); err != nil {
			return err
		}
	}

	// 写入记录
	if err := sw.writer.Write(rec); err != nil {
		return fmt.Errorf("写入失败: %w", err)
	}

	sw.currentRec += int(rec.NumRows())
	return nil
}

// 切换新文件
func (sw *StreamWriter) rotateFile() error {
	// 关闭现有写入器
	if sw.writer != nil {
		if err := sw.writer.Close(); err != nil {
			return err
		}
	}

	// 生成新文件名
	sw.filePath = filepath.Join(DataDir,
		fmt.Sprintf("data_%d.parquet", time.Now().UnixNano()))

	// 创建新写入器
	f, err := os.Create(sw.filePath)
	if err != nil {
		return fmt.Errorf("创建文件失败: %w", err)
	}

	props := parquet.NewWriterProperties(
		parquet.WithCompression(parquet.CompressionSnappy),
		parquet.WithMaxRowGroupLength(128*1024),
	)

	writer, err := pqarrow.NewFileWriter(
		sw.schema,
		f,
		props,
		pqarrow.DefaultWriterProps(),
	)
	if err != nil {
		f.Close()
		return fmt.Errorf("创建写入器失败: %w", err)
	}

	sw.writer = writer
	sw.currentRec = 0
	return nil
}

// 异步合并器
type Merger struct {
	mu    sync.Mutex
	files []string // 待合并文件列表
}

// 添加待合并文件
func (m *Merger) AddFile(path string) {
	m.mu.Lock()
	defer m.mu.Unlock()
	m.files = append(m.files, path)
}

// 启动合并协程
func (m *Merger) Start(ctx context.Context) {
	go func() {
		ticker := time.NewTicker(MergeInterval)
		defer ticker.Stop()

		for {
			select {
			case <-ticker.C:
				m.tryMerge()
			case <-ctx.Done():
				return
			}
		}
	}()
}

// 尝试合并文件
func (m *Merger) tryMerge() {
	m.mu.Lock()
	defer m.mu.Unlock()

	if len(m.files) < MergeThreshold {
		return
	}

	// 排序文件（按时间顺序）
	sort.Strings(m.files)
	toMerge := m.files[:MergeThreshold]
	m.files = m.files[MergeThreshold:]

	// 执行合并
	mergedFile := filepath.Join(DataDir,
		fmt.Sprintf("merged_%d.parquet", time.Now().UnixNano()))
	
	if err := mergeParquetFiles(toMerge, mergedFile); err != nil {
		log.Printf("合并失败: %v", err)
		return
	}

	// 删除原始文件（生产环境建议先验证合并结果）
	for _, f := range toMerge {
		if err := os.Remove(f); err != nil {
			log.Printf("删除文件失败 %s: %v", f, err)
		}
	}
}

// 合并多个Parquet文件
func mergeParquetFiles(inputs []string, output string) error {
	// 读取第一个文件获取schema
	f, err := os.Open(inputs[0])
	if err != nil {
		return err
	}
	defer f.Close()

	reader, err := pqarrow.NewFileReader(f, pqarrow.ArrowReadProperties{}, memory.DefaultAllocator)
	if err != nil {
		return err
	}
	schema := reader.Schema()

	// 创建输出文件
	outFile, err := os.Create(output)
	if err != nil {
		return err
	}
	defer outFile.Close()

	writer, err := pqarrow.NewFileWriter(
		schema,
		outFile,
		parquet.NewWriterProperties(
			parquet.WithCompression(parquet.CompressionSnappy),
		),
		pqarrow.DefaultWriterProps(),
	)
	if err != nil {
		return err
	}
	defer writer.Close()

	// 合并数据
	for _, path := range inputs {
		tbl, err := readParquetFile(path)
		if err != nil {
			return err
		}
		defer tbl.Release()

		if err := writer.WriteTable(tbl); err != nil {
			return err
		}
	}
	return nil
}

// 生成测试数据
func generateTestData(schema *arrow.Schema) arrow.Record {
	pool := memory.NewGoAllocator()
	b := array.NewRecordBuilder(pool, schema)
	defer b.Release()

	// 填充测试数据
	ts := time.Now().UnixNano() / 1e6
	for i := 0; i < 1000; i++ {
		b.Field(0).(*array.Int64Builder).Append(int64(i))
		b.Field(1).(*array.StringBuilder).Append(fmt.Sprintf("item-%d", i))
		b.Field(2).(*array.Float64Builder).Append(float64(i)*1.5)
		b.Field(3).(*array.TimestampBuilder).Append(arrow.Timestamp(ts + int64(i)))
	}

	return b.NewRecord()
}

func main() {
	// 初始化数据目录
	if err := os.MkdirAll(DataDir, 0755); err != nil {
		log.Fatal("创建目录失败:", err)
	}

	// 定义数据模式
	schema := arrow.NewSchema(
		[]arrow.Field{
			{Name: "id", Type: arrow.PrimitiveTypes.Int64},
			{Name: "name", Type: arrow.BinaryTypes.String},
			{Name: "value", Type: arrow.PrimitiveTypes.Float64},
			{Name: "timestamp", Type: arrow.FixedWidthTypes.Timestamp_ms},
		},
		nil,
	)

	// 初始化组件
	writer := NewStreamWriter(schema)
	merger := &Merger{}
	ctx, cancel := context.WithCancel(context.Background())
	defer cancel()

	// 启动合并协程
	merger.Start(ctx)

	// 模拟实时数据流
	go func() {
		for {
			rec := generateTestData(schema)
			defer rec.Release()

			if err := writer.Write(rec); err != nil {
				log.Printf("写入失败: %v", err)
				continue
			}

			// 记录需要合并的文件
			if writer.filePath != "" {
				merger.AddFile(writer.filePath)
			}

			time.Sleep(100 * time.Millisecond) // 模拟数据产生间隔
		}
	}()

	// 保持主程序运行
	select {}
}

// 读取Parquet文件（辅助函数）
func readParquetFile(path string) (arrow.Table, error) {
	f, err := os.Open(path)
	if err != nil {
		return nil, err
	}
	defer f.Close()

	reader, err := pqarrow.NewFileReader(f, pqarrow.ArrowReadProperties{}, memory.DefaultAllocator)
	if err != nil {
		return nil, err
	}

	return reader.ReadTable(nil)
}
```
