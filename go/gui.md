
- Cogent Core
  https://github.com/cogentcore/core
- Fyne
  https://github.com/fyne-io/fyne

## Cogent Core

使用 OpenGL 的 GLFW 3 框架

- core\system\driver\desktop\window.go glfw.Window
- 
```go
package main

import (
	_ "embed"
	"log"

	"cogentcore.org/core/core"
	"cogentcore.org/core/icons"
)

//go:embed icon.svg
var AppIcon icons.Icon

func main() {
	core.AppIcon = string(AppIcon)
	log.Println(AppIcon)
	b := core.NewBody("GoLang")
	core.NewButton(b).SetText("Hello, World!")
	b.RunMainWindow()
}

```
