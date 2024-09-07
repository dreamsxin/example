
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
