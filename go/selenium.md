```go
package main

import (
	"fmt"
	"time"

	"github.com/tebeka/selenium"
)

func main() {
	// 启动Chrome浏览器
	caps := selenium.Capabilities{"browserName": "chrome"}
	wd, err := selenium.NewRemote(caps, "")
	if err != nil {
		fmt.Println("启动Chrome浏览器失败：", err)
		return
	}
	defer wd.Quit()

	// 打开网页
	err = wd.Get("
	if err != nil {
		fmt.Println("打开网页失败：", err)
		return
	}

	// 等待一段时间
	time.Sleep(5 * time.Second)
}
```
