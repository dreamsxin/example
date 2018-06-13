# Linux 各类错误记录

```shell
grep -E "error|Error|ERROR|fail|Fail|FAIL" /var/log/syslog
```

## ACPI Error: No handler for Region 导致系统自动重启

由于BIOS中开启了中断重映射，在ERST(芯片集中的错误校验表)校验时发生错误，导致高级配置电源管理模块无法处理IPMI驱动请求，预存数据到内存发生错误，造成kernel寻找了空指针。

```text
Jun 12 17:51:09 ubuntu kernel: [    1.277657] ERST: Error Record Serialization Table (ERST) support is initialized.
Jun 12 17:51:09 ubuntu kernel: [    2.666433] ata2.00: failed to resume link (SControl 0)
Jun 12 17:51:09 ubuntu kernel: [    2.986241] ata1.01: failed to resume link (SControl 0)
Jun 12 17:51:09 ubuntu kernel: [    3.693964] ata2.01: failed to resume link (SControl 0)
Jun 12 17:51:09 ubuntu kernel: [   14.344474] EXT4-fs (sda1): re-mounted. Opts: errors=remount-ro
Jun 12 17:51:09 ubuntu kernel: [   14.408621] ACPI Error: No handler for Region [IPMI] (ffff880819606240) [IPMI] (20131115/evregion-162)
Jun 12 17:51:09 ubuntu kernel: [   14.408625] ACPI Error: Region IPMI (ID=7) has no handler (20131115/exfldio-299)
Jun 12 17:51:09 ubuntu kernel: [   14.408634] ACPI Error: Method parse/execution failed [\_SB_.PMI0._GHL] (Node ffff880819605b18), AE_NOT_EXIST (20131115/psparse-536)
Jun 12 17:51:09 ubuntu kernel: [   14.408643] ACPI Error: Method parse/execution failed [\_SB_.PMI0._PMC] (Node ffff880819605a78), AE_NOT_EXIST (20131115/psparse-536)
Jun 12 17:51:09 ubuntu failsafe: Failsafe of 120 seconds reached.
Jun 12 17:51:11 ubuntu kernel: [   16.939278] init: failsafe main process (899) killed by TERM signal
```

* 解决方法（不一定有效）

方法一添加 `intremap=off` 或者 `intremap=no_x2apic_optout`
ubuntu：
`/etc/default/grub`
```text
GRUB_CMDLINE_LINUX_DEFAULT="intermap=off"
```
CentOS:
修改 `/boot/grub/grub.conf`，在 kernel 一行最后加上

然后重启服务器即可。

参数解释：
intremap={on,off,nosid,no_x2apic_optout}
 
on：(默认值)开启中断重映射,BIOS中默认开启 
off：关闭中断重映射
nosid：重映射时不对SID(Source ID)做检查
no_x2apic_optout：无视BIOS的设置，强制禁用x2APIC特性，主要用于解决某些对x2APIC支持有缺陷的BIOS导致的故障


方法二升级内核或重装高版本系统

查看当前内核：
```shell
cat /proc/version
dpkg -l|grep linux-image
```

查看可以更新的内核版本以及安装：
```shell
sudo apt-cache search linux-image
sudo apt-get install linux-image-4.4.0-98-generic linux-image-extra-4.4.0-98-generic
sudo apt-get install linux-image-3.13.0-129-generic linux-image-extra-3.13.0-129-generic
# 卸载新内核，回滚到老版本
sudo apt-get purge linux-image-4.4.0-98-generic linux-image-extra-4.4.0-98-generic
```