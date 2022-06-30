# The IPv4 protocol & UDP

- https://pkg.go.dev/golang.org/x/net/ipv4
- https://colobu.com/2016/10/19/Go-UDP-Programming/

## UDP 一个简单的例子

server:
```go
package main
import (
	"fmt"
	"net"
)
func main() {
	listener, err := net.ListenUDP("udp", &net.UDPAddr{IP: net.ParseIP("127.0.0.1"), Port: 9981})
	if err != nil {
		fmt.Println(err)
		return
	}
	fmt.Printf("Local: <%s> \n", listener.LocalAddr().String())
	data := make([]byte, 1024)
	for {
		n, remoteAddr, err := listener.ReadFromUDP(data)
		if err != nil {
			fmt.Printf("error during read: %s", err)
		}
		fmt.Printf("<%s> %s\n", remoteAddr, data[:n])
		_, err = listener.WriteToUDP([]byte("world"), remoteAddr)
		if err != nil {
			fmt.Printf(err.Error())
		}
	}
}
```
client:
```go
package main
import (
	"fmt"
	"net"
)
func main() {
	sip := net.ParseIP("127.0.0.1")
	srcAddr := &net.UDPAddr{IP: net.IPv4zero, Port: 0}
	dstAddr := &net.UDPAddr{IP: ip, Port: 9981}
	conn, err := net.DialUDP("udp", srcAddr, dstAddr)
	if err != nil {
		fmt.Println(err)
	}
	defer conn.Close()
	conn.Write([]byte("hello"))

  data := make([]byte, 1024)
	n, err := conn.Read(data)
	fmt.Printf("read %s from <%s>\n", data[:n], conn.RemoteAddr())
}
```

## 互为服务端和客户端

```go
package main
import (
	"fmt"
	"net"
	"os"
	"time"
)
func read(conn *net.UDPConn) {
	for {
		data := make([]byte, 1024)
		n, remoteAddr, err := conn.ReadFromUDP(data)
		if err != nil {
			fmt.Printf("error during read: %s", err)
		}
		fmt.Printf("receive %s from <%s>\n", data[:n], remoteAddr)
	}
}
func main() {
	addr1 := &net.UDPAddr{IP: net.ParseIP("127.0.0.1"), Port: 9981}
	addr2 := &net.UDPAddr{IP: net.ParseIP("127.0.0.1"), Port: 9982}
	go func() {
		listener1, err := net.ListenUDP("udp", addr1)
		if err != nil {
			fmt.Println(err)
			return
		}
		go read(listener1)
		time.Sleep(5 * time.Second)
		listener1.WriteToUDP([]byte("ping to #2: "+addr2.String()), addr2)
	}()
	go func() {
		listener1, err := net.ListenUDP("udp", addr2)
		if err != nil {
			fmt.Println(err)
			return
		}
		go read(listener1)
		time.Sleep(5 * time.Second)
		listener1.WriteToUDP([]byte("ping to #1: "+addr1.String()), addr1)
	}()
	b := make([]byte, 1)
	os.Stdin.Read(b)
}
```

通常来说，服务端 使用 `ReadFromUDP` 和 `WriteToUDP`，客户端使用 `Read` 和 `Write`。

你必须清楚知道你的UDP是连接的(connected)还是未连接(unconnected)的，这样你才能正确的选择的读写方法。

如果 `*UDPConn` 是 connected, 读写方法是 Read 和 Write。
如果 `*UDPConn` 是 unconnected, 读写方法是 ReadFromUDP 和 WriteToUDP（以及ReadFrom和WriteTo)。

## 通用多播编程

Go标准库也支持多播，但是我们首先我们看通用的多播是如何实现的，它使用 `golang.org/x/net/ipv4` 或者 `golang.org/x/net/ipv6` 进行控制。
首先找到要进行多播所使用的网卡,然后监听本机合适的地址和服务端口。
将这个应用加入到多播组中，它就可以从组中监听包信息，当然你还可以对包传输进行更多的控制设置。
应用收到包后还可以检查包是否来自这个组的包。

```go
package main
import (
	"fmt"
	"net"
	"golang.org/x/net/ipv4"
)
func main() {
	//1. 得到一个interface
	en4, err := net.InterfaceByName("en4")
	if err != nil {
		fmt.Println(err)
	}
	group := net.IPv4(224, 0, 0, 250)
	//2. bind一个本地地址
	c, err := net.ListenPacket("udp4", "0.0.0.0:1024")
	if err != nil {
		fmt.Println(err)
	}
	defer c.Close()
	//3.
	p := ipv4.NewPacketConn(c)
	if err := p.JoinGroup(en4, &net.UDPAddr{IP: group}); err != nil {
		fmt.Println(err)
	}
	//4.更多的控制
	if err := p.SetControlMessage(ipv4.FlagDst, true); err != nil {
		fmt.Println(err)
	}
	//5.接收消息
	b := make([]byte, 1500)
	for {
		n, cm, src, err := p.ReadFrom(b)
		if err != nil {
			fmt.Println(err)
		}
		if cm.Dst.IsMulticast() {
			if cm.Dst.Equal(group) {
				fmt.Printf("received: %s from <%s>\n", b[:n], src)
				n, err = p.WriteTo([]byte("world"), cm, src)
				if err != nil {
					fmt.Println(err)
				}
			} else {
				fmt.Println("Unknown group")
				continue
			}
		}
	}
}
```

