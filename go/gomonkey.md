# 测试框架 GoMonkey
GoMonkey 通过汇编 mock 函数入口的一种测试框架。
函数运行时是会有入口地址的，可以通过 jmp 这样的汇编命令进行劫持的。
所以测试的时候需要关掉所有的内联，执行`go test -v`时加上参数`-gcflags=-l`（Go 1.10以下）或者`-gcflags=all=-l`（Go1.10及以上）。

## 安装

```shell
go get "github.com/agiledragon/gomonkey"
```
## 对全局变量进行打桩
GoMonkey对全局变量进行打桩和 GoStub 基本一致，此处仅贴出语法：
```go
// 对全局变量Num进行打桩
patch := gomonkey.ApplyGlobalVar(&Num,150)
defer patch.Reset()
```

## 对函数进行打桩
GoMonkey可以直接把函数作为参数对函数进行mock，我们还是拿我们熟悉的IsOdd做例子：
```go
package function
​
// IsOdd 判断一个数是否为奇数
func IsOdd(n int) bool {
    return n%2 != 0
}

func TestIsOdd(t *testing.T) {
    patch := gomonkey.ApplyFunc(IsOdd, func(_ int) bool {
        return true
    })
    defer patch.Reset()
    Convey("Testing IsOdd after gomonkey.ApplyFunc", t, func() {
        So(IsOdd(2), ShouldBeTrue)
    })
}
```
由于IsOdd很短，会被当作内联展开，我们需要显示指定禁用内联，执行：`go test -v -gcflags=all=-l`

## 对函数变量进行打桩

```go
package function
var JudgeNumFunc = JudgeNum
​
func JudgeNum() bool {
    return Num > 100
}
stub := gomonkey.ApplyFuncVar(&JudgeNumFunc, true)
defer stub.Reset()
```

## 对方法进行打桩
GoMonkey还可以对方法进行打桩，所谓方法，就是指一种type里面含有的一些函数，比如说我有这样的一个demo：
```go
package function
​
type MyString struct {
    str string
}
​
func (s *MyString) ReturnString() string {
    return s.str
}

func TestMyString_ReturnString(t *testing.T) {
    var temp *MyString
    patch := gomonkey.ApplyMethod(reflect.TypeOf(temp), "ReturnString", func(_ *MyString) string {
        return "hello,world!"
    })
    defer patch.Reset()
    Convey("ReturnString should return hello,world!", t, func() {
        var test *MyString
        test = new(MyString)
        So(test.ReturnString(), ShouldEqual, "hello,world!")
    })
}
```

## 打序列桩
有时候我们可能需要执行多个测试用例，那么我们就要频繁调用Apply***函数，看起来代码非常地不简洁，我们可以一次 mock 多个值。
```go
package function
​
// IsOdd 判断一个数是否为奇数
func IsOdd(n int) bool {
    return n%2 != 0
}

func TestIsOdd(t *testing.T) {
    Convey("TestApplyFuncSeq", t, func() {
        Convey("Test first seq", func() {
            output := []gomonkey.OutputCell{
                {Values: gomonkey.Params{true}},
                {Values: gomonkey.Params{false}},
            }
            patch := gomonkey.ApplyFuncSeq(IsOdd, output)
            defer patch.Reset()
            So(IsOdd(0), ShouldEqual, true)
            So(IsOdd(0), ShouldEqual, false)
        })
    })
}
```

