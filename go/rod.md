
```go
package main

import (
	"context"
	"crypto/rand"
	"encoding/base64"
	"fmt"
	"log"
	"net/http"

	"github.com/go-rod/rod"
	"github.com/go-rod/rod/lib/cdp"
)

func isValidChallengeKey(s string) bool {

	if s == "" {
		return false
	}
	decoded, err := base64.StdEncoding.DecodeString(s)
	return err == nil && len(decoded) == 16
}

var nonceKeySize = 16
var nonceSize = 24 // base64.StdEncoding.EncodedLen(nonceKeySize)
func initNonce(dst []byte) {
	// NOTE: bts does not escape.
	bts := make([]byte, nonceKeySize)
	if _, err := rand.Read(bts); err != nil {
		panic(fmt.Sprintf("rand read error: %s", err))
	}
	base64.StdEncoding.Encode(dst, bts)
}

func main() {
	u := "ws://192.168.0.1:9090?apiKey=xxxx"
	nonce := make([]byte, nonceSize)
	initNonce(nonce)
	header := http.Header{}
	header["Sec-WebSocket-Key"] = []string{string(nonce)}
	log.Println(header, isValidChallengeKey(string(nonce)))

	//rod.New().ControlURL(u).MustConnect().MustPage("https://example.com")
	browser := rod.New().Client(cdp.MustStartWithURL(context.TODO(), u, header)).MustConnect()
	page := browser.MustPage("https://example.com/")
	page.MustWaitStable().MustScreenshot("a.png")
	page.MustClose()
}
```
