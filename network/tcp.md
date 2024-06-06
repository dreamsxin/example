## 三次握手

https://github.com/dreamsxin/example/blob/master/tools/tcpdump.md

### 使用 tcpdump 抓包
···shell
tcpdump -XAns 4096 -iany port 80
```
- -n 不把主机的网络地址转换成名字
- -t 在每列倾倒资料上不显示时间戳记
- -x 用十六进制字码列出数据包资料
- -i 抓取指定网卡的包
- -c 抓取包的个数

输出
```tcpdump
14:25:55.733153 eth0  In  IP 115.206.121.69.34476 > 172.31.36.243.9004: Flags [S], seq 1097659038, win 65535, options [mss 1452,sackOK,TS val 2198095855 ecr 0,nop,wscale 8], length 0
        0x0000:  4520 003c fe39 4000 2506 993c 73ce 7945  E..<.9@.%..<s.yE
        0x0010:  ac1f 24f3 86ac 232c 416c f29e 0000 0000  ..$...#,Al......
        0x0020:  a002 ffff e109 0000 0204 05ac 0402 080a  ................
        0x0030:  8304 47ef 0000 0000 0103 0308            ..G.........
14:25:55.733182 eth0  Out IP 172.31.36.243.9004 > 115.206.121.69.34476: Flags [S.], seq 3058750161, ack 1097659039, win 62643, options [mss 8961,sackOK,TS val 3663227419 ecr 2198095855,nop,wscale 1], length 0
        0x0000:  4500 003c 0000 4000 4006 7c96 ac1f 24f3  E..<..@.@.|...$.
        0x0010:  73ce 7945 232c 86ac b650 d2d1 416c f29f  s.yE#,...P..Al..
        0x0020:  a012 f4b3 be54 0000 0204 2301 0402 080a  .....T....#.....
        0x0030:  da58 6a1b 8304 47ef 0103 0301            .Xj...G.....
14:25:55.948120 eth0  In  IP 115.206.121.69.34476 > 172.31.36.243.9004: Flags [.], ack 1, win 343, options [nop,nop,TS val 2198096070 ecr 3663227419], length 0
        0x0000:  4520 0034 fe3a 4000 2506 9943 73ce 7945  E..4.:@.%..Cs.yE
        0x0010:  ac1f 24f3 86ac 232c 416c f29f b650 d2d2  ..$...#,Al...P..
        0x0020:  8010 0157 3ff9 0000 0101 080a 8304 48c6  ...W?.........H.
        0x0030:  da58 6a1b                                .Xj.
```
两位16进制的数刚好就是一个字节(二进制数字的8位)组成。
这里数据包共包含60字节，其中前20字节是IP头部，后40字节是TCP头部，不包含应用程序数据（length值为0）。
- F : FIN - 结束; 结束会话
- S : SYN - 同步; 表示开始会话请求
- R : RST - 复位;中断一个连接
- P : PUSH - 推送; 数据包立即发送
- ./A : ACK - 应答

- flags MF表示有更多分片，DF 表示不分片。

## websocket

下面是 tcpdump 抓下来的两条数据，第一条是客户端发给服务端的，第二天是服务端回复给客户端的。第一条内容无法显示。

```cap
14:40:54.037229 enX0  In  IP client.36518 > 172.31.8.14.9000: Flags [P.], seq 41:82, ack 588, win 166, options [nop,nop,TS val 1750599413 ecr 1061762697], length 41
        0x0000:  4538 005d 89a7 4000 2e06 b4d0 dfee 79cf  E8.]..@.......y.
        0x0010:  ac1f 080e 8ea6 232b 28af 8dc7 e848 dcdf  ......#+(....H..
        0x0020:  8018 00a6 8692 0000 0101 080a 6858 06f5  ............hX..
        0x0030:  3f49 3689 81a3 3e47 6939 4565 0a56 532a  ?I6...>Gi9Ee.VS*
        0x0040:  0857 5a65 5308 0e77 5909 0e77 451b 5326  .WZeS..wY..wE.S&
        0x0050:  0a51 5729 0c70 5a65 531b 0f65 14         .QW).pZeS..e.
14:40:54.037648 enX0  Out IP 172.31.8.14.9000 > client.36518: Flags [P.], seq 588:620, ack 82, win 29338, options [nop,nop,TS val 1061763425 ecr 1750599413], length 32
        0x0000:  4500 0054 02ca 4000 7f06 eaee ac1f 080e  E..T..@.........
        0x0010:  dfee 79cf 232b 8ea6 e848 dcdf 28af 8df0  ..y.#+...H..(...
        0x0020:  8018 729a 0e32 0000 0101 080a 3f49 3961  ..r..2......?I9a
        0x0030:  6858 06f5 811e 7b22 7661 6c75 6522 3a7b  hX....{"value":{
        0x0040:  7d2c 2263 6f6d 6d61 6e64 223a 3130 3030  },"command":1000
        0x0050:  3030 307d                                000}
