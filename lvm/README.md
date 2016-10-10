## 查看磁盘

```shell
sudo fdisk -l
```

## 扫描

```shell
sudo pvscan
```

## pvcreate

```shell
sudo pvcreate /dev/sda2
```

## 创建vg

```shell
vgcreate wxservice
```

## 查看vg

```shell
vgdisplay
```

## 扩展vg

```shell
sudo vgextend wxservice /dev/sda2
```

## 查看lv

```shell
lvdisplay
```

## 扩展lv

```shell
sudo lvextend -L 2400G /dev/wxservice/root
```

## 重设大小


```shell
sudo resize2fs /dev/wxservice/root
```