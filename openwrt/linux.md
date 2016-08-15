# 更换 Linux 内核版本

相关补丁： https://dev.openwrt.org/browser/trunk/target/linux?rev=44800&order=name

将openwrt14.07中的内核版本从3.10.49更换成3.10.102

1. 更改文件`include/kernel-version.mk`
2. 修改文件​`target/linux/<路由器架构>`下的Makefile，将`KERNEL_PATCHVER:=xx`那一行的xx改成自己希望的版本系列即可,但不能具体为3.10.102，应为3.10。不然会报错。
3. 修改`target/linux/generic/patches-3.10`中的文件。
4. 修改`target/linux/ramips/patches-3.10`中的文件，ramips即make menuconfig中选中的platform。
5. 在`target/linux/generic/`中添加config-3.10
6. 在`target/linux/ramips/mt7620a`中添加`config-3.10`。（mt7620a为编译时选择的开发板，若更新为内核3.14，则添加config-3.14）
7. 检查`target/linux/generic/files`中的文件有没有被修改。
8. 更改`package/kernel`中的文件
9. 还要修改include/中的mk文件


更换内核版本后，可能需要运行make kernel_menuconfig进行设置。