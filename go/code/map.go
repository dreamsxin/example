package main

import "fmt"

type Vertex struct {
	Lat, Long float64
}

var m map[string]Vertex

func main() {
	m = make(map[string]Vertex)
	if m != nil {
		fmt.Println("not nil!")
	}
	m["Bell Labs"] = Vertex{
		40.68433, -74.39967,
	}
	fmt.Println(m["Bell Labs"])

	m2 := make(map[string]int)

	// 插入元素
	m2["Answer"] = 42
	fmt.Println("The value:", m2["Answer"])

	// 修改元素
	m2["Answer"] = 48
	fmt.Println("The value:", m2["Answer"])

	// 删除元素
	delete(m2, "Answer")
	fmt.Println("The value:", m2["Answer"])

	// 检测元素
	v, ok := m2["Answer"]
	fmt.Println("The value:", v, "Present?", ok)
}