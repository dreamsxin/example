## logging system

systemd 提供了自己的日志系统（logging system），称为 journal。使用 systemd 日志，无需额外安装日志服务（syslog）。读取日志的命令：

# journalctl 
​ 重要：显示所有的日志信息，notice或warning以粗体显示，红色显示error级别以上的信息

显示最后行数的日志：

# journalctl -n
显示最详细信息：

# journalctl -f
​ 提示：其实它很像tailf命令，默认显示十行。随着匹配日志的增长而持续输出。

只显示错误、冲突和重要告警信息

# journalctl -p err..alert
​ 提示：也可以使用数字表示哟。

显示指定单元的所有消息：

# journalctl -u netcfg
​ 重要：一般 -u 参数是 systemctl status 调用的参数之一（journalctl -l 可查看所有）

​ 提示：如果希望显示 kernel 的信息需要使用 journalctl -k 进行内核环缓存消息查询。

显示从某个时间 ( 例如 20分钟前 ) 的消息:

# journalctl --since "20 min ago"
# journalctl --since today
# journalctl --until YYYY-MM-DD 
显示本次启动后的所有日志：

# journalctl -b
不过，一般大家更关心的不是本次启动后的日志，而是上次启动时的（例如，刚刚系统崩溃了）。可以使用 -b 参数：

journalctl -b -0 显示本次启动的信息
journalctl -b -1 显示上次启动的信息
journalctl -b -2 显示上上次启动的信息
journalctl -b -2 只显示错误、冲突和重要告警信息
显示特定进程的所有消息:

# journalctl _PID=1
1. \_COMM 显示特定程序的所有消息，例如：``journalctl /usr/lib/systemd/systemd``
2. \_EXE 进程的可执行文件的路径
3. \_PID 进程的PID
4. \_UID 运行该进程用户的UID
5. _SYSTEMD_UNIT 启动该进程的 `systemd` 单元
​ 提示：以上筛选条件可组合使用，例如：journalctl _SYSTEMD_UNIT=sshd.service _PID=1182

显示更多输出方案：

# journalctl -o short|short-iso|short-percise|short-monotonic|verbose|export|json|json-pretty|json-sse|cat

##

`/usr/lib/systemd/system/tomcat9.service`

```txt
StandardOutput=syslog
StandardError=syslog
```

```shell
sudo systemctl edit --full tomcat9.service

tail -f /var/log/syslog
```
