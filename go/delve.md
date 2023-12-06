# Delve

Delve 是一个 Go 语言的调试器，目标是给 Go 提供一个简单和特性齐全的调试工具，并且是容易调用和使用的。
 使用方法 跟 gdb 相同
## 安装
```shell
go get -u github.com/go-delve/delve/cmd/dlv
```

## 配置 `launch.json` 文件
```shell
mkdir .vscode
touch launch.json
```
```json
// .vscode/launch.json
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
      {
        "name": "Launch",
        "type": "go",
        "request": "launch",
        "mode": "auto",
        "program": "${fileDirname}",
        "env": {},
        "args": [],
        // 这里跳过go版本检查时因为我本地go版本有点高，所以跳过了
        "dlvFlags": ["--check-go-version=false"] 
      }
    ]
}
```

一般而言，vscode 左侧在点击 Run and Debug 后，如果没有配置文件，会有提示，让你创建 launch.json文件。

## 使用

1. 在需要查看的点位，打断点
2. 点击run and debug
3. continue / skip over / skip into / restart / stop

continue 只在断点停留
skip over 只在当前代码块-非常关键，不进入具体跳入函数中，用于关注当前上下文执行
skip into 调入函数执行细节中
restart 重新再次执行（丢弃本次）
stop 停止调试

## 实例

```shell
#查看占用cpu高的进程pid
top -d 1
#查看进城对应的线程
top -d 1 -p pid -H
#中断进程并附加进程
dlv attach pid
#通过线程id找到对应的协程编号，然后切换到协程goroutine命令 序号
grs gid
#然后打印堆栈信息
bt
```

