package main

import (
	"context"
	"errors"
	"fmt"
	"log"
	"net"
	"net/http"
	"net/http/httputil"
	"net/url"
	"os"
	"time"
)

// Config 保存代理的配置选项
type Config struct {
	// DefaultTarget 默认目标 URL，当请求未提供 target 参数时使用
	DefaultTarget *url.URL
	// SetXForwarded 是否设置 X-Forwarded-* 头
	SetXForwarded bool
	// PassHost 是否传递客户端原始 Host 头（透明代理）
	PassHost bool
	// Logger 自定义日志记录器，为 nil 时使用标准日志
	Logger *log.Logger
}

// contextKey 用于上下文传值的键类型
type contextKey string

const (
	sourceIPKey contextKey = "sourceIP"
	proxyErrKey contextKey = "proxyError"
)

// DynamicReverseProxy 创建一个可根据请求参数动态转发并指定出口 IP 的反向代理
func DynamicReverseProxy(cfg Config) http.Handler {
	if cfg.Logger == nil {
		cfg.Logger = log.New(os.Stderr, "[proxy] ", log.LstdFlags)
	}

	// 创建自定义 Transport，支持从上下文获取源 IP 并绑定本地地址
	transport := &http.Transport{
		// 禁用 Keep-Alive 以确保每个请求使用独立的连接，从而能按请求绑定源 IP
		DisableKeepAlives: true,
		// 其他 Transport 配置可按需调整
		MaxIdleConns:        0, // 禁用空闲连接
		IdleConnTimeout:     0,
		TLSHandshakeTimeout: 10 * time.Second,
	}

	// 包装 DialContext，从请求上下文读取 sourceIP 并设置 LocalAddr
	transport.DialContext = func(ctx context.Context, network, addr string) (net.Conn, error) {
		dialer := &net.Dialer{
			Timeout:   30 * time.Second,
			KeepAlive: 30 * time.Second,
		}

		// 尝试从上下文中获取 sourceIP
		if ipStr, ok := ctx.Value(sourceIPKey).(string); ok && ipStr != "" {
			if ip := net.ParseIP(ipStr); ip != nil {
				// 设置本地地址
				dialer.LocalAddr = &net.TCPAddr{IP: ip, Port: 0}
				cfg.Logger.Printf("使用源 IP %s 连接到 %s", ipStr, addr)
			} else {
				cfg.Logger.Printf("警告: 无效的源 IP 地址 %s，将使用默认路由", ipStr)
			}
		}

		return dialer.DialContext(ctx, network, addr)
	}

	proxy := &httputil.ReverseProxy{
		Rewrite: func(r *httputil.ProxyRequest) {
			// 解析并处理查询参数
			query := r.In.URL.Query()
			targetStr := query.Get("_target")
			sourceIP := query.Get("_source_ip")

			// 从查询参数中删除 target 和 source_ip
			query.Del("_target")
			query.Del("_source_ip")
			// 重新编码查询参数，供后续使用
			cleanRawQuery := query.Encode()

			// 确定目标 URL
			var targetURL *url.URL
			if targetStr != "" {
				var err error
				targetURL, err = url.Parse(targetStr)
				if err != nil || targetURL.Scheme == "" || targetURL.Host == "" {
					errMsg := fmt.Sprintf("无效的 target 参数: %q", targetStr)
					cfg.Logger.Print(errMsg)
					// 将错误存入上下文，让 ErrorHandler 处理
					ctx := context.WithValue(r.In.Context(), proxyErrKey, errors.New(errMsg))
					r.In = r.In.WithContext(ctx)
					// 设置 r.Out = nil 会触发 ErrorHandler
					r.Out = nil
					return
				}
			} else if cfg.DefaultTarget != nil {
				targetURL = cfg.DefaultTarget
			} else {
				errMsg := "缺少 target 参数且未配置默认目标"
				cfg.Logger.Print(errMsg)
				ctx := context.WithValue(r.In.Context(), proxyErrKey, errors.New(errMsg))
				r.In = r.In.WithContext(ctx)
				r.Out = nil
				return
			}

			// 将 sourceIP 存入上下文，供 Transport 使用
			if sourceIP != "" {
				ctx := context.WithValue(r.In.Context(), sourceIPKey, sourceIP)
				r.In = r.In.WithContext(ctx)
			}

			// 构建最终要发送的查询参数
			finalQuery := cleanRawQuery
			if targetURL.RawQuery != "" {
				// 合并目标 URL 自带的查询参数与原始查询参数（目标参数优先）
				targetQuery, _ := url.ParseQuery(targetURL.RawQuery)
				originalQuery, _ := url.ParseQuery(cleanRawQuery)
				for k, v := range targetQuery {
					originalQuery[k] = v // 覆盖或新增
				}
				finalQuery = originalQuery.Encode()
			}

			// 设置转发目标
			r.SetURL(targetURL)

			// 设置最终的查询字符串
			r.Out.URL.RawQuery = finalQuery

			// 可选添加 X-Forwarded-* 头
			if cfg.SetXForwarded {
				r.SetXForwarded()
			}

			// 透明转发：保留客户端原始 Host 头
			if cfg.PassHost {
				r.Out.Host = r.In.Host
			}
		},
		Transport: transport,
		ErrorHandler: func(w http.ResponseWriter, req *http.Request, err error) {
			// 优先检查上下文中是否设置了自定义错误
			if ctxErr := req.Context().Value(proxyErrKey); ctxErr != nil {
				http.Error(w, ctxErr.(error).Error(), http.StatusBadRequest)
				return
			}
			// 默认错误处理
			http.Error(w, "代理错误: "+err.Error(), http.StatusBadGateway)
		},
		ErrorLog: cfg.Logger, // 使用自定义 Logger
	}

	return proxy
}

func main() {
	println(os.Args)
	// 示例用法：从命令行参数读取默认目标（可选）
	var defaultTarget *url.URL
	if len(os.Args) == 2 {
		var err error
		defaultTarget, err = url.Parse(os.Args[1])
		if err != nil {
			fmt.Fprintf(os.Stderr, "无效的默认目标 URL: %v\n", err)
			os.Exit(1)
		}
	} else if len(os.Args) > 2 {
		fmt.Fprintf(os.Stderr, "用法: %s [默认目标URL]\n", os.Args[0])
		os.Exit(1)
	}

	cfg := Config{
		DefaultTarget: defaultTarget,
		SetXForwarded: false,
		PassHost:      false,
		Logger:        log.New(os.Stdout, "[dynamic-proxy] ", log.LstdFlags),
	}

	handler := DynamicReverseProxy(cfg)

	fmt.Println("动态透明代理启动，监听 :8080")
	if defaultTarget != nil {
		fmt.Println("默认目标:", defaultTarget.String())
	}
	fmt.Println("参数说明:")
	fmt.Println("  target:    转发目标 URL (必需，除非有默认值)")
	fmt.Println("  source_ip: 指定出口 IP (可选，必须是本机有效IP)")

	if err := http.ListenAndServe(":8080", handler); err != nil {
		fmt.Fprintf(os.Stderr, "服务器启动失败: %v\n", err)
		os.Exit(1)
	}
}
