内核日志查看方式汇总

1、系统启动后的信息和错误日志：

cat /var/log/message

2、守护进程启动和停止相关的日志消息

cat /var/log/boot.log

3、系统异常日志

cat /var/log/syslog

cat /var/log/kern

journalctl --system -f

journalctl -xe

4、内核缓冲区日志信息

dmesg

cat /proc/kmsg

5、Linux系统安全日志，记录用户和工作组变坏情况、用户登陆认证情况

cat /var/log/secure

cat /var/log/auth.log

cat /var/log/faillog

7、记录定时任务的日志

cat /var/log/cron

8、记录Linux登陆失败的用户、时间以及远程IP地址以及记录最后一次用户成功登陆的信息

cat /var/log/btmp ：

cat /var/log/lastlog
