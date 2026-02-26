package main

import (
	"context"
	"crypto/tls"
	"errors"
	"fmt"
	"log"
	"net"
	"net/http"
	"net/http/httptrace"
	"net/http/httputil"
	"net/url"
	"os"
	"time"

	"github.com/spf13/viper"
)

type ProxyConfig struct {
	SetXForwarded         bool          `mapstructure:"set_x_forwarded"`
	PassHost              bool          `mapstructure:"pass_host"`
	DialTimeout           time.Duration `mapstructure:"dial_timeout"`
	KeepAlive             time.Duration `mapstructure:"keep_alive"`
	TLSHandshakeTimeout   time.Duration `mapstructure:"tls_handshake_timeout"`
	ResponseHeaderTimeout time.Duration `mapstructure:"response_header_timeout"`
	ExpectContinueTimeout time.Duration `mapstructure:"expect_continue_timeout"`
	MaxIdleConns          int           `mapstructure:"max_idle_conns"`
	IdleConnTimeout       time.Duration `mapstructure:"idle_conn_timeout"`
}

// AppConfig 包含所有应用配置
type AppConfig struct {
	Server struct {
		Host string `mapstructure:"host"`
		Port int    `mapstructure:"port"`
	} `mapstructure:"server"`

	Proxy struct {
		DefaultTarget string                   `mapstructure:"default_target"`
		ProxyConfig   `mapstructure:",squash"` // 扁平化嵌入，使 ProxyConfig 的字段直接成为 proxy 的子字段
	} `mapstructure:"proxy"`
}

