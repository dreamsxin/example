```shell
sudo apt-get install sysdig
```

## 查看可用过滤器的完整列表

```shell
sysdig -l
```

**显示在“信息”级别 syslog 日志**

```shell
sudo sysdig syslog.severity.str=info
```
**过滤进程名**
```shell
sudo sysdig proc.name=nano
```
**过滤事件**
```shell
sudo sysdig -w sysdig-write-events.scap evt.type=write
```

**过滤ip**
```shell
sysdig -r sysdig-write-events.scap fd.rip!=11.11.11.11
```

## 写入到文件

```shell
sudo sysdig -w sysdig-trace-file.scap
sudo sysdig -n 300 -w sysdig-file.scap
```

连续保存到大小不超过1 MB的文件中，并且只保留最后五个文件(这是-W选项的作用)，请执行以下命令：

```shel
sudo sysdig -C 1 -W 5 -w sysdig-trace.scap
```

## 查看系统上可用的 `chisels` 工具列表**

```shell
sysdig -cl
```

## 使用 netstat 工具**

```shell
#查看帮助信息
sysdig -i netstat
sudo sysdig -c netstat
```

## 查看终端用户活动
```shell
sudo sysdig -c spy_users
```


