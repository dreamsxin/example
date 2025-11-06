# 基于 go-resty 的完整 HTTP 压测工具

支持 CSV 参数化

## 完整代码实现

### 1. 主程序文件 `main.go`

```go
package main

import (
	"context"
	"crypto/tls"
	"encoding/csv"
	"encoding/json"
	"flag"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"strconv"
	"strings"
	"sync"
	"sync/atomic"
	"time"

	"github.com/go-resty/resty/v2"
)

// Config 配置结构体
type Config struct {
	URL           string
	Method        string
	TotalRequests int
	Concurrency   int
	Duration      time.Duration
	Headers       map[string]string
	Body          string
	Timeout       time.Duration
	KeepAlive     bool
	CSVFile       string
	OutputFile    string
	Verbose       bool
}

// Result 测试结果结构体
type Result struct {
	TotalRequests      int64
	SuccessfulRequests int64
	FailedRequests     int64
	TotalDuration      time.Duration
	StatusCodes        sync.Map
	MinResponseTime    time.Duration
	MaxResponseTime    time.Duration
	TotalResponseTime  int64
	ErrorCounts        sync.Map
	StartTime          time.Time
	EndTime            time.Time
}

// StressTester 压测器
type StressTester struct {
	config     *Config
	client     *resty.Client
	result     *Result
	csvData    []map[string]string
	csvIndex   int64
	wg         sync.WaitGroup
	ctx        context.Context
	cancel     context.CancelFunc
	outputFile *os.File
}

// NewStressTester 创建压测器
func NewStressTester(cfg *Config) (*StressTester, error) {
	client := resty.New()
	
	// 配置客户端
	client.SetTimeout(cfg.Timeout)
	
	if !cfg.KeepAlive {
		client.SetCloseConnection(true)
	}
	
	// 跳过 TLS 验证
	client.SetTLSClientConfig(&tls.Config{InsecureSkipVerify: true})
	
	// 设置重试
	client.SetRetryCount(0)
	
	// 读取 CSV 数据
	var csvData []map[string]string
	if cfg.CSVFile != "" {
		data, err := readCSVFile(cfg.CSVFile)
		if err != nil {
			return nil, fmt.Errorf("failed to read CSV file: %v", err)
		}
		csvData = data
	}
	
	// 设置 headers
	if len(cfg.Headers) > 0 {
		client.SetHeaders(cfg.Headers)
	}
	
	ctx, cancel := context.WithCancel(context.Background())
	
	var outputFile *os.File
	if cfg.OutputFile != "" {
		file, err := os.Create(cfg.OutputFile)
		if err != nil {
			log.Printf("Warning: failed to create output file: %v", err)
		} else {
			outputFile = file
		}
	}
	
	return &StressTester{
		config:     cfg,
		client:     client,
		result:     &Result{MinResponseTime: time.Hour},
		csvData:    csvData,
		ctx:        ctx,
		cancel:     cancel,
		outputFile: outputFile,
	}, nil
}

// readCSVFile 读取 CSV 文件
func readCSVFile(filename string) ([]map[string]string, error) {
	file, err := os.Open(filename)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	reader := csv.NewReader(file)
	records, err := reader.ReadAll()
	if err != nil {
		return nil, err
	}

	if len(records) < 1 {
		return nil, fmt.Errorf("CSV file is empty")
	}

	headers := records[0]
	var data []map[string]string

	for i := 1; i < len(records); i++ {
		row := make(map[string]string)
		for j, header := range headers {
			if j < len(records[i]) {
				row[strings.TrimSpace(header)] = strings.TrimSpace(records[i][j])
			}
		}
		data = append(data, row)
	}

	return data, nil
}

// processTemplate 处理模板，替换变量
func (st *StressTester) processTemplate(template string, data map[string]string) string {
	result := template
	for key, value := range data {
		placeholder := "{{" + key + "}}"
		result = strings.ReplaceAll(result, placeholder, value)
	}
	return result
}

// getNextCSVRow 获取下一个 CSV 行（循环）
func (st *StressTester) getNextCSVRow() map[string]string {
	if len(st.csvData) == 0 {
		return nil
	}
	index := atomic.AddInt64(&st.csvIndex, 1) - 1
	return st.csvData[int(index)%len(st.csvData)]
}

// makeRequest 发送单个请求
func (st *StressTester) makeRequest() {
	start := time.Now()
	
	// 获取 CSV 数据
	csvRow := st.getNextCSVRow()
	
	// 创建请求
	req := st.client.R()
	
	// 处理动态 headers
	if len(st.config.Headers) > 0 && csvRow != nil {
		for key, value := range st.config.Headers {
			processedValue := st.processTemplate(value, csvRow)
			req.SetHeader(key, processedValue)
		}
	}
	
	// 处理请求体
	var body interface{}
	if st.config.Body != "" {
		processedBody := st.config.Body
		if csvRow != nil {
			processedBody = st.processTemplate(st.config.Body, csvRow)
		}
		
		// 尝试解析为 JSON，如果不是 JSON 则作为字符串处理
		var jsonData map[string]interface{}
		if err := json.Unmarshal([]byte(processedBody), &jsonData); err == nil {
			body = jsonData
		} else {
			body = processedBody
		}
		req.SetBody(body)
	}
	
	// 处理 URL 参数
	url := st.config.URL
	if csvRow != nil {
		url = st.processTemplate(url, csvRow)
	}
	
	var resp *resty.Response
	var err error
	
	switch strings.ToUpper(st.config.Method) {
	case "GET":
		resp, err = req.Get(url)
	case "POST":
		resp, err = req.Post(url)
	case "PUT":
		resp, err = req.Put(url)
	case "DELETE":
		resp, err = req.Delete(url)
	case "PATCH":
		resp, err = req.Patch(url)
	default:
		err = fmt.Errorf("unsupported method: %s", st.config.Method)
	}
	
	duration := time.Since(start)
	
	// 统计结果
	st.recordResult(resp, err, duration, csvRow)
}

// recordResult 记录请求结果
func (st *StressTester) recordResult(resp *resty.Response, err error, duration time.Duration, csvRow map[string]string) {
	atomic.AddInt64(&st.result.TotalRequests, 1)
	atomic.AddInt64(&st.result.TotalResponseTime, int64(duration))
	
	// 输出详细日志
	if st.config.Verbose && st.outputFile != nil {
		st.writeDetailedLog(resp, err, duration, csvRow)
	}
	
	if err != nil {
		atomic.AddInt64(&st.result.FailedRequests, 1)
		errorMsg := err.Error()
		if len(errorMsg) > 100 {
			errorMsg = errorMsg[:100] + "..."
		}
		
		var count int64
		if val, ok := st.result.ErrorCounts.Load(errorMsg); ok {
			count = val.(int64)
		}
		st.result.ErrorCounts.Store(errorMsg, count+1)
		return
	}
	
	atomic.AddInt64(&st.result.SuccessfulRequests, 1)
	
	// 记录状态码
	statusCode := resp.StatusCode()
	var codeCount int64
	if val, ok := st.result.StatusCodes.Load(statusCode); ok {
		codeCount = val.(int64)
	}
	st.result.StatusCodes.Store(statusCode, codeCount+1)
	
	// 更新最小响应时间
	for {
		oldMin := atomic.LoadInt64((*int64)(&st.result.MinResponseTime))
		if duration < time.Duration(oldMin) || oldMin == 0 {
			if atomic.CompareAndSwapInt64((*int64)(&st.result.MinResponseTime), oldMin, int64(duration)) {
				break
			}
		} else {
			break
		}
	}
	
	// 更新最大响应时间
	for {
		oldMax := atomic.LoadInt64((*int64)(&st.result.MaxResponseTime))
		if duration > time.Duration(oldMax) {
			if atomic.CompareAndSwapInt64((*int64)(&st.result.MaxResponseTime), oldMax, int64(duration)) {
				break
			}
		} else {
			break
		}
	}
}

// writeDetailedLog 写入详细日志
func (st *StressTester) writeDetailedLog(resp *resty.Response, err error, duration time.Duration, csvRow map[string]string) {
	logEntry := map[string]interface{}{
		"timestamp": time.Now().Format(time.RFC3339),
		"duration_ms": duration.Milliseconds(),
		"success": err == nil,
	}
	
	if csvRow != nil {
		logEntry["csv_data"] = csvRow
	}
	
	if err != nil {
		logEntry["error"] = err.Error()
	} else {
		logEntry["status_code"] = resp.StatusCode()
		logEntry["response_size"] = len(resp.Body())
	}
	
	jsonData, _ := json.Marshal(logEntry)
	st.outputFile.WriteString(string(jsonData) + "\n")
}

// worker 工作协程
func (st *StressTester) worker(requests <-chan struct{}) {
	defer st.wg.Done()
	
	for {
		select {
		case <-st.ctx.Done():
			return
		case _, ok := <-requests:
			if !ok {
				return
			}
			st.makeRequest()
		}
	}
}

// Run 运行压测
func (st *StressTester) Run() *Result {
	st.result.StartTime = time.Now()
	
	requests := make(chan struct{}, st.config.Concurrency)
	
	// 启动工作协程
	for i := 0; i < st.config.Concurrency; i++ {
		st.wg.Add(1)
		go st.worker(requests)
	}
	
	// 发送请求
	if st.config.Duration > 0 {
		// 基于时间的压测
		go func() {
			timer := time.NewTimer(st.config.Duration)
			defer timer.Stop()
			
			for {
				select {
				case <-timer.C:
					close(requests)
					st.cancel()
					return
				default:
					select {
					case requests <- struct{}{}:
					case <-st.ctx.Done():
						return
					}
				}
			}
		}()
	} else {
		// 基于请求数量的压测
		go func() {
			for i := 0; i < st.config.TotalRequests; i++ {
				select {
				case requests <- struct{}{}:
				case <-st.ctx.Done():
					break
				}
			}
			close(requests)
		}()
	}
	
	st.wg.Wait()
	st.result.EndTime = time.Now()
	st.result.TotalDuration = st.result.EndTime.Sub(st.result.StartTime)
	
	if st.outputFile != nil {
		st.outputFile.Close()
	}
	
	return st.result
}

// PrintReport 打印测试报告
func (st *StressTester) PrintReport() {
	result := st.result
	
	fmt.Println("\n" + strings.Repeat("=", 60))
	fmt.Println("HTTP STRESS TEST REPORT")
	fmt.Println(strings.Repeat("=", 60))
	
	fmt.Printf("Target URL:          %s\n", st.config.URL)
	fmt.Printf("HTTP Method:         %s\n", st.config.Method)
	fmt.Printf("Concurrency:         %d\n", st.config.Concurrency)
	
	if st.config.Duration > 0 {
		fmt.Printf("Test Duration:       %v\n", st.config.Duration)
	} else {
		fmt.Printf("Total Requests:      %d\n", st.config.TotalRequests)
	}
	
	if len(st.csvData) > 0 {
		fmt.Printf("CSV Data Rows:       %d\n", len(st.csvData))
	}
	
	fmt.Printf("Actual Duration:     %v\n", result.TotalDuration)
	fmt.Printf("Total Requests:      %d\n", result.TotalRequests)
	fmt.Printf("Successful:          %d\n", result.SuccessfulRequests)
	fmt.Printf("Failed:              %d\n", result.FailedRequests)
	fmt.Printf("Success Rate:        %.2f%%\n", 
		float64(result.SuccessfulRequests)/float64(result.TotalRequests)*100)
	
	if result.TotalRequests > 0 {
		rps := float64(result.TotalRequests) / result.TotalDuration.Seconds()
		fmt.Printf("Requests/sec:        %.2f\n", rps)
		
		avgResponseTime := time.Duration(result.TotalResponseTime / int64(result.TotalRequests))
		fmt.Printf("Avg Response Time:   %v\n", avgResponseTime)
		fmt.Printf("Min Response Time:   %v\n", result.MinResponseTime)
		fmt.Printf("Max Response Time:   %v\n", result.MaxResponseTime)
	}
	
	// 状态码分布
	fmt.Println("\nStatus Code Distribution:")
	st.result.StatusCodes.Range(func(key, value interface{}) bool {
		code := key.(int)
		count := value.(int64)
		percentage := float64(count) / float64(result.TotalRequests) * 100
		fmt.Printf("  %d: %d (%.2f%%)\n", code, count, percentage)
		return true
	})
	
	// 错误分布
	var totalErrors int64
	st.result.ErrorCounts.Range(func(_, value interface{}) bool {
		totalErrors += value.(int64)
		return true
	})
	
	if totalErrors > 0 {
		fmt.Printf("\nError Distribution (Total: %d):\n", totalErrors)
		st.result.ErrorCounts.Range(func(key, value interface{}) bool {
			errorMsg := key.(string)
			count := value.(int64)
			percentage := float64(count) / float64(totalErrors) * 100
			fmt.Printf("  %s: %d (%.2f%%)\n", errorMsg, count, percentage)
			return true
		})
	}
	
	fmt.Println(strings.Repeat("=", 60))
}

func main() {
	var config Config
	
	flag.StringVar(&config.URL, "url", "", "Target URL (required)")
	flag.StringVar(&config.Method, "method", "GET", "HTTP method")
	flag.IntVar(&config.TotalRequests, "n", 1000, "Total number of requests")
	flag.IntVar(&config.Concurrency, "c", 10, "Number of concurrent workers")
	flag.DurationVar(&config.Duration, "d", 0, "Test duration (e.g., 30s, 5m)")
	flag.StringVar(&config.Body, "body", "", "Request body")
	flag.StringVar(&config.CSVFile, "csv", "", "CSV file for parameterization")
	flag.StringVar(&config.OutputFile, "output", "", "Output file for detailed logs")
	flag.DurationVar(&config.Timeout, "timeout", 30*time.Second, "Request timeout")
	flag.BoolVar(&config.KeepAlive, "keep-alive", true, "Enable keep-alive connections")
	flag.BoolVar(&config.Verbose, "verbose", false, "Enable verbose logging")
	
	var headers string
	flag.StringVar(&headers, "headers", "", "Request headers (JSON format)")
	
	flag.Parse()
	
	// 验证必需参数
	if config.URL == "" {
		log.Fatal("Error: URL is required")
	}
	
	// 解析 headers
	config.Headers = make(map[string]string)
	if headers != "" {
		if err := json.Unmarshal([]byte(headers), &config.Headers); err != nil {
			log.Fatalf("Error parsing headers: %v", err)
		}
	}
	
	// 创建压测器
	tester, err := NewStressTester(&config)
	if err != nil {
		log.Fatalf("Error creating stress tester: %v", err)
	}
	
	fmt.Printf("Starting stress test...\n")
	fmt.Printf("URL:          %s\n", config.URL)
	fmt.Printf("Method:       %s\n", config.Method)
	fmt.Printf("Concurrency:  %d\n", config.Concurrency)
	
	if config.Duration > 0 {
		fmt.Printf("Duration:     %v\n", config.Duration)
	} else {
		fmt.Printf("Total:        %d\n", config.TotalRequests)
	}
	
	if config.CSVFile != "" {
		fmt.Printf("CSV File:     %s\n", config.CSVFile)
	}
	
	// 运行压测
	result := tester.Run()
	
	// 打印报告
	tester.PrintReport()
	
	// 检查是否有大量错误
	if result.FailedRequests > 0 && float64(result.FailedRequests)/float64(result.TotalRequests) > 0.1 {
		fmt.Printf("\n⚠️  Warning: High failure rate (%.1f%%) detected!\n", 
			float64(result.FailedRequests)/float64(result.TotalRequests)*100)
		os.Exit(1)
	}
}
```

