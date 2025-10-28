package main

import (
	"context"
	"fmt"
	"log"
	"log/slog"
	"math/rand"
	"time"

	"github.com/chromedp/cdproto/input"
	"github.com/chromedp/chromedp"
	"github.com/chromedp/chromedp/kb"
)

var keywords = []string{"playwright"}
func GetKeyword() string {
	return keywords[rand.Intn(len(keywords))]
}

// 模拟人类随机延迟
func humanDelay(min, max int) {
	delay := rand.Intn(max-min+1) + min
	time.Sleep(time.Duration(delay) * time.Millisecond)
}

// 随机输入速度模拟
func typeWithRandomSpeed(selector, text string) chromedp.Action {
	return chromedp.ActionFunc(func(ctx context.Context) error {
		for _, char := range text {
			if err := chromedp.SendKeys(selector, string(char)).Do(ctx); err != nil {
				return err
			}
			time.Sleep(time.Duration(rand.Intn(100)) * time.Millisecond)
		}
		return nil
	})
}

func main() {
	log.SetFlags(log.Lshortfile)
	keyword := GetKeyword()

	// 创建浏览器选项
	opts := append(chromedp.DefaultExecAllocatorOptions[:],
		chromedp.ExecPath("E:\\soft\\ungoogled-chromium_138.0.7204.183-1.1_windows_x64\\ungoogled-chromium_138.0.7204.183-1.1_windows_x64\\chrome.exe"),
		chromedp.Flag("headless", false),
	)

	// 创建上下文
	ctx, cancel := chromedp.NewExecAllocator(context.Background(), opts...)
	defer cancel()

	ctx, cancel = chromedp.NewContext(ctx)
	defer cancel()

	// 获取HTML内容
	var htmlContent string
	// 执行搜索流程
	err := chromedp.Run(ctx,
		// 访问Google
		chromedp.Navigate("https://www.google.com"),
		chromedp.WaitReady("body"),

		// 随机延迟
		chromedp.ActionFunc(func(ctx context.Context) error {
			humanDelay(1000, 3000)
			return nil
		}),

		// 随机移动鼠标
		chromedp.ActionFunc(func(ctx context.Context) error {
			x := rand.Float64()*800 + 200
			y := rand.Float64()*400 + 100
			return chromedp.MouseEvent(input.MouseMoved, x, y).Do(ctx)
		}),

		// 输入搜索词
		typeWithRandomSpeed("//form//textarea", keyword),
		chromedp.ActionFunc(func(ctx context.Context) error {
			humanDelay(500, 1500)
			return nil
		}),

		// 按回车键搜索
		chromedp.KeyEvent(kb.Enter),
		//chromedp.SendKeys("//form//textarea", kb.Enter, chromedp.ByQuery),
		chromedp.WaitReady("body"),
		chromedp.ActionFunc(func(ctx context.Context) error {
			humanDelay(2000, 5000)
			return nil
		}),
		chromedp.ActionFunc(func(ctx context.Context) error {
			return chromedp.OuterHTML("html", &htmlContent, chromedp.ByQuery).Do(ctx)
		}),
		// 随机滚动页面
		chromedp.ActionFunc(func(ctx context.Context) error {
			var scrollHeight int
			if err := chromedp.Evaluate("document.body.scrollHeight", &scrollHeight).Do(ctx); err != nil {
				return err
			}

			scrollStep := scrollHeight / (rand.Intn(10) + 5)
			for i := 0; i < scrollHeight; i += scrollStep {
				if err := chromedp.Evaluate(fmt.Sprintf("window.scrollTo(0, %d)", i), nil).Do(ctx); err != nil {
					return err
				}
				time.Sleep(100 * time.Millisecond)
			}
			return nil
		}),

		// // 随机点击结果
		// chromedp.ActionFunc(func(ctx context.Context) error {
		// 	var links []string
		// 	if err := chromedp.Evaluate(`Array.from(document.querySelectorAll('a h3')).map(el => el.parentElement.getAttribute('href'))`, &links).Do(ctx); err != nil {
		// 		return err
		// 	}

		// 	if len(links) > 0 {
		// 		humanDelay(1000, 3000)
		// 		randomLink := links[rand.Intn(len(links))]
		// 		return chromedp.Navigate(randomLink).Do(ctx)
		// 	}
		// 	return nil
		// }),
	)
	if len(htmlContent) > 10000 {
		slog.Info("HumanGoogleSearch success", "keyword", keyword)
		return
	}
	if err != nil {
		slog.Error("HumanGoogleSearch failed", "keyword", keyword, "error", err)
		return
	}
}
