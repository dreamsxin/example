# Playwright
Playwright is a framework for Web Testing and Automation. It allows testing Chromium, Firefox and WebKit with a single API.

https://github.com/playwright-community/playwright-go


# 匹配 button 标签
- page.locator('button').click()
# 根据 id 匹配,匹配 id 为 container 的节点
- page.locator('#container').click()
# CSS伪类匹配，匹配可见的 button 按钮 
- page.locator("button:visible").click()
# :has-text 匹配任意内部包含指定文本的节点
- page.locator(':has-text("Playwright")').click()
# 匹配 article 标签内包含 products 文本的节点
- page.locator('article:has-text("products")').click()
# 匹配 article 标签下包含类名为 promo 的 div 标签的节点
- page.locator("article:has(div.promo)").click()

## 模拟登录
```go
package main

import (
	"encoding/json"
	"fmt"
	"log"
	"math/rand"
	"net/url"
	"os"
	"strings"
	"sync"
	"time"

	"github.com/dreamsxin/go-now"
	"github.com/playwright-community/playwright-go"
)

func GetRandInt(max int) int {
	return rand.Intn(max)
}

func RandSleep(t time.Duration) {
	time.Sleep(t + time.Duration(100+GetRandInt(999))*time.Millisecond)
}

func RequestHandle(request playwright.Request) {
	//fmt.Printf(">> %s %s\n", request.Method(), request.URL())
}
func ResponseHandle(response playwright.Response) {
	go func() {
		urlstr := response.URL()
		u, err := url.Parse(urlstr)
		if err != nil {
			return
		}
		if strings.HasPrefix(u.Path, "/api/products/shopify-products") {
			sdatestr := u.Query().Get("published_time_begin")
			fmt.Printf("sdatestr %s << %v %s\n", sdatestr, response.Status(), urlstr)

			t := time.Now()
			if sdatestr != "" {
				if response.Status() == 200 {
					bodystr, err := response.Body()
					if err == nil {
						dataChan <- Data{Urlstr: urlstr, Bodystr: string(bodystr)}
					}
				}
				t, _ = time.Parse(time.DateOnly, sdatestr)
				t = t.AddDate(0, -1, 0)
			}

			RandSleep(2 * time.Second)
			handle_selectdate(response.Frame().Page(), t)
		}
	}()

}

func handle_login(browserpage playwright.Page) {

	RandSleep(time.Millisecond)
	log.Println("输入账号名")

	err := browserpage.GetByPlaceholder("请输入您的邮箱").Click()
	if err != nil {
		log.Println("browserpage.GetByPlaceholder", err)
		return
	}
	browserpage.GetByPlaceholder("请输入您的邮箱").Fill("")

	err = browserpage.GetByPlaceholder("请输入您的密码").Click()
	if err != nil {
		log.Println("browserpage.GetByPlaceholder", err)
		return
	}
	browserpage.GetByPlaceholder("请输入您的密码").Fill("")

	RandSleep(time.Second)
	browserpage.Locator("#loginBut").Click()
}

func handle_selectdate(browserpage playwright.Page, t time.Time) {
	sdate := now.With(t).BeginningOfMonth()
	edate := now.With(t).EndOfMonth()

	log.Println("handle_selectdate", sdate.Format(time.DateOnly), edate.Format(time.DateOnly))

	RandSleep(time.Millisecond)

	err := browserpage.GetByPlaceholder("开始日期").Fill(sdate.Format(time.DateOnly))
	if err != nil {
		log.Println("browserpage.GetByPlaceholder", err)
		return
	}

	err = browserpage.GetByPlaceholder("结束日期").Fill(edate.Format(time.DateOnly))
	if err != nil {
		log.Println("browserpage.GetByPlaceholder", err)
		return
	}

	RandSleep(10 * time.Second)
	err = browserpage.GetByText("搜索").Click()
	if err != nil {
		log.Println("browserpage.GetByText", err)
		return
	}
}

func OnLoadHandle(browserpage playwright.Page) {
	urlstr := browserpage.URL()
	u, err := url.Parse(urlstr)
	if err != nil {
		return
	}
	if u.Host != "sellercenter.io" {
		return
	}
	if strings.HasPrefix(u.Path, "/cn/shopify-product") {
		log.Printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< page %s\n", browserpage.URL())
		go func() {
			RandSleep(time.Millisecond)
			err := browserpage.GetByText("现在登录").Click()
			if err != nil {
				log.Println("browserpage.GetByText", err)

				t := time.Now()
				handle_selectdate(browserpage, t)
				return
			}
			handle_login(browserpage)
		}()
	}
}

var syncgroup sync.WaitGroup

type Data struct {
	Urlstr  string
	Bodystr string
}

var dataChan chan Data

func main() {
	syncgroup.Add(1)
	dataChan = make(chan Data, 1000)

	file, err := os.OpenFile(fmt.Sprintf("./log%s.txt", time.Now().Format(time.DateOnly)), os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()
	go func() {
		// 循环读取 dataChan
		for data := range dataChan {

			b, err := json.Marshal(data)
			if err != nil {
				log.Println("json.Marshal", err)
				continue
			}
			// 追加内容
			file.Write(b)
			file.WriteString("\n")
		}
	}()
	err = playwright.Install()
	if err != nil {
		log.Fatalf("could not install playwright: %v", err)
	}
	pw, err := playwright.Run()
	if err != nil {
		log.Fatalf("could not start playwright: %v", err)
	}
	browser, err := pw.Chromium.Launch(playwright.BrowserTypeLaunchOptions{
		Headless: playwright.Bool(false),
		Timeout:  playwright.Float(60000),
	})
	if err != nil {
		log.Fatalf("could not launch browser: %v", err)
	}
	page, err := browser.NewPage()
	if err != nil {
		log.Fatalf("could not create page: %v", err)
	}
	page.SetDefaultTimeout(60000)
	page.OnRequest(RequestHandle)
	page.OnResponse(ResponseHandle)
	page.OnLoad(OnLoadHandle)
	if _, err = page.Goto("https://sellercenter.io/cn/shopify-product"); err != nil {
		log.Fatalf("could not goto: %v", err)
	}

	syncgroup.Wait()
	if err = browser.Close(); err != nil {
		log.Fatalf("could not close browser: %v", err)
	}
	if err = pw.Stop(); err != nil {
		log.Fatalf("could not stop Playwright: %v", err)
	}
}

```