```

用 wireshark 打开可以，看到如下内容：
```text
Frame 10: 113 bytes on wire (904 bits), 113 bytes captured (904 bits)
Linux cooked capture v2
Internet Protocol Version 4, Src: 122.234.55.192, Dst: 172.31.36.243
Transmission Control Protocol, Src Port: 35568, Dst Port: 9003, Seq: 289, Ack: 1121, Len: 41
WebSocket
    1... .... = Fin: True
    .000 .... = Reserved: 0x0
    .... 0001 = Opcode: Text (1)
    1... .... = Mask: True
    .010 0011 = Payload length: 35
    Masking-Key: dce7ee72
    Masked payload
    Payload
Line-based text data (1 lines)
```
可以看到有 `Masking-Key` 和 `Masked payload`，可见客户端发送给服务端的内容经过异或运算。

### 关于掩码 (Mask)

如果是客户端发送到服务端的数据包，我们需要使用掩码对 `payload` 的每一个字节进行异或运算，生成 `masked payload` 才能被服务器读取。
具体的运算其实很简单。

假设 `payload` 长度为 `pLen`，`mask-key` 长度为 `mLen`，`i` 作为 payload 的游标，`j` 作为 mask-key 的游标，伪代码如下：
```js
for (i = 0; i < pLen; i++){
    maskedPayload[j] = payload[i] ^ maskKey[i % mLen];
}
```
解码
```js
for (i = 0; i < pLen; i++){
    payload[j] = maskedPayload[i] ^ maskKey[i % mLen];
}
```
### Payload 长度

Payload Length位占用了可选的7bit或者7 + 16bit 或者 7 + 64bit

读取负载数据，需要知道读到那里为止。因此获知负载数据长度很重要。这个过程稍微有点复杂，要以下这些步骤：

读取9-15位 (包括9和15位本身)，并转换为无符号整数。如果值小于或等于125，这个值就是长度；如果是 126，请转到步骤 2。如果它是 127，请转到步骤 3。
读取接下来的 16 位并转换为无符号整数，并作为长度。
读取接下来的 64 位并转换为无符号整数，并作为长度。
```php
  //判断是否是经过掩码处理的帧
  function isMasked($byte) {
    return (ord($byte) & 0x80) > 0;
  }：

  //获取负载的长度
  //首字节如果小于126，则长度为首字节的值
  //首字节如果等于126，则长度为后面紧跟的两个字节表示的字节数
  //如果首字节是127，则长度为后面紧跟8字节表示的字节数
  function getPayloadLen($data) {
    $first = ord($data[0]) & 0x7F;
    $second = (ord($data[1]) << 8) + ord($data[2]);
    $third = (ord($data[3]) << 40) + (ord($data[4]) << 32) + (ord($data[5]) << 24) + (ord($data[6]) << 16) + (ord($data[7]) << 8) + ord($data[8]);
    if ($first < 126) {
      return $first;
    } else if ($first === 126) {
      return $second;
    } else {
      return ($second << 48) + $third;
    }
  }

  //获取负载的内容，根据帧的结构
  //第0字节为结束标记及帧类型
  //第1字节是否掩码标识位及负载长度的首字节
  //根据协议要求，浏览器发送的数据必须经过掩码处理，所以偏移量至少是1字节的帧首字节+4字节的掩码长度
  //根据帧长度的不同，表示长度的字节数可能为1、3或9
  //所以根据不同的情况截取不同长度的数据即为负载内容
  function getPayload($data, $len) {
    $offset = 5;
    if ($len < 126) {
      return substr($data, $offset + 1, $len);
    } else if ($len < 65536) {
      return substr($data, $offset + 3, $len);
    } else {
      return substr($data, $offset + 9, $len);
    }
  }

  //获取掩码的值
  //仍然是根据不同的偏移位置截取
  function getMask($data, $len) {
    $offset = 1;
    if ($len < 126) {
      return substr($data, $offset + 1, 4);
    } else if ($len < 65536) {
      return substr($data, $offset + 3, 4);
    } else {
      return substr($data, $offset + 9, 4);
    }
  }

  //获取帧类型
  function getFrameType($byte) {
    return ord($byte) & 0x0F;
  }

  //判断是否为结束帧
  function isFin($byte) {
    return (ord($byte[0]) & 0x80) > 0;
  }

  //判断是否为控制帧，控制帧包含关闭帧、PING帧和PONG帧
  function isControlFrame($frameType) {
    return $frameType === self::FRAME_CLOSE || $frameType === self::FRAME_PING || $frameType === self::FRAME_PONG;
  }

  //处理负载的掩码，将其还原
  function parseRawFrame($payload, $mask) {
    $payloadLen = strlen($payload);
    $dest = '';
    $maskArr = array();
    for ($i = 0; $i < 4; $i++) {
      $maskArr[$i] = ord($mask[$i]);
    }
    for ($i = 0; $i < $payloadLen; $i++) {
      $dest .= chr(ord($payload[$i]) ^ $maskArr[$i % 4]);
    }
    return $dest;
  }

  function parseTextFrame($payload, $mask) {
    return $this->parseRawFrame($payload, $mask);
  }

  function parseBinaryFrame($payload, $mask) {
    return $this->parseRawFrame($payload, $mask);
  }

  //创建并发送关闭帧
  function closeFrame($socketId, $closeCode = 1000, $closeMsg = 'goodbye') {
    $closeCode = chr(intval($closeCode / 256)) . chr($closeCode % 256);
    $frame = $this->createFrame($closeCode . $closeMsg, self::FRAME_CLOSE);
    $this->socketSend($socketId, $frame);
    $this->disconnect($socketId);
  }

  function sendPing($socketId, $data = 'ping') {
    $frame = $this->createFrame($data, self::FRAME_PING);
    $this->socketSend($socketId, $frame);
  }

  function sendPong($socketId, $data = 'pong') {
    $frame = $this->createFrame($data, self::FRAME_PONG);
    $this->socketSend($socketId, $frame);
  }

  //封装帧头的相关标识位、长度等信息
  function createFrame($data, $type, $fin = 0x01) {
    $dataLen = strlen($data);
    $frame = chr(($fin << 7) + $type);
    if ($dataLen < 126) {
      $frame .= chr($dataLen);
    } else if ($dataLen < 65536) {
      $frame .= chr(126);
      $frame .= chr(intval($dataLen / 256));
      $frame .= chr(intval($dataLen % 256));
    } else {
      $frame .= chr(127);
      $hexLen = str_pad(base_convert($dataLen, 10, 16), 16, '0', STR_PAD_LEFT);
      for ($i = 0; $i < 15; $i += 2) {
        $frame .= chr((intval($hexLen[$i], 16) << 8) + intval($hexLen[$i + 1], 16));
      }
    }
    $frame .= $data;
    return $frame;
  }
