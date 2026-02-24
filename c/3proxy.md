# 3proxy

- https://github.com/3proxy/3proxy
- https://3proxy.org/doc/man3/3proxy.cfg.3.html

```conf
daemon
nserver 8.8.8.8
nscache 65536
timeouts 1 5 30 60 180 1800 15 60
log /var/log/3proxy/3proxy_24.log D
logformat "-L%t.%. %N.%p %E %U %C:%c %R:%r %O %I %h %T"
users 3APA3A:CL:3apa3a "test:CR:$1$qwer$CHFTUFGqkjue9HyhcMHEe1"
allow 3APA3A,test
auth strong
maxconn 1000
backlog 512
flush
```

```conf
#!/usr/local/bin/3proxy
# Yes, 3proxy.cfg can be executable, in this case you should place
# something like
#config /usr/local/3proxy/3proxy.cfg
# to show which configuration 3proxy should re-read on realod.

#system "echo Hello world!"
# you may use system to execute some external command if proxy starts

# We can configure nservers to avoid unsafe gethostbyname() usage
nserver 10.1.2.1
nserver 10.2.2.2
# nscache is good to save speed, traffic and bandwidth
nscache 65536

#nsrecord porno.security.nnov.ru 0.0.0.0
# nobody will be able to access porno.security.nnov.ru by the name.
#nsrecord wpad.security.nnov.ru www.security.nnov.ru
# wpad.security.nnov.ru will resolve to www.security.nnov.ru for
# clients


timeouts 1 5 30 60 180 1800 15 60
# Here we can change timeout values

users 3APA3A:CL:3apa3a "test:CR:$1$qwer$CHFTUFGqkjue9HyhcMHEe1"
# note that "" required, overvise $... is treated as include file name.
# $1$qwer$CHFTUFGqkjue9HyhcMHEe1 is 'test' in MD5 crypt format.
#users $/usr/local/etc/3proxy/passwd
# this example shows you how to include passwd file. For included files
# <CR> and <LF> are treated as field separators.

#daemon
# now we will not depend on any console (daemonize). daemon must be given
# before any significant command on *nix.

service
# service is required under NT if you want 3proxy to start as service

#log /var/log/3proxy/log D
log c:\3proxy\logs\3proxy.log D
# log allows to specify log file location and rotation, D means logfile
# is created daily

#logformat "L%d-%m-%Y %H:%M:%S %z %N.%p %E %U %C:%c %R:%r %O %I %h %T"
#logformat "Linsert into log (l_date, l_user, l_service, l_in, l_out, l_descr) values ('%d-%m-%Y %H:%M:%S', '%U', '%N', %I, %O, '%T')"
#Compatible with Squid access.log:
#
#"- +_G%t.%. %D %C TCP_MISS/200 %I %1-1T %2-2T %U DIRECT/%R application/unknown"
#or, more compatible format without %D
#"- +_G%t.%.      1 %C TCP_MISS/200 %I %1-1T %2-2T %U DIRECT/%R application/unknown"
#
#Compatible with ISA 2000 proxy WEBEXTD.LOG (fields are TAB-delimited):
#
#"-	+ L%C	%U	Unknown	Y	%Y-%m-%d	%H:%M:%S	w3proxy	3PROXY	-	%n	%R	%r	%D	%O	%I	http	TCP	%1-1T	%2-2T	-	-	%E	-	-	-"
#
#Compatible with ISA 2004 proxy WEB.w3c
#
#"-	+ L%C	%U	Unknown	%Y-%m-%d	%H:%M:%S	3PROXY	-	%n	%R	%r	%D	%O	%I	http	%1-1T	%2-2T	-	%E	-	-	Internal	External	0x0	Allowed"
#
#Compatible with ISA 2000/2004 firewall FWSEXTD.log (fields are TAB-delimited):
#
#"-	+ L%C	%U	unnknown:0:0.0	N	%Y-%m-%d	%H:%M:%S	fwsrv	3PROXY	-	%n	%R	%r	%D	%O	%I	%r	TCP	Connect	-	-	-	%E	-	-	-	-	-"
#
#Compatible with HTTPD standard log (Apache and others)
#
#"-""+_L%C - %U [%d/%o/%Y:%H:%M:%S %z] ""%T"" %E %I"
#or more compatible without error code
#"-""+_L%C - %U [%d/%o/%Y:%H:%M:%S %z] ""%T"" 200 %I"

# in log file we want to have underscores instead of spaces
logformat "- +_L%t.%.  %N.%p %E %U %C:%c %R:%r %O %I %h %T"


#archiver gz /bin/gzip %F
#archiver zip zip -m -qq %A %F
#archiver zip pkzipc -add -silent -move %A %F
archiver rar rar a -df -inul %A %F
# if archiver specified log file will be compressed after closing.
# you should specify extension, path to archiver and command line, %A will be
# substituted with archive file name, %f - with original file name.
# Original file will not be removed, so archiver should care about it.

rotate 30
# We will keep last 30 log files

auth iponly
#auth nbname
#auth strong
# auth specifies type of user authentication. If you specify none proxy
# will not do anything to check name of the user. If you specify
# nbname proxy will send NetBIOS name request packet to UDP/137 of
# client and parse request for NetBIOS name of messanger service.
# Strong means that proxy will check password. For strong authentication
# unknown user will not be allowed to use proxy regardless of ACL.
# If you do not want username to be checked but wanna ACL to work you should
# specify auth iponly.


#allow ADMINISTRATOR,root
#allow * 127.0.0.1,192.168.1.1 * *
#parent 1000 http 192.168.1.2 80 * * * 80
#allow * 192.168.1.0/24 * 25,53,110,20-21,1024-65535
# we will allow everything if username matches ADMINISTRATOR or root or
# client ip is 127.0.0.1 or 192.168.1.1. Overwise we will redirect any request
# to port 80 to our Web-server 192.168.0.2.
# We will allow any outgoing connections from network 192.168.1.0/24 to
# SMTP, POP3, FTP, DNS and unprivileged ports.
# Note, that redirect may also be used with proxy or portmapper. It will
# allow you to redirect requests to different ports or different server
# for different clients.

#  sharing access to internet

external 10.1.1.1
# external is address 3proxy uses for outgoing connections. 0.0.0.0 means any
# interface. Using 0.0.0.0 is not good because it allows to connect to 127.0.0.1

internal 192.168.1.1
# internal is address of interface proxy will listen for incoming requests
# 127.0.0.1 means only localhost will be able to use this proxy. This is
# address you should specify for clients as proxy IP.
# You MAY use 0.0.0.0 but you shouldn't, because it's a chance for you to
# have open proxy in your network in this case.

auth none
# no authentication is requires

dnspr

# dnsproxy listens on UDP/53 to answer client's DNS requests. It requires
# nserver/nscache configuration.


#external $./external.ip
#internal $./internal.ip
# this is just an alternative form fo giving external and internal address
# allows you to read this addresses from files

auth strong
# We want to protect internal interface
deny * * 127.0.0.1,192.168.1.1
# and llow HTTP and HTTPS traffic.
allow * * * 80-88,8080-8088 HTTP
allow * * * 443,8443 HTTPS
proxy -n

auth none
# pop3p will be used without any authentication. It's bad choice
# because it's possible to use pop3p to access any port
pop3p

tcppm 25 mail.my.provider 25
#udppm -s 53 ns.my.provider 53
# we can portmap port TCP/25 to provider's SMTP server and UDP/53
# to provider's DNS.
# Now we can use our proxy as SMTP and DNS server.
# -s switch for UDP means "single packet" service - instead of setting
# association for period of time association will only be set for 1 packet.
# It's very userfull for services like DNS but not for some massive services
# like multimedia streams or online games.

auth strong
flush
allow 3APA3A,test
maxconn 20
socks
# for socks we will use password authentication and different access control -
# we flush previously configured ACL list and create new one to allow users
# test and 3APA3A to connect from any location


auth strong
flush
internal 127.0.0.1
allow 3APA3A 127.0.0.1
maxconn 3
admin
#only allow acces to admin interface for user 3APA3A from 127.0.0.1 address
#via 127.0.0.1 address.

# map external 80 and 443 ports to internal Web server
# examples below show how to use 3proxy to publish Web server in internal
# network to Internet. We must switch internal and external addresses and
# flush any ACLs

#auth none
#flush
#external $./internal.ip
#internal $./external.ip
#maxconn 300
#tcppm 80 websrv 80
#tcppm 443 websrv 443


#chroot /usr/local/jail
#setgid 65535
#setuid 65535
# now we needn't any root rights. We can chroot and setgid/setuid.
```

