# Too many open files

查看系统

```shell
ulimit -a
```

需要调大限制
```shell
ulimit -u 65535
ulimit -n 65535
```

```shell
lsof|awk '{print $2}'|wc -l
lsof|grep tomcat|wc -l
```

/etc/profile：
```shell
ulimit -u 65535
ulimit -SHn 65535 
```

前两句是修改参数 最后一句是生效
修改内核参数
```shell
/etc/sysctl.conf  
echo "fs.file-max=65536" >> /etc/sysctl.conf  
sysctl -p
```

`/etc/security/limits.conf`
```shell
*        soft    noproc 65535
*        hard    noproc 65535
*        soft    nofile 65535
*        hard    nofile 65535
```

* ulimit 参数说明

```txt
选项 [options]	含义	例子
-H 	设置硬资源限制，一旦设置不能增加。 	ulimit – Hs 64；限制硬资源，线程栈大小为 64K。
-S 	设置软资源限制，设置后可以增加，但是不能超过硬资源设置。 	ulimit – Sn 32；限制软资源，32 个文件描述符。
-a 	显示当前所有的 limit 信息。 	ulimit – a；显示当前所有的 limit 信息。
-c 	最大的 core 文件的大小， 以 blocks 为单位。 	ulimit – c unlimited； 对生成的 core 文件的大小不进行限制。
-d 	进程最大的数据段的大小，以 Kbytes 为单位。 	ulimit -d unlimited；对进程的数据段大小不进行限制。
-f 	进程可以创建文件的最大值，以 blocks 为单位。 	ulimit – f 2048；限制进程可以创建的最大文件大小为 2048 blocks。
-l 	最大可加锁内存大小，以 Kbytes 为单位。 	ulimit – l 32；限制最大可加锁内存大小为 32 Kbytes。
-m 	最大内存大小，以 Kbytes 为单位。 	ulimit – m unlimited；对最大内存不进行限制。
-n 	可以打开最大文件描述符的数量。 	ulimit – n 128；限制最大可以使用 128 个文件描述符。
-p 	管道缓冲区的大小，以 Kbytes 为单位。 	ulimit – p 512；限制管道缓冲区的大小为 512 Kbytes。
-s 	线程栈大小，以 Kbytes 为单位。 	ulimit – s 512；限制线程栈的大小为 512 Kbytes。
-t 	最大的 CPU 占用时间，以秒为单位。 	ulimit – t unlimited；对最大的 CPU 占用时间不进行限制。
-u 	用户最大可用的进程数。 	ulimit – u 64；限制用户最多可以使用 64 个进程。
-v 	进程最大可用的虚拟内存，以 Kbytes 为单位。 	ulimit – v 200000；限制最大可用的虚拟内存为 200000 Kbytes。
```

## 11: Resource temporarily unavailable

```shell
echo 'net.core.somaxconn = 2048' >> /etc/sysctl.conf
sysctl -p
# or
sudo sysctl -w net.core.somaxconn=1024
```

PHP FPM
```conf
listen.backlog = 2048
```



* 文件预读
```shell
blockdev --getra /dev/sda
```
设置
```shell
blockdev --setra 4096 /dev/sda
```
将命令添加至`rc.local`

* `sysctl.conf` 其它选项

- vm.swappiness = 0         # 0有物理内存的情况下就不使用文件交换， 10为积极使用swapper 中间以此类推
- vm.overcommit_memory = 1  # 1最大化分配物理内存，2为可超过物理加虚拟内存，0为内存溢出就返回至应用
- fs.file-max = 655350　　# 系统文件描述符总量
- net.ipv4.ip_local_port_range = 1024 65535　　# 打开端口范围
- net.ipv4.tcp_max_tw_buckets = 2000　　# 设置tcp连接时TIME_WAIT个数
- net.ipv4.tcp_tw_recycle = 1　　# 开启快速tcp TIME_WAIT快速回收
- net.ipv4.tcp_tw_reuse = 1　　# 开启TIME_WAIT重用
- net.ipv4.tcp_syncookies = 1　　# 开启SYN cookies 当出现syn等待溢出，启用cookies来处理，可防范少量的syn攻击
- net.ipv4.tcp_syn_retries = 2　　# 对于一个新建的tcp连接，内核要发送几个SYN连接请求才决定放弃
- net.ipv4.tcp_synack_retries = 2　　# 这里是三次握手的第二次连接，服务器端发送syn+ack响应 这里决定内核发送次数
- net.ipv4.tcp_keepalive_time = 1200　　# tcp的长连接，这里注意：tcp的长连接与HTTP的长连接不同
- net.ipv4.tcp_fin_timeout = 15　　  # 设置保持在FIN_WAIT_2状态的时间
- net.ipv4.tcp_max_syn_backlog = 20000　　# tcp半连接最大限制数
- net.core.somaxconn = 65535　　# 定义一个监听最大的队列数
- net.core.netdev_max_backlog = 65535　　# 当网络接口比内核处理数据包速度快时，允许送到队列数据包的最大数目