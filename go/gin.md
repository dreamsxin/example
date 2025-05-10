# gin

## 使用流程

基于 `net/http` 开发 `web` 服务的一般流程:

利用 `http.HandleFunc` 函数注册路由，并指定处理函数，函数签名为 `func(w http.ResponseWriter, r *http.Request)`

设置响应状态码、设置响应头(如Content-Type)、处理结果序列化并写入响应体
调用http.ListenAndServer进行端口监听
```go
func main() {
    //注册路由
    http.HandleFunc("/time", func(w http.ResponseWriter, r *http.Request) {
        //需要在处理函数内部获取查询参数、路径参数、读取请求体并反序列化
        //设置响应状态码、设置响应头(如Content-Type)、处理结果序列化并写入响应体
        w.WriteHeader(http.StatusOK)
        w.Header().Set("Content-Type", "application/json")
        //业务逻辑处理
        tim := map[string]string{
            "time": time.Now().Format("2006-01-02"),
        }
        byts, err := json.Marshal(tim)
        if err != nil {
            //错误处理(设置错误响应状态码、错误信息等)
            panic(err)
        }
        w.Write(byts)
    })
    //调用http.ListenAndServer进行端口监听
    http.ListenAndServe("0.0.0.0:8080", nil)
}
```
gin 封装简化了操作：
```go
func main() {
    mux := gin.Default()
    mux.GET("/time", func(c *gin.Context) {
        m := map[string]string{
            "time": time.Now().Format("2006-01-02"),
        }
        c.JSON(http.StatusOK, m)
    })
    err := mux.Run("0.0.0.0:8080")
    if err != nil {
        panic(err)
    }
}
```

**总结**

1. 创建gin.Engine、注册middleware
2. 注册路由，编写处理函数，在函数内通过gin.Context获取参数，进行逻辑处理，通过gin.Context暴露的方法(如JSON())写回输出
3. 监听端口

## 内置中间件
在使用Gin框架开发Web应用时，常常需要自定义中间件，不过，Gin也内置一些中间件，我们可以直接使用，下面是内置中间件列表：
```go
func BasicAuth(accounts Accounts) HandlerFunc
func BasicAuthForRealm(accounts Accounts, realm string) HandlerFunc
func Bind(val interface{}) HandlerFunc
func ErrorLogger() HandlerFunc
func ErrorLoggerT(typ ErrorType) HandlerFunc
func Logger() HandlerFunc
func LoggerWithConfig(conf LoggerConfig) HandlerFunc
func LoggerWithFormatter(f LogFormatter) HandlerFunc
func LoggerWithWriter(out io.Writer, notlogged ...string) HandlerFunc
func Recovery() HandlerFunc  //自动捕获应用程序中的 panic 异常，防止程序崩溃，并返回500错误码，同时在控制台输出错误信息。
func RecoveryWithWriter(out io.Writer) HandlerFunc
func WrapF(f http.HandlerFunc) HandlerFunc
func WrapH(h http.Handler) HandlerFunc
```

## 路由注册流程

### 核心数据结构

#### gin.Engine
```go
type Handler interface {
    ServeHTTP(ResponseWriter, *Request)
}
```
`gin.Engine` 对象其实就是 `Handler` 接口的一个实现
`http.ListenAndServe` 函数的第二个参数是 `Handler` 接口类型，只要实现了该接口的 `ServeHTTP(ResponseWriter, *Request)` 方法，就能够对请求进行自定义处理。

```go
func (engine *Engine) ServeHTTP(w http.ResponseWriter, req *http.Request) {
  ...
}

func Default() *Engine {
    debugPrintWARNINGDefault() // 检测go版本是否在 1.14+
    engine := New() // 创建 Engine 对象
    engine.Use(Logger(), Recovery()) // 其实是先调用 New 方法，再调用 Use 注册 middleware，这里先忽略。
    return engine
}

func New() *Engine {
  ...
    engine := &Engine{
        // NOTE: 实例化RouteGroup，路由管理相关(Engine自身也是一个RouterGroup)
        RouterGroup: RouterGroup{
            Handlers: nil,
            basePath: "/",
            root:     true,
        },
        ...
        // NOTE: 负责存储路由和处理方法的映射，采用类似字典树的结构(这里构造了几棵树，每棵树对应一个http请求方法)
        trees:            make(methodTrees, 0, 9),
        ...
    }
  ...
    // NOTE: 基于sync.Pool实现的context池，能够避免context频繁销毁和重建
    engine.pool.New = func() any {
        return engine.allocateContext(engine.maxParams)
    }
    return engine
}
```

