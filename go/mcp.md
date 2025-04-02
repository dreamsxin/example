

## 测试工具

```shell
npx @modelcontextprotocol/inspector
npx @modelcontextprotocol/inspector D:/xxxx/mcp.exe
```

## 实例1

- github.com/mark3labs/mcp-go/mcp

```go
package main

import (
	"context"
	"errors"
	"fmt"

	"github.com/mark3labs/mcp-go/mcp"
	"github.com/mark3labs/mcp-go/server"
)

func main() {

	// Create MCP server
	s := server.NewMCPServer(
		"Demo 🚀",
		"1.0.0",
	)

	// Add tool
	tool := mcp.NewTool("hello_world",
		mcp.WithDescription("Say hello to someone"),
		mcp.WithString("name",
			mcp.Required(),
			mcp.Description("Name of the person to greet"),
		),
	)

	// Add tool handler
	s.AddTool(tool, helloHandler)
	tools.InitTools(s)
	// Start the stdio server
	if err := server.ServeStdio(s); err != nil {
		fmt.Printf("Server error: %v\n", err)
	}
}

func helloHandler(ctx context.Context, request mcp.CallToolRequest) (*mcp.CallToolResult, error) {
	name, ok := request.Params.Arguments["name"].(string)
	if !ok {
		return nil, errors.New("name must be a string")
	}

	return mcp.NewToolResultText(fmt.Sprintf("Hello, %s!", name)), nil
}

```

## 实例2

- github.com/dreamsxin/mcp-golang

```go
package main

import (
	"context"
	"fmt"
	"log"
	"time"

	mcp_golang "github.com/dreamsxin/mcp-golang"
	"github.com/dreamsxin/mcp-golang/transport"
	"github.com/dreamsxin/mcp-golang/transport/http"
	"github.com/dreamsxin/mcp-golang/transport/stdio"
	"github.com/gin-gonic/gin"
	"github.com/spf13/viper"
)

// TimeArgs defines the arguments for the time tool
type TimeArgs struct {
	Format string `json:"format" jsonschema:"description=The time format to use"`
}

func main() {
	log.SetFlags(log.LstdFlags | log.Lshortfile)

	v := viper.New()
	v.SetConfigFile("./config.json")
	//v.SetConfigType("yaml")
	err := v.ReadInConfig()
	if err != nil {
		panic(fmt.Sprintf("Fatal error config file: %v \n", err))
	}

	if err = v.Unmarshal(&config.Cfg); err != nil {
		log.Panic(err)
	}

	var trans transport.Transport
	if config.Cfg.Server.GetType() == "gin" {
		// Create a Gin transport
		trans = http.NewGinTransport()
	} else if config.Cfg.Server.GetType() == "sse" {

	} else {
		trans = stdio.NewStdioServerTransport()
	}

	// Create a new server with the transport
	server := mcp_golang.NewServer(trans, mcp_golang.WithName("mcp-cdp"), mcp_golang.WithVersion("0.0.1"))
	err = server.RegisterTool("time", "Returns the current time in the specified format", func(ctx context.Context, args TimeArgs) (*mcp_golang.ToolResponse, error) {
		ginCtx, ok := ctx.Value("ginContext").(*gin.Context)
		if !ok {
			return nil, fmt.Errorf("ginContext not found in context")
		}
		userAgent := ginCtx.GetHeader("User-Agent")
		log.Printf("Request from User-Agent: %s", userAgent)

		format := args.Format
		if format == "" {
			format = time.RFC3339
		}
		return mcp_golang.NewToolResponse(mcp_golang.NewTextContent(time.Now().Format(format))), nil
	})
	if err != nil {
		log.Panic(err)
	}

	tools.InitTools(server)
	go server.Serve()

	// Create a Gin router
	r := gin.Default()

	if config.Cfg.Server.GetType() == "gin" {
		// Add the MCP endpoint
		trans := trans.(*http.GinTransport)
		r.Any("/mcp", trans.Handler())
	}

	// Start the server
	addr := fmt.Sprintf("%s:%d", config.Cfg.Server.GetHost(), config.Cfg.Server.GetPort())
	log.Printf("Starting Gin server on %s...\n", addr)
	if err := r.Run(addr); err != nil {
		log.Fatalf("Server error: %v", err)
	}
}
```
