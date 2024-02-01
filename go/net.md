TCPAddr类型

位于tcpsock.go中TCPAddr类型包含一个IP和一个port的结构:

type TCPAddr struct {
    IP   IP
    Port int
}
ResolveTCPAddr

func ResolveTCPAddr(net, addr string) (*TCPAddr, os.Error) 
该函数用来创建一个TCPAddr,第一个参数为,tcp,tcp4或者tcp6,addr是一个字符串，由主机名或IP地址，以及”:“后跟随着端口号组成，例如： “www.google.com:80” 或 ‘127.0.0.1:22”。如果地址是一个IPv6地址，由于已经有冒号，主机部分，必须放在方括号内, 例如：”[::1]:23”. 另一种特殊情况是经常用于服务器, 主机地址为0, 因此，TCP地址实际上就是端口名称, 例如：”:80” 用来表示HTTP服务器。

addr, _ := net.ResolveTCPAddr("tcp", "www.baidu.com:80")
fmt.Println(addr)   //220.181.111.147:80
udp
使用UDPConn接口体来表示，它实现了Conn、PacketConn两种接口。使用如下两个函数进行Dial和Listen。

func DialUDP(net string, laddr, raddr *UDPAddr) (*UDPConn, error)    
DialTCP在网络协议net上连接本地地址laddr和远端地址raddr。net必须是”udp”、”udp4”、”udp6”；如果laddr不是nil，将使用它作为本地地址，否则自动选择一个本地地址。

func ListenUDP(net string, laddr *UDPAddr) (*UDPConn, error)
ListenUDP创建一个接收目的地是本地地址laddr的UDP数据包的网络连接。net必须是”udp”、”udp4”、”udp6”；如果laddr端口为0，函数将选择一个当前可用的端口，可以用Listener的Addr方法获得该端口。返回的*UDPConn的ReadFrom和WriteTo方法可以用来发送和接收UDP数据包（每个包都可获得来源地址或设置目标地址）。

类型

1、UDPAddr类型

type UDPAddr struct {
    IP   IP
    Port int
}
ResolveUDPAddr同样的功能

2、UnixAddr类型

type UnixAddr struct {
    Name string
    Net  string
}
ResolveUnixAddr同样的功能
