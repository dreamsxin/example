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
```

## 挂载分区到/var/www

```shell
mkdir /mnt/www
mount /dev/xdb1 /mnt/www
service apache2ctl stop 
mv /www/* /mnt/www
umount /dev/www
mount /dev/xdb1 /www 
```

## 实现开机自动挂在
编辑 `/etc/fstab`
```txt
/dev/xdb1   /var/www ext4    defaults    0  0
# or
/dev/xdb1   /var/www ext4    barrier=0  0  0
```
然后使用`cat /etc/fstab`命令查看，出现以下信息就表示写入成功。

执行 `mount -a` 挂载新分区