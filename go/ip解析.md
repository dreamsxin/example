```go
package main

import (
    "fmt"
    "net"
    "strings"
)

func main() {
    ipWall := NewFireWall()
    // 设置可以访问应用的ip段
    ipWall.ParseNode("127.0.0.1")
    ipWall.ParseNode("192.0.2.1/24")
    ipWall.ParseNode("2001:db8::/32") // 可以支持ipv6

    // 测试
    fmt.Println("127.0.0.1", ipWall.Check("127.0.0.1"))
    fmt.Println("192.0.2.10", ipWall.Check("192.0.2.10"))
    fmt.Println("192.0.3.10", ipWall.Check("192.0.3.10"))
    fmt.Println("2001:db8::1", ipWall.Check("2001:db8::1"))
    fmt.Println("2001:db9::1", ipWall.Check("2001:db9::1"))

}

// 定义防火墙，保存规则nodes

type FireWall struct {
    nodes []net.IPNet
}

func NewFireWall() *FireWall {
    return &FireWall{
        nodes: make([]net.IPNet, 0),
    }
}

// 添加规则

func (b *FireWall) ParseNode(line string) {
    if !strings.Contains(line, "/") {
        parsedIP := net.ParseIP(line)

        if ipv4 := parsedIP.To4(); ipv4 != nil {
            // return ip in a 4-byte representation
            parsedIP = ipv4
        }
        if parsedIP != nil {
            switch len(parsedIP) {
            case net.IPv4len:
                line += "/32"
            case net.IPv6len:
                line += "/128"
            }
        }
    }
    _, cidrNet, err := net.ParseCIDR(line)
    if err == nil {
        b.nodes = append(b.nodes, *cidrNet)
    }
}

// 检查某个ip在不在设置的规则里

func (b *FireWall) Check(ip string) bool {
    for _, cidr := range b.nodes {
        remoteIP := net.ParseIP(ip)
        if cidr.Contains(remoteIP) {
            return true
        }
    }
    return false
}
```
执行以上代码，输出
```txt
127.0.0.1 true
192.0.2.10 true
192.0.3.10 false
2001:db8::1 true
2001:db9::1 false
```
以上就是ip防火墙的实现了，在gin框架里`SetTrustedProxies`也是这么实现的
