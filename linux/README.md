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
iptables -A FORWARD -s 192.168.1.159  -m limit --limit 30/s -j ACCEPT
iptables -A FORWARD -d 192.168.1.159  -m limit --limit 30/s -j ACCEPT
```

# 断开指定ip
```shell
sudo iptables -A FORWARD -d 192.168.1.159 -j DROP
sudo iptables -A FORWARD -s 192.168.1.159 -j DROP

sudo iptables -A INPUT -s 111.173.141.211 -p TCP -j DROP
# 指定优先级
sudo iptables -I FORWARD 2 -d 192.168.1.159 -j DROP
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