## 模板调试实时渲染

```go
	r.FuncMap = template.FuncMap{
		"html": func(s string) template.HTML {
			return template.HTML(s)
		},
		"upper": strings.ToUpper,
		"lower": strings.ToLower,
		"strip": func(str string) string {
			return strings.ReplaceAll(strip.StripTags(str), "&nbsp;", "")
		},
	}

	filenames := []string{}
	filenames1, err := filepath.Glob("./includes/*.html")
	if err == nil {
		filenames = append(filenames, filenames1...)
	}
	filenames2, err := filepath.Glob("./*.html")

	if err == nil {
		filenames = append(filenames, filenames2...)
	}
	if cfg.Server.Mode == "debug" {
		gin.SetMode(gin.DebugMode)
	}

	r.LoadHTMLFiles(filenames...)
```

### 模板错误输出

模板解析错误放在了 `gin.Context` 中的 `Errors` 里，所以可以在中间件中输出：
```go
if len(c.Errors) > 0 {
	log.Pritnln("Errors", c.Errors)
}
```

## 断点续传服务模块

```text
resumable/
├── config.go       // 配置相关
├── download.go     // 下载功能
├── upload.go       // 上传功能
├── utils.go        // 工具函数
└── resumable.go    // 主模块
```

- resumable.go
```go
package resumable

import (
	"fmt"

	"github.com/gin-gonic/gin"
)

// Service 断点续传服务
type Service struct {
	config *Config
}

```

- config.go
```go
package resumable

// Config 断点续传服务的配置
type Config struct {
	// 上传文件存储目录
	UploadDir string
	// 单次上传内存限制
	MaxMemory int64
	// 是否启用日志
	EnableLog bool
}

// DefaultConfig 返回默认配置
func DefaultConfig() *Config {
	return &Config{
		UploadDir:  "./uploads",
		MaxMemory:  8 << 20, // 8 MB
		EnableLog:  true,
	}
}
```

- utils.go
```go
package resumable

import (
	"fmt"
	"os"
	"path/filepath"
	"strconv"
	"strings"
)

// httpRange 表示HTTP Range头部的范围
type httpRange struct {
	start, end int64
}

// parseRange 解析HTTP Range头部
func parseRange(rangeHeader string, fileSize int64) ([]httpRange, error) {
	if !strings.HasPrefix(rangeHeader, "bytes=") {
		return nil, fmt.Errorf("无效的 Range 头: %s", rangeHeader)
	}

	rangeHeader = strings.TrimPrefix(rangeHeader, "bytes=")
	var ranges []httpRange

	for _, r := range strings.Split(rangeHeader, ",") {
		r = strings.TrimSpace(r)
		if r == "" {
			continue
		}

		parts := strings.Split(r, "-")
		if len(parts) != 2 {
			return nil, fmt.Errorf("无效的范围格式: %s", r)
		}

		start, end := parts[0], parts[1]
		var startByte, endByte int64
		var err error

		if start == "" {
			// 如果起始位置为空，例如 -500，表示最后 500 字节
			endByte, err = strconv.ParseInt(end, 10, 64)
			if err != nil {
				return nil, fmt.Errorf("无效的范围结束位置: %s", end)
			}
			if endByte <= 0 {
				return nil, fmt.Errorf("范围结束位置必须大于 0")
			}
			startByte = fileSize - endByte
			if startByte < 0 {
				startByte = 0
			}
			endByte = fileSize - 1
		} else {
			// 正常范围，例如 500-999
			startByte, err = strconv.ParseInt(start, 10, 64)
			if err != nil {
				return nil, fmt.Errorf("无效的范围起始位置: %s", start)
			}

			if end == "" {
				// 如果结束位置为空，例如 500-，表示从 500 到文件末尾
				endByte = fileSize - 1
			} else {
				endByte, err = strconv.ParseInt(end, 10, 64)
				if err != nil {
					return nil, fmt.Errorf("无效的范围结束位置: %s", end)
				}
			}
		}

		if startByte >= fileSize {
			return nil, fmt.Errorf("范围起始位置超出文件大小")
		}
		if endByte >= fileSize {
			endByte = fileSize - 1
		}
		if startByte > endByte {
			return nil, fmt.Errorf("范围起始位置大于结束位置")
		}

		ranges = append(ranges, httpRange{startByte, endByte})
	}

	if len(ranges) == 0 {
		return nil, fmt.Errorf("没有有效的范围")
	}

	return ranges, nil
}

// ensureDir 确保目录存在
func ensureDir(dir string) error {
	return os.MkdirAll(dir, 0755)
}

// safeFilename 确保文件名安全
func safeFilename(filename string) string {
	return filepath.Base(filename)
}
```

