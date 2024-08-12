
- Cogent Core
  https://github.com/cogentcore/core
- Fyne
  https://github.com/fyne-io/fyne

## 例子

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
