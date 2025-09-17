- https://github.com/ChromeDevTools/devtools-protocol
- https://github.com/CompileYouth/front-end-study/tree/master/tool/devtools

```shell
./chrome.exe --remote-debugging-port=9222 --user-data-dir="E:\chrome-test" --remote-allow-origins=*
```

浏览 `https://localhost:9222/json`，复制里面的 `devtoolsFrontendUrl` 

`http://localhost:9222/devtools/devtools_app.html?ws=localhost:9222/devtools/page/XXXXXX`

## 透传

```go
package main

import (
	"net/http"
	"net/http/httputil"
	"net/url"
	"strings"
)

func main() {
	// 目标服务URL
	target, _ := url.Parse("http://localhost:9222")
	proxy := httputil.NewSingleHostReverseProxy(target)

	// 前缀匹配路由
	http.HandleFunc("/browser/", func(w http.ResponseWriter, r *http.Request) {
		// 去除前缀后转发
		r.URL.Path = strings.TrimPrefix(r.URL.Path, "/browser")
		r.URL.RawQuery = ""
		proxy.ServeHTTP(w, r)
	})

	// 其他路由处理
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		w.Write([]byte("Default handler"))
	})

	http.ListenAndServe(":8080", nil)
}
```

```go
package main

import (
	"log"
	"net/http"
	"net/http/httputil"
	"net/url"
	"strings"
	"time"
)

func loggingMiddleware(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		start := time.Now()
		log.Printf("Started %s %s", r.Method, r.URL.Path)

		next.ServeHTTP(w, r)

		log.Printf("Completed %s in %v", r.URL.Path, time.Since(start))
	})
}

func main() {
	// 目标服务URL
	target, _ := url.Parse("http://localhost:9222")
	proxy := httputil.NewSingleHostReverseProxy(target)

	// 路由配置
	mux := http.NewServeMux()
	mux.HandleFunc("/browser/", func(w http.ResponseWriter, r *http.Request) {
		r.URL.Path = strings.TrimPrefix(r.URL.Path, "/browser")
		r.URL.RawQuery = ""
		proxy.ServeHTTP(w, r)
	})

	// 应用中间件
	handler := loggingMiddleware(mux)

	// 启动服务
	log.Println("Starting proxy server on :8080")
	log.Fatal(http.ListenAndServe(":8080", handler))
}
```
