package main

import (
	"bufio"
	"fmt"
	"log"
	"log/slog"
	"net/http"
	"net/url"
	"os"
	"path/filepath"
	"sort"
	"sync"
	"time"

	"github.com/go-resty/resty/v2"
)

func init() {
	log.SetFlags(log.Lshortfile)
}

// ProxyResult holds the result of a proxy test.
type ProxyResult struct {
	URL     string
	Latency time.Duration
	IsAlive bool
}

type ProxyHealthChecker struct {
	client *resty.Client
}

func NewProxyHealthChecker() *ProxyHealthChecker {
	return &ProxyHealthChecker{
		client: resty.New().SetTimeout(15 * time.Second),
		//SetRetryCount(2).
		//SetRetryWaitTime(1 * time.Second),
	}
}

// 检查单个代理
func (p *ProxyHealthChecker) CheckProxy(proxyURL, testURL string) (time.Duration, bool) {
	client := p.client.SetProxy(proxyURL)
	client.SetTimeout(5 * time.Second)

	start := time.Now()
	resp, err := client.R().
		//SetHeader("User-Agent", "ProxyHealthCheck/1.0").
		Get(testURL)

	duration := time.Since(start)

	if err != nil {
		log.Println("CheckProxy", proxyURL, err)
		return duration, false
	}

	log.Println("CheckProxy", proxyURL, resp.StatusCode())
	return duration, resp.StatusCode() == 200
}

// TestProxyLatency checks a single proxy's status and latency.
func TestProxyLatency(proxyURL string, timeout time.Duration) (time.Duration, bool) {
	proxy, err := url.Parse(proxyURL)
	if err != nil {
		slog.Error(fmt.Sprintf("Invalid proxy URL: %v", err))
		return 0, false
	}

	// 使用 resty 库

	client := &http.Client{
		Transport: &http.Transport{
			Proxy: http.ProxyURL(proxy),
		},
		Timeout: timeout,
	}

	start := time.Now()
	resp, err := client.Get("https://ipinfo.io/json")
	latency := time.Since(start)

	if err != nil {
		// It's common for proxy tests to fail, so we can log this as Info or Debug.
		slog.Info(fmt.Sprintf("Proxy check failed for %s, error: %v", proxyURL, err))
		return latency, false
	}
	defer resp.Body.Close()

	slog.Debug(fmt.Sprintf("Checking proxy status: %s, status code: %d", proxyURL, resp.StatusCode))

	return latency, resp.StatusCode == http.StatusOK
}

// loadProxiesFromFile reads a list of proxy URLs from a file.
func loadProxiesFromFile(filePath string) ([]string, error) {
	file, err := os.Open(filePath)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	var proxies []string
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		proxies = append(proxies, scanner.Text())
	}

	return proxies, scanner.Err()
}

func main() {
	var proxies []string
	// Generate a list of proxy URLs to test.
	// Reduced the range for a quicker demonstration.

	var wg sync.WaitGroup

	// 文件名通过命令行参数获取
	// 检查命令行参数是否提供了文件名
	if len(os.Args) < 2 {
		log.Fatalf("Usage: %s <proxy_file>", os.Args[0])
	}
	filename := os.Args[1]
	proxies, err := loadProxiesFromFile(filename)
	if err != nil {
		log.Fatalf("Failed to read proxy list: %v", err)
	}
	filterProxies := map[string]bool{}
	if len(os.Args) > 2 {
		arr, err := loadProxiesFromFile(os.Args[2])
		if err != nil {
			log.Fatalf("Failed to read fitler proxy list: %v", err)
		} else {
			for _, ip := range arr {
				filterProxies[ip] = true
			}
		}
	}

	basename := filepath.Base(filename)
	healthChecker := NewProxyHealthChecker()
	results := make(chan ProxyResult, len(proxies))
	// 并发限制
	c := make(chan struct{}, 50)
	for _, proxyURL := range proxies {
		wg.Add(1)
		c <- struct{}{}
		go func(pURL string) {
			defer wg.Done()
			defer func() { <-c }()
			//latency, isAlive := TestProxyLatency(pURL, 15*time.Second)
			//log.Println("CheckProxy", pURL)
			if filterProxies[pURL] {
				return
			}
			latency, isAlive := healthChecker.CheckProxy(pURL, "https://myip.ipipv.com/")
			results <- ProxyResult{URL: pURL, Latency: latency, IsAlive: isAlive}
		}(proxyURL)
	}

	// Wait for all the proxy checks to complete.
	wg.Wait()
	close(results)
	badFile, err := os.OpenFile(fmt.Sprintf("bad%s", basename), os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0644)
	if err != nil {
		log.Fatalf("Failed to open bad.txt: %v", err)
	}
	defer badFile.Close()

	aliveFile, err := os.OpenFile(fmt.Sprintf("aliveip-%s", basename), os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0644)
	if err != nil {
		log.Fatalf("Failed to open aliveip.txt: %v", err)
	}
	defer aliveFile.Close()
	// Process the results.
	latencyMap := make(map[string]time.Duration)
	totalDur := time.Duration(0)
	for result := range results {
		if result.IsAlive {
			log.Printf("Proxy %s is alive, latency: %v", result.URL, result.Latency)
			latencyMap[result.URL] = result.Latency
			totalDur += result.Latency
		} else {
			badFile.WriteString(result.URL + "\n")
			//log.Printf("Proxy %s is not alive, check took: %v", result.URL, result.Latency)
		}
	}

	latencyFile, err := os.OpenFile(fmt.Sprintf("latency-%s", basename), os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0644)
	if err != nil {
		log.Fatalf("Failed to open latency.txt: %v", err)
	}
	defer latencyFile.Close()
	// 按照延时排序
	type ProxyLatency struct {
		URL     string
		Latency time.Duration
	}
	var latencyList []ProxyLatency
	for url, latency := range latencyMap {
		latencyList = append(latencyList, ProxyLatency{URL: url, Latency: latency})
	}
	sort.Slice(latencyList, func(i, j int) bool {
		return latencyList[i].Latency < latencyList[j].Latency
	})
	for _, latency := range latencyList {
		log.Printf("Proxy %s latency: %v", latency.URL, latency.Latency)
		aliveFile.WriteString(latency.URL + "\n")
		latencyFile.WriteString(latency.URL + "-" + latency.Latency.String() + "\n")
	}
	log.Printf("Total duration: %v", totalDur)
	log.Printf("Average latency: %v", totalDur/time.Duration(len(latencyMap)))

}
