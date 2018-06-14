使用strace追踪多个进程

batch-strace.sh

```shell
#!/bin/bash

additional_strace_args="$1"

MASTER_PID=$(ps auwx | grep php-fpm | grep -v grep | grep 'master process'  | cut -d ' ' -f 6)

while read -r pid;
do
	if [[ $pid != $MASTER_PID ]]; then
		nohup strace -r -p "$pid" $additional_strace_args >"$pid.trc" 2>&1 &
	fi
done < <(pgrep php-fpm)
```

strace是Linux环境下的一款程序调试工具，用来监察一个应用程序所使用的系统调用及它所接收的系统信息。追踪程序运行时的整个生命周期，输出每一个系统调用的名字，参数，返回值和执行消耗的时间等。
strace常用参数：
-p 跟踪指定的进程
-f 跟踪由fork子进程系统调用
-F 尝试跟踪vfork子进程系统调吸入，与-f同时出现时, vfork不被跟踪
-o filename 默认strace将结果输出到stdout。通过-o可以将输出写入到filename文件中
-ff 常与-o选项一起使用，不同进程(子进程)产生的系统调用输出到filename.PID文件
-r 打印每一个系统调用的相对时间
-t 在输出中的每一行前加上时间信息。 -tt 时间确定到微秒级。还可以使用-ttt打印相对时间
-v 输出所有系统调用。默认情况下，一些频繁调用的系统调用不会输出
-s 指定每一行输出字符串的长度,默认是32。文件名一直全部输出
-c 统计每种系统调用所执行的时间，调用次数，出错次数。
-e expr 输出过滤器，通过表达式，可以过滤出掉你不想要输出

1. strace追踪多个进程方法：
当有多个子进程的情况下，比如php-fpm、nginx等，用strace追踪显得很不方便。可以使用下面的方法来追踪所有的子进程。

```shell
function straceall {
	strace $(pidof "${1}" | sed 's/\([0-9]*\)/-p \1/g')
}
```
Run
```run
source /root/.bashrc
traceall php-fpm
```

2. 追踪web服务器系统调用情况

```shell	
# strace -f -F -s 1024 -o nginx-strace /usr/local/nginx/sbin/nginx -c /usr/local/nginx/conf/nginx.conf
# strace -f -F -o php-fpm-strace /usr/local/php/sbin/php-fpm -y /usr/local/php/etc/php-fpm.conf
```

3. 追踪mysql执行语句

```shell	
# strace -f -F -ff -o mysqld-strace -s 1024 -p mysql_pid
# find ./ -name "mysqld-strace*" -type f -print |xargs grep -n "SELECT.*FROM"
```

4. whatisdong---查看程序在干啥

```shell
#!/bin/bash
# This script is from http://poormansprofiler.org/
nsamples=1
sleeptime=0
pid=$(pidof $1)

for x in $(seq 1 $nsamples)
  do
    gdb -ex "set pagination 0" -ex "thread apply all bt" -batch -p $pid
    sleep $sleeptime
  done | \
awk '
  BEGIN { s = ""; } 
  /^Thread/ { print s; s = ""; } 
  /^\#/ { if (s != "" ) { s = s "," $4} else { s = $4 } } 
  END { print s }' | \
sort | uniq -c | sort -r -n -k 1,1
```
