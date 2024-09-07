# chromedp
A faster, simpler way to drive browsers supporting the Chrome DevTools Protocol.

https://github.com/chromedp/chromedp

## 例子

### 
```go
func main() {
		opts := []chromedp.ExecAllocatorOption{
			chromedp.Flag("no-first-run", true),
			chromedp.Flag("headless", false),
			chromedp.Flag("auto-open-devtools-for-tabs", true),
		}
		allocCtx, _ := chromedp.NewExecAllocator(context.Background(), opts...)

		ctx, cancel = chromedp.NewContext(allocCtx)
		if err := chromedp.Run(ctx, chromedp.Navigate("about:blank")); err != nil {
			log.Fatalln(err)
		}
}
```
```go
func main() {
    opts := append(chromedp.DefaultExecAllocatorOptions[:],
        chromedp.DisableGPU,
        chromedp.UserDataDir("someUserDir"),
        chromedp.Flag("headless", false),
        chromedp.Flag("enable-automation", false),
        chromedp.Flag("restore-on-startup", false),
    )
    allocCtx, _ := chromedp.NewExecAllocator(context.Background(), opts...)

    ctx, _ := chromedp.NewContext(allocCtx)
    if err := chromedp.Run(ctx, chromedp.Navigate("about:blank")); err != nil {
        log.Fatalln(err)
    }

    for i := 0; i < 5; i++ {
        var res *runtime.RemoteObject
        if err := chromedp.Run(ctx, chromedp.Evaluate(`window.open("about:blank", "", "resizable,scrollbars,status")`, &res)); err != nil {
            log.Fatalln(err)
        }
        targets, err := chromedp.Targets(ctx)
        if err != nil {
            log.Fatalln(err)
        }
        for _, t := range targets {
            if !t.Attached {
                newCtx, _ := chromedp.NewContext(ctx, chromedp.WithTargetID(t.TargetID))
                if err := chromedp.Run(newCtx, chromedp.Navigate("https://example.com")); err != nil {
                    log.Fatalln(err)
                }
            }
        }
    }

    time.Sleep(time.Minute)
}
```

```go
package main

import (
	"context"
	"log"
	"strings"

	"github.com/chromedp/cdproto/cdp"
	"github.com/chromedp/chromedp"
)

func init() {
	log.SetFlags(log.LstdFlags | log.Lshortfile)
}

func main() {

	opts := chromedp.DefaultExecAllocatorOptions[:]
	ctx := context.Background()
	allocCtx, allocCancel := chromedp.NewExecAllocator(ctx, opts...)
	defer allocCancel()

	ctx, cancel := chromedp.NewContext(allocCtx)
	defer cancel()
	var nodes []*cdp.Node
	if err := chromedp.Run(ctx,
		chromedp.Navigate("https://github.com/wppconnect-team/wa-version/tree/main/html"),
		chromedp.WaitReady("body"),
		//chromedp.Sleep(1*time.Second),
		chromedp.Nodes("a", &nodes, chromedp.ByQueryAll),
	); err != nil {
		log.Fatalln(err)
		return
	}
	lasturl := ""
	for _, n := range nodes {
		urlstr := n.AttributeValue("href")
		//log.Println(urlstr)
		prefixstr := "/wppconnect-team/wa-version/blob/main/html"
		if strings.HasPrefix(urlstr, prefixstr) {
			lasturl = n.AttributeValue("title")
		}
	}
	log.Println(lasturl)
}

```

## Tab

```go
package main

import (
	"context"
	"log"

	"github.com/chromedp/chromedp"
)

func main() {
	allocatorContext, cancel := chromedp.NewRemoteAllocator(context.Background())
	defer cancel()
	ctx, cancel := chromedp.NewContext(allocatorContext)
	defer cancel()

	// get the list of the targets
	infos, err := chromedp.Targets(ctx)
	if err != nil {
		log.Fatal(err)
	}
	if len(infos) == 0 {
		log.Println("no targets")
		return
	}

	// create context attached to the specified target ID.
	// this example just uses the first target,
	// you can search for the one you want.
	tabCtx, cancel := chromedp.NewContext(ctx, chromedp.WithTargetID(infos[0].TargetID))
	defer cancel()

	if err := chromedp.Run(tabCtx, chromedp.Navigate("https://www.google.com/")); err != nil {
		log.Fatal(err)
	}
}
```

## 远程示例

- --remote-debugging-port=9222

```shell
google-chrome-stable --remote-debugging-protocol=9222 --user-data-dir=test
```

```shell
go run main.go -url ws://127.0.0.1:9222
```
```go
// Command remote is a chromedp example demonstrating how to connect to an
// existing Chrome DevTools instance using a remote WebSocket URL.
package main

import (
	"bytes"
	"context"
	"errors"
	"flag"
	"fmt"
	"image/png"
	"log"
	"os"
	"time"

	"github.com/chromedp/chromedp"
	"github.com/kenshaw/rasterm"
)

func main() {
	verbose := flag.Bool("v", false, "verbose")
	urlstr := flag.String("url", "ws://127.0.0.1:9222", "devtools url")
	nav := flag.String("nav", "https://www.duckduckgo.com/", "nav")
	d := flag.Duration("d", 1*time.Second, "wait duration")
	flag.Parse()
	if err := run(context.Background(), *verbose, *urlstr, *nav, *d); err != nil {
		fmt.Fprintf(os.Stderr, "error: %v\n", err)
		os.Exit(1)
	}
}

func run(ctx context.Context, verbose bool, urlstr, nav string, d time.Duration) error {
	if urlstr == "" {
		return errors.New("invalid remote devtools url")
	}
	// create allocator context for use with creating a browser context later
	allocatorContext, _ := chromedp.NewRemoteAllocator(context.Background(), urlstr)
	// defer cancel()

	// build context options
	var opts []chromedp.ContextOption
	if verbose {
		opts = append(opts, chromedp.WithDebugf(log.Printf))
	}

	// create context
	ctx, _ = chromedp.NewContext(allocatorContext, opts...)
	// defer cancel()

	// run task list
	var body string
	var buf []byte
	if err := chromedp.Run(ctx,
		chromedp.Navigate(nav),
		chromedp.Sleep(d),
		chromedp.OuterHTML("html", &body),
		chromedp.CaptureScreenshot(&buf),
	); err != nil {
		return fmt.Errorf("Failed getting body of %s: %v", nav, err)
	}
	fmt.Printf("Body of %s starts with:\n", nav)
	fmt.Println(body[0:100])
	img, err := png.Decode(bytes.NewReader(buf))
	if err != nil {
		return err
	}
	return rasterm.Encode(os.Stdout, img)
}
```
```shell
