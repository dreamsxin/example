package main

import "fmt"
import "math"

func add(x, y int) int {
	return x + y
}

func swap(x, y string) (string, string) {
	return y, x
}

func split(sum int) (x, y int) {
	// 按照5比4的比例分割
	x = sum * 4 / 9
	y = sum - x
	return
}

func main() {
	hypot := func(x, y float64) float64 {
		return math.Sqrt(x*x + y*y)
	}

	fmt.Println(hypot(3, 4))

	fmt.Println(add(42, 13))

	// 这里使用了，短声明变量 :=
	a, b := swap("hello", "world")
	fmt.Println(a, b)

	fmt.Println(split(17))
}