```

## TIME_WAIT问题
linux系统下，TCP连接断开后，会以TIME_WAIT状态保留一定时间，然后才会释放端口。当并发请求过多的时候，就会产生大量的TIME_WAIT状态的连接。如果没有及时断开，会有大量的端口资源的服务器资源被占用。对此我们有必要调整下linux的TCP内核参数，让系统更快地释放TIME_WAIT连接。

统计TCP各种状态的数量
```shell
netstat -n | awk '/^tcp/ {++S[$NF]} END {for(a in S) print a, S[a]}'
```
编辑配置文件/etc/sysctl.conf，加入以下内容：
```conf
net.ipv4.tcp_syncookies= 1
net.ipv4.tcp_tw_reuse= 1
net.ipv4.tcp_tw_recycle= 1
net.ipv4.tcp_fin_timeout= 30
```
生效：
```shell
sysctl -p
```
如果编辑的文件在 /etc/sysctl.d/ 目录下, 需要改成使用以下命令
```shell
sysctl --system
```

## 高并发下端口配置优化
```conf
net.ipv4.tcp_keepalive_time= 1200
net.ipv4.ip_local_port_range= 1024 65535
net.ipv4.tcp_max_syn_backlog= 8192
net.ipv4.tcp_max_tw_buckets= 5000
···

## tcp_keepalive_time

- net.ipv4.tcp_keepalive_time：这个参数定义了TCP发送keepalive探测消息的间隔时间。默认值是7200秒（2小时）。

```shell
sysctl net.ipv4.tcp_keepalive_time
```

## tcp_fin_timeout

- net.ipv4.tcp_fin_timeout：这个参数定义了在服务器端关闭连接时，socket保持在FIN-WAIT-2状态的时间。默认值通常是60秒。

查看
```shell
sysctl net.ipv4.tcp_fin_timeout
```

## 参数说明
对于不同的linux发行版，默认值可能不一样。

