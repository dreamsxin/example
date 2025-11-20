```go
package main

import (
	"bufio"
	"crypto/tls"
	"flag"
	"fmt"
	"io"
	"log"
	"net/http"
	"net/url"
	"os"
	"sort"
	"strings"
	"sync"
	"time"
)

// 测试结果结构体
type TestResult struct {
	Success    bool
	Duration   time.Duration
	StatusCode int
	Error      error
	BytesRead  int64
}

// 统计信息结构体
type Stats struct {
	TotalTests    int
	SuccessCount  int
	FailureCount  int
	TotalDuration time.Duration
	MinDuration   time.Duration
	MaxDuration   time.Duration
	AvgDuration   time.Duration
	TotalBytes    int64
	StatusCodes   map[int]int
	Errors        map[string]int
}

// 配置结构体
type Config struct {
	ProxyURL     string
	TargetURL    string
	Concurrency  int
	Requests     int
	Timeout      time.Duration
	TestDuration time.Duration
	OutputFile   string
	Verbose      bool
}

func main() {
	config := parseFlags()

	fmt.Printf("开始代理稳定性测试\n")
	fmt.Printf("代理服务器: %s\n", config.ProxyURL)
	fmt.Printf("目标URL: %s\n", config.TargetURL)
	fmt.Printf("并发数: %d\n", config.Concurrency)
	fmt.Printf("超时时间: %v\n", config.Timeout)
	fmt.Printf("测试时长: %v\n", config.TestDuration)
	fmt.Println("===================================================")

	// 验证代理连接
	if !validateProxy(config) {
		log.Fatal("代理服务器验证失败")
	}

	// 运行测试
	results := runTests(config)

	// 生成报告
	stats := calculateStats(results)
	generateReport(stats, config)
}

func parseFlags() *Config {
	config := &Config{}

	flag.StringVar(&config.ProxyURL, "proxy", "", "代理服务器地址 (例如: http://proxy.example.com:8080)")
	flag.StringVar(&config.TargetURL, "target", "http://httpbin.org/get", "要测试的目标URL")
	flag.IntVar(&config.Concurrency, "concurrency", 5, "并发连接数")
	flag.IntVar(&config.Requests, "requests", 100, "总请求数 (默认100，0表示基于时长测试)")
	flag.DurationVar(&config.Timeout, "timeout", 10*time.Second, "请求超时时间")
	flag.DurationVar(&config.TestDuration, "duration", 0, "测试时长 (例如: 5m)")
	flag.StringVar(&config.OutputFile, "output", "", "输出结果到文件")
	flag.BoolVar(&config.Verbose, "verbose", false, "详细输出模式")

	flag.Parse()

	if config.ProxyURL == "" {
		log.Fatal("必须指定代理服务器地址")
	}

	return config
}

func validateProxy(config *Config) bool {
	fmt.Println("验证代理服务器连接...")

	proxyURL, err := url.Parse(config.ProxyURL)
	if err != nil {
		log.Printf("代理URL解析失败: %v", err)
		return false
	}

	transport := &http.Transport{
		Proxy: http.ProxyURL(proxyURL),
		TLSClientConfig: &tls.Config{
			InsecureSkipVerify: true, // 仅用于测试
		},
	}

	client := &http.Client{
		Transport: transport,
		Timeout:   config.Timeout,
	}

	resp, err := client.Get(config.TargetURL)
	if err != nil {
		log.Printf("代理连接测试失败: %v", err)
		return false
	}
	defer resp.Body.Close()

	fmt.Printf("代理验证成功 - 状态码: %d\n", resp.StatusCode)
	return true
}

func runTests(config *Config) []TestResult {
	var results []TestResult
	var mu sync.Mutex
	var wg sync.WaitGroup

	resultsChan := make(chan TestResult, 1000)
	stopChan := make(chan struct{})

	// 启动结果收集器
	go func() {
		for result := range resultsChan {
			mu.Lock()
			results = append(results, result)
			mu.Unlock()

			if config.Verbose {
				if result.Success {
					fmt.Printf("请求成功 - 状态码: %d, 耗时: %v, 数据量: %d bytes\n",
						result.StatusCode, result.Duration, result.BytesRead)
				} else {
					fmt.Printf("请求失败 - 错误: %v\n", result.Error)
				}
			}
		}
	}()

	// 设置测试时长
	if config.TestDuration > 0 {
		go func() {
			time.Sleep(config.TestDuration)
			close(stopChan)
		}()
	}

	// 创建worker池
	semaphore := make(chan struct{}, config.Concurrency)

	testFunc := func() {
		defer wg.Done()

		for {
			select {
			case <-stopChan:
				return
			case semaphore <- struct{}{}:
				// 获取到信号量，执行测试
				result := performRequest(config)
				resultsChan <- result
				<-semaphore

				// 如果设置了总请求数且已完成，则停止
				if config.Requests > 0 && len(results) >= config.Requests {
					return
				}
			default:
				return
			}
		}
	}

	// 启动worker
	workerCount := config.Concurrency
	if config.Requests > 0 && config.Requests < config.Concurrency {
		workerCount = config.Requests
	}

	wg.Add(workerCount)
	for i := 0; i < workerCount; i++ {
		go testFunc()
	}

	// 等待所有worker完成或超时
	if config.TestDuration > 0 {
		<-time.After(config.TestDuration + time.Second)
	} else if config.Requests > 0 {
		for len(results) < config.Requests {
			time.Sleep(100 * time.Millisecond)
		}
	}

	close(stopChan)
	wg.Wait()
	close(resultsChan)

	return results
}

func performRequest(config *Config) TestResult {
	start := time.Now()

	proxyURL, _ := url.Parse(config.ProxyURL)
	transport := &http.Transport{
		Proxy: http.ProxyURL(proxyURL),
		TLSClientConfig: &tls.Config{
			InsecureSkipVerify: true,
		},
	}

	client := &http.Client{
		Transport: transport,
		Timeout:   config.Timeout,
	}

	resp, err := client.Get(config.TargetURL)
	duration := time.Since(start)

	if err != nil {
		return TestResult{
			Success:  false,
			Duration: duration,
			Error:    err,
		}
	}
	defer resp.Body.Close()

	// 读取响应体来计算数据量
	body, err := io.ReadAll(resp.Body)
	bytesRead := int64(len(body))

	return TestResult{
		Success:    true,
		Duration:   duration,
		StatusCode: resp.StatusCode,
		BytesRead:  bytesRead,
	}
}

func calculateStats(results []TestResult) *Stats {
	stats := &Stats{
		TotalTests:  len(results),
		StatusCodes: make(map[int]int),
		Errors:      make(map[string]int),
		MinDuration: time.Hour, // 初始化为一个很大的值
	}

	if len(results) == 0 {
		return stats
	}

	// 计算持续时间
	durations := make([]time.Duration, 0, len(results))

	for _, result := range results {
		if result.Success {
			stats.SuccessCount++
			stats.TotalDuration += result.Duration
			stats.TotalBytes += result.BytesRead
			stats.StatusCodes[result.StatusCode]++

			durations = append(durations, result.Duration)

			if result.Duration < stats.MinDuration {
				stats.MinDuration = result.Duration
			}
			if result.Duration > stats.MaxDuration {
				stats.MaxDuration = result.Duration
			}
		} else {
			stats.FailureCount++
			if result.Error != nil {
				stats.Errors[result.Error.Error()]++
			}
		}
	}

	// 计算平均时间
	if stats.SuccessCount > 0 {
		stats.AvgDuration = stats.TotalDuration / time.Duration(stats.SuccessCount)
	}

	// 计算百分位数
	if len(durations) > 0 {
		sort.Slice(durations, func(i, j int) bool {
			return durations[i] < durations[j]
		})

		p50 := durations[len(durations)*50/100]
		p95 := durations[len(durations)*95/100]
		p99 := durations[len(durations)*99/100]

		fmt.Printf("\n响应时间百分位数:\n")
		fmt.Printf("  P50: %v\n", p50)
		fmt.Printf("  P95: %v\n", p95)
		fmt.Printf("  P99: %v\n", p99)
	}

	return stats
}

func generateReport(stats *Stats, config *Config) {
	fmt.Printf("\n" + strings.Repeat("=", 50) + "\n")
	fmt.Printf("代理稳定性测试报告\n")
	fmt.Printf(strings.Repeat("=", 50) + "\n")

	fmt.Printf("测试概览:\n")
	fmt.Printf("  总请求数: %d\n", stats.TotalTests)
	fmt.Printf("  成功请求: %d\n", stats.SuccessCount)
	fmt.Printf("  失败请求: %d\n", stats.FailureCount)
	fmt.Printf("  成功率: %.2f%%\n", float64(stats.SuccessCount)/float64(stats.TotalTests)*100)

	if stats.SuccessCount > 0 {
		fmt.Printf("\n响应时间统计:\n")
		fmt.Printf("  平均响应时间: %v\n", stats.AvgDuration)
		fmt.Printf("  最小响应时间: %v\n", stats.MinDuration)
		fmt.Printf("  最大响应时间: %v\n", stats.MaxDuration)
		fmt.Printf("  总数据传输: %.2f MB\n", float64(stats.TotalBytes)/1024/1024)
	}

	if len(stats.StatusCodes) > 0 {
		fmt.Printf("\n状态码分布:\n")
		for code, count := range stats.StatusCodes {
			fmt.Printf("  %d: %d 次 (%.1f%%)\n", code, count,
				float64(count)/float64(stats.SuccessCount)*100)
		}
	}

	if len(stats.Errors) > 0 {
		fmt.Printf("\n错误分布:\n")
		for err, count := range stats.Errors {
			fmt.Printf("  %s: %d 次\n", err, count)
		}
	}

	// 计算吞吐量
	if config.TestDuration > 0 {
		throughput := float64(stats.TotalTests) / config.TestDuration.Seconds()
		fmt.Printf("\n吞吐量: %.2f 请求/秒\n", throughput)
	}

	// 保存结果到文件
	if config.OutputFile != "" {
		saveResultsToFile(stats, config)
	}
}

func saveResultsToFile(stats *Stats, config *Config) {
	file, err := os.Create(config.OutputFile)
	if err != nil {
		log.Printf("无法创建输出文件: %v", err)
		return
	}
	defer file.Close()

	writer := bufio.NewWriter(file)

	fmt.Fprintf(writer, "代理稳定性测试报告\n")
	fmt.Fprintf(writer, "测试时间: %s\n", time.Now().Format("2006-01-02 15:04:05"))
	fmt.Fprintf(writer, "代理服务器: %s\n", config.ProxyURL)
	fmt.Fprintf(writer, "目标URL: %s\n", config.TargetURL)
	fmt.Fprintf(writer, "并发数: %d\n", config.Concurrency)
	fmt.Fprintf(writer, "超时时间: %v\n", config.Timeout)
	fmt.Fprintf(writer, "\n")

	fmt.Fprintf(writer, "总请求数: %d\n", stats.TotalTests)
	fmt.Fprintf(writer, "成功请求: %d\n", stats.SuccessCount)
	fmt.Fprintf(writer, "失败请求: %d\n", stats.FailureCount)
	fmt.Fprintf(writer, "成功率: %.2f%%\n", float64(stats.SuccessCount)/float64(stats.TotalTests)*100)

	if stats.SuccessCount > 0 {
		fmt.Fprintf(writer, "平均响应时间: %v\n", stats.AvgDuration)
		fmt.Fprintf(writer, "最小响应时间: %v\n", stats.MinDuration)
		fmt.Fprintf(writer, "最大响应时间: %v\n", stats.MaxDuration)
	}

	writer.Flush()
	fmt.Printf("结果已保存到: %s\n", config.OutputFile)
}
```