## 低延时专用配置示例

```bash
#!/usr/local/bin/3proxy
# 极低延时优化配置
setuid 65534
setgid 65534

# DNS优化
nserver 8.8.8.8
nserver 1.1.1.1
nscache 131072  # 大缓存
dnspr -p53 -t1  # DNS代理，1秒超时

# 超时优化（游戏/实时应用）
timeouts 1 1 5 10 15 30 5 15
# connect: 1秒
# auth: 1秒
# client: 5秒
# server: 10秒
# pconnect: 15秒

# 连接优化
maxconn 10000
nofiles 100000
socketpolicy nodelay notdefer quickack

# 缓冲区优化
buffersize 65536    # 缓冲区大小
allocrate 32        # 快速内存分配

# HTTP/SOCKS代理（低延时模式）
auth iponly         # IP认证更快
internal 0.0.0.0:8080
allow * 192.168.1.0/24 * *
proxy -p8080 -n     # -n: 不验证请求格式

internal 0.0.0.0:1080
socks -p1080 -i     # -i: 忽略身份验证请求

# UDP优化（游戏语音）
udppm -p27015 192.168.1.200 27015  # CS:GO语音
udppm -p9987 192.168.1.201 9987    # TeamSpeak
```

## 3. 所有参数详细说明

### 基础参数
- **setuid/setgid** - 设置运行用户/组（安全必需）
- **chroot** - 改变根目录，增强安全性
- **daemon/service** - 守护进程/服务模式
- **pidfile** - PID文件路径

