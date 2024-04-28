目前Go语言支持GDB、LLDB和Delve几种调试器。其中GDB是最早支持的调试工具，LLDB是macOS系统推荐的标准调试工具。但是GDB和LLDB对Go语言的专有特性都缺乏很大支持，而只有Delve是专门为Go语言设计开发的调试工具。而且Delve本身也是采用Go语言开发，对Windows平台也提供了一样的支持。

## 打印堆栈
```go
package main

import (
    "fmt"
    "runtime/debug"
)

func test1() {
    test2()
}

func test2() {
    test3()
}

func test3() {
    fmt.Printf("%s", debug.Stack())
    debug.PrintStack()
}

func main() {
    test1()
}
```

## plan9 汇编


```shell
go tool compile -N -l -S main.go
```

plan9汇编简介
主要包括：文件命名、指令集、寄存器、函数声明、全局变量声明、跳转、栈分布、调用栈、编译/反编译工具

文件命名
由于不同的平台架构支持的汇编指令集不一样，需要针对不同的架构写不同的汇编实现。
通常文件命名格式：功能名_arch.s
比如：indexbyte_386.s, indexbyte_arm64.s, indexbyte_s390x.s
使用go build编译的时候，会自动根据当前arch平台使用对应的arch文件（或者使用+build tag）

指令集
这里自行去查找不同的架构指令集即可（cat /proc/cpuinfo | grep flags | head -1）

寄存器
有4个核心的伪寄存器，这4个寄存器是编译器用来维护上下文、特殊标识等作用的：
FP(Frame pointer): arguments and locals
PC(Program counter): jumps and branches
SB(Static base pointer): global symbols
SP(Stack pointer): top of stack

所有用户空间的数据都可以通过FP/SP(局部数据、输入参数、返回值)和SB(全局数据)访问。 通常情况下，不会对SB/FP寄存器进行运算操作，通常情况以会以SB/FP/SP作为基准地址，进行偏移解引用 等操作。

其中
1： SP有伪SP和硬件SP的区分：
伪SP： 本地变量最高起始地址
硬件SP： 函数栈真实栈顶地址
他们的关系是：

如果没有本地变量：伪SP=硬件SP+8
如果有本地变量：伪SP=硬件SP+16+本地变量空间大小
2： FP伪寄存器
FP伪寄存器： 用来标识函数参数、返回值
和伪SP寄存器的关系是：

如果有本地变量或者栈调用存严格split在关系（无NOSPLIT），伪FP=伪SP+16
否则，伪FP=伪SP+8
FP是访问入参、出参的基址，一般用正向偏移来寻址，SP是访问本地变量的起始基址，一般用负向偏移来寻址
修改硬件SP，会引起伪SP、FP同步变化
SUBQ $16, SP // 这里golang解引用时，伪SP/FP都会-16
3： 参数/本地变量访问
通过symbol+/-offset(FP/SP)的方式进行使用
例如arg0+0(FP)表示第函数第一个参数其实的位置，arg1+8(FP)表示函数参数偏移8byte的另一个参数。arg0/arg1用于助记，但是必须存在，否则无法通过编译（golang会识别并做处理）。
其中对于SP来说，还有一种访问方式：
+/-offset(FP)
这里SP前面没有symbol修饰，代表这硬件SP

4： PC寄存器
实际上就是在体系结构的知识中常见的pc寄存器，在x86平台下对应ip寄存器，amd64上则是rip。除了个别跳转 之外，手写代码与PC寄存器打交道的情况较少。

5： SB寄存器
SP是栈指针寄存器，指向当前函数栈的栈顶，通过symbol+offset(SP)的方式使用。offset 的合法取值是 [-framesize, 0)，注意是个左闭右开的区间。假如局部变量都是8字节，那么第一个局部变量就可以用localvar0-8(SP) 来表示。

6： BP寄存器
还有BP寄存器，是表示已给调用栈的起始栈底（栈的方向从大到小，SP表示栈顶）；一般用的不多，如果需要做手动维护调用栈关系，需要用到BP寄存器，手动split调用栈

7： 通用寄存器
在plan9汇编里还可以直接使用的amd64的通用寄存器，应用代码层面会用到的通用寄存器主要是: rax, rbx, rcx, rdx, rdi, rsi, r8~r15这14个寄存器。plan9中使用寄存器不需要带r或e的前缀，例如rax，只要写AX即可:

MOVQ $101, AX
