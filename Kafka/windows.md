## 生成随机值
```shell
# 可以用 kafka-storage.bat random-uuid 命令生成一个随机 UUID：
.\bin\windows\kafka-storage.bat random-uuid
```
上述日志中有个错误提示可以忽略。记录此ID：4LBwTZK_QhCTbsmqDPw4lw

## 配置
`config\server.propertie`增加以下关键参数：

```conf
# 集群唯一标识（之前生成的 UUID）
cluster.id=4LBwTZK_QhCTbsmqDPw4lw
 
# 控制器选举配置（单机模式示例）
controller.quorum.voters=1@localhost:9093
```

## 格式化日志目录
```shell
# 然后复制这个 UUID 作为 -t 的值。
.\bin\windows\kafka-storage.bat format -t W-Gl4OprTxOYIcDnQcickg -c .\config\server.properties
#输出类似
Formatting metadata directory /tmp/kraft-combined-logs with metadata.version 3.9-IV0.
```

## 启动

```shell
.\bin\windows\kafka-server-start.bat .\config\server.properties
```
