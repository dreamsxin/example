## 超时时间

### 服务端

发送 10 个 chunk，每个间隔 1.5 秒

```go
package main

import (
	"fmt"
	"log"
	"net/http"
	"time"
)

func chunkHandler(w http.ResponseWriter, r *http.Request) {
	// 设置内容类型，并确保支持 Flush
	w.Header().Set("Content-Type", "text/plain")
	flusher, ok := w.(http.Flusher)
	if !ok {
		http.Error(w, "Streaming unsupported", http.StatusInternalServerError)
		return
	}

	// 发送 10 个 chunk，每个间隔 1.5 秒
	for i := 0; i < 10; i++ {
		fmt.Fprintf(w, "Chunk %d (time: %s)\n", i, time.Now().Format("15:04:05.000"))
		flusher.Flush() // 立即发送当前 chunk
		time.Sleep(1500 * time.Millisecond)
	}

	// 发送结束标志
	fmt.Fprintln(w, "Done.")
	flusher.Flush()
	log.Println("Finished sending chunks")
}

func main() {
	http.HandleFunc("/", chunkHandler)
	log.Println("Starting server on :8080")
	log.Fatal(http.ListenAndServe(":8080", nil))
}
```

### go 客户端

`go run . -mode global` 一次性读取，超时时间5s，会超时。
`go run . -mode interval` 间隔读取，间隔超时时间2s，不会超时。
```go
package main

import (
	"context"
	"flag"
	"fmt"
	"io"
	"log"
	"net/http"
	"net/url"
	"time"
)

func testGlobalTimeout(proxyURLStr string) {
	client := &http.Client{
		Timeout: 2 * time.Second, // 整个请求（含读取响应体）最多5秒
	}

	if proxyURLStr != "" {
		proxyURL, err := url.Parse(proxyURLStr)
		if err != nil {
			log.Fatalf("Invalid proxy URL: %v", err)
		}
		transport := &http.Transport{
			Proxy: http.ProxyURL(proxyURL),
		}
		client.Transport = transport
	}

	req, err := http.NewRequest("GET", "http://localhost:8080/", nil)
	if err != nil {
		log.Fatal(err)
	}

	start := time.Now()
	resp, err := client.Do(req)
	if err != nil {
		log.Printf("Request failed after %v: %v", time.Since(start), err)
		return
	}
	defer resp.Body.Close()

	// 读取响应
	buf := make([]byte, 1024)
	for {
		n, err := resp.Body.Read(buf)
		if n > 0 {
			fmt.Print(string(buf[:n]))
		}
		if err != nil {
			if err == io.EOF {
				break
			}
			log.Printf("Read error after %v: %v", time.Since(start), err)
			break
		}
	}
	log.Printf("Total time: %v", time.Since(start))
}

// testReadIntervalTimeout 使用 context 实现读取间隔超时（示例2）
func testReadIntervalTimeout(proxyURLStr string) {
	client := &http.Client{
		// 不设置全局 Timeout，我们手动控制
	}

	if proxyURLStr != "" {
		proxyURL, err := url.Parse(proxyURLStr)
		if err != nil {
			log.Fatalf("Invalid proxy URL: %v", err)
		}
		transport := &http.Transport{
			Proxy: http.ProxyURL(proxyURL),
		}
		client.Transport = transport
	}

	// 使用一个可取消的 context 以便在超时或完成后终止
	ctx, cancel := context.WithCancel(context.Background())
	defer cancel()

	req, err := http.NewRequestWithContext(ctx, "GET", "http://localhost:8080/", nil)
	if err != nil {
		log.Fatal(err)
	}

	start := time.Now()
	resp, err := client.Do(req)
	if err != nil {
		log.Printf("Request failed after %v: %v", time.Since(start), err)
		return
	}
	defer resp.Body.Close()

	readTimeout := 2 * time.Second // 读取间隔超时
	buf := make([]byte, 1024)

	for {
		// 为每次读取创建一个带超时的 context
		readCtx, readCancel := context.WithTimeout(context.Background(), readTimeout)
		// 使用 channel 接收读取结果
		readChan := make(chan struct {
			n   int
			err error
		}, 1)

		go func() {
			n, err := resp.Body.Read(buf)
			readChan <- struct {
				n   int
				err error
			}{n, err}
		}()

		select {
		case <-readCtx.Done():
			readCancel()
			log.Printf("Read timeout after %v (no data for %v)", time.Since(start), readTimeout)
			return
		case res := <-readChan:
			readCancel() // 取消 context 防止 goroutine 泄漏
			if res.n > 0 {
				fmt.Print(string(buf[:res.n]))
			}
			if res.err != nil {
				if res.err == io.EOF {
					log.Printf("Finished after %v", time.Since(start))
					return
				}
				log.Printf("Read error after %v: %v", time.Since(start), res.err)
				return
			}
		}
	}
}

func main() {
	var mode string
	var proxy string
	flag.StringVar(&mode, "mode", "global", "Test mode: global or interval")
	flag.StringVar(&proxy, "proxy", "", "Proxy URL (e.g., http://localhost:3128)")
	flag.Parse()

	switch mode {
	case "global":
		testGlobalTimeout(proxy)
	case "interval":
		testReadIntervalTimeout(proxy)
	default:
		log.Fatalf("Unknown mode: %s", mode)
	}
}
```

### python 客户端

一次性读取，不会超时。

```python
import http.client
import requests

# 强制使用 HTTP/1.0
http.client.HTTPConnection._http_vsn = 10
http.client.HTTPConnection._http_vsn_str = 'HTTP/1.0'

import time

url = "http://localhost:8080/"

try:
    start = time.time()

    r = requests.get(url, timeout=(1, 2))
    print("Receiving chunks:\n" + "-"*40)
    print(r.content)
    print(r.status_code)

    elapsed = time.time() - start
    print(f"\nTotal time: {elapsed:.2f} seconds")
except Exception as e:
    elapsed = time.time() - start
    print(f"Exception after {elapsed:.2f} seconds: {type(e).__name__}: {e}")
```

流式读取，不会超时。
```python
import requests
import time

url = "http://localhost:8080/"

try:
    start = time.time()

    r = requests.get(url, timeout=(1, 2), stream=True)
    print("Receiving chunks:\n" + "-"*40)

    for chunk in r.iter_content(chunk_size=None):
        if chunk:
            print(chunk.decode(), end='')
    elapsed = time.time() - start
    print(f"\nTotal time: {elapsed:.2f} seconds")
except Exception as e:
    elapsed = time.time() - start
    print(f"Exception after {elapsed:.2f} seconds: {type(e).__name__}: {e}")
````