net.core.somaxconn
一般情况下默认值是128，不同linux发行版可能会有区别。

该参数用于控制处于监听状态的套接字的最大连接队列长度，对于高并发的nginx服务器而言要注意调大该参数值，比如16384，32768。

net.core.xmem_default和net.core.xmem_max
参数	说明	默认值
net.core.rmem_default	系统范围接收数据的内核缓冲区初始大小	262144byte，即256KB
net.core.wmem_default	系统范围发送数据的内核缓冲区初始大小	262144byte，即256KB
net.core.rmem_max	系统范围接收数据的内核缓冲区最大大小	262144byte，即256KB
net.core.wmem_max	系统范围发送数据的内核缓冲区最大大小	262144byte，即256KB
默认值在不同的linux发行版可能会有所不同。

网络环境良好和内存资源充足的情况下，增大上述四个参数的值有助于提高并发能力，减少丢包和延迟。

网络环境较差或内存资源不足的情况下，可以考虑减小上述四个参数的值。

如果xmem_default的值大于xmem_max的值，将以xmem_max为准，且超出的部分内存将被浪费。

net.ipv4.ip_local_port_range
一般情况下默认值为32768 60999，表示本地端口范围为32768到60999，不同linux发行版可能会有所不同。

常见优化配置：net.ipv4.ip_local_port_range = 1024 65535

通过将本地端口号限制在指定的范围内，可以避免与系统或其它应用程序使用的端口号发生冲突。如果服务器上还运行了后端应用程序，注意要错开后端服务的端口号。

net.ipv4.tcp_fastopen
该参数用于启用或禁用 TCP 的快速打开（TCP Fast Open）功能。TCP 快速打开是一种优化的 TCP 握手过程，旨在减少客户端与服务器之间的往返延迟时间，从而加速连接的建立。传统的 TCP 握手过程需要三次往返（3-way handshake）才能建立连接。而 TCP 快速打开通过在初始 SYN 数据包中携带客户端发送的应用层数据，使服务器可以在接收到 SYN 数据包后直接发送 SYN+ACK 数据包，从而减少了一个往返的延迟。

net.ipv4.tcp_fastopen 参数有以下几个取值：

0：表示禁用 TCP 快速打开功能。
1：表示启用 TCP 快速打开功能。
2：表示启用 TCP 快速打开功能，并允许客户端在第一次握手时发送数据包。
需要注意的是，启用 TCP 快速打开功能需要支持该功能的客户端和服务器。如果客户端或服务器不支持 TCP 快速打开，即使在内核中启用了该功能，TCP 连接仍然会回退到传统的三次握手过程。对于linux服务器，内核版本应高于3.7。

net.ipv4.tcp_fin_timeout
一般情况下默认值为60，单位秒，不同linux发行版可能会有所不同。

用于控制TCP/IP协议栈中的FIN-WAIT-2状态的超时时间。

在TCP协议中，当一段的连接主动关闭后，会进入FIN-WAIT-2状态，等待对方的确认，以确保双方都完成了连接关闭。当FIN-WAIT-2状态持续超过该参数值是，连接会被内核强制关闭，这对于释放系统资源，提高连接处理能力非常重要。

较小的参数值可以更快地释放系统资源，但可能导致一些连接在网络不稳定的情况下被错误地关闭。

net.ipv4.tcp_keepalive_time
一般情况下默认值为7200，单位秒，不同linux发行版可能会有所不同。

该参数用于控制TCP/IP协议栈中的 TCP keepalive 检测时间间隔。TCP keepalive是一种机制，用于检测处于空闲状态的连接是否仍然有效。当一段时间内没有数据传输时，TCP Keepalive会发送一些特定的探测报文到对方，以确认连接的状态。这对于检测死连接、清理空闲连接和提高连接可靠性很重要。

如果该参数值默认2小时，如果修改为很小的值，将会带来频繁的keepalive检测，这会增加网络流量和系统负载，不必要的连接也可能被中断。同时也会增加系统安全问题，攻击者可以利用Keepalive探测报文进行DoS攻击或网络扫描。

net.ipv4.tcp_max_tw_buckets
不同linux发行版可能会有所不同，可能是65536或180000。

该参数用于控制 TIME_WAIT 状态的 TCP 连接的最大数量。当TIME_WAIT数超过该参数值，新的连接请求可能会被丢弃或拒绝。

较小的值会加快清理TIME_WAIT，但可能会有连接异常。一般情况下默认即可，根据实际情况可以考虑减少或增多。

net.ipv4.tcp_max_syn_backlog
一般情况下默认值为1024，不同linux发行版可能会有所不同。