### 2. Go Module 文件 `go.mod`

```go
module stress-tester

go 1.19

require (
	github.com/go-resty/resty/v2 v2.7.0
)

require golang.org/x/net v0.0.0-20211029224645-99673261e6eb // indirect
```

### 3. CSV 示例文件 `users.csv`

```csv
id,username,email,role,token
1,john_doe,john@example.com,user,token_123
2,jane_smith,jane@example.com,admin,token_456
3,bob_wilson,bob@example.com,user,token_789
4,alice_brown,alice@example.com,moderator,token_101
```

### 4. 使用说明

#### 编译和安装
```bash
go mod tidy
go build -o stress-tester main.go
```

#### 基本使用
```bash
# 基础压测
./stress-tester -url https://api.example.com/users -n 10000 -c 100

# 带 JSON Body 的 POST 请求
./stress-tester -url https://api.example.com/users -method POST -n 5000 -c 50 \
  -body '{"name":"test","email":"test@example.com"}'

# 使用 CSV 参数化
./stress-tester -url https://api.example.com/users -method POST -n 10000 -c 100 \
  -csv users.csv \
  -body '{"id":{{id}},"username":"{{username}}","email":"{{email}}","role":"{{role}}"}' \
  -headers '{"Content-Type":"application/json","Authorization":"Bearer {{token}}"}'

# 基于时间的测试
./stress-tester -url https://api.example.com/users -c 100 -d 1m

# 输出详细日志
./stress-tester -url https://api.example.com/users -n 1000 -c 10 -output results.json -verbose
```

#### 高级用法示例

**带动态参数的复杂请求：**
```bash
./stress-tester \
  -url "https://api.example.com/users/{{id}}/profile" \
  -method PUT \
  -csv users.csv \
  -body '{
    "user": {
      "id": {{id}},
      "username": "{{username}}",
      "email": "{{email}}",
      "metadata": {
        "role": "{{role}}",
        "timestamp": "2024-01-01"
      }
    }
  }' \
  -headers '{
    "Content-Type": "application/json",
    "Authorization": "Bearer {{token}}",
    "X-User-ID": "{{id}}",
    "X-Request-ID": "req-{{id}}-{{username}}"
  }' \
  -n 5000 \
  -c 50 \
  -output detailed_logs.json \
  -verbose
```

### 5. 特性说明

1. **CSV 参数化**：支持从 CSV 文件读取数据，在 URL、Headers 和 Body 中使用 `{{column_name}}` 模板
2. **多种测试模式**：支持基于请求数量或基于时间的测试
3. **详细统计**：提供完整的性能指标和错误分析
4. **JSON 输出**：支持将详细请求日志输出到 JSON 文件
5. **灵活配置**：支持自定义 Headers、Timeout、Keep-Alive 等
6. **错误处理**：自动检测高错误率并返回非零退出码
