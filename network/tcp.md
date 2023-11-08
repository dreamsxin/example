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
