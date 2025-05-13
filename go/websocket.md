
```go
package main

import (
	"log"
	"time"

	"github.com/gorilla/websocket"
)

func main() {

	dialer := websocket.Dialer{
		HandshakeTimeout: 10 * time.Second,
	}
	//websocat --ping-interval 10 ws://localhost
	c, res, err := dialer.Dial("ws://localhost", nil)
	if err != nil {
		if res != nil {
			log.Println("res", res.StatusCode, res.Status, res.Header)
		}
		log.Fatal("dial:", err)
	}
	defer c.Close()
	// 接收心跳
	c.SetPongHandler(func(appData string) error {
		log.Println("pong", appData)
		return nil
	})

	t := time.NewTicker(60 * time.Second)
	defer t.Stop()
	go func() {
		for range t.C {
			log.Println("time", time.Now().String())
		}
	}()
	for {

		_, msgBytes, err := c.ReadMessage()
		if err != nil {
			log.Println("read:", err)
			return
		}
		log.Printf("recv: %s", msgBytes)
	}
}
```
