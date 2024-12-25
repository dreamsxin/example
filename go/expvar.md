# expvar
提供了一种输出应用内部状态信息的标准化方案。
```go
package main

import (
	"expvar"
	"fmt"
	"net/http"
)

// 自定义性能指标
var intVar *expvar.Int
var mapVar *expvar.Map

// 自定义结构体，用于导出性能指标
type CustomStuct struct {
	Field1 int64   `json:"field1"`
	Field2 float64 `json:"field2"`
}

var (
	field1 expvar.Int
	field2 expvar.Float
)

func exportStruct() interface{} {
	return CustomStuct{
		Field1: field1.Value(),
		Field2: field2.Value(),
	}
}

func init() {
	intVar = expvar.NewInt("intVar")
	intVar.Set(17)

	mapVar = expvar.NewMap("mapVar")

	mapVar.Set("field1", &expvar.Int{})
	mapVar.Set("field2", &expvar.Int{})

	mapVar.Add("field1", 1)
	mapVar.AddFloat("field2", 0.001)

	expvar.Publish("customStuct", expvar.Func(exportStruct)) // 强制转换类型为 type Func func() any
	field1.Add(2)
	field2.Add(0.002)
}

func main() {
	mux := http.NewServeMux()
	mux.Handle("/hi", http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		w.Write([]byte("hi"))
	}))
	mux.Handle("/debug/vars", expvar.Handler()) // 注册 expvar 处理器，将会输出所有性能指标，包括自定义的性能指标
	fmt.Println(http.ListenAndServe("localhost:8080", mux))
}

```
