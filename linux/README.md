# Ubuntu 挂载数据盘

## 查看 CPU 核数

# 总核数 = 物理CPU个数 X 每颗物理CPU的核数 
# 总逻辑CPU数 = 物理CPU个数 X 每颗物理CPU的核数 X 超线程数

# 查看物理CPU个数
cat /proc/cpuinfo| grep "physical id"| sort| uniq| wc -l

# 查看每个物理CPU中core的个数(即核数)
cat /proc/cpuinfo| grep "cpu cores"| uniq

# 查看逻辑CPU的个数
cat /proc/cpuinfo| grep "processor"| wc -

## 查看数据盘

```shell
fdisk -l
```

## 对数据库盘进行分区
```shell
fdisk /dev/xdb
```

根据提示，依次输入`n`、`p`、`1`，两次回车，`wq`，分区就开始了，很快就会完成。
使用“fdisk -l”命令可以看到，新的分区 xdb1 已经建立完成了。
此时也可以使用`df –h`命令，查看新分区大小。

## 格式化新分区
```shell
mkfs.ext4 /dev/vdb1
mkfs.ext4 /dev/vdc1
mkfs.ext4 /dev/vdc2
```

## 挂载分区到/var/www

```shell
mkdir /mnt/www
mount /dev/xdb1 /mnt/www
service apache2ctl stop 
mv /www/* /mnt/www
umount /mnt/www
mount /dev/xdb1 /www 

mount /dev/vdc1 /tmp

mkdir /mnt/log
mount /dev/vdc2 /mnt/log
mv /var/log/* /mnt/log
umount /mnt/log
mount /dev/vdc2 /var/log

mkfs.ext4 /dev/vdc3
mkdir /mnt/pg_xlog
mount /dev/vdc3 /mnt/pg_xlog
mv /var/log/* /mnt/pg_xlog
umount /mnt/pg_xlog
mount /dev/vdc3 /var/lib/postgresql/9.4/main/pg_xlog
```

## 实现开机自动挂在
编辑 `/etc/fstab`
```txt
/dev/xdb1   /var/www ext4    defaults    0  0
# or
/dev/xdb1   /var/www ext4    barrier=0  0  0

/dev/vdc1   /tmp   ext4    defaults        0 0
/dev/vdc2   /var/log   ext4    defaults        0 0
```
然后使用`cat /etc/fstab`命令查看，出现以下信息就表示写入成功。

执行 `mount -a` 挂载新分区

/dev/xdb1   /var/lib/postgresql ext4    barrier=0  0  0




# 网卡配置
```ini
auto lo
iface lo inet loopback

auto eth0
iface eth0 inet static
address 192.168.1.108
#gateway 192.168.1.108
netmask 255.255.255.0
dns-nameservers 220.248.192.12 220.248.192.13
mtu 1200

auto eth1
iface eth1 inet static
address 113.195.207.216
netmask 255.255.255.128
gateway 113.195.207.129
dns-nameservers 220.248.192.12 220.248.192.13
mtu 1200
```

# iptables 优先级

在前面的规则优先执行

# 限速
```shel
iptables -A FORWARD -s 192.168.1.159  -m limit --limit 100/s -j ACCEPT
iptables -A FORWARD -d 192.168.1.159  -m limit --limit 100/s -j ACCEPT
# 指定优先级
sudo iptables -I FORWARD 1 -s 192.168.1.159  -m limit --limit 100/s -j ACCEPT
sudo iptables -I FORWARD 1 -d 192.168.1.159  -m limit --limit 100/s -j ACCEPT
```

# 断开指定ip
```shell
sudo iptables -A FORWARD -d 192.168.1.159 -j DROP
sudo iptables -A FORWARD -s 192.168.1.159 -j DROP

sudo iptables -A INPUT -s 192.168.1.159 -p TCP -j DROP
sudo iptables -A INPUT -d 192.168.1.159 -p TCP -j DROP
# 指定优先级
sudo iptables -I FORWARD 1 -s 192.168.1.159 -j DROP
sudo iptables -I FORWARD 1 -d 192.168.1.159 -j DROP

sudo iptables -I INPUT 1 -s 192.168.1.159 -p TCP -j DROP
sudo iptables -I INPUT 1 -d 192.168.1.159 -p TCP -j DROP
```

## 显示规则编号
```shell
sudo iptables -L FORWARD --line-numbers
sudo iptables -L INPUT --line-numbers
```

## 删除规则
```shell
sudo iptables -D FORWARD 8
sudo iptables -D FORWARD 9

sudo iptables -D INPUT 21
```

## 记录用户操作

### 使用环境变量记录用户操作