- download.go
```go
package resumable

import (
	"fmt"
	"io"
	"net/http"
	"os"
	"path/filepath"
	"strconv"

	"github.com/gin-gonic/gin"
)

// HandleDownload 处理文件下载，支持断点续传
func (s *Service) HandleDownload(c *gin.Context) {
	filename := c.Param("filename")
	filepath := filepath.Join(s.config.UploadDir, safeFilename(filename))

	// 检查文件是否存在
	fileInfo, err := os.Stat(filepath)
	if err != nil {
		c.JSON(http.StatusNotFound, gin.H{"error": "文件不存在"})
		return
	}

	// 打开文件
	file, err := os.Open(filepath)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "无法打开文件"})
		return
	}
	defer file.Close()

	// 获取文件大小
	fileSize := fileInfo.Size()

	// 设置响应头
	c.Header("Content-Disposition", fmt.Sprintf("attachment; filename=\"%s\"", filename))
	c.Header("Accept-Ranges", "bytes")
	c.Header("Content-Type", "application/octet-stream")
	c.Header("Content-Length", strconv.FormatInt(fileSize, 10))

	// 检查是否有 Range 头
	rangeHeader := c.Request.Header.Get("Range")
	if rangeHeader != "" {
		// 解析 Range 头
		ranges, err := parseRange(rangeHeader, fileSize)
		if err != nil {
			c.JSON(http.StatusRequestedRangeNotSatisfiable, gin.H{"error": "Range 头无效"})
			return
		}

		if len(ranges) > 1 {
			c.JSON(http.StatusRequestedRangeNotSatisfiable, gin.H{"error": "不支持多重范围请求"})
			return
		}

		// 获取范围
		start := ranges[0].start
		end := ranges[0].end

		// 设置部分内容响应
		c.Status(http.StatusPartialContent)
		c.Header("Content-Range", fmt.Sprintf("bytes %d-%d/%d", start, end, fileSize))
		c.Header("Content-Length", strconv.FormatInt(end-start+1, 10))

		// 移动文件指针到起始位置
		_, err = file.Seek(start, io.SeekStart)
		if err != nil {
			c.JSON(http.StatusInternalServerError, gin.H{"error": "无法定位文件"})
			return
		}

		// 发送部分内容
		_, err = io.CopyN(c.Writer, file, end-start+1)
		if err != nil {
			// 客户端可能中断连接，这是预期的
			if s.config.EnableLog {
				fmt.Printf("下载中断: %v\n", err)
			}
			return
		}
	} else {
		// 发送整个文件
		c.Status(http.StatusOK)
		_, err = io.Copy(c.Writer, file)
		if err != nil {
			// 客户端可能中断连接，这是预期的
			if s.config.EnableLog {
				fmt.Printf("下载中断: %v\n", err)
			}
			return
		}
	}
}
```

