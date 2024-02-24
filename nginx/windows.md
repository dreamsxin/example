
## 下载
https://nginx.org/en/download.html

## 启动

```shell
start nginx
```
## 停止

```shell
tasklist |findstr "nginx"
taskkill /f /t /pid xxxx
taskkill /f /t /im nginx.exe
```

## 查看日志

```shell
Get-Content logs/error.log -Wait | Select-Object -Last 10
```
