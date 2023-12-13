```go
package main

import (
	"fmt"
	"time"

	"github.com/tebeka/selenium"
)

func main() {
	c := seleniumtest.Config{
			Path: *chromeBinary,
		}
	c.Browser = "chrome"
	//c.Headless = *headless

	selenium.SetDebug(true)
	c.ServiceOptions = append(c.ServiceOptions, selenium.Output(os.Stderr))

	port, err := pickUnusedPort()
	if err != nil {
		t.Fatalf("pickUnusedPort() returned error: %v", err)
	}
	c.Addr = fmt.Sprintf("http://127.0.0.1:%d/wd/hub", port)
        s, err = selenium.NewChromeDriverService(*chromeDriverPath, port, c.ServiceOptions...)
	// 启动Chrome浏览器
	caps := selenium.Capabilities{"browserName": "chrome"}
	wd, err := selenium.NewRemote(caps, "")
	if err != nil {
		fmt.Println("启动Chrome浏览器失败：", err)
		return
	}
	defer wd.Quit()

	// 打开网页
	err = wd.Get("www.baidu.com")
	if err != nil {
		fmt.Println("打开网页失败：", err)
		return
	}

	// 等待一段时间
	time.Sleep(5 * time.Second)
}
```
