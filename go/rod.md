
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

## 模拟人类

```go
package main

import (
	"log"
	"math/rand"
	"os"
	"runtime"
	"time"

	"github.com/go-rod/rod"
	"github.com/go-rod/rod/lib/input"
	"github.com/go-rod/rod/lib/launcher"
	"github.com/go-rod/rod/lib/proto"
	"github.com/go-rod/rod/lib/utils"
)

// 随机延迟函数
func randomDelay(min, max int) {
	delay := rand.Intn(max-min+1) + min
	log.Printf("随机延迟 %d 毫秒", delay)
	time.Sleep(time.Duration(delay) * time.Millisecond)
}

// 人类化输入函数 - 模拟真实打字速度
func humanType(element *rod.Element, text string) {
	log.Printf("开始输入文本: %s", text)
	for _, char := range text {
		element.MustInput(string(char))
		// 随机字符间隔 50-200ms
		delay := rand.Intn(150) + 50
		time.Sleep(time.Duration(delay) * time.Millisecond)
	}
	log.Println("输入完成")
}

// 鼠标移动函数 - 模拟人类鼠标移动
func humanMouseMove(page *rod.Page, x, y float64) {
	log.Printf("移动鼠标到坐标: %.1f, %.1f", x, y)
	page.Mouse.MoveTo(proto.Point{X: x, Y: y})
	// 随机停留时间 100-300ms
	delay := rand.Intn(200) + 100
	time.Sleep(time.Duration(delay) * time.Millisecond)
}

// 页面滚动函数 - 模拟人类滚动行为
func humanScroll(page *rod.Page, direction string, distance int) {
	log.Printf("页面滚动: %s %d 像素", direction, distance)

	switch direction {
	case "down":
		page.Mouse.Scroll(0, -float64(distance), 0)
	case "up":
		page.Mouse.Scroll(0, float64(distance), 0)
	case "left":
		page.Mouse.Scroll(float64(distance), 0, 0)
	case "right":
		page.Mouse.Scroll(-float64(distance), 0, 0)
	}

	// 滚动后随机等待 500-1500ms
	delay := rand.Intn(1000) + 500
	time.Sleep(time.Duration(delay) * time.Millisecond)
}

// 随机点击函数 - 随机点击搜索结果
func randomClickSearchResult(page *rod.Page) {
	// 等待搜索结果加载
	randomDelay(1000, 2000)

	// 查找搜索结果链接
	links, err := page.Elements("h3")
	if err != nil || len(links) == 0 {
		log.Println("未找到搜索结果")
		return
	}

	// 随机选择一个结果（前5个结果中）
	maxResults := len(links)
	if maxResults > 5 {
		maxResults = 5
	}

	randomIndex := rand.Intn(maxResults)
	log.Printf("随机点击第 %d 个搜索结果", randomIndex+1)

	// 滚动到元素位置
	links[randomIndex].MustScrollIntoView()
	randomDelay(500, 1000)

	// 点击元素
	links[randomIndex].MustClick()
	log.Println("点击搜索结果完成")

	// 等待页面加载
	randomDelay(2000, 3000)
}

// 模拟人类搜索行为
func simulateHumanSearch(page *rod.Page, searchTerm string) {
	log.Println("开始模拟人类搜索行为")

	// 查找搜索框
	//searchBox, err := page.Element("#APjFqb")
	searchBox, err := page.ElementX("//form//textarea")
	if err != nil {
		log.Printf("查找搜索框失败: %v", err)
		return
	}

	// 点击搜索框
	searchBox.MustClick()
	randomDelay(300, 800)

	// 模拟鼠标移动
	pageWidth := 1920.0
	pageHeight := 1080.0
	for i := 0; i < 3; i++ {
		x := rand.Float64() * pageWidth
		y := rand.Float64() * pageHeight
		humanMouseMove(page, x, y)
	}

	// 人类化输入搜索词
	humanType(searchBox, searchTerm)

	// 随机延迟后按回车
	randomDelay(500, 1500)
	page.Keyboard.Press(input.Enter)
	//page.Keyboard.Release(input.Enter)

	log.Println("搜索操作完成")
}

func safeSetTimezone(page *rod.Page, timezone string) {
	err := proto.EmulationSetTimezoneOverride{
		TimezoneID: timezone,
	}.Call(page)
	if err != nil {
		log.Printf("设置时区失败 %s: %v", timezone, err)
		// 尝试备用时区
		if timezone == "Asia/Shanghai" {
			_ = proto.EmulationSetTimezoneOverride{
				TimezoneID: "UTC",
			}.Call(page)
		}
	}
}

func safeSetGeolocation(page *rod.Page, Latitude, Longitude *float64) {
	// 设置地理位置
	err := proto.EmulationSetGeolocationOverride{
		Latitude:  Latitude,
		Longitude: Longitude,
		//Accuracy:  100,
	}.Call(page)
	if err != nil {
		log.Printf("设置地理位置失败 %f, %f: %v", Latitude, Longitude, err)
	}
}

func safeSetLanguage(page *rod.Page, language string) {
	err := proto.EmulationSetUserAgentOverride{
		//UserAgent:      "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36",
		AcceptLanguage: language,
	}.Call(page)
	if err != nil {
		log.Printf("设置语言失败 %s: %v", language, err)
	}
}

// 获取Chrome可执行文件路径
func getChromePath() string {
	switch runtime.GOOS {
	case "windows":
		// Windows系统常见的Chrome安装路径
		paths := []string{
			`.\chromium\chrome.exe`,
			`C:\Program Files\Google\Chrome\Application\chrome.exe`,
			`C:\Program Files (x86)\Google\Chrome\Application\chrome.exe`,
			`%LOCALAPPDATA%\Google\Chrome\Application\chrome.exe`,
		}
		for _, path := range paths {
			expandedPath := os.ExpandEnv(path)
			if _, err := os.Stat(expandedPath); err == nil {
				return expandedPath
			}
		}
	case "linux":
		// Linux系统常见的Chrome安装路径
		paths := []string{
			"/usr/bin/google-chrome",
			"/usr/bin/google-chrome-stable",
			"/usr/bin/chromium",
			"/usr/bin/chromium-browser",
			"/snap/bin/chromium",
		}
		for _, path := range paths {
			if _, err := os.Stat(path); err == nil {
				return path
			}
		}
	case "darwin":
		// macOS系统常见的Chrome安装路径
		paths := []string{
			"/Applications/Google Chrome.app/Contents/MacOS/Google Chrome",
			"/Applications/Chromium.app/Contents/MacOS/Chromium",
		}
		for _, path := range paths {
			if _, err := os.Stat(path); err == nil {
				return path
			}
		}
	}
	return "" // 返回空字符串表示使用系统默认
}

func main() {
	// 初始化随机数种子
	rand.Seed(time.Now().UnixNano())

	// 创建用户数据目录
	userDataDir, err := os.MkdirTemp("", "rod-user-data-*")
	if err != nil {
		log.Fatal(err)
	}
	defer os.RemoveAll(userDataDir)

	// 方法1: 自动检测Chrome路径
	chromePath := getChromePath()
	if chromePath != "" {
		log.Printf("检测到Chrome路径: %s", chromePath)
	} else {
		log.Println("未检测到Chrome，将使用系统默认浏览器")
	}

	//proxyURL := "http://proxy-server:8080"
	l := launcher.New().
		// 指定Chrome可执行文件路径
		Bin(chromePath). // 如果chromePath为空，则使用系统默认
		// 基本设置
		Headless(false). // 无头模式
		Devtools(false). // 禁用开发者工具（生产环境）
		Leakless(false). // 防止内存泄漏
		//NoSandbox(true). // 禁用沙盒（Docker/Linux环境常用）

		//Proxy(proxyURL).
		//Set("lang", configs[0].language).
		//Set("timezone", configs[0].timezone).
		UserDataDir(userDataDir) // 使用相同的目录

	controlURL, err := l.Launch()
	if err != nil {
		log.Fatal(err)
	}

	browser := rod.New().ControlURL(controlURL)
	err = browser.Connect()
	if err != nil {
		log.Fatal(err)
	}
	defer browser.MustClose()

	// 获取浏览器版本信息
	version, err := browser.Version()
	if err != nil {
		log.Printf("获取浏览器版本失败: %v", err)
	} else {
		log.Printf("浏览器版本: %s", version.Product)
	}

	page := browser.MustPage()

	// 安全地设置时区和语言
	// safeSetTimezone(page, "Asia/Shanghai")
	// safeSetLanguage(page, "zh-CN")

	// 导航到Google
	page.MustNavigate("https://www.google.com").MustWaitLoad()
	log.Println("页面加载完成")

	// 随机延迟 1-3秒
	randomDelay(1000, 3000)

	// 模拟人类搜索行为
	simulateHumanSearch(page, "golang rod browser automation")

	// 模拟页面滚动
	randomDelay(1000, 2000)
	humanScroll(page, "down", 300)
	randomDelay(500, 1000)
	humanScroll(page, "down", 200)
	randomDelay(500, 1000)

	// 随机点击搜索结果
	randomClickSearchResult(page)

	// 继续浏览行为 - 模拟阅读时间
	randomDelay(3000, 5000)

	// 模拟继续滚动
	humanScroll(page, "down", 400)
	randomDelay(1000, 2000)

	log.Println("人类化访问行为模拟完成")

	// 等待一段时间让页面完全加载
	utils.Sleep(2)
}
```