// Config 保存代理的配置选项
type Config struct {
	DefaultTarget *url.URL
	Logger        *log.Logger
	ProxyConfig   // 嵌入公共配置
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
		DisableKeepAlives:     cfg.MaxIdleConns == 0, // 如果 MaxIdleConns 为 0，禁用 Keep-Alive
		MaxIdleConns:          cfg.MaxIdleConns,
		IdleConnTimeout:       cfg.IdleConnTimeout,
		TLSHandshakeTimeout:   cfg.TLSHandshakeTimeout,
		ResponseHeaderTimeout: cfg.ResponseHeaderTimeout,
		ExpectContinueTimeout: cfg.ExpectContinueTimeout,
	}

	// 包装 DialContext，从请求上下文读取 sourceIP 并设置 LocalAddr
	transport.DialContext = func(ctx context.Context, network, addr string) (net.Conn, error) {
		dialer := &net.Dialer{
			Timeout:   cfg.DialTimeout,
			KeepAlive: cfg.KeepAlive,
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
		} else {
			cfg.Logger.Printf("警告: 无法从上下文获取 sourceIP，将使用默认路由")
		}

		// 分离主机和端口
		host, port, err := net.SplitHostPort(addr)
		if err != nil {
			return nil, fmt.Errorf("invalid address %q: %w", addr, err)
		}

		// 如果主机名不是 IP，则手动解析并计时
		if net.ParseIP(host) == nil {
			// 可选：从配置中获取 DNS 超时，构造带超时的 context
			// dnsCtx, cancel := context.WithTimeout(ctx, cfg.DNSTimeout)
			// defer cancel()
			start := time.Now()
			ips, err := net.DefaultResolver.LookupIPAddr(ctx, host) // 使用原 ctx 或带超时的 ctx
			dnsDuration := time.Since(start)
			if err != nil {
				cfg.Logger.Printf("DNS 解析失败 %s: %v (耗时 %v)", host, err, dnsDuration)
				return nil, err
			}
			if len(ips) == 0 {
				return nil, fmt.Errorf("no IP addresses for %s", host)
			}
			// 简单取第一个 IP（可根据策略选择）
			ip := ips[0].IP
			cfg.Logger.Printf("DNS 解析 %s -> %s, 耗时 %v", host, ip, dnsDuration)
			addr = net.JoinHostPort(ip.String(), port)
		}

		// 现在 addr 为 IP:port，dialer.DialContext 将直接连接
		return dialer.DialContext(ctx, network, addr)
	}

	proxy := &httputil.ReverseProxy{
		Rewrite: func(r *httputil.ProxyRequest) {
			// 解析并处理查询参数
			query := r.In.URL.Query()
			targetStr := query.Get("_target")
			sourceIP := query.Get("_source_ip")
			cfg.Logger.Printf("targetStr: %v, sourceIP: %v", targetStr, sourceIP)

			// 从查询参数中删除 request_id、target 和 source_ip
			query.Del("_request_id")
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
				ctx := context.WithValue(r.Out.Context(), sourceIPKey, sourceIP)

				// 4. 创建 ClientTrace，注册 TLS 握手回调
				trace := &httptrace.ClientTrace{
					TLSHandshakeStart: func() {
						fmt.Println("TLS 握手开始", time.Now().Format("2006-01-02 15:04:05.000"))
					},
					TLSHandshakeDone: func(connState tls.ConnectionState, err error) {
						fmt.Println("TLS 握手结束", time.Now().Format("2006-01-02 15:04:05.000"))
						if err != nil {
							fmt.Printf("TLS 握手错误: %v\n", err)
						} else {
							fmt.Printf("TLS 握手成功, 连接状态: %v\n", connState)
						}
					},
				}

				// 5. 将跟踪器附加到请求的上下文中
				ctx = httptrace.WithClientTrace(ctx, trace)
				r.Out = r.Out.WithContext(ctx)
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
	//return proxy
	// 包装一层以记录请求开始、结束及时长
	handler := http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		start := time.Now()
		cfg.Logger.Printf("请求开始: %s %s", r.Method, r.URL)
		proxy.ServeHTTP(w, r)
		cfg.Logger.Printf("请求结束: %s %s, 耗时: %v", r.Method, r.URL, time.Since(start))
	})
	return handler
}
func main() {

	// 初始化 viper
	viper.SetConfigName("config")   // 配置文件名称（无扩展名）
	viper.SetConfigType("yaml")     // 配置文件类型
	viper.AddConfigPath(".")        // 查找配置文件的路径
	viper.AddConfigPath("./config") // 可添加多个路径

	// 设置默认值（可选）
	viper.SetDefault("server.host", "0.0.0.0")
	viper.SetDefault("server.port", 8080)
	viper.SetDefault("proxy.set_x_forwarded", false)
	viper.SetDefault("proxy.pass_host", false)

	viper.SetDefault("proxy.dial_timeout", "30s")
	viper.SetDefault("proxy.keep_alive", "30s")
	viper.SetDefault("proxy.tls_handshake_timeout", "10s")
	viper.SetDefault("proxy.response_header_timeout", "0s")
	viper.SetDefault("proxy.expect_continue_timeout", "0s")
	viper.SetDefault("proxy.max_idle_conns", 0)
	viper.SetDefault("proxy.idle_conn_timeout", "0s")

	// 读取环境变量（支持前缀 PROXY_）
	viper.SetEnvPrefix("PROXY")
	viper.AutomaticEnv()

	// 读取配置文件
	if err := viper.ReadInConfig(); err != nil {
		// 如果配置文件不存在，仅打印警告并继续使用默认值
		if _, ok := err.(viper.ConfigFileNotFoundError); ok {
			log.Println("未找到配置文件，将使用默认值和环境变量")
		} else {
			log.Fatalf("读取配置文件失败: %v", err)
		}
	} else {
		log.Println("使用配置文件:", viper.ConfigFileUsed())
	}

	// 将配置解析到结构体
	var appConfig AppConfig
	if err := viper.Unmarshal(&appConfig); err != nil {
		log.Fatalf("解析配置失败: %v", err)
	}

	// 处理默认目标
	var defaultTargetURL *url.URL
	if appConfig.Proxy.DefaultTarget != "" {
		var err error
		defaultTargetURL, err = url.Parse(appConfig.Proxy.DefaultTarget)
		if err != nil {
			log.Fatalf("配置文件中的默认目标 URL 无效: %v", err)
		}
	}

	// 构建代理配置
	cfg := Config{
		DefaultTarget: defaultTargetURL,
		Logger:        log.New(os.Stdout, "[dynamic-proxy] ", log.LstdFlags),
		ProxyConfig:   appConfig.Proxy.ProxyConfig, // 直接赋值嵌入的结构体
	}

	// 创建处理器
	handler := DynamicReverseProxy(cfg)

	// 组装监听地址
	listenAddr := fmt.Sprintf("%s:%d", appConfig.Server.Host, appConfig.Server.Port)

	// 启动服务
	fmt.Printf("动态透明代理启动，监听 %s\n", listenAddr)
	if defaultTargetURL != nil {
		fmt.Println("默认目标:", defaultTargetURL.String())
	}
	fmt.Println("参数说明:")
	fmt.Println("  _target:    转发目标 URL (必需，除非有默认值)")
	fmt.Println("  _source_ip: 指定出口 IP (可选，必须是本机有效IP)")

	if err := http.ListenAndServe(listenAddr, handler); err != nil {
		fmt.Fprintf(os.Stderr, "服务器启动失败: %v\n", err)
		os.Exit(1)
	}
}
