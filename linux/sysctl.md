## 1. **内核网络参数（sysctl）**

### 查看参数
```bash
# 查看所有参数
sysctl -a

# 查看特定参数
sysctl net.ipv4.tcp_low_latency

# 查看IP转发状态
sysctl net.ipv4.ip_forward

# 查看TCP相关参数
sysctl net.ipv4.tcp_*
```

### 临时修改（重启失效）
```bash
# 启用TCP低延迟
sudo sysctl -w net.ipv4.tcp_low_latency=1

# 启用IP转发
sudo sysctl -w net.ipv4.ip_forward=1

# 调整TCP缓冲区大小
sudo sysctl -w net.core.rmem_max=16777216
sudo sysctl -w net.core.wmem_max=16777216
```

### 永久修改
```bash
# 编辑配置文件
sudo nano /etc/sysctl.conf

# 添加或修改参数
net.ipv4.tcp_low_latency = 1
net.ipv4.ip_forward = 1

# 立即生效
sudo sysctl -p
```

## 2. **网卡配置**

### 查看网卡信息
```bash
# 查看所有网卡
ip link show

# 查看IP地址
ip addr show
# 或
ifconfig

# 查看特定网卡
ip addr show eth0
```

### 配置网卡
```bash
# 临时配置IP（重启失效）
sudo ip addr add 192.168.1.100/24 dev eth0

# 启用/禁用网卡
sudo ip link set eth0 up
sudo ip link set eth0 down

# 修改MAC地址
sudo ip link set eth0 address 00:11:22:33:44:55
```

## 3. **路由设置**

### 查看路由表
```bash
# 查看路由表
ip route show
# 或
route -n
```

### 修改路由
```bash
# 添加默认网关
sudo ip route add default via 192.168.1.1

# 添加静态路由
sudo ip route add 10.0.0.0/8 via 192.168.1.254

# 删除路由
sudo ip route del 10.0.0.0/8

# 清空路由表
sudo ip route flush cache
```

## 4. **DNS配置**

### 查看DNS
```bash
# 查看当前DNS
cat /etc/resolv.conf

# 使用nslookup查看
nslookup google.com

# 使用dig查看
dig google.com
```

### 修改DNS
```bash
# 临时修改
sudo echo "nameserver 8.8.8.8" > /etc/resolv.conf

# 永久修改（编辑配置文件）
sudo nano /etc/resolv.conf
# 添加
nameserver 8.8.8.8
nameserver 8.8.4.4

# 对于systemd系统
sudo nano /etc/systemd/resolved.conf
```

## 5. **网络连接状态**

### 查看连接状态
```bash
# 查看所有连接
ss -tulnp
# 或
netstat -tulnp

# 查看TCP连接状态统计
ss -s

# 查看路由缓存
ip route show cache
```

## 6. **常用网络调优参数**

```bash
# TCP优化
sudo sysctl -w net.ipv4.tcp_slow_start_after_idle=0
sudo sysctl -w net.ipv4.tcp_notsent_lowat=16384
sudo sysctl -w net.ipv4.tcp_fastopen=3

# 连接数限制调整
sudo sysctl -w net.core.somaxconn=65535
sudo sysctl -w net.ipv4.tcp_max_syn_backlog=65536

# 时间戳和窗口缩放
sudo sysctl -w net.ipv4.tcp_timestamps=1
sudo sysctl -w net.ipv4.tcp_window_scaling=1
```

## 7. **配置文件位置**

| 配置类型 | 配置文件路径 |
|----------|--------------|
| 永久IP配置 | `/etc/network/interfaces` (Debian/Ubuntu) |
| 永久IP配置 | `/etc/sysconfig/network-scripts/ifcfg-eth0` (RHEL/CentOS) |
| DNS配置 | `/etc/resolv.conf` |
| 主机名 | `/etc/hostname`, `/etc/hosts` |
| 内核参数 | `/etc/sysctl.conf`, `/etc/sysctl.d/*.conf` |

## 8. **实用脚本示例**

```bash
#!/bin/bash
# 网络优化脚本

# 启用IP转发
echo "Enabling IP forwarding..."
sudo sysctl -w net.ipv4.ip_forward=1

# TCP优化
echo "Optimizing TCP settings..."
sudo sysctl -w net.ipv4.tcp_syncookies=1
sudo sysctl -w net.ipv4.tcp_max_syn_backlog=65536
sudo sysctl -w net.core.somaxconn=65535

# 保存到配置文件
echo "Saving to /etc/sysctl.d/99-network.conf"
cat << EOF | sudo tee /etc/sysctl.d/99-network.conf
net.ipv4.ip_forward = 1
net.ipv4.tcp_syncookies = 1
net.ipv4.tcp_max_syn_backlog = 65536
net.core.somaxconn = 65535
EOF

# 应用配置
sudo sysctl -p /etc/sysctl.d/99-network.conf
```

## 注意事项：
1. **临时修改**：使用 `sysctl -w` 或 `ip` 命令立即生效，但重启后失效
2. **永久修改**：需要编辑配置文件并执行 `sysctl -p` 或重启网络服务
3. **谨慎操作**：某些参数修改可能导致网络中断，建议在测试环境先验证
4. **系统差异**：不同Linux发行版的配置文件位置可能不同

这些命令涵盖了大部分网络参数的查看和修改需求，根据具体场景选择使用。
