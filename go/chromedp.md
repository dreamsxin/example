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
