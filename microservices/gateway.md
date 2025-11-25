- https://github.com/traefik/traefik
- https://github.com/apache/apisix
- https://github.com/go-kratos/gateway
- https://github.com/alibaba/higress

## 1. Apache APISIX 部署

### 方式一：使用 Docker Compose 部署（推荐）

```yaml
# docker-compose.yml
version: "3"
services:
  etcd:
    image: bitnami/etcd:3.4.15
    environment:
      ETCD_ENABLE_V2: "true"
      ALLOW_NONE_AUTHENTICATION: "yes"
      ETCD_ADVERTISE_CLIENT_URLS: "http://0.0.0.0:2379"
      ETCD_LISTEN_CLIENT_URLS: "http://0.0.0.0:2379"
    ports:
      - "2379:2379"
    networks:
      - apisix-network

  apisix:
    image: apache/apisix:3.8.0-debian
    restart: always
    volumes:
      - ./apisix/config.yaml:/usr/local/apisix/conf/config.yaml:ro
    depends_on:
      - etcd
    ports:
      - "9080:9080/tcp"   # 网关端口
      - "9091:9091/tcp"   # Admin API 端口
      - "9443:9443/tcp"   # HTTPS 端口
    networks:
      - apisix-network

  apisix-dashboard:
    image: apache/apisix-dashboard:3.0.1-alpine
    restart: always
    volumes:
      - ./apisix/dashboard/conf.yaml:/usr/local/apisix-dashboard/conf/conf.yaml:ro
    depends_on:
      - apisix
    ports:
      - "9000:9000/tcp"
    networks:
      - apisix-network

networks:
  apisix-network:
    driver: bridge
```

### APISIX 配置文件

```yaml
# apisix/config.yaml
deployment:
  admin:
    admin_key:
      - name: "admin"
        key: edd1c9f034335f136f87ad84b625c8f1
        role: admin
      - name: "viewer"
        key: 4054f7cf07e344346cd3f287985e76a2
        role: viewer

apisix:
  node_listen:
    - 9080
    - 9443
  enable_admin: true
  admin_key_required: false

etcd:
  host:
    - "http://etcd:2379"
  prefix: "/apisix"
  timeout: 30
```

### Dashboard 配置文件

```yaml
# apisix/dashboard/conf.yaml
conf:
  listen:
    host: 0.0.0.0
    port: 9000

  etcd:
    endpoints:
      - "http://etcd:2379"

  log:
    error_log:
      level: "warn"
      file_path: "/usr/local/apisix-dashboard/logs/error.log"
    access_log:
      file_path: "/usr/local/apisix-dashboard/logs/access.log"
```

启动服务：
```bash
docker-compose up -d
```

## 2. Go WebSocket 服务示例

```go
// main.go
package main

import (
    "fmt"
    "log"
    "net/http"
    "time"

    "github.com/gorilla/websocket"
)

var upgrader = websocket.Upgrader{
    CheckOrigin: func(r *http.Request) bool {
        return true
    },
}

type Message struct {
    Type    string `json:"type"`
    Content string `json:"content"`
    User    string `json:"user"`
    Time    string `json:"time"`
}

func handleWebSocket(w http.ResponseWriter, r *http.Request) {
    conn, err := upgrader.Upgrade(w, r, nil)
    if err != nil {
        log.Printf("WebSocket upgrade error: %v", err)
        return
    }
    defer conn.Close()

    log.Printf("WebSocket client connected: %s", r.RemoteAddr)

    // 欢迎消息
    welcomeMsg := Message{
        Type:    "system",
        Content: "欢迎连接到 WebSocket 服务!",
        Time:    time.Now().Format("15:04:05"),
    }
    conn.WriteJSON(welcomeMsg)

    for {
        var msg Message
        err := conn.ReadJSON(&msg)
        if err != nil {
            log.Printf("Read error: %v", err)
            break
        }

        log.Printf("Received message: %+v", msg)

        // 添加时间戳并回复
        msg.Time = time.Now().Format("15:04:05")
        
        if err := conn.WriteJSON(msg); err != nil {
            log.Printf("Write error: %v", err)
            break
        }
    }

    log.Printf("WebSocket client disconnected: %s", r.RemoteAddr)
}

func main() {
    http.HandleFunc("/ws", handleWebSocket)
    http.HandleFunc("/health", func(w http.ResponseWriter, r *http.Request) {
        w.WriteHeader(http.StatusOK)
        w.Write([]byte("OK"))
    })

    port := ":8080"
    log.Printf("WebSocket server starting on port %s", port)
    log.Fatal(http.ListenAndServe(port, nil))
}
```

### Go 模块配置

```go
// go.mod
module websocket-server

go 1.19

require github.com/gorilla/websocket v1.5.0
```

运行 Go 服务：
```bash
go mod tidy
go run main.go
```