- upload.go
```go
package resumable

import (
	"fmt"
	"io"
	"net/http"
	"os"
	"path/filepath"
	"strconv"
	"strings"

	"github.com/gin-gonic/gin"
)

// HandleUpload 处理文件上传，支持断点续传
func (s *Service) HandleUpload(c *gin.Context) {
	// 限制请求体大小
	c.Request.Body = http.MaxBytesReader(c.Writer, c.Request.Body, s.config.MaxMemory)

	// 获取文件名和当前上传位置
	filename := c.PostForm("filename")
	if filename == "" {
		c.JSON(http.StatusBadRequest, gin.H{"error": "未提供文件名"})
		return
	}

	// 安全检查文件名
	filename = safeFilename(filename)
	filePath := filepath.Join(s.config.UploadDir, filename)

	// 获取 Content-Range 头
	contentRange := c.Request.Header.Get("Content-Range")
	var start, end, total int64
	var err error

	if contentRange != "" {
		// 解析 Content-Range 头 (格式: bytes start-end/total)
		parts := strings.Split(contentRange, " ")
		if len(parts) != 2 || !strings.HasPrefix(parts[0], "bytes") {
			c.JSON(http.StatusBadRequest, gin.H{"error": "Content-Range 格式无效"})
			return
		}

		rangeParts := strings.Split(parts[1], "/")
		if len(rangeParts) != 2 {
			c.JSON(http.StatusBadRequest, gin.H{"error": "Content-Range 格式无效"})
			return
		}

		positions := strings.Split(rangeParts[0], "-")
		if len(positions) != 2 {
			c.JSON(http.StatusBadRequest, gin.H{"error": "Content-Range 格式无效"})
			return
		}

		start, err = strconv.ParseInt(positions[0], 10, 64)
		if err != nil {
			c.JSON(http.StatusBadRequest, gin.H{"error": "Content-Range 起始位置无效"})
			return
		}

		end, err = strconv.ParseInt(positions[1], 10, 64)
		if err != nil {
			c.JSON(http.StatusBadRequest, gin.H{"error": "Content-Range 结束位置无效"})
			return
		}

		total, err = strconv.ParseInt(rangeParts[1], 10, 64)
		if err != nil {
			c.JSON(http.StatusBadRequest, gin.H{"error": "Content-Range 总大小无效"})
			return
		}
	} else {
		// 如果没有 Content-Range，则假设是从头开始上传
		start = 0
		end = -1
		total = -1
	}

	// 获取上传的文件
	file, err := c.FormFile("file")
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": "未找到上传的文件"})
		return
	}

	// 打开上传的文件
	src, err := file.Open()
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "无法打开上传的文件"})
		return
	}
	defer src.Close()

	var dst *os.File

	// 检查文件是否已存在
	if _, err := os.Stat(filePath); err == nil && start > 0 {
		// 文件存在且不是从头开始上传，打开文件进行追加
		dst, err = os.OpenFile(filePath, os.O_WRONLY, 0644)
		if err != nil {
			c.JSON(http.StatusInternalServerError, gin.H{"error": "无法打开目标文件"})
			return
		}
		defer dst.Close()

		// 移动到指定位置
		_, err = dst.Seek(start, io.SeekStart)
		if err != nil {
			c.JSON(http.StatusInternalServerError, gin.H{"error": "无法定位到指定位置"})
			return
		}
	} else {
		// 文件不存在或从头开始上传，创建新文件
		dst, err = os.Create(filePath)
		if err != nil {
			c.JSON(http.StatusInternalServerError, gin.H{"error": "无法创建目标文件"})
			return
		}
		defer dst.Close()
	}

	// 复制数据
	written, err := io.Copy(dst, src)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "写入文件失败"})
		return
	}

	// 获取当前文件大小
	fileInfo, err := os.Stat(filePath)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"error": "无法获取文件信息"})
		return
	}
	currentSize := fileInfo.Size()

	// 判断是否上传完成
	isComplete := total == -1 || currentSize >= total

	status := "部分上传"
	if isComplete {
		status = "上传完成"
	}

	c.JSON(http.StatusOK, gin.H{
		"status":       status,
		"filename":     filename,
		"size":         currentSize,
		"total":        total,
		"bytesWritten": written,
	})
}

// HandleUploadStatus 获取上传文件的状态
func (s *Service) HandleUploadStatus(c *gin.Context) {
	filename := c.Param("filename")
	filePath := filepath.Join(s.config.UploadDir, safeFilename(filename))

	// 检查文件是否存在
	fileInfo, err := os.Stat(filePath)
	if err != nil {
		if os.IsNotExist(err) {
			c.Header("Upload-Exists", "false")
			c.Header("Upload-Size", "0")
			c.Status(http.StatusOK)
			return
		}
		c.JSON(http.StatusInternalServerError, gin.H{"error": "无法获取文件信息"})
		return
	}

	// 文件存在，返回当前大小
	c.Header("Upload-Exists", "true")
	c.Header("Upload-Size", strconv.FormatInt(fileInfo.Size(), 10))
	c.Status(http.StatusOK)
}
```
