
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