### DNS参数
- **nserver** - DNS服务器地址
- **nscache** - DNS缓存大小(KB)
- **nscache6** - IPv6 DNS缓存
- **dnspr** - DNS代理服务
  - `-p` - 端口
  - `-t` - 超时
  - `-e` - 仅外部查询

### 超时参数 (timeouts)
```
timeouts connect auth client server pconnect anon udp udpdata
```
- **connect** - TCP连接建立超时
- **auth** - 认证过程超时
- **client** - 客户端空闲超时
- **server** - 服务器端空闲超时
- **pconnect** - 持久连接超时
- **anon** - 匿名会话超时
- **udp** - UDP会话超时
- **udpdata** - UDP数据包超时

### 认证参数
- **auth** - 认证类型：none/iponly/strong
- **users** - 用户定义
  - 格式：`用户名:认证类型:密码`
  - 认证类型：CL(明文)/CR(crypt)/MD5
- **allow/deny** - 访问控制
  - 格式：`allow 用户 IP 端口 协议 时间`

### 代理服务参数

#### HTTP代理 (proxy)
- `-p` - 监听端口
- `-a` - 匿名模式
- `-n` - 不验证请求格式
- `-i` - 内部代理
- `-e` - 外部地址

#### SOCKS代理 (socks)
- `-p` - 监听端口
- `-i` - 忽略身份验证
- `-e` - 外部地址
- `-s4` - SOCKS4协议
- `-s5` - SOCKS5协议（默认）

#### 端口映射
- **tcppm** - TCP端口映射
  - 格式：`tcppm 本地端口 目标地址 目标端口`
- **udppm** - UDP端口映射
  - 格式：`udppm 本地端口 目标地址 目标端口`
  - `-s` - 单包模式

### 性能参数
- **maxconn** - 最大并发连接数
- **nofiles** - 文件描述符限制
- **stacksize** - 线程栈大小(KB)
- **bandlim** - 带宽限制(KB/s)
- **socketpolicy** - Socket策略
  - nodelay - 禁用Nagle算法
  - notdefer - 不延迟ACK
  - quickack - 快速ACK

### 日志参数
- **log** - 日志文件
  - 后缀：D(天)/M(月)/H(小时)
- **logformat** - 日志格式
- **rotate** - 日志轮转数量
- **archiver** - 日志压缩器

### 监控参数
- **admin** - 管理接口
- **stat** - 统计接口
- **monitor** - 监控配置
- **counter** - 计数器文件

## 4. 启动和管理

```bash
# 测试配置文件
3proxy --test /etc/3proxy/3proxy.cfg

# 启动服务
3proxy /etc/3proxy/3proxy.cfg

# 重新加载配置
killall -HUP 3proxy

# 查看状态
netstat -tlnp | grep 3proxy

# 测试代理
curl --proxy http://user:pass@localhost:8080 https://example.com
```

## 5. 安全建议

1. **永远使用强认证** (auth strong)
2. **限制访问IP范围**
3. **设置合理的连接限制**
4. **定期更新密码**
5. **启用日志审计**
6. **使用chroot隔离**
7. **避免使用root运行**

## CONNECTION_L

在 `proxychild` 函数（HTTP/HTTPS/FTP 代理的核心处理逻辑）中，`CONNECTION_L` 主要作用于特定场景：

### 1. **CONNECT 隧道（HTTPS 代理）**
当客户端发送 `CONNECT` 请求建立隧道时，代理会调用 `mapsocket` 在客户端和目标服务器之间双向转发原始数据。此处使用了 `CONNECTION_L`：
```c
if(isconnect && param->redirtype != R_HTTP) {
    // ... 
    param->res = mapsocket(param, conf.timeouts[CONNECTION_L]);
}
```
这意味着 `CONNECTION_L` 决定了 HTTPS 隧道建立后，在没有数据交换时连接能够保持的最大空闲时间。