同一个应用可以加入到多个组中，多个应用也可以加入到同一个组中。
多个UDP listener可以监听同样的端口，加入到同一个group中。

```go
c1, err := net.ListenPacket("udp4", "224.0.0.0:1024")
if err != nil {
	// error handling
}
defer c1.Close()
c2, err := net.ListenPacket("udp4", "224.0.0.0:1024")
if err != nil {
	// error handling
}
defer c2.Close()
p1 := ipv4.NewPacketConn(c1)
if err := p1.JoinGroup(en0, &net.UDPAddr{IP: net.IPv4(224, 0, 0, 248)}); err != nil {
	// error handling
}
p2 := ipv4.NewPacketConn(c2)
if err := p2.JoinGroup(en0, &net.UDPAddr{IP: net.IPv4(224, 0, 0, 248)}); err != nil {
	// error handling
}
```

## 标准库多播编程

标准库的多播编程简化了上面的操作，当然也减少了更多的控制。如果想实现一个简单的多播程序，可以使用这样的方法。
Server:
```go
func main() {
	//如果第二参数为nil,它会使用系统指定多播接口，但是不推荐这样使用
	addr, err := net.ResolveUDPAddr("udp", "224.0.0.250:9981")
	if err != nil {
		fmt.Println(err)
	}
	listener, err := net.ListenMulticastUDP("udp", nil, addr)
	if err != nil {
		fmt.Println(err)
		return
	}
	fmt.Printf("Local: <%s> \n", listener.LocalAddr().String())
	data := make([]byte, 1024)
	for {
		n, remoteAddr, err := listener.ReadFromUDP(data)
		if err != nil {
			fmt.Printf("error during read: %s", err)
		}
		fmt.Printf("<%s> %s\n", remoteAddr, data[:n])
	}
}
```
Client:
```go
func main() {
	ip := net.ParseIP("224.0.0.250")
	srcAddr := &net.UDPAddr{IP: net.IPv4zero, Port: 0}
	dstAddr := &net.UDPAddr{IP: ip, Port: 9981}
	conn, err := net.DialUDP("udp", srcAddr, dstAddr)
	if err != nil {
		fmt.Println(err)
	}
	defer conn.Close()
	conn.Write([]byte("hello"))
	fmt.Printf("<%s>\n", conn.RemoteAddr())}
}
```

## 广播

广播的编程方式和多播的编程方式有所不同。简单说，广播意味着你吼一嗓子，局域网内的所有的机器都会收到。

服务器端代码：
```go
func main() {
	listener, err := net.ListenUDP("udp", &net.UDPAddr{IP: net.IPv4zero, Port: 9981})
	if err != nil {
		fmt.Println(err)
		return
	}
	fmt.Printf("Local: <%s> \n", listener.LocalAddr().String())
	data := make([]byte, 1024)
	for {
		n, remoteAddr, err := listener.ReadFromUDP(data)
		if err != nil {
			fmt.Printf("error during read: %s", err)
		}
		fmt.Printf("<%s> %s\n", remoteAddr, data[:n])
		_, err = listener.WriteToUDP([]byte("world"), remoteAddr)
		if err != nil {
			fmt.Printf(err.Error())
		}
	}
}
```

客户端代码有所不同，它不是通过DialUDP “连接” 广播地址，而是通过ListenUDP创建一个unconnected的 *UDPConn,然后通过WriteToUDP发送数据报，这和你脑海中的客户端不太一致：
```go
func main() {
	ip := net.ParseIP("172.24.14.255")
	srcAddr := &net.UDPAddr{IP: net.IPv4zero, Port: 0}
	dstAddr := &net.UDPAddr{IP: ip, Port: 9981}
	conn, err := net.ListenUDP("udp", srcAddr)
	if err != nil {
		fmt.Println(err)
	}
	n, err := conn.WriteToUDP([]byte("hello"), dstAddr)
	if err != nil {
		fmt.Println(err)
	}
	data := make([]byte, 1024)
	n, _, err = conn.ReadFrom(data)
	if err != nil {
		fmt.Println(err)
	}
	fmt.Printf("read %s from <%s>\n", data[:n], conn.RemoteAddr())
	b := make([]byte, 1)
	os.Stdin.Read(b)
}
```

广播地址(Broadcast Address)是专门用于同时向网络中所有工作站进行发送的一个地址。在使用TCP/IP 协议的网络中，主机标识段host ID 为全1 的IP 地址为广播地址，广播的分组传送给host ID段所涉及的所有计算机。例如，对于10.1.1.0 （255.255.255.0 ）网段，其广播地址为10.1.1.255 （255 即为2 进制的11111111 ），当发出一个目的地址为10.1.1.255 的分组（封包）时，它将被分发给该网段上的所有计算机。
