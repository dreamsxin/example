package main

import (
	"log"
	"os"
	"runtime"

	"github.com/go-rod/rod"
	"github.com/go-rod/rod/lib/launcher"
	"github.com/go-rod/rod/lib/proto"
)

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
	configs := []struct {
		name     string
		language string
		timezone string
	}{
		{"中文-上海", "zh-CN", "Asia/Shanghai"},
		{"英文-纽约", "en-US", "America/New_York"},
		{"日文-东京", "ja-JP", "Asia/Tokyo"},
		{"德文-柏林", "de-DE", "Europe/Berlin"},
	}
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

	proxyURL := "http://proxy-server:8080"
	l := launcher.New().
		// 指定Chrome可执行文件路径
		Bin(chromePath). // 如果chromePath为空，则使用系统默认
		// 基本设置
		Headless(true).  // 无头模式
		Devtools(false). // 禁用开发者工具（生产环境）
		Leakless(true).  // 防止内存泄漏
		NoSandbox(true). // 禁用沙盒（Docker/Linux环境常用）

		Proxy(proxyURL).
		Set("lang", configs[0].language).
		Set("timezone", configs[0].timezone).
		UserDataDir(userDataDir) // 使用相同的目录
	if runtime.GOOS == "linux" {
		// l = l.XVFB("-screen", "0", "1280x720x24")
		l = l.XVFB("-screen", "0", "1920x1080x24", "-ac", "-nolisten", "tcp").
			// Linux特定参数
			Set("no-zygote", "true").                   // 禁用zygote进程
			Set("no-first-run", "true").                // 跳过首次运行
			Set("disable-dev-shm-usage", "true").       // 禁用/dev/shm使用（Docker环境重要）
			Set("disable-gpu", "true").                 // 禁用GPU（服务器环境）
			Set("disable-software-rasterizer", "true"). // 禁用软件光栅化器
			Set("disable-setuid-sandbox", "true").      // 禁用setuid沙盒
			Set("single-process", "false").             // 多进程模式
			Set("process-per-site", "false")            // 不按站点分进程
	}
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
	safeSetTimezone(page, "Asia/Shanghai")
	safeSetLanguage(page, "zh-CN")

	page.MustNavigate("https://example.com").MustWaitLoad()
	log.Println("页面加载完成")
}
