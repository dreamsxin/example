package main

import (
	"encoding/json"
	"fmt"
	"log"
	"math/rand"
	"net/url"
	"os"
	"os/user"
	"path/filepath"
	"strings"
	"sync"
	"time"

	"github.com/dreamsxin/go-now"
	"github.com/go-resty/resty/v2"
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
			fmt.Printf("<<<<<<<<<<  %s %v %s\n", sdatestr, response.Status(), urlstr)

			//t := time.Now()
			if sdatestr != "" {
				if response.Status() == 200 {
					bodystr, err := response.Body()
					if err == nil {
						dataChan <- Data{Urlstr: urlstr, Bodystr: string(bodystr)}
					}
				}
				//t, _ = time.Parse(time.DateOnly, sdatestr)
				//t = t.AddDate(0, -1, 0)
			}
			//RandSleep(2 * time.Second)
			//handle_selectdate(response.Frame().Page(), t)
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
	browserpage.GetByPlaceholder("请输入您的邮箱").Fill("dreamsxin@gmail.com")

	err = browserpage.GetByPlaceholder("请输入您的密码").Click()
	if err != nil {
		log.Println("browserpage.GetByPlaceholder", err)
		return
	}
	browserpage.GetByPlaceholder("请输入您的密码").Fill("12345678")

	RandSleep(time.Second)
	browserpage.Locator("#loginBut").Click()
}

func handle_fetch(browserpage playwright.Page) {

	res, err := browserpage.Evaluate(`() => fetch('/api/shopify-plugin/best-selling?domain=https:%2F%2Fwww.gannikus-shop.com').then(r => r.text())")`)
	if err != nil {
		log.Println("----------------handle_fetch browserpage.Evaluate", err)
	}
	log.Printf("----------------handle_fetch browserpage.Evaluate %#v", res)

	client := resty.New().SetHeader("Accept", "application/json; charset=UTF-8").
		SetHeader("Accept-Language", "zh-CN,zh;q=0.9,en-US;q=0.8,en;q=0.7,zh-TW;q=0.6").
		SetHeader("Accept-Encoding", "gzip, deflate").
		SetHeader("Token", "xxxxx").
		SetHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36")

	resp, err := client.R().
		Get(`https://sellercenter.io/api/shopify-plugin/best-selling?domain=https:%2F%2Fwww.gannikus-shop.com`)
	//Get(`https://sellercenter.io/api/shopify-plugin/store-basic?domain=https:%2F%2Fwww.gannikus-shop.com`)

	if err != nil {
		log.Println("----------------handle_fetch resty err", err)
	}
	log.Printf("----------------handle_fetch resty Body %#v", string(resp.Body()))
}

func handle_getcookie(browserpage playwright.Page) {
	cookies, err := browserpage.Context().Cookies()
	if err != nil {
		log.Println("handle_getcookie", err)
		return
	}
	log.Printf("handle_getcookie %#v\n", cookies)
}

func handle_selectdate(browserpage playwright.Page, t time.Time) {
	sdate := now.With(t).BeginningOfMonth()
	edate := now.With(t).EndOfMonth()

	log.Println("handle_selectdate", sdate.Format(time.DateOnly), edate.Format(time.DateOnly))

	RandSleep(time.Millisecond)

	err := browserpage.GetByPlaceholder("开始日期").Click()
	if err != nil {
		log.Println("browserpage.Focus", err)
		return
	}
	err = browserpage.GetByPlaceholder("开始日期").Fill(sdate.Format(time.DateOnly))
	if err != nil {
		log.Println("browserpage.Fill", err)
		return
	}
	//browserpage.GetByPlaceholder("开始日期").Blur()

	RandSleep(100 * time.Millisecond)
	err = browserpage.GetByPlaceholder("结束日期").Click()
	if err != nil {
		log.Println("browserpage.Focus", err)
		return
	}
	err = browserpage.GetByPlaceholder("结束日期").Fill(edate.Format(time.DateOnly))
	if err != nil {
		log.Println("browserpage.Fill", err)
		return
	}
	err = browserpage.Locator("body").Click()
	if err != nil {
		log.Println("browserpage.body", err)
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
	log.Printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< OnLoadHandle page %s\n", browserpage.URL())
	if strings.HasPrefix(u.Path, "/cn/shopify-product") {
		go func() {
			isVisible, err := browserpage.GetByText("现在登录").IsVisible()
			if err != nil {
				log.Println("判断是否登录失败", err)
				return
			}
			if isVisible {
				err := browserpage.GetByText("现在登录").Click()
				if err != nil {
					log.Println("点击现在登录失败", err)
					return
				}
				handle_login(browserpage)
				return
			}
			RandSleep(time.Millisecond)
			handle_getcookie(browserpage)
			handle_fetch(browserpage)
			//handle_selectdate(browserpage, t)
		}()
	}
}

var syncgroup sync.WaitGroup

type Data struct {
	Urlstr  string
	Bodystr string
}

var dataChan chan Data
var browser playwright.Browser

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
	currentUser, err := user.Current()
	if err != nil {
		log.Println("user.Current", err)
		return
	}
	userdir := filepath.Join(currentUser.HomeDir, "AppData", "Local", "Chromium", "Usersellercenter")

	err = playwright.Install()
	if err != nil {
		log.Fatalf("could not install playwright: %v", err)
	}
	pw, err := playwright.Run()
	if err != nil {
		log.Fatalf("could not start playwright: %v", err)
	}
	// browser, err = pw.Chromium.Launch(playwright.BrowserTypeLaunchOptions{
	// 	Headless: playwright.Bool(false),
	// 	Timeout:  playwright.Float(60000),
	// })

	browser, err := pw.Chromium.LaunchPersistentContext(userdir, playwright.BrowserTypeLaunchPersistentContextOptions{
		Headless:          playwright.Bool(false),
		Timeout:           playwright.Float(60000),
		IgnoreDefaultArgs: []string{"--enable-automation"},
		Args:              []string{},
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
