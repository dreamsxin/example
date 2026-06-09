```python
#!/usr/bin/env python3
"""
局域网 80 端口 HTTP 服务扫描器
用法:
    python scanner.py                     # 自动探测局域网 /24 网段
    python scanner.py 192.168.1.0/24      # 扫描指定网段
"""

import sys
import socket
import ipaddress
from concurrent.futures import ThreadPoolExecutor, as_completed

# 扫描配置
PORT = 80
CONNECT_TIMEOUT = 1.5      # 连接超时(秒)
READ_TIMEOUT = 2.0         # 读取响应超时(秒)
MAX_WORKERS = 100          # 并发线程数

def get_local_network():
    """
    获取本机所在局域网的 /24 网段（默认子网掩码 255.255.255.0）。
    若获取本机 IP 失败，则回退到 192.168.1.0/24。
    """
    try:
        # 通过连接外部地址获取本机用于外网的 IP
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
            s.connect(('8.8.8.8', 80))
            local_ip = s.getsockname()[0]
        # 构建 /24 网络对象
        network = ipaddress.ip_network(f"{local_ip}/24", strict=False)
        return network
    except Exception:
        # 无法获取时返回默认网段
        print("[!] 无法获取本机 IP，使用默认网段 192.168.1.0/24")
        return ipaddress.ip_network("192.168.1.0/24")

def is_http_service(ip, port):
    """
    检测指定 IP 的端口是否提供可用的 HTTP 服务。
    发送 HTTP GET 请求，若响应状态码为 2xx 或 3xx 则视为可用。
    """
    request = (
        f"GET / HTTP/1.1\r\n"
        f"Host: {ip}:{port}\r\n"
        f"Connection: close\r\n"
        f"User-Agent: PortScanner\r\n"
        f"\r\n"
    ).encode()

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.settimeout(CONNECT_TIMEOUT)
            sock.connect((ip, port))
            sock.sendall(request)

            sock.settimeout(READ_TIMEOUT)
            # 接收响应头（最大 4096 字节，足够包含状态行）
            response = sock.recv(4096)
            if not response:
                return False

            # 解析状态行，例如 b'HTTP/1.1 200 OK'
            first_line = response.split(b'\r\n')[0].decode('utf-8', errors='ignore')
            if not first_line.startswith('HTTP/'):
                return False

            # 提取状态码
            parts = first_line.split()
            if len(parts) >= 2:
                status_code = int(parts[1])
                # 2xx 或 3xx 表示服务可用
                return 200 <= status_code < 400
            return False
    except (socket.timeout, ConnectionRefusedError, socket.error, ValueError):
        return False

def scan_ip(ip_str):
    """扫描单个 IP，返回 IP 字符串（若服务可用）或 None"""
    if is_http_service(ip_str, PORT):
        return ip_str
    return None

def main():
    # 解析命令行参数，确定扫描网段
    if len(sys.argv) == 2:
        try:
            network = ipaddress.ip_network(sys.argv[1], strict=False)
        except ValueError:
            print(f"[错误] 无效的网段格式: {sys.argv[1]}")
            return
    else:
        network = get_local_network()
        print(f"[*] 自动探测到局域网网段: {network}")

    # 生成所有主机 IP（排除网络地址和广播地址）
    hosts = [str(ip) for ip in network.hosts()]
    print(f"[*] 共 {len(hosts)} 个 IP 待扫描 (端口 {PORT}) ...")

    available_ips = []
    with ThreadPoolExecutor(max_workers=MAX_WORKERS) as executor:
        # 提交所有扫描任务
        future_to_ip = {executor.submit(scan_ip, ip): ip for ip in hosts}
        # 处理完成的任务
        for future in as_completed(future_to_ip):
            result = future.result()
            if result:
                available_ips.append(result)
                print(f"[+] 发现可用服务: {result}:{PORT}")

    # 输出最终结果
    print("\n" + "=" * 50)
    if available_ips:
        print("扫描完成，以下 IP 的 80 端口提供 HTTP 服务：")
        for ip in available_ips:
            print(ip)
    else:
        print("未发现任何在 80 端口提供 HTTP 服务的 IP。")

if __name__ == "__main__":
    main()
```
