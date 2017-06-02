package main

import "fmt"

type Vertex struct {
	X int
	Y int
}

// 分配方式
var (
	v1 = Vertex{1, 2}  // 类型为 Vertex
	v2 = Vertex{X: 1}  // Y:0 被省略
	v3 = Vertex{}      // X:0 和 Y:0
	p1  = &Vertex{1, 2} // 类型为 *Vertex
)

func main() {
	fmt.Println(Vertex{1, 2})

	v := Vertex{1, 2}
	fmt.Printf("%#v\n", v)
	v.X = 4
	fmt.Println(v.X, v.Y)

	p := &v
	p.X = 1e9
	fmt.Println(v)

	fmt.Println(v1, p1, v2, v3)

	// 定义新的结构数组
	var vs = []struct {
		x string
		y string
		z int
	}{
		{"0", "0", 0},
		{"1", "1", 1},
		{"2", "2", 2},
	}

	fmt.Println(vs)

	for index, value := range vs {
		fmt.Println(index, value)
	}

	// 下划线表示那个值舍去，即舍去下标索引
	for _, value := range vs {
		fmt.Println(value)
	}
}