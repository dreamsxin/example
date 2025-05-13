## WebSocket 代理配置
配置需放置在 Nginx 的 `http` 模块内：

```conf
http {
    map $http_upgrade $connection_upgrade {
        default upgrade;
        '' close;
    }

    server {
        <!-- 其他配置 -->

        location /chat/ {
            proxy_pass http://192.168.100.18:9663;
            proxy_http_version 1.1;
            proxy_set_header Upgrade $http_upgrade;
            proxy_set_header Connection $connection_upgrade;
            proxy_set_header Host $host;
            proxy_set_header X-Real-IP $remote_addr;
            proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        }
    }
}
```

### 核心配置说明‌

关键是通过以下头部实现协议升级：
```conf
proxy_set_header Upgrade $http_upgrade;
proxy_set_header Connection $connection_upgrade;
```

### 协议升级响应示例
```txt
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
```
=======================================================================

## nginx tcp 反向代理：stream和http是在同一级别上
### 列子1：代理 mysql
```conf
stream {
    upstream cloudsocket {
        hash $remote_addr consistent;
        server 192.168.182.155:3306 weight=5 max_fails=3 fail_timeout=30s;
    }

    server {
        listen 3306;  # 监听数据库端口
        proxy_connect_timeout 10s;
        proxy_timeout 300s;  # 空闲超时自动断开
        proxy_pass cloudsocket;
    }
}
```

### 例子2：代理 SSH 服务
```conf
stream {
    upstream socket_proxy {
        hash $remote_addr consistent;
        server 192.168.96.128:22 weight=5 max_fails=3 fail_timeout=300s;
    }

    server {
        listen 22;  # 监听 SSH 端口
        proxy_connect_timeout 100s;
        proxy_timeout 300s;  # 超时设定
        proxy_pass socket_proxy;
    }
}
```

### 列子3：代理自定义 TCP 服务
```conf
stream {
    upstream tcpstream {
        hash $remote_addr consistent;
        server 127.0.0.1:8088 max_fails=3 fail_timeout=10s;
    }

    server {
        listen 80;  # 监听 HTTP 端口
        proxy_connect_timeout 20s;
        proxy_timeout 5m;  # 长连接超时设置
        proxy_pass tcpstream;
    }
}
```
