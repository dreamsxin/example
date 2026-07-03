# DNS解析

---

## 🧩 核心原理：让所有 DNS 解析“被迫”交给代理

关键在于组合使用两个 Chrome 参数：

```bash
--proxy-server="socks5://127.0.0.1:1080"   # 或 http://127.0.0.1:1080
--host-resolver-rules="MAP * ~NOTFOUND"
```

**工作流程**：
1. `--host-resolver-rules="MAP * ~NOTFOUND"` 强制 Chrome **所有域名解析都失败**（返回 `~NOTFOUND`）。
2. 由于本地解析失败，Chrome 在发起代理连接时，会**直接把原始域名**发送给代理服务器（SOCKS5 的 `CONNECT` 命令或 HTTP 的 `CONNECT` 请求中都会携带域名，而非 IP）。
3. 你本地运行的桥服务（监听 1080 端口）作为代理服务器，接收这些请求，**从中提取目标域名**。
4. 桥服务根据你定义的策略（本地缓存 / Hosts / 白名单 / 远端解析）自行完成 DNS 解析，得到 IP 后，再连接目标服务器，将数据转发回去。

这样，**所有 DNS 解析都发生在你的桥服务内部**，浏览器本身不做任何解析，彻底杜绝 DNS 泄露。

---

## 🛠️ 如何实现这个“本地桥服务”？

你需要实现一个**代理服务器**（支持 SOCKS5 或 HTTP 代理协议），并在处理连接时插入 DNS 解析逻辑。下面以 Python 为例，展示核心实现。

### 1. 选择代理协议：SOCKS5 更通用
推荐 SOCKS5，因为它支持域名传输，且不解析数据内容，性能更佳。你可以使用现成的 Python 库 `socksio`（解析 SOCKS5 协议）或 `asyncio` 手写。

### 2. 核心功能模块

- **代理协议解析**：接收客户端连接，读取 SOCKS5 握手，提取目标域名和端口。
- **DNS 解析策略**：
  - 检查本地 Hosts 文件（`/etc/hosts`）。
  - 检查内存缓存（TTL 可控）。
  - 检查白名单（如 `*.local`、`localhost`、内网 IP 段），若匹配则使用本地 DNS（如 223.5.5.5）。
  - 否则，通过上游代理（或直接请求远端 DNS 如 1.1.1.1）解析。
- **连接目标**：得到 IP 后，与目标服务器建立 TCP 连接，并转发浏览器和服务器之间的双向数据。

### 3. 简单示例（Python + `socksio`）

```python
import socket
import socksio  # pip install socksio
import threading
import time
import dns.resolver  # pip install dnspython

# 配置
LISTEN_HOST = "127.0.0.1"
LISTEN_PORT = 1080
LOCAL_DNS = "223.5.5.5"
REMOTE_DNS = "1.1.1.1"
SOCKS5_PROXY = ("127.0.0.1", 1081)  # 若需要通过代理转发远端 DNS，可设置

# 本地白名单
BYPASS = {".local", "localhost", "127.0.0.1"}

def resolve_locally(domain):
    """使用本地 DNS 解析"""
    try:
        resolver = dns.resolver.Resolver()
        resolver.nameservers = [LOCAL_DNS]
        answers = resolver.resolve(domain, 'A')
        return str(answers[0])
    except:
        return None

def resolve_remotely(domain):
    """通过上游 SOCKS5 代理进行远端解析（即让远端代理自己解析）"""
    # 这里可以用两种方式：
    # 方式1：直接通过代理连接远端 DNS 服务器（53端口），发送 DNS 查询
    # 方式2：使用系统 DNS 并信任代理转发（但这不是远端解析）
    # 更好的方式：让代理客户端去解析，但你的桥服务本身就是代理，所以可以自行实现
    # 这里简单起见，我们直接向 1.1.1.1 发送 DNS 查询（不走代理，如果要求走代理则需通过 SOCKS）
    # 若需走代理，则需使用 socks 库建立 SOCKS 连接再查询。
    try:
        resolver = dns.resolver.Resolver()
        resolver.nameservers = [REMOTE_DNS]
        # 如果 REMOTE_DNS 需要经过代理，则需通过 socks 库建立连接，这里不展开
        answers = resolver.resolve(domain, 'A')
        return str(answers[0])
    except:
        return None

def handle_client(client_sock):
    """处理一个 SOCKS5 连接"""
    # 1. 读取 SOCKS5 握手（省略细节，可参考 socksio 文档）
    # 2. 解析得到目标域名和端口
    # 3. 根据域名决定解析策略
    domain = "example.com"  # 实际从请求中提取
    port = 80
    if is_bypass(domain):
        ip = resolve_locally(domain)
    else:
        ip = resolve_remotely(domain)
    if not ip:
        client_sock.close()
        return
    # 4. 连接目标
    target_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    target_sock.connect((ip, port))
    # 5. 回复 SOCKS 连接成功，开始转发数据
    # 6. 双向转发（可用线程或 asyncio）
    # ...

def main():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((LISTEN_HOST, LISTEN_PORT))
    server.listen(5)
    print(f"本地桥服务已启动，监听 {LISTEN_HOST}:{LISTEN_PORT}")
    while True:
        client, _ = server.accept()
        threading.Thread(target=handle_client, args=(client,)).start()

if __name__ == "__main__":
    main()
```

### 4. 如何让 Chrome 的 Bypass 生效？

在桥内部实现 bypass 逻辑，只需在判断解析策略时，将匹配 `--proxy-bypasslist` 中规则的域名归为“本地解析”即可。你可以将 bypass 列表作为配置文件加载。

---

## ✅ 这个方案的优点

1. **完全掌控 DNS**：所有解析策略都由你定义，不受浏览器限制。
2. **兼容所有代理协议**：HTTP/SOCKS 均可，但建议 SOCKS5。
3. **无需修改系统 DNS**：只影响 Chrome（通过启动参数），不影响其他应用。
4. **支持 `--proxy-bypasslist`**：通过桥内逻辑实现，甚至可以更灵活（如支持 IP 段）。

---

## ⚠️ 注意事项

1. **本地桥需要实现完整的代理协议**：SOCKS5 握手、认证（可选）、命令处理、数据转发。
2. **DNS 解析性能**：建议增加缓存（遵守 TTL），避免每次请求都解析。
3. **`--host-resolver-rules` 与 HTTP 代理的配合**：对于 HTTP 代理，同样有效（因为 CONNECT 也会携带域名），所以两种协议都行。
4. **保持桥服务稳定**：若桥服务崩溃，Chrome 将无法访问任何网站。