### 2. **FTP over HTTP 代理**
当代理处理 FTP 协议（通过 HTTP 代理访问 FTP 资源）时，数据传输阶段同样使用了 `CONNECTION_L`：
```c
if(!mode){
    // ... 文件传输
    res = mapsocket(param, conf.timeouts[CONNECTION_L]);
}
```
这影响 FTP 数据连接的空闲超时。

### **长连接 vs 短连接**
- 对于普通的 HTTP 请求（GET/POST）且使用 **Keep-Alive** 时，`mapsocket` 调用的是 `CONNECTION_S`（短连接超时）：
  ```c
  if(!hascontent && !param->chunked) {
      RETURN(mapsocket(param, conf.timeouts[CONNECTION_S]));
  }
  ```
  因此，`CONNECTION_L` 并不直接影响普通 HTTP 请求的持久连接空闲时间。

- 但在某些情况下（如 `isconnect` 成功后的隧道），`CONNECTION_L` 会覆盖 `CONNECTION_S` 成为隧道连接的空闲超时。

### 总结
- **`CONNECTION_L` 对 HTTP 代理的影响范围**：主要影响 **CONNECT 方法建立的隧道** 和 **FTP over HTTP 的数据连接**，这些属于长连接场景。
- **普通 HTTP 请求（GET/POST）** 的 Keep-Alive 空闲超时由 `CONNECTION_S` 控制。
- 两者共同决定了 HTTP 代理在不同操作下的超时行为，调整 `CONNECTION_L` 会改变 HTTPS 代理和 FTP 代理的会话保持时间。

在 `proxychild` 函数中，`redirtype` 并没有被直接赋值，它是在进入 `proxychild` 之前或通过过滤器函数间接设置的。对于 HTTPS CONNECT 隧道，`redirtype` 的判断和设置主要发生在以下阶段：

### **请求解析后的过滤器处理**
在 `proxychild` 读取并解析完 HTTP 请求头后，会调用两个关键的过滤器函数：
```c
action = handlereqfilters(param, &req, &reqbufsize, 0, &reqsize);
action = handlehdrfilterscli(param, &buf, &bufsize, 0, &inbuf);
```
这些过滤器（包括 parent 规则处理）会根据配置的 `parent` 指令、访问控制列表等条件，决定是否将请求重定向到父代理。如果匹配了需要父代理转发的规则，过滤器内部会将 `param->redirtype` 设置为 `R_HTTP`（或其他重定向类型），同时可能设置 `param->redirectfunc` 用于后续处理。

### **CONNECT 请求的处理分支**
在 `isconnect` 分支中，代码会根据 `redirtype` 决定行为：
```c
if(isconnect && param->redirtype != R_HTTP) {
    // 直接建立隧道：发送 200 Connection established，然后 mapsocket
    socksend(param, param->clisock, (unsigned char *)proxy_stringtable[8], ...);
    param->res = mapsocket(param, conf.timeouts[CONNECTION_L]);
} else {
    // 如果 redirtype == R_HTTP，则不会执行上述代码，请求将转发给父代理
}
```
- 如果 `redirtype != R_HTTP`（默认情况），则立即向客户端回复 `200 Connection established`，并调用 `mapsocket` 在客户端和目标服务器之间建立双向数据转发隧道。
- 如果 `redirtype == R_HTTP`，说明该 CONNECT 请求需要经由 HTTP 父代理处理，此时不会发送成功响应，也不会建立本地隧道，而是将请求转发给父代理（后续可能由 `redirectfunc` 或继续发送请求头完成）。

### **parent 规则的触发**
`redirtype` 的赋值通常由 `parent` 指令触发。例如配置：
```
parent 1000 http 192.168.1.100 3128
```
当 CONNECT 请求的目标匹配该 parent 规则时，过滤器会设置 `redirtype = R_HTTP`，使得请求被转发到指定的父代理，而不是直接连接目标服务器。

### 总结
- **设置位置**：`redirtype` 在 `handlereqfilters` 和 `handlehdrfilterscli` 等过滤器函数中被赋值（具体实现在 3proxy 核心的过滤/路由模块，未在给出的代码段中）。
- **判断时机**：`proxychild` 中通过 `if(isconnect && param->redirtype != R_HTTP)` 来判断是否直接建立隧道。
- **对 HTTPS CONNECT 的影响**：该判断决定了 CONNECT 请求是本地直连还是通过父代理转发。
