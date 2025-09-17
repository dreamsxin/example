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
