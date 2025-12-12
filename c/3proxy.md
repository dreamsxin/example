# 3proxy

- https://github.com/3proxy/3proxy

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
