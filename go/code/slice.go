package main

import "fmt"

func main() {
	// 空值
	var z []int
	fmt.Println(z, len(z), cap(z))
	if z == nil {
		fmt.Println("nil!")
	}

	// 创建切片
	p := []int{2, 3, 5, 7, 11, 13}
	fmt.Println("p ==", p)

	fmt.Println("p[1:4] ==", p[1:4])

	// 省略下标代表从 0 开始
	fmt.Println("p[:3] ==", p[:3])

	// 省略上标代表到 len(s) 结束
	fmt.Println("p[4:] ==", p[4:])

	// 从源数组中切片
	a := [10]int{1, 2, 3, 4, 5, 6, 7, 8, 9, 0}
	s := a[2:8]
	fmt.Println(s)

	for i := 0; i < len(p); i++ {
		fmt.Printf("p[%d] == %d\n", i, p[i])
	}

	// make函数第一个参数表示构建的数组的类型，第二个参数为数组的长度，第三个参数可选，是slice的容量，默认为第二个参数值。 
	s2 := make([]int, 10, 20)
	fmt.Println(s2) //输出：[0 0 0 0 0 0 0 0 0 0]
	fmt.Printf("len=%d cap=%d\n", len(s2), cap(s2))

	// 追加值
	s = append(s, 0)
	printSlice("s", s)

	// the slice grows as needed.
	s = append(s, 1)
	printSlice("s", s)

	// we can add more than one element at a time.
	s = append(s, 2, 3, 4)
	printSlice("s", s)

	// 删除值
	s = append(s[1:2], s[2:]...)
	printSlice("s", s)
}

func printSlice(s string, x []int) {
	fmt.Printf("%s len=%d cap=%d %v\n", s, len(x), cap(x), x)
}
