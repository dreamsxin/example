
- https://github.com/pion/stun
- https://github.com/pion/turn
- https://github.com/firefart/stunner
- https://github.com/ga666666-new/pion-stun-server

```go
package main

import (
	"github.com/pion/stun"
	"net"
)

func main() {
	conn, err := net.ListenPacket("udp", ":3478")
	if err != nil {
		panic(err)
	}
	defer conn.Close()

	buf := make([]byte, 1500)
	for {
		n, addr, err := conn.ReadFrom(buf)
		if err != nil {
			continue
		}

		msg := &stun.Message{Raw: buf[:n]}
		if err := msg.Decode(); err != nil {
			continue
		}

		res := stun.MustBuild(
			stun.NewType(stun.MethodBinding, stun.ClassSuccessResponse),
			stun.TransactionID,
			stun.XORMappedAddress{IP: addr.(*net.UDPAddr).IP, Port: addr.(*net.UDPAddr).Port},
		)

		conn.WriteTo(res.Raw, addr)
	}
}
```

- https://github.com/pion/stun
```go
package main

import (
	"fmt"

	"github.com/pion/stun"
)

func main() {
	// Parse a STUN URI
	u, err := stun.ParseURI("stun:localhost:3478")
	if err != nil {
		panic(err)
	}

	// Creating a "connection" to STUN server.
	c, err := stun.DialURI(u, &stun.DialConfig{})
	if err != nil {
		panic(err)
	}
	// Building binding request with random transaction id.
	message := stun.MustBuild(stun.TransactionID, stun.BindingRequest)
	// Sending request to STUN server, waiting for response message.
	if err := c.Do(message, func(res stun.Event) {
		if res.Error != nil {
			panic(res.Error)
		}
		// Decoding XOR-MAPPED-ADDRESS attribute from message.
		var xorAddr stun.XORMappedAddress
		if err := xorAddr.GetFrom(res.Message); err != nil {
			panic(err)
		}
		fmt.Println("your IP is", xorAddr.IP)
	}); err != nil {
		panic(err)
	}
}
```
