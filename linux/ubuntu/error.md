# 
按 ESC 进入grub引导菜单
使用引导盘修复

## grub 说明

- GRUB_DEFAULT=0 #默认选择第一个内核
- GRUB_TIMEOUT_STYLE=hidden #隐藏引导菜单
- GRUB_TIMEOUT=0 #引导菜单超时时间
- GRUB_DISTRIBUTOR=`lsb_release -i -s 2> /dev/null || echo Debian`
- GRUB_CMDLINE_LINUX_DEFAULT="quiet splash"
- GRUB_CMDLINE_LINUX="" #导入到每个启动项（包括recovery mode启动项)的'linux'命令行，当有英伟达显卡驱动异常时添加GRUB_CMDLINE_LINUX=“pci=realloc=off”

## 引导修复

```shell
mount /dev/sda8 /media/tmp
grub-install --root-directory=/media/tmp /dev/sda
sudo update-grub2
```

## 显卡驱动黑屏

`/etc/default/grub`修改
`GRUB_CMDLINE_LINUX_DEFAULT=quiet" splash"`
#修改为：
GRUB`_CMDLINE_LINUX_DEFAULT="quiet splash nomodeset"`

1.Intel 82852/82855或8系列显示晶片
i915.modeset=1 或 i915.modeset=0

2.Nvidia：nomodeset

3.其它厂牌(如ATI，技嘉)
xforcevesa 或 radeon.modeset=0 xforcevesa
