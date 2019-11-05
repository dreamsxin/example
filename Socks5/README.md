## SOCKS5 协议

SOCKS5 是 TCP/IP 层面的网络代理协议。
ss-server 端解密出来的数据就是采用 SOCKS5 协议封装的，通过 SOCKS5 协议 ss-server 端能读出本机软件想访问的服务的真正地址以及要传输的原数据


### 建立连接

客户端向服务端连接连接，客户端发送的数据包如下：

| VER  | NMETHODS | METHODS |
| ---- | -------- | ------- |
| 1    | 1        | 1       |

> `VER`：代表 SOCKS 的版本，SOCKS5 默认为`0x05`，其固定长度为1个字节；
> `NMETHODS`：表示第三个字段METHODS的长度，它的长度也是1个字节；
> `METHODS`：表示客户端支持的验证方式，可以有多种，他的长度是1-255个字节。

目前支持的验证方式共有：

- `0x00`：NO AUTHENTICATION REQUIRED（不需要验证）
- `0x01`：GSSAPI
- `0x02`：USERNAME/PASSWORD（用户名密码）
- `0x03`: to X'7F' IANA ASSIGNED
- `0x80`: to X'FE' RESERVED FOR PRIVATE METHODS
- `0xFF`: NO ACCEPTABLE METHODS（都不支持，没法连接了）


### 响应连接

服务端收到客户端的验证信息之后，就要回应客户端，服务端需要客户端提供哪种验证方式的信息。服务端回应的包格式如下：

| VER  | METHOD |
| - | - |
| 1    | 1      |

> `VER`：代表 SOCKS 的版本，SOCKS5 默认为`0x05`，其固定长度为1个字节；
>
> `METHOD`：代表服务端需要客户端按此验证方式提供的验证信息，其值长度为1个字节，可为上面六种验证方式之一


### 和目标服务建立连接

客户端发起的连接由服务端验证通过后，客户端下一步应该告诉真正目标服务的地址给服务器，服务器得到地址后再去请求真正的目标服务。也就是说客户端需要把 Google 服务的地址`google.com:80`告诉服务端，服务端再去请求`google.com:80`。
目标服务地址的格式为 (IP或域名)+端口，客户端需要发送的包格式如下：

| VER  | CMD  | RSV    | ATYP | DST.ADDR | DST.PORT |
| ---- | ---- | ------ | ---- | -------- | -------- |
| 1    | 1    | `0x00` | 1    | Variable | 2        |

> - `VER`：代表 SOCKS 协议的版本，SOCKS 默认为0x05，其值长度为1个字节；
> - CMD：代表客户端请求的类型，值长度也是1个字节，有三种类型；
>   - `CONNECT`： `0x01`；
>   - `BIND`： `0x02`；
>   - `UDP`： ASSOCIATE `0x03`；
> - `RSV`：保留字，值长度为1个字节；
> - ATYP：代表请求的远程服务器地址类型，值长度1个字节，有三种类型；
>   - `IPV4`： address: `0x01`；
>   - `DOMAINNAME`: `0x03`；
>   - `IPV6`： address: `0x04`；
> - `DST.ADDR`：代表远程服务器的地址，根据 `ATYP` 进行解析，值长度不定；
> - `DST.PORT`：代表远程服务器的端口，要访问哪个端口的意思，值长度2个字节

服务端在得到来自客户端告诉的目标服务地址后，便和目标服务进行连接，不管连接成功与否，服务器都应该把连接的结果告诉客户端。在连接成功的情况下，服务端返回的包格式如下：

| VER  | REP  | RSV    | ATYP | BND.ADDR | BND.PORT |
| ---- | ---- | ------ | ---- | -------- | -------- |
| 1    | 1    | `0x00` | 1    | Variable | 2        |

> - `VER`：代表 SOCKS 协议的版本，SOCKS 默认为0x05，其值长度为1个字节；
> - REP代表响应状态码，值长度也是1个字节，有以下几种类型
>   - `0x00` succeeded
>   - `0x01` general SOCKS server failure
>   - `0x02` connection not allowed by ruleset
>   - `0x03` Network unreachable
>   - `0x04` Host unreachable
>   - `0x05` Connection refused
>   - `0x06` TTL expired
>   - `0x07` Command not supported
>   - `0x08` Address type not supported
>   - `0x09` to `0xFF` unassigned
> - `RSV`：保留字，值长度为1个字节
> - ATYP：代表请求的远程服务器地址类型，值长度1个字节，有三种类型
>   - IP V4 address： `0x01`
>   - DOMAINNAME： `0x03`
>   - IP V6 address： `0x04`
> - `BND.ADDR`：表示绑定地址，值长度不定。
> - `BND.PORT`： 表示绑定端口，值长度2个字节


### 数据转发

客户端在收到来自服务器成功的响应后，就会开始发送数据了，服务端在收到来自客户端的数据后，会转发到目标服务

SOCKS5 协议的目的其实就是为了把来自原本应该在本机直接请求目标服务的流程，放到了服务端去代理客户端访问
