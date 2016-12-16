# 安裝

```shell
sudo apt-get install gccgo-go
```

## Hello world

```go
package main

import (
    "fmt"
    "runtime"
)

func main() {
    fmt.Println("Hellow World!", runtime.Version())
}
```

## 解释执行

```shell
go run hello.go
```

## 编译执行

```shell
go build hello.go
./hello
# or
gccgo ./hello.go
./a.out
```
