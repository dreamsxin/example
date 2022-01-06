# TCP & UDP 的分片问题

这里的分片并不是我们在代码中处理的分包、粘包问题。

当我们发送一个 50K 的数据，给另一台互联网上的电脑时，网络底层实际上是把 50K 的数据，分成了很多小分片，依次发送。
要了解这个东西，需要涉及网络中的这四层（层级由低到高）：

- 链路层(以太网)
- 网络层(IP 协议)
- 传输层(TCP、UDP等)
- 应用层（填充实际的用户数据）
  
以太网帧的封包格式为：`Frame=Ethernet Header +IP Header +TCP Header +TCP Segment Data`
- Ethernet Header = 14 Byte = Dst Physical Address（6 Byte）+ Src Physical Address（6 Byte）+ Type（2 Byte），以太网帧头以下称之为数据帧。
- IP Header = 20 Byte（without options field），数据在IP层称为Datagram，分片称为Fragment。
- TCP Header = 20 Byte（without options field），数据在TCP层称为Stream，分段称为Segment（UDP中称为Message）。
- 54个字节后为TCP数据负载部分（Data Portion），即应用层用户数据。

Ethernet Header以下的IP数据报最大传输单位为MTU（Maximum Transmission Unit，Effect of short board），对于大多数使用以太网的局域网来说，MTU=1500。

TCP数据包每次能够传输的最大数据分段为MSS，为了达到最佳的传输效能，在建立TCP连接时双方将协商MSS值——双方提供的MSS值中的最小值为这次连接的最大MSS值。MSS往往基于MTU计算出来，通常MSS=MTU-sizeof(IP Header)-sizeof(TCP Header)=1500-20-20=1460。

## 链路层

https://www.rfc-editor.org/rfc/rfc894.txt

*数据链路层不单单只有一个以太网（Ethernet）格式，本文将以它为例，展开介绍。其它链路层协议：点对点（Point-to-Point Protocol）等。*

这一层的数据：就是一个完整的网络包，一般可以认为，它不会被再次分割。

它的长度范围是 [64 byte, 1518 byte] 其中 18 byte 的数据是链路层本身的数据，它会自动添加上。我们的代码正常情况下是无法控制，也不需要控制的。

具体格式如下：

`6 byte 目标地址 + 6 byte 源地址 + 2 byte 类型 + **网络层的数据( IP 包)** + 4 byte 校验和`

这里的地址指： 网卡的 MAC 地址，不是具体的 ip 地址。

也就是说：留给网络层的数据长度（以太网的有效载荷：Payload）只有 [64 -18, 1518 - 18] = [46, 1500] 

*我们把 1500 这个值，称为 以太网 的 MTU：Maximum Transmission Unit*

## 网络层

IP reference: http://www.networksorcery.com/enp/protocol/ip.htm
TCP reference: http://www.networksorcery.com/enp/protocol/tcp.htm
Ethernet frame header format: http://en.wikipedia.org/wiki/Ethernet_frame
IP header format: http://en.wikipedia.org/wiki/IPv4
TCP header format: http://en.wikipedia.org/wiki/Transmission_Control_Protocol

*网络也不单单只有 IP 协议，本文将以它为例，展开介绍。*
IP 协议，也会自动填充协议头，协议头一般占用了 20 byte。

不一般的情况就是：IP 协议本身提供了可选字段。所以如果有可选字段的话，那么实际的 IP 协议头占用会超过 20 byte

它的格式如下：
```text
// 该表格宽度占用了 4 byte
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Version|  IHL  |Type of Service|          Total Length         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         Identification        |Flags|      Fragment Offset    |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  Time to Live |    Protocol   |         Header Checksum       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                       Source Address                          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Destination Address                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Options                    |    Padding    | <-- optional
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                            DATA ...                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// 第一行占用了 4 byte
4 bit 版本号 + 4 bit IP 协议头长度（该长度值一般就是 20） + 8 bit 的 功能字段 + 16 bit 的 总长度 
// 第二行占用了 4 byte
16 bit 的分片标识 + 3 bit flags（也是用于分片标识） + 13 bit 的 分片的偏移值
// 第三行占用了 4 byte
8 bit 的TTL + 8 bit 的协议号 + 16 bit 的 头部校检和
// 第四行占用了 4 byte
源 IP 地址
// 第五行占用了 4 byte
目标 IP 地址
```

也就是说留给传输层数据 只有：[46-20, 1500-20] = [26, 1480]

## 传输层

*传输层也不单单只有 UDP、TCP 两个协议，本文将以它们为例，展开介绍。*

TCP 和 UDP 都是属于传输层的协议。

它们也有自己的头信息，但是由于功能并不同，所以它们两的协议头占用大小也是不同的。

其中 TCP 协议头占了 20 byte， UDP 协议头占了 8 byte。（TCP 之所以协议头比 UDP 大，就是因为它功能比较多：可靠、流速控制、自动分包等等）

那么也就是说：
TCP 可以传的用户数据实际上只有 [26 - 20, 1480 - 20] = [6, 1460]

## 应用层

那么如果用户在这一层，发送的数据超过了这个传输层能够承载的范围，会怎么样？

我们以“分别用UDP、TCP 发送 50k 数据”为例：

* 对于 TCP 来说
这里首先要说下：MSS（Maxitum Segment Size）最大分段大小，它是 TCP 协议里面的一个概念。

TCP 在建立连接的时候，会协商双方的MSS值，通常这个 MSS 会控制在 MTU 以内：最大 IP 包大小减去 IP 和 TCP 协议头的大小。（其最终目的：就是尽量避免 IP 分片）

这样 TCP 就可以在自己这一层，把用户发送的数据，预先分成多个大小限制在 MTU 里的 TCP 包。每个 TCP 的分片包，都完整了包含了 TCP 头信息，方便在接收方重组。

> 如果某些情况导致：已经分好的 TCP 分片，还是大于了 MTU，那就在 IP 层中，再执行一次分片。
> 这个时候如果数据丢了，那也只需要重传这一个 TCP 的分片，而不是整个原始的 50k 数据。

* 对于 UDP 来说
它并没有协商的能力，所以它只能直接把用户发送的数据，传给网络层（IP层），由网络层来进行分片。

对 网络层（IP层）来说：它并不知道上层传过来的数据，到底是 TCP 还是 UDP，它并不关心这个东西，也没有能力去区分。

如果发现数据过大，那么 IP 层会自动对数据进行切割，分片。

> 注意：因为 TCP 是在自己这一层执行分片，所以：IP层并不需要再去分片。
> 每个TCP分片，都是包含了完整的 TCP 协议头，这样另一方收到包之后，可以方便重组，可以计算出到底哪个分片包丢了，然后重新传这个分片包。

* UDP 数据过大，带来的问题

用 UDP 协议发送，那么如果网络发生了波动，丢失了某个 IP 包分片， 对于 UDP 而言， 它没有反馈丢失了哪个分片给发送方的能力，这就意味着：50k 的数据全都丢失了，如果需要重传，就得再次完整的传递这 50k 的数据。

UDP 协议头有 2 byte 表示长度的字段。所以实际 UDP 数据包的长度不能超过65507字节（65,535 − 8字节UDP报头 − 20字节IP头部）

TCP 是流数据，没有该限制

而 TCP 只会重传这一个丢失的分片包。

所以如果一个应用采用 UDP 来通讯，一般都会特意控制下单个包体的大小，从而提高传输效率
