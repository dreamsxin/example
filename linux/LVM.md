# 备份

* 记录分区块的位置
```shell
fdisk -l
```
`/etc/fstab`和`/boot/grub/menu.lst(grub.cfg)`里面的UUID

* 备份主引导
```shell
# 备份拷贝到U盘
dd if=/dev/sda of=/xxx/mbr bs=1 count=512

# 用0填充破坏
dd if=/dev/zero of=/dev/sda bs=1 count=512
hexdump -C -n 512 /dev/sda
# 进入恢复模式
dd if=/xxx/mbr of=/dev/sda bs=1 count=512  
```
`sfdisk/sgdisk`
```shell
# 备份分区表:
sfdisk -d /dev/sda > sda-partition.backup
# 恢复分区表
cat sda-partition.backup | sfdisk -f /dev/sda
# 同理，克隆sda分区表到sdb(需要两块盘一样)
sfdisk -d /dev/sda | sfdisk -f /dev/sdb
```

恢复分区后`vgchange -ay` 就能识别到lvm分区了

恢复的步骤
1、恢复分区表；
2、vgchange -ay 识别所有lvm；
3、恢复引导记录
4、挂载根分区
5、将根分区的内容解压到挂在点；
6、umount && reboot
7、对比下/etc/fstab和/boot/grub/menu.lst(grub.cfg)

```shell
vgextend VolGroup /dev/sda3
vgextend VolGroup /dev/sda3
lvresize/lvextend -L +20G /dev/mapper/VolGroup-lv_root
resize2fs /dev/mapper/VolGroup-lv_root
vgreduce VolGroup /dev/sda3
pvmove /dev/sda3
```
  
  Size of logical volume VolGroup/lv_root changed from 17.51 GiB (4482 extents) to 36.51 GiB (9346 extents).
  Limit for the maximum number of semaphores reached. You can check and set the limits in /proc/sys/kernel/sem.
  Unable to resume VolGroup-lv_root (253:0)
  Problem reactivating logical volume VolGroup/lv_root.

  You can face this problem after trying to resize your logical volume partitions:

lvresize -L +2G /dev/mapper/VolGroup-lv_root
Extending logical volume lv_root to 10.07 GiB
device-mapper: resume ioctl on failed: Invalid argument
Unable to resume VolGroup-lv_root (253:0)
Problem reactivating lv_root

you can fix this booted from rescue image:
```shell
fdisk /dev/sda
d 2
n 2
old_start new_end
t 2
8e (Linux LVM)
w
```
then:
```shell
lvextend -l +100%FREE /dev/VolGroup/lv_root
lvmdiskscan
```

```shell
umount /dev/mapper/VolGroup-lv_home
#Sanity check on the drive
e2fsck -f /dev/mapper/VolGroup-lv_home
#Resize the partition
resize2fs /dev/mapper/VolGroup-lv_home 20G
#Reduce the VolGroup partition
lvreduce -L-400G /dev/mapper/VolGroup-lv_home
#Increase the root VolGroup partition
lvextend -L+400G /dev/mapper/VolGroup-lv_root
#Increase the partitions to occupy full remaining space
resize2fs /dev/mapper/VolGroup-lv_root
resize2fs /dev/mapper/VolGroup-lv_home
reboot
```
