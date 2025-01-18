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
