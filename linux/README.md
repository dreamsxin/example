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

批量使用文件MD5值替换文件名

```shell
md5sum * | sed -e 's/\([^ ]*\) \(.*\(\..*\)\)$/mv -v \2 \1\3/' | sh
```

ISO 制作
```shell
mkisofs -r -o iso1.iso iso1/
sudo dd if=/dev/sr0 of=动物功夫系列.iso
```

# 查看连接数
```shell
netstat -n | awk '/^tcp/ {++S[$NF]} END {for(a in S) print a, S[a]}'
```

# 查看 apache 进程数
```shell
ps -ef | grep apache | wc -l
```

# wget 断点续传

```shell
# -O 指定文件名保存 -c 继续执行上次终端的任务
sudo wget -c -O 保存到本地的文件名 "下载地址"
```

压缩图片

```shell
sudo apt-get install optipng
optipng xxx.png
find ./images/ -iname *.png -print0 |xargs -0 optipng -o7
find . -name '*.png' | xargs optipng -nc -nb -o7 -full
find . -type f -name "*.png" -exec optipng {} \;

sudo apt-get install jpegoptim
jpegoptim xxx.jpg
# 用50%质量压缩图片:
jpegoptim -m50 xxx.jpg
find . -type f -name "*.jpg" -exec jpegoptim {} \;
```

# 查找指定大小的文件，并按大小排序
```shell
find . -type f -size +100M  -print0 | xargs -0 du  | sort -nr
```

# 查找占用空间最大的目录，并按大小排序
```shell
du -hm --max-depth=2 | sort -nr | head -12
```

# ssh 代理

```shell
ssh -C -v -N -D 127.0.0.1:7070 xxx@x.x.x.x -p 22022 -pw 密码
ssh -qTfnN -D 7070 xxx@x.x.x.x -p 22
ssh x.x.x.x -l username -p 22 -D 7070
```

# cat 

## cat 创建文件

```shell
cat > hello.txt << EOF
hello world
EOF
```

## cat 查看文件

```shell
cat hello.txt
```

## cat 合并文件

```shell
cat hello1.txt hello2.txt hello3.txt > helloall.txt
```

# more

more [参数选项] [文件]

参数如下：
- +num   从第num行开始显示；
- -num   定义屏幕大小，为num行；
- +/pattern   从pattern 前两行开始显示；
- -c   从顶部清屏然后显示；
- -d   提示Press space to continue, 'q' to quit.（按空格键继续，按q键退出），禁用响铃功能；
- -l    忽略Ctrl+l （换页）字符；
- -p    通过清除窗口而不是滚屏来对文件进行换页。和-c参数有点相似；
- -s    把连续的多个空行显示为一行；
- -u    把文件内容中的下划线去掉退出more的动作指令是q 

```shell
more -dc /etc/profile		// 显示提示，并从终端或控制台顶部显示；
more +4 /etc/profile		// 从profile的第4行开始显示；
more -4 /etc/profile		// 每屏显示4行；
more +/MAIL /etc/profile	// 从profile中的第一个MAIL单词的前两行开始显示；
```

## tail

- tail -f：当文件被删除或移走后，即使重新创建的文件也不会再出现新文件内容。
- tail -F：当文件删除或者移走后仍然追踪此文件，此时重新创建文件，会继续显示内容。

```shell
tail -f file
```

## less

进入less后，我们得学几个动作，这样更方便 我们查阅文件内容；最应该记住的命令就是q，这个能让less终止查看文件退出；

动作

- 回车键 向下移动一行；
- y 向上移动一行；
- 空格键 向下滚动一屏；
- b 向上滚动一屏；
- d 向下滚动半屏；
- h less的帮助；
- u 向上洋动半屏；
- w 可以指定显示哪行开始显示，是从指定数字的下一行显示；比如指定的是6，那就从第7行显示；
- g 跳到第一行；
- G 跳到最后一行；
- p n% 跳到n%，比如 10%，也就是说比整个文件内容的10%处开始显示；
- /pattern 搜索pattern ，比如 /MAIL表示在文件中搜索MAIL单词；
- v 调用vi编辑器；
- q 退出less
- !command 调用SHELL，可以运行命令；比如!ls 显示当前列当前目录下的所有文件；

执行命令 F（大写锁定），可以实现类似 tail -f 的效果。

## 修改用户主分组

```shell
usermod -g groupname username
```

## 修改用户次分组

```shell
usermod -G groupname username
```

## 追加用户次分组

```shell
gpasswd -a username groupname
usermod -a -G groupname username
```

## 改变文件所有者和组

```shell
chown username:groupname file
chown username file
```

## 改变文件权限

```shell
chmod u+x file			// 给file的属主增加执行权限
chmod 751 file			// 给file的属主分配读、写、执行(7)的权限，给file的所在组分配读、执行(5)的权限，给其他用户分配执行(1)的权限
chmod u=rwx,g=rx,o=x file	// 上例的另一种形式
chmod =r file			// 为所有用户分配读权限
chmod 444 file			// 同上例
chmod a-wx,a+r   file		// 同上例
chmod -R u+r directory		// 递归地给directory目录下所有文件和子目录的属主分配读的权限
chmod 4755			// 设置用ID，给属主分配读、写和执行权限，给组和其他用户分配读、执行的权限。
```

## 简体中文转繁体

使用 `cconv` 转换

```shell
#!/bin/bash
find . -type f -name "*.html"|while read line;do
echo $line
cconv -f utf-8 -t utf8-tw $line -o ${line}.zh_TW
mv $line ${line}.zh_CN
mv ${line}.zh_TW $line
done
find . -type f -name "*.zh_CN" -exec rm -f {} \;
```
