## 使用 log/slog 记录带有请求 ID 的日志

### 1. 定义 Context Key 和中间件
生成唯一请求 ID 并存入请求的上下文（Context）。

```go
package main

import (
    "context"
    "net/http"
    "github.com/google/uuid" // 需要安装：go get github.com/google/uuid
)

// 定义 Context Key 类型（避免字符串冲突）
type contextKey string

const (
    requestIDKey contextKey = "requestID"
)

// 中间件：生成请求 ID 并存入 Context
func requestIDMiddleware(next http.Handler) http.Handler {
    return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
        requestID := uuid.New().String() // 生成 UUID
        ctx := context.WithValue(r.Context(), requestIDKey, requestID)
        next.ServeHTTP(w, r.WithContext(ctx))
    })
}
```

### 2. 自定义 Slog Handler

扩展 Handler 以自动从 Context 提取请求 ID 并添加到日志。
```go
import (
    "log/slog"
    "os"
)

type ContextHandler struct {
    slog.Handler
}

// 重写 Handle 方法，注入请求 ID
func (h *ContextHandler) Handle(ctx context.Context, r slog.Record) error {
    if requestID, ok := ctx.Value(requestIDKey).(string); ok {
        r.AddAttrs(slog.String("requestID", requestID))
    }
    return h.Handler.Handle(ctx, r)
}

// 初始化全局 Logger（JSON 格式示例）
func initLogger() {
    handler := &ContextHandler{slog.NewJSONHandler(os.Stdout, nil)}
    logger := slog.New(handler)
    slog.SetDefault(logger)
}
```

### 3. 在 HTTP 处理函数中记录日志

使用 *Context 方法传递 Context，触发请求 ID 的自动添加。
```go
func handleRequest(w http.ResponseWriter, r *http.Request) {
    ctx := r.Context()
    slog.InfoContext(ctx, "开始处理请求")
    // 业务逻辑...
    slog.WarnContext(ctx, "请求处理完成", "status", 200)
}
```

