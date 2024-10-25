
## 根据端口查找进程ID

```shell
netstat -ano | findstr 9222
```
## 根据进程ID查找进程名
```shell
tasklist | findstr 25720
```

## 杀死进程

```shell
taskkill  /PID 25720
taskkill  /f /PID 25720
```

## 查看进程

```shell
wmic process where caption="chrome.exe"
wmic process where caption="chrome.exe" get caption,commandline /value
wmic process 进程id get caption,commandline,ParentProcessId /value
wmic process where ParentProcessId=进程id
```

## ProcessMonitor

https://learn.microsoft.com/en-us/sysinternals/downloads/procmon#download
