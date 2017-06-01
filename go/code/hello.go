package main

// 导入单个包
import "fmt"

/* 导入多个包 */
import (
	"time"
    "runtime"
	"math/rand"
)

func main() {
    fmt.Println("Hellow World!", runtime.Version(), ", The time is", time.Now())

	// 设置种子
	rand.Seed(time.Now().UnixNano())
    fmt.Println("My favorite number is", rand.Intn(10))

	// 创建实例
	r := rand.New(rand.NewSource(time.Now().UnixNano()))
    fmt.Println("My favorite number is", r.Intn(10))
}
