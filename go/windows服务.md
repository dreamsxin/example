
- https://github.com/kardianos/service
- https://github.com/winsw/winsw/

```go
package main
 
import (
    "log"
    "os"
    "syscall"
    "time"
    "unsafe"
 
    "github.com/kardianos/service"
)
 
var (
    kernel32DLL       = syscall.NewLazyDLL("kernel32.dll")
    outputDebugString = kernel32DLL.NewProc("OutputDebugStringW")
)
 
func OutputDebugString(message string) {
    strPtr, _ := syscall.UTF16PtrFromString(message)
    outputDebugString.Call(uintptr(unsafe.Pointer(strPtr)))
}
 
// MyService 实现了 service.Service 接口
type MyService struct{}
 
func (m *MyService) Start(s service.Service) error {
    go m.run()
    return nil
}
 
func (m *MyService) run() {
    OutputDebugString("Service start...")
    // 在这里编写你的服务逻辑
    for {
        OutputDebugString("Service is running...")
        time.Sleep(1 * time.Second)
    }
}
 
func (m *MyService) Stop(s service.Service) error {
    // 停止服务的逻辑
    OutputDebugString("Service exit...")
    return nil
}
 
func main() {
 
    OutputDebugString("main starting...")
 
    // 服务的名称、显示名称和描述
    svcConfig := &service.Config{
        Name:        "MyGoService",
        DisplayName: "My Golang Service",
        Description: "This is a sample service by golang.",
    }
 
    prg := &MyService{}
    s, err := service.New(prg, svcConfig)
    if err != nil {
        log.Fatal(err)
    }
 
    // 如果服务已经安装，可以通过以下命令来启动、停止、重启或卸载服务：
    // service.exe install
    // service.exe start
    // service.exe stop
    // service.exe restart
    // service.exe uninstall
 
    // 通过以下代码来控制服务的启动和停止
    if len(os.Args) > 1 {
        err = service.Control(s, os.Args[1])
        if err != nil {
            log.Fatal(err)
        }
        return
    }
 
    err = s.Run()
    if err != nil {
        log.Fatal(err)
    }
}
```

**安装服务**

```shell
servicedemo.exe install
```

**启动服务**
```shell
servicedemo.exe start
```

**停止服务使用**
```shell
servicedemo.exe stop
```

**卸载服务使用**

```shell
servicedemo.exe uninstall
```

## bat

https://learn.microsoft.com/zh-cn/windows-server/administration/windows-commands/sc-create

### 安装
```bat
@echo off
set SERVICE_NAME=browser-agent
set EXE_ARGS="\"C:\cloud-browser\browser\browser.exe\" start"

REM 请求管理员权限
if not "%1"=="amAdmin" (
    powershell start -verb runas '%0' amAdmin
    exit /b
)

REM 创建服务
sc create %SERVICE_NAME% binPath= %EXE_ARGS% start= auto

REM 检查结果
if %errorlevel% equ 0 (
    echo 服务创建成功！可执行以下命令启动：
    echo net start %SERVICE_NAME%
) else (
    echo 服务创建失败，请检查路径和权限！
)
pause
```


