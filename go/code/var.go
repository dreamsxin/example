package main

import "fmt"
import "reflect"

var i, j int = 1, 2

func main() {
	var c, python = true, false
	java := "no!"
	fmt.Println(i, j, c, python, java)

	var (
		empty_i int
		empty_f float32
		empty_b bool
		empty_s string
	)
	fmt.Println(empty_i, reflect.TypeOf(empty_i), empty_f, reflect.TypeOf(empty_f), empty_b, reflect.TypeOf(empty_b), empty_s, reflect.TypeOf(empty_s))
	fmt.Printf("%v %v %v %q\n", empty_i, empty_f, empty_b, empty_s)
}