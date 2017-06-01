package main

import "fmt"

func main() {
	defer fmt.Println("world")

	fmt.Println("hello")

	i := 1
	// 此时确定了 i 值
	defer fmt.Println("Deferred print:", i)
	i++
	fmt.Println("Normal print:", i)

	fmt.Println("counting")

	for i := 0; i < 10; i++ {
		defer fmt.Println(i)
	}

	fmt.Println("done")
}