在/etc/profile和/etc/bashrc中添加以下命令，并 source 一次
```shell
export PROMPT_COMMAND='{ date "+[ %Y%m%d %H:%M:%S `whoami` ] `history 1 | { read x cmd; echo "$cmd"; }`"; } >> /var/log/command.log'
```


这条命令会把登录用户所有按回车输入的内容都记录到command.log文件中去，即使是敲错的命令也一样；
要保证command.log文件所有用户都可写，权限定为222比较合适，这样其他用户只能写，但是查看不了内容。最后使用chattr命令防止用户自行修改记录或删除文件，

```shell
touch /var/log/command.log
chmod 722 /var/log/command.log
// chattr -a 可以解除
chattr +a /var/log/command.log
```

再配合logrotate定期进行log轮替。
`/etc/logrotate.conf`
```conf
/var/log/command.log {
	prerotate
		/usr/bin/chattr -a /var/log/command.log
	endscript
	compress
	delaycompress
	notifempty
	rotate 30
	size 10M
	create 0222 root root
	postrotate
		/usr/bin/chattr +a /var/log/command.log
	endscript
}
```

## 获取占用CPU资源最多的10个进程

```shell
ps aux|head -1;ps aux|grep -v PID|sort -rn -k +3|head
```

## 获取占用内存资源最多的10个进程
```shell
ps aux|head -1;ps aux|grep -v PID|sort -rn -k +4|head
```

命令组合解析（针对CPU的，MEN也同样道理）：
```shell
ps aux|head -1;ps aux|grep -v PID|sort -rn -k +3|head
```

该命令组合实际上是下面两句命令：
```shell
ps aux|head -1
ps aux|grep -v PID|sort -rn -k +3|head
```

查看占用cpu最高的进程
```shell
ps aux|head -1;ps aux|grep -v PID|sort -rn -k +3|head
```
或者top （然后按下M，注意这里是大写）

查看占用内存最高的进程
```shell
ps aux|head -1;ps aux|grep -v PID|sort -rn -k +4|head
```
或者top （然后按下P，注意这里是大写）

该命令组合实际上是下面两句命令：
```shell
ps aux|head -1
ps aux|grep -v PID|sort -rn -k +3|head
```
其中第一句主要是为了获取标题（USER PID %CPU %MEM VSZ RSS TTY STAT START TIME COMMAND）。
接下来的`grep -v PID`是将ps aux命令得到的标题去掉，即grep不包含PID这三个字母组合的行，再将其中结果使用sort排序。
`sort -rn -k +3`该命令中的-rn的r表示是结果倒序排列，n为以数值大小排序，而-k +3则是针对第3列的内容进行排序，再使用head命令获取默认前10行数据。(其中的|表示管道操作)

补充:内容解释

- PID：进程的ID
- USER：进程所有者
- PR：进程的优先级别，越小越优先被执行
- NInice：值
- VIRT：进程占用的虚拟内存
- RES：进程占用的物理内存
- SHR：进程使用的共享内存
- S：进程的状态。S表示休眠，R表示正在运行，Z表示僵死状态，N表示该进程优先值为负数
- %CPU：进程占用CPU的使用率
- %MEM：进程使用的物理内存和总内存的百分比
- TIME+：该进程启动后占用的总的CPU时间，即占用CPU使用时间的累加值。
- COMMAND：进程启动命令名称

## 可以使用以下命令查使用内存最多的K个进程

方法1：
```shell
ps -aux | sort -k4nr | head -K
```
如果是10个进程，K=10，如果是最高的三个，K=3

说明：`ps -aux`中
- a 指代all——所有的进程，
- u 指代userid——执行该进程的用户id
- x 指代显示所有程序，不以终端机来区分

`ps -aux`的输出格式如下：
```text
USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root         1  0.0  0.0  19352  1308 ?        Ss   Jul29   0:00 /sbin/init
root         2  0.0  0.0      0     0 ?        S    Jul29   0:00 [kthreadd]
root         3  0.0  0.0      0     0 ?        S    Jul29   0:11 [migration/0]
```
- sort -k4nr
k代表从第几个位置开始，后面的数字 4 即是其开始位置，结束位置如果没有，则默认到最后；
n指代numberic sort，根据其数值排序；
r指代reverse，这里是指反向比较结果，输出时默认从小到大，反向后从大到小。）。
本例中，可以看到%MEM在第4个位置，根据%MEM的数值进行由大到小的排序。

- head -K
K指代行数，即输出前几位的结果

- |
为管道符号，将查询出的结果导到下面的命令中进行下一步的操作。

方法2：
```shell
top # 然后按下M，注意大写
```

### 令查使用CPU最多的K个进程

方法1：
```shell
ps -aux | sort -k3nr | head -K
```
方法2：
```shell
top # 然后按下P，注意大写
```