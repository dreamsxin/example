## 邀请客户端

```go
package main

import (
	"bufio"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"log"
	"os"
	"strconv"
	"time"

	"github.com/pion/webrtc/v4"
)

func main() {
	config := webrtc.Configuration{
		ICEServers: []webrtc.ICEServer{
			{
				URLs: []string{"stun:stun.l.google.com:19302"},
			},
		},
	}

	// 1. create peer connection
	peerConnection, err := webrtc.NewPeerConnection(config)
	if err != nil {
		return
	}
	defer func() {
		if err := peerConnection.Close(); err != nil {
			log.Println(err.Error())
		}
	}()

	// 2. create data channel
	dataChannel, err := peerConnection.CreateDataChannel("foo", nil)
	dataChannel.OnOpen(func() {
		log.Println("data channel has opened")
		i := -1000
		for range time.NewTicker(time.Second * 5).C {
			if err := dataChannel.SendText("offer : hello world " + strconv.Itoa(i)); err != nil {
				log.Println(err.Error())
			}
		}
	})
	dataChannel.OnMessage(func(msg webrtc.DataChannelMessage) {
		fmt.Println(string(msg.Data))
	})

	// 3. create offer
	offer, err := peerConnection.CreateOffer(nil)
	if err != nil {
		return
	}

	// 4. set local description
	err = peerConnection.SetLocalDescription(offer)
	if err != nil {
		return
	}

	// 5. print offer
	println("OFFER:")
	println(encode(&offer))

	// 6. input answer
	println("请输入ANSWER:")
	var answer webrtc.SessionDescription
	answerStr, _ := bufio.NewReader(os.Stdin).ReadString('\n')
	decode(answerStr, &answer)

	// 7. set remote description
	if err := peerConnection.SetRemoteDescription(answer); err != nil {
		panic(err)
	}

	// 主循环，
	select {}
}
func encode(obj *webrtc.SessionDescription) string {
	b, err := json.Marshal(obj)
	if err != nil {
		panic(err)
	}

	return base64.StdEncoding.EncodeToString(b)
}

func decode(in string, obj *webrtc.SessionDescription) {
	b, err := base64.StdEncoding.DecodeString(in)
	if err != nil {
		panic(err)
	}

	if err = json.Unmarshal(b, obj); err != nil {
		panic(err)
	}
}
```

## 应答客户端

```go
package main

import (
	"bufio"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"log"
	"os"
	"strconv"
	"time"

	"github.com/pion/webrtc/v4"
)

func main() {
	config := webrtc.Configuration{
		ICEServers: []webrtc.ICEServer{
			{
				URLs: []string{"stun:stun.l.google.com:19302"},
			},
		},
	}

	// 1. create peer connection
	peerConnection, err := webrtc.NewPeerConnection(config)
	if err != nil {
		return
	}
	defer func() {
		if err := peerConnection.Close(); err != nil {
			log.Println(err.Error())
		}
	}()

	// 2. on data channel
	peerConnection.OnDataChannel(func(dataChannel *webrtc.DataChannel) {
		dataChannel.OnOpen(func() {
			log.Println("data channel has opened")
			i := -1000
			for range time.NewTicker(time.Second * 5).C {
				if err := dataChannel.SendText("answer : hello world " + strconv.Itoa(i)); err != nil {
					log.Println(err.Error())
				}
			}
		})
		dataChannel.OnMessage(func(msg webrtc.DataChannelMessage) {
			fmt.Println(string(msg.Data))
		})
	})

	// 3. input offer
	println("请输入OFFER:")
	offerStr, _ := bufio.NewReader(os.Stdin).ReadString('\n')
	var offer webrtc.SessionDescription
	decode1(offerStr, &offer)

	// 4. set remote description
	if err := peerConnection.SetRemoteDescription(offer); err != nil {
		panic(err)
	}

	// 5. create answer
	answer, err := peerConnection.CreateAnswer(nil)
	if err != nil {
		panic(err)
	}

	// 6. set local description
	if err := peerConnection.SetLocalDescription(answer); err != nil {
		panic(err)
	}

	// 7. gather complete
	gatherComplete := webrtc.GatheringCompletePromise(peerConnection)
	<-gatherComplete

	// 8. print answer
	fmt.Println("ANSWER:")
	fmt.Println(encode1(peerConnection.LocalDescription()))

	// 主循环
	select {}
}

func encode1(obj *webrtc.SessionDescription) string {
	b, err := json.Marshal(obj)
	if err != nil {
		panic(err)
	}

	return base64.StdEncoding.EncodeToString(b)
}

func decode1(in string, obj *webrtc.SessionDescription) {
	b, err := base64.StdEncoding.DecodeString(in)
	if err != nil {
		panic(err)
	}

	if err = json.Unmarshal(b, obj); err != nil {
		panic(err)
	}
}
```
