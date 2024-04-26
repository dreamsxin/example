go env 用于查看和设置 Go 环境变量。

默认情况下 go env 输出格式为 Shell 脚本格式（如 Windows 上是 batch 文件格式）。如果指定变量名称，则只输出变量的值。

2.格式

go env [-json] [-u] [-w] [var ...]

其中 -json 表示以 JSON 格式输出而不是 Shell 脚本格式。

-u 表示 unset，取消环境变量。

-w 表示 write，设置环境变量。

要使其针对其他操作系统进行编译，请设置GOOS环境变量。

要使其针对其他体系结构（CPU）进行编译，请设置GOARCH环境变量。

要查看它们的当前值是什么，请运行go env。

GOOS的有效值：android，darwin，dragonfly，freebsd，js，linux，netbsd，openbsd，plan9，solaris，windows。

值darwin代表Mac OS和iOS。 值js代表Web汇编的编译。

GOARCH的有效值：arm，arm64、386，amd64，ppc64，ppc64le，mpis，mpisle，mps64，mips64le，s390x，wasm。

值386表示32位Intel兼容CPU，而amd64是64位Intel兼容CPU。 价值wasm适用于Web Assembly。

并非GOOS和GOARCH的所有组合都有效。 要查看所有可能的组合，请运行go tool dist list。

从Linux或Mac进行交叉编译的示例:

这是如何编译例如 对于Linux或Mac上的32位Windows：GOOS = windows GOARCH = 386 go build

这会将GOOS和GOOARCH临时设置为所需的值，然后运行go build。

我们还可以使用导出GOOS = windows在shell会话的生存期内设置它们。

从Windows交叉编译的示例:

在Windows上，当使用PowerShell并交叉编译到Linux和64位ARM处理器时：
$Env:GOOS = "linux"
$Env:GOARCH = "arm64"
go build
当心$Env:GOOS ="linux"在PowerShell会话期间设置GOOS变量，即它将影响所有将来的go build调用。

在Mac 或Linux上，交叉编译到Windows和64位AMD处理器时：(待验证)
go env -w SET GOOS= windows
go env -w SET GOARCH = amd64
go build