## 下载视频

```go
//go:build ignore
// +build ignore

package main

import (
	"fmt"
	"log"

	"github.com/playwright-community/playwright-go"
)

func main() {
	pw, err := playwright.Run()
	if err != nil {
		log.Fatalf("could not launch playwright: %v", err)
	}
	browser, err := pw.Chromium.Launch()
	if err != nil {
		log.Fatalf("could not launch Chromium: %v", err)
	}
	page, err := browser.NewPage(playwright.BrowserNewPageOptions{
		RecordVideo: &playwright.RecordVideo{
			Dir: "videos/",
		},
	})
	if err != nil {
		log.Fatalf("could not create page: %v", err)
	}
	gotoPage := func(url string) {
		fmt.Printf("Visiting %s\n", url)
		if _, err = page.Goto(url); err != nil {
			log.Fatalf("could not goto: %v", err)
		}
		fmt.Printf("Visited %s\n", url)
	}
	gotoPage("https://playwright.dev")
	gotoPage("https://github.com")
	gotoPage("https://microsoft.com")
	if err := page.Close(); err != nil {
		log.Fatalf("failed to close page: %v", err)
	}
	path, err := page.Video().Path()
	if err != nil {
		log.Fatalf("failed to get video path: %v", err)
	}
	fmt.Printf("Saved to %s\n", path)
	if err = browser.Close(); err != nil {
		log.Fatalf("could not close browser: %v", err)
	}
	if err = pw.Stop(); err != nil {
		log.Fatalf("could not stop Playwright: %v", err)
	}
}
```

## 点击下载文件
```go
//go:build ignore
// +build ignore

package main

import (
	"fmt"
	"log"
	"net/http"

	"github.com/playwright-community/playwright-go"
)

func assertErrorToNilf(message string, err error) {
	if err != nil {
		log.Fatalf(message, err)
	}
}

func main() {
	startHttpServer()

	pw, err := playwright.Run()
	assertErrorToNilf("could not launch playwright: %w", err)
	browser, err := pw.Chromium.Launch(playwright.BrowserTypeLaunchOptions{
		Headless: playwright.Bool(true),
	})
	assertErrorToNilf("could not launch Chromium: %w", err)
	context, err := browser.NewContext()
	assertErrorToNilf("could not create context: %w", err)
	page, err := context.NewPage()
	assertErrorToNilf("could not create page: %w", err)
	_, err = page.Goto("http://localhost:1234")
	assertErrorToNilf("could not goto: %w", err)
	assertErrorToNilf("could not set content: %w", page.SetContent(`<a href="/download" download>download</a>`))
	download, err := page.ExpectDownload(func() error {
		return page.Locator("text=download").Click()
	})
	assertErrorToNilf("could not download: %w", err)
	fmt.Println(download.SuggestedFilename())
	assertErrorToNilf("could not close browser: %w", browser.Close())
	assertErrorToNilf("could not stop Playwright: %w", pw.Stop())
}

func startHttpServer() {
	http.HandleFunc("/download", func(w http.ResponseWriter, r *http.Request) {
		w.Header().Add("Content-Type", "application/octet-stream")
		w.Header().Add("Content-Disposition", "attachment; filename=file.txt")
		if _, err := w.Write([]byte("foobar")); err != nil {
			log.Printf("could not write: %v", err)
		}
	})
	go func() {
		log.Fatal(http.ListenAndServe(":1234", nil))
	}()
}
```
