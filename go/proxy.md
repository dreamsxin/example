## 常规代理请求(本地DNS解析)

```go
package main

import (
    "fmt"
    "io"
    "net/http"
    "net/url"
)

func main() {
    proxy, _ := url.Parse("socks5://1.1.1.1:1080")   // 定义代理服务器变量
    client := &http.Client{}
    client.Transport = &http.Transport{
        Proxy: http.ProxyURL(proxy),   // 为 Transport 设置 Proxy 参数指定代理
    }

    // 构造 http 请求，可自定义请求 Header 参数
    req, _ := http.NewRequest("GET", "https://www.moonfly.net/", nil)
    req.Header.Set("User-Agent", "curl/8.4.0-checkproxy-go")

    // 发送请求
    resp, err := client.Do(req)
    if err != nil {
        fmt.Println(err.Error())
        return
    }
    defer resp.Body.Close()

    // 读取返回数据
    body, _ := io.ReadAll(resp.Body)
    fmt.Println(string(body))
    return
}
```

## 通过远程DNS解析的代理请求
在 Go 语言中，我们可以使用 golang.org/x/net/proxy 包来实现 SOCKS5 代理。
以下是使用这个包来创建一个具有远程 DNS 解析功能的 SOCKS5 代理 HTTP 客户端：

```go
package main

import (
    "context"
    "fmt"
    "io"
    "net"
    "net/http"
    "os"

    "golang.org/x/net/proxy"
)

func main() {
    //创建一个 socks5 的拨号器
    dialer, err := proxy.SOCKS5("tcp", "1.1.1.1:1080", nil, proxy.Direct)
    if err != nil {
        fmt.Fprintln(os.Stderr, "无法连接到代理服务器:", err)
        os.Exit(1)
    }

    // 设置 http.Transport 采用 DialContext 方式指定代理
    httpTransport := &http.Transport{
        DialContext: func(ctx context.Context, network, addr string) (net.Conn, error) {
            return dialer.Dial(network, addr)
        },
    }

    // 创建连接
    client := &http.Client{Transport: httpTransport}

    // 创建请求
    req, err := http.NewRequest("GET", "https://www.moonfly.net", nil)
    if err != nil {
        fmt.Println(err)
        return
    }

    // 发送请求
    resp, err := client.Do(req)
    if err != nil {
        fmt.Println(err)
        return
    }
    defer resp.Body.Close()

    // 读取响应
    body, err := io.ReadAll(resp.Body)
    if err != nil {
        fmt.Println(err)
        return
    }

    // 打印响应
    fmt.Println(string(body))
}

package main

import (
    "context"
    "fmt"
    "io"
    "net"
    "net/http"
    "os"

    "golang.org/x/net/proxy"
)

func main() {
    //创建一个 socks5 的拨号器
    dialer, err := proxy.SOCKS5("tcp", "1.1.1.1:1080", nil, proxy.Direct)
    if err != nil {
        fmt.Fprintln(os.Stderr, "无法连接到代理服务器:", err)
        os.Exit(1)
    }

    // 设置 http.Transport 采用 DialContext 方式指定代理
    httpTransport := &http.Transport{
        DialContext: func(ctx context.Context, network, addr string) (net.Conn, error) {
            return dialer.Dial(network, addr)
        },
    }

    // 创建连接
    client := &http.Client{Transport: httpTransport}

    // 创建请求
    req, err := http.NewRequest("GET", "https://www.moonfly.net", nil)
    if err != nil {
        fmt.Println(err)
        return
    }

    // 发送请求
    resp, err := client.Do(req)
    if err != nil {
        fmt.Println(err)
        return
    }
    defer resp.Body.Close()

    // 读取响应
    body, err := io.ReadAll(resp.Body)
    if err != nil {
        fmt.Println(err)
        return
    }

    // 打印响应
    fmt.Println(string(body))
}
```

以上代码使用 proxy.SOCKS5 创建 SOCKS5 代理时，DNS 解析是由代理服务器远程执行的。这是因为使用了 proxy.SOCKS5 函数的 "tcp" 网络参数，这意味着所有 TCP 连接（包括 DNS 查询）都将通过 SOCKS5 代理进行。

这种行为与 curl 命令的 --socks5-hostname 参数类似，表示通过 SOCKS5 代理发送未解析的主机名，由代理服务器进行 DNS 解析。

如果希望在本机进行 DNS 解析，然后将 IP 地址发送到代理服务器进行访问，可以使用 proxy.SOCKS5 函数的 "tcp4" 或 "tcp6" 网络参数，这将导致 Go 在本地解析 DNS，然后通过代理建立到目标 IP 地址的连接。

## 总结

在 Go 语言中，http.Transport 的 Proxy 字段和 golang.org/x/net/proxy 包提供的 SOCKS5 代理都可以用来设置 HTTP 客户端的代理服务器。但是，它们在处理 DNS 解析时的行为是不同的。

当你使用 http.Transport 的 Proxy 字段设置代理服务器时，Go 会在本地解析 DNS，然后将 IP 地址发送到代理服务器。这种行为类似于 curl 命令的 --socks5 参数。

当你使用 golang.org/x/net/proxy 包的 SOCKS5 代理时，DNS 解析是由代理服务器远程执行的。这种行为类似于 curl 命令的 --socks5-hostname 参数。

因此，如果我们希望程序在本地进行 DNS 解析，就可可以直接使用 http.Transport 的 Proxy 字段来设置代理。如果希望由代理服务器来替我们进行 DNS 解析，就需要使用 golang.org/x/net/proxy 包的 SOCKS5 代理。