该参数用于控制TCP/IP协议栈中SYN队列的最大长度。在 TCP 握手过程中，当客户端发送 SYN 报文请求建立连接时，服务器端会将这些 SYN 请求放入 SYN 队列中等待处理。net.ipv4.tcp_max_syn_backlog 参数指定了 SYN 队列的最大长度，即能够同时等待处理的 SYN 请求的最大数量。较小的 net.ipv4.tcp_max_syn_backlog 值可能会导致 SYN 队列溢出，从而无法处理所有的连接请求。这可能会导致客户端无法成功建立连接，出现连接超时或连接被拒绝的情况。

net.ipv4.tcp_syncookies
一般情况下默认为0，表示关闭，不同linux发行版可能会有所不同。置为1表示开启。

表示开启SYNCookies。当出现SYN等待队列溢出时，启用cookies来处理，可防范少量SYN攻击。

当系统遭受SYN Flood攻击时，攻击者会发送大量的TCP SYN请求，消耗服务器资源并导致服务不可用。

启用SYN Cookie机制后，当服务器接收到一个新的 TCP SYN 请求时，会根据该请求生成一个 SYN Cookie，并将 SYN Cookie 发送回给客户端。客户端在后续的请求中需要携带该 SYN Cookie。服务器在收到后续请求时，会验证 SYN Cookie 的合法性，并根据其中的信息还原出原始的 SYN 请求。

通过使用 SYN Cookie 机制，服务器可以在不消耗太多资源的情况下抵御 SYN Flood 攻击，确保系统的稳定性和可用性。

启用 SYN Cookie 机制也可能带来一些问题，一些网络设备可能无法正确处理SYN Cookie的连接请求，导致连接无法建立或其它问题。

net.ipv4.tcp_synack_retries
一般情况下默认为5，不同linux发行版可能会有所不同。

该参数用于设置在连接建立过程中，发送 SYN-ACK（同步应答）包后等待客户端 ACK（确认应答）包的最大重试次数。

在 TCP 连接的三次握手过程中，服务器收到客户端的 SYN（同步）包后，会回复一个 SYN-ACK 包作为应答。然后服务器等待客户端发送 ACK 包来确认连接的建立。如果服务器在等待期间未收到 ACK 包，它将重试发送 SYN-ACK 包，重试次数由 net.ipv4.tcp_synack_retries 参数确定。

网络环境糟糕的情况下可以考虑增加参数值，以允许更多的重试次数，增加连接建立的成功率。

减小参数值有助于快速建立连接和减少资源占用。

net.ipv4.tcp_syn_retries
一般情况下默认为6，不同linux发行版可能会有所不同。

该参数用于设置在连接建立过程中，发送 SYN（同步）包后等待对方响应的最大重试次数。当客户端发送 SYN 包后，如果没有收到服务器的 SYN-ACK（同步应答）包，客户端会重试发送 SYN 包，重试次数由 net.ipv4.tcp_syn_retries 参数确定。

net.ipv4.tcp_timestamps
一般情况下默认为1，表示开启，不同linux发行版可能会有所不同。置为0表示关闭。

启用后允许在TCP报文中添加时间戳信息，用于测量报文的往返时间（RTT）和计算报文的时序。

net.ipv4.tcp_tw_reuse
一般情况下默认为0，表示关闭，不同linux发行版可能会有所不同。置为1表示开启。

允许重用TIME_WAIT Socket，也就是可以重用TIME_WAIT占用的端口。

启用net.ipv4.tcp_tw_reuse也可能带来一些问题。例如，如果处于TIME_WAIT连接上仍然存在未完全处理的数据包，重用该端口可能导致数据包被传递到错误的连接上，从而导致数据错乱或安全问题。

net.ipv4.tcp_tw_recycle
一般情况下默认为0，表示关闭，不同linux发行版可能会有所不同。置为1表示开启。

启用快速回收TIME_WAIT Socket，内核根据一定规则释放TIME_WAIT的端口资源。具体的回收规则可以根据net.ipv4.tcp_timestamps参数和其它相关参数进行调整。

当多个客户端位于同一个NAT网络后面时，启用快速回收可能导致来自不同客户端的连接被错误服用，导致数据错乱或安全问题。

net.ipv4.tcp_rmem和net.ipv4.tcp_wmem
用于设置tcp接收缓冲区和发送缓冲区的大小，有三个值组成，分别是最小值、默认值和最大值。类似于net.core.xmem_default和net.core.xmem_max。不过net.core是系统全局参数，适用于所有类型的socket，包括tcp和udp。
