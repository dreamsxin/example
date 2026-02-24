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

## 1. 完整配置示例

```bash
#!/usr/local/bin/3proxy
# ========== 基础配置 ==========
# 设置系统用户和权限
setuid 65534    # 运行用户 (nobody)
setgid 65534    # 运行组 (nogroup)
chroot /var/empty  # 安全隔离目录
# daemon        # 守护进程模式 (Linux)
service        # 服务模式 (Windows)

# ========== 低延时优化 ==========
nserver 8.8.8.8
nserver 8.8.4.4
nserver 1.1.1.1
nscache 65536   # DNS缓存大小，单位KB

# 连接超时设置（低延时优化）
timeouts 1 2 10 20 30 60 10 30
# 参数说明：connect auth client server pconnect anon udp udp
# connect: 连接建立超时(1秒)
# auth: 认证超时(2秒) 
# client: 客户端空闲超时(10秒)
# server: 服务器空闲超时(20秒)
# pconnect: 持久连接超时(30秒)
# anon: 匿名会话超时(60秒)
# udp: UDP会话超时(10秒)
# udp: UDP数据包超时(30秒)

# 性能调优
maxconn 5000     # 最大连接数
nofiles 65536    # 文件描述符限制
stacksize 65536  # 栈大小(KB)
bandlim 1024000  # 带宽限制(KB/s)，0为无限制

# ========== 日志配置 ==========
log /var/log/3proxy/3proxy.log D
logformat "L%t.%p %E %U %C:%c %R:%r %O %I %h %T %N"
rotate 30        # 保留30个日志文件
# logformat 变量说明：
# %t - 时间戳
# %p - 进程ID
# %E - 错误码
# %U - 用户名
# %C - 客户端地址
# %c - 客户端端口
# %R - 请求地址
# %r - 请求端口
# %O - 发送字节数
# %I - 接收字节数
# %h - 协议类型
# %T - 请求内容
# %N - 服务名称

# ========== 用户认证配置 ==========
# 认证方式选择（三选一）：
# auth none       # 无需认证（不推荐）
# auth iponly     # IP认证
# auth strong     # 强密码认证（推荐）
auth strong

# 用户定义（多种格式）：
# 1. 明文密码（不推荐）：
# users username:CL:password

# 2. crypt加密密码（推荐）：
# 生成密码：openssl passwd -crypt password
users proxyuser:CL:yW2H5cJX8eLQ6

# 3. MD5加密密码：
# 生成密码：openssl passwd -1 password
users md5user:CL:$1$salt$hashed_password

# 4. 从文件读取用户：
# users $/path/to/passwd
# passwd文件格式：username:CL:password

# 5. IP段用户（IP认证时使用）：
allow * 192.168.1.0/24 * *  # 允许整个网段

# ========== 网络接口配置 ==========
# 外部地址（出口IP）
external 1.2.3.4

# 内部地址（监听IP）
# 可配置多个监听地址：
internal 0.0.0.0    # 监听所有接口
# internal 192.168.1.100  # 监听特定IP
# internal eth0      # 监听特定网卡
# internal 0.0.0.0:3128  # 指定端口

# ========== HTTP代理配置 ==========
# 基础HTTP代理
proxy -p3128 -a     # -p端口，-a启用匿名
allow * * * 80,443,8080  # 允许访问的端口
deny * * * 25        # 禁止访问的端口

# 带认证的HTTP代理
auth strong
flush               # 清除之前的ACL规则
allow proxyuser     # 允许指定用户
proxy -p8080

# HTTP CONNECT代理（用于HTTPS）
allow * * * 443     # 允许HTTPS连接
proxy -p8443

# 父级代理（链式代理）
# parent 1000 http 10.0.0.1 3128 proxyuser password
# 参数：权重 协议 地址 端口 用户名 密码

# ========== SOCKS5代理配置 ==========
# 基础SOCKS5代理
socks -p1080
allow * * * 1-65535  # SOCKS允许所有端口

# 带认证的SOCKS5代理
auth strong
flush
allow proxyuser
socks -p1081

# SOCKS4/SOCKS4a代理
socks -p1082 -s4    # -s4指定SOCKS4协议

# ========== 高级功能配置 ==========
# DNS代理
dnspr -p53          # DNS代理端口

# POP3代理
pop3p -p110

# FTP代理
ftppr -p2121

# 端口映射
# TCP端口映射
tcppm 2222 192.168.1.100 22   # 将2222映射到内网SSH
tcppm 3389 192.168.1.101 3389 # RDP映射

# UDP端口映射
udppm 5353 8.8.8.8 53        # DNS映射

# ========== 访问控制列表(ACL) ==========
# 基于时间的访问控制
allow * * * * 08:00-18:00    # 工作时间允许
deny * * * * 00:00-08:00     # 夜间禁止

# 基于URL的过滤
filter /etc/3proxy/filter     # 过滤规则文件
filter regexp "(porno|adult)" # 正则表达式过滤

# 带宽控制
bandlim 512000 proxyuser      # 用户带宽限制
bandlim 1024000 * 192.168.1.* # IP段带宽限制

# 连接数限制
maxconn 100 proxyuser         # 用户最大连接数
maxconn 50 * 192.168.1.100    # IP最大连接数

# ========== 安全配置 ==========
# 防止滥用
deny * 10.0.0.0/8 * 25        # 禁止内部发邮件
deny * * 192.168.1.100 *      # 禁止访问特定主机

# 监控接口（仅本地访问）
auth strong
internal 127.0.0.1
allow adminuser 127.0.0.1
admin -p8089                  # 管理端口
stat -p8090                   # 统计端口

# 防止DDoS
monitor /etc/3proxy/monitor   # 监控配置
deny * * * * * 100/60         # 60秒内100次连接

# ========== 高级优化 ==========
# TCP优化
socketpolicy notdefer         # 不延迟ACK
socketpolicy nodelay          # 禁用Nagle算法

# 内存优化
allocrate 16                  # 内存分配率
allocalert 80                 # 内存警报阈值(%)

# 子进程管理
children 4                    # 子进程数
restart 3600                  # 重启间隔(秒)

# 插件支持
# plugin /path/to/plugin.so   # 加载插件
```

## 2. 低延时专用配置示例

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
