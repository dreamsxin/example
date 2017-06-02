package main

import "fmt"

func main() {
	// 定义变量 a 是一个有2个字符串的数组
	var a [2]string
	a[0] = "Hello"
	a[1] = "World"
	fmt.Println(a[0], a[1])
	fmt.Println(a)
}