## 3. 配置 APISIX 路由

### 方式一：使用 Admin API

```bash
# 创建上游 (Upstream)
curl "http://127.0.0.1:9091/apisix/admin/upstreams/1" \
-H "X-API-KEY: edd1c9f034335f136f87ad84b625c8f1" \
-X PUT -d '
{
  "name": "websocket-backend",
  "type": "roundrobin",
  "nodes": {
    "host.docker.internal:8080": 1
  }
}'

# 创建路由 (Route)
curl "http://127.0.0.1:9091/apisix/admin/routes/1" \
-H "X-API-KEY: edd1c9f034335f136f87ad84b625c8f1" \
-X PUT -d '
{
  "name": "websocket-route",
  "uri": "/ws",
  "upstream_id": 1,
  "plugins": {
    "proxy-rewrite": {
      "scheme": "ws"
    }
  }
}'

# 创建健康检查路由
curl "http://127.0.0.1:9091/apisix/admin/routes/2" \
-H "X-API-KEY: edd1c9f034335f136f87ad84b625c8f1" \
-X PUT -d '
{
  "name": "health-check-route",
  "uri": "/health",
  "upstream_id": 1
}'
```

### 方式二：使用 Dashboard

1. 访问 `http://localhost:9000` 进入 Dashboard
2. 创建上游：
   - 名称：websocket-backend
   - 类型：roundrobin
   - 节点：host.docker.internal:8080 (权重 1)

3. 创建路由：
   - 路径：/ws
   - 选择上游：websocket-backend
   - 启用 WebSocket 支持

## 4. 测试 WebSocket 连接

### HTML 测试页面

```html
<!DOCTYPE html>
<html>
<head>
    <title>WebSocket Test</title>
</head>
<body>
    <div>
        <input type="text" id="messageInput" placeholder="输入消息">
        <button onclick="sendMessage()">发送</button>
    </div>
    <div id="messages" style="margin-top: 20px; border: 1px solid #ccc; padding: 10px; height: 300px; overflow-y: scroll;"></div>

    <script>
        const ws = new WebSocket('ws://localhost:9080/ws');
        const messagesDiv = document.getElementById('messages');
        
        ws.onopen = function(event) {
            addMessage('系统', 'WebSocket 连接已建立', 'system');
        };
        
        ws.onmessage = function(event) {
            const data = JSON.parse(event.data);
            addMessage(data.user || '系统', data.content, data.type);
        };
        
        ws.onclose = function(event) {
            addMessage('系统', 'WebSocket 连接已关闭', 'system');
        };
        
        function sendMessage() {
            const input = document.getElementById('messageInput');
            const message = {
                type: 'chat',
                content: input.value,
                user: '测试用户'
            };
            ws.send(JSON.stringify(message));
            input.value = '';
        }
        
        function addMessage(user, content, type) {
            const messageElement = document.createElement('div');
            messageElement.innerHTML = `<strong>${user}:</strong> ${content} <small>${new Date().toLocaleTimeString()}</small>`;
            if (type === 'system') {
                messageElement.style.color = 'blue';
            }
            messagesDiv.appendChild(messageElement);
            messagesDiv.scrollTop = messagesDiv.scrollHeight;
        }
        
        // 回车发送消息
        document.getElementById('messageInput').addEventListener('keypress', function(e) {
            if (e.key === 'Enter') {
                sendMessage();
            }
        });
    </script>
</body>
</html>
```

### 使用 wscat 测试

```bash
# 安装 wscat
npm install -g wscat

# 测试连接
wscat -c ws://localhost:9080/ws
```

## 5. 高级配置

### 启用 WebSocket 插件配置

```json
{
  "plugins": {
    "proxy-rewrite": {
      "scheme": "ws"
    }
  },
  "upstream": {
    "type": "roundrobin",
    "nodes": {
      "host.docker.internal:8080": 1
    },
    "timeout": {
      "connect": 6,
      "send": 6,
      "read": 6
    }
  }
}
```

### SSL 配置（可选）

```bash
# 创建 SSL 路由
curl "http://127.0.0.1:9091/apisix/admin/routes/3" \
-H "X-API-KEY: edd1c9f034335f136f87ad84b625c8f1" \
-X PUT -d '
{
  "name": "websocket-secure-route",
  "uri": "/wss",
  "upstream_id": 1,
  "plugins": {
    "proxy-rewrite": {
      "scheme": "wss"
    }
  }
}'
```

## 6. 验证部署

1. 检查 APISIX 状态：
```bash
curl http://127.0.0.1:9091/apisix/admin/routes -H "X-API-KEY: edd1c9f034335f136f87ad84b625c8f1"
```

2. 测试健康检查：
```bash
curl http://localhost:9080/health
```

3. 测试 WebSocket 连接：
   打开 HTML 测试页面或使用 wscat 工具
