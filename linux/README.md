# Ubuntu 挂载数据盘

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
