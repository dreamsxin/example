```go
package main

import (
	"flag"
	"fmt"
	"log"
	"net/url"
	"os"
	"os/signal"
	"time"

	"github.com/gorilla/websocket"
)

/*
{"id":1,"method":"session.status","params":{}}
{"id":2,"method":"session.new","params":{"capabilities":{"alwaysMatch":{"acceptInsecureCerts":false,"unhandledPromptBehavior":{"default":"ignore"},"webSocketUrl":true,"goog:prerenderingDisabled":true}}}}
{"id":3,"method":"browser.getUserContexts","params":{}}
{"id":4,"method":"browsingContext.getTree","params":{}}
{"id":5,"method":"session.subscribe","params":{"events":["browsingContext","network","log","script"]}}
{"id":6,"method":"browsingContext.navigate","params":{"context":"3bda832e-21da-45a8-a933-086d32aa8057","url":"https://www.baidu.com","wait":"interactive"}}
*/
var addr = flag.String("addr", "localhost:57107", "http service address")

func SendMessage(c *websocket.Conn, packet string) error {
	err := c.WriteMessage(websocket.TextMessage, []byte(packet))
	return err
}
func main() {
	flag.Parse()
	log.SetFlags(0)

	interrupt := make(chan os.Signal, 1)
	signal.Notify(interrupt, os.Interrupt)

	u := url.URL{Scheme: "ws", Host: *addr, Path: "/session"}
	log.Printf("connecting to %s", u.String())

	c, _, err := websocket.DefaultDialer.Dial(u.String(), nil)
	if err != nil {
		log.Fatal("dial:", err)
	}
	defer c.Close()

	done := make(chan struct{})

	go func() {
		defer close(done)
		for {
			_, message, err := c.ReadMessage()
			if err != nil {
				log.Println("read:", err)
				return
			}
			log.Printf("recv: %s", message)
		}
	}()

	SendMessage(c, `{"id":1,"method":"session.status","params":{}}`)
	for {
		select {
		case <-done:
			return
		case <-interrupt:
			log.Println("interrupt")
			err := c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, ""))
			if err != nil {
				log.Println("write close:", err)
				return
			}
			select {
			case <-done:
			case <-time.After(time.Second):
			}
			return
		default:
			var packet string
			fmt.Println("Please enter packet: ")
			fmt.Scanln(&packet)

			err := SendMessage(c, packet)
			if err != nil {
				log.Println("write:", err)
				return
			}
		}
	}
}

```

## 测试
```go
package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net/url"
	"os/exec"
	"time"

	"github.com/gorilla/websocket"
)

// Command 表示发送给 WebDriver BiDi 的命令
type Command struct {
	ID     int         `json:"id"`
	Method string      `json:"method"`
	Params interface{} `json:"params,omitempty"`
}

// Response 表示从 WebDriver BiDi 接收的响应
type Response struct {
	ID     int             `json:"id"`
	Result json.RawMessage `json:"result,omitempty"`
	Error  *Error          `json:"error,omitempty"`
}

// Error 表示 WebDriver BiDi 返回的错误
type Error struct {
	Code    int    `json:"code"`
	Message string `json:"message"`
}

// NewSessionParams 表示创建新会话的参数
type NewSessionParams struct {
	Capabilities map[string]interface{} `json:"capabilities"`
}

// ScriptEvaluateParams 表示执行脚本的参数
type ScriptEvaluateParams struct {
	Expression string `json:"expression"`
}

// EventListenerParams 表示监听事件的参数
type EventListenerParams struct {
	Event string `json:"event"`
}

func main() {

	log.SetFlags(log.Lshortfile | log.LstdFlags)
	cmd := exec.Command("D:/Program Files/Mozilla Firefox/firefox.exe", "--remote-debugging-port=9222", "--marionette")
	err := cmd.Start()
	if err != nil {
		log.Fatal("启动 Firefox 失败:", err)
	}

	// 等待 Firefox 启动完成
	time.Sleep(2 * time.Second)

	// WebSocket URL for WebDriver BiDi
	u := url.URL{Scheme: "ws", Host: "localhost:9222", Path: "/session"}

	// 连接到 WebSocket 服务器
	conn, _, err := websocket.DefaultDialer.Dial(u.String(), nil)
	if err != nil {
		log.Fatal("dial:", err)
	}
	defer conn.Close()

	// 1. 创建新会话
	newSessionCmd := Command{
		ID:     1,
		Method: "session.new",
		Params: NewSessionParams{
			Capabilities: map[string]interface{}{
				"browserName": "chrome",
			},
		},
	}

	err = conn.WriteJSON(newSessionCmd)
	if err != nil {
		log.Fatal("write:", err)
	}

	// 读取创建会话的响应
	_, message, err := conn.ReadMessage()
	if err != nil {
		log.Fatal("read:", err)
	}

	var newSessionResponse Response
	err = json.Unmarshal(message, &newSessionResponse)
	if err != nil {
		log.Fatal("unmarshal:", err)
	}

	if newSessionResponse.Error != nil {
		log.Fatalf("session.new error: %v", newSessionResponse.Error)
	}

	fmt.Printf("New session created: %s\n", newSessionResponse.Result)

	// 2. 执行脚本
	scriptCmd := Command{
		ID:     2,
		Method: "script.evaluate",
		Params: ScriptEvaluateParams{
			Expression: "1 + 2",
		},
	}

	err = conn.WriteJSON(scriptCmd)
	if err != nil {
		log.Fatal("write:", err)
	}

	// 读取脚本执行的响应
	_, message, err = conn.ReadMessage()
	if err != nil {
		log.Fatal("read:", err)
	}
	log.Println("message:", string(message))

	var scriptResponse Response
	err = json.Unmarshal(message, &scriptResponse)
	if err != nil {
		log.Fatal("unmarshal:", err)
	}

	if scriptResponse.Error != nil {
		log.Fatalf("script.evaluate error: %v", scriptResponse.Error)
	}

	fmt.Printf("Script result: %s\n", scriptResponse.Result)

	// 3. 监听事件
	eventCmd := Command{
		ID:     3,
		Method: "event.listen",
		Params: EventListenerParams{
			Event: "DOM.contentLoaded",
		},
	}

	err = conn.WriteJSON(eventCmd)
	if err != nil {
		log.Fatal("write:", err)
	}

	// 持续接收事件
	for {
		_, message, err := conn.ReadMessage()
		if err != nil {
			log.Fatal("read:", err)
		}
		log.Println("message:", string(message))

		var event Response
		err = json.Unmarshal(message, &event)
		if err != nil {
			log.Fatal("unmarshal:", err)
		}

		fmt.Printf("Event received: %s\n", event.Result)
		time.Sleep(1 * time.Second)
	}
}
```
