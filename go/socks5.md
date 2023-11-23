```go
package main
 
import (
	"flag"
	"fmt"
	"github.com/armon/go-socks5"
	"log"
)
 
func main() {
	var port int
	flag.IntVar(&port, "port", 58888, "Port for the proxy server")
	flag.Parse()
 
	// 创建 SOCKS5 服务器实例
	conf := &socks5.Config{}
	server, err := socks5.New(conf)
	if err != nil {
		log.Fatalf("Failed to create SOCKS5 server: %v", err)
	}
 
	// 监听并处理请求
	addr := fmt.Sprintf(":%d", port)
	log.Printf("SOCKS5 proxy server started on port %d\n", port)
	if err := server.ListenAndServe("tcp", addr); err != nil {
		log.Fatalf("Failed to start SOCKS5 server: %v", err)
	}
}
```

```shell
go env -w GOARCH=386
go env -w CGO_ENABLED=1
go.exe build -x -v -buildmode=c-archive .
go.exe env -w GOARCH=amd64
go.exe env -w CGO_ENABLED=0
go get github.com\armon\go-socks5
```
