# 崩溃日志收集

## core dump
```shell
ulimit -c unlimited
echo "ulimit -c unlimited" >> ~/.profile
```

### 设置 core dump 文件的位置
`/etc/sysctl.conf` 设置
`kernel.core_pattern=/var/core/core%t%p_%e`
%t: 生成的文件的时间戳
%p:发生断言的进程的id
%e:发生断言的的代码文件

执行:
```shell
sysctl -p /etc/sysctl.conf
```

### 设置Go环境变量
```shell
export GOTRACEBACK=crash
echo "export GOTRACEBACK=crash " >> ~/.profile
```

### delve

https://github.com/go-delve/delve
```shell
go get -u github.com/derekparker/delve/cmd/dlv
dlv debug main.go
```
```gdb
(dlv) b main.main
(dlv) n #下一步
(dlv) s #step步入
(dlv) stepout #Step out of the current function.退出当前函数
(dlv) u #up 上一帧
(dlv) l #list
(dlv) p #print
(dlv) locals #Print local variables.打印本地变量
(dlv) args #Print function arguments.打印函数参数
(dlv) bp #Print out info for active breakpoints.打印激活的断点
(dlv) sources #Print list of source files.  打印源码列表
(dlv) bt #Print stack trace.打印调用堆栈
(dlv) whatis cmd   #Prints type of an expression.打印表达式的类型
```


## runtime/debug

```go
func ErrorPbResponse(errCode string, errMsg string) pb.Response {
    LogMessage("errcode[" + errCode + "] Errmsg:" + errMsg)
    
    if errCode == RESP_CODE_SYSTEM_ERROR {
        PrintStack() // 堆栈函数
    }
    // TODO
}
func PrintStack() {
    var buf [4096]byte
    n := runtime.Stack(buf[:], false)
    fmt.Printf("==> %s\n", string(buf[:n]))
}
```
```go
package main

import (
    "fmt"
    "os"
    "runtime/debug"
    "time"
)

func TryE() {
    errs := recover()
    if errs == nil {
        return
    }
    exeName := os.Args[0] // 获取程序名称

    now := time.Now()  // 获取当前时间
    pid := os.Getpid() // 获取进程 ID

    time_str := now.Format("20060102150405")                          // 设定时间格式
    fname := fmt.Sprintf("%s-%d-%s-dump.log", exeName, pid, time_str) // 保存错误信息文件名: 程序名 - 进程 ID - 当前时间 (年月日时分秒)
    fmt.Println("dump to file", fname)

    f, err := os.Create(fname)
    if err != nil {
        return
    }
    defer f.Close()

    f.WriteString(fmt.Sprintf("%v\r\n", errs)) // 输出 panic 信息
    f.WriteString("========\r\n")

    f.WriteString(string(debug.Stack())) // 输出堆栈信息
}
```

