# syslog 

日志存放在目录 `/run/log/journal/`
```shell
systemctl status systemd-journald
journalctl -p err
```

##

```shell
journalctl --since "1 hour ago"
```

## 参数

- -f
只显示最近的日志讯息，另外，在将新的记录项目新增到日志时会列印这些新项目。

- -e
列印讯息并跳转到日志末尾，以便在页面巡览区中显示最新的项目。

- -r
以反向顺序列印记录讯息，使最新的项目列在最前面。

- -k
只显示核心讯息。这等同于栏位比对 `_TRANSPORT=kernel`

- -u
只显示指定 systemd 单元的讯息。这等同于栏位比对 `_SYSTEMD_UNIT=UNIT`

* 定开始日期或结束日期
```shell
journalctl --since "now" -f
journalctl --since "today" --until "3:20"
```

列出所有可用的开机信息：
```shell
journalctl --list-boots
```
根据开机ID的显示讯息:
```shell
journalctl _BOOT_ID=xxx
```

显示目前开机中的所有讯息︰
```shell
journalctl -b
```
上一次开机的讯息:
```shell
journalctl -b -1
```

显示特定程序 ID 产生的讯息︰
```shell
journalctl _PID=1039
```

显示属于特定使用者 ID 的讯息︰
```shell
journalctl _UID=1000
```
显示来自核心环缓衝区的讯息 (与 dmesg 产生的结果相同)︰
```shell
journalctl _TRANSPORT=kernel
```
显示来自服务之标准输出或错误输出的讯息︰
```shell
journalctl _TRANSPORT=stdout
```
仅显示指定服务产生的讯息︰
```shell
journalctl _SYSTEMD_UNIT=avahi-daemon.service
```
如果指定了两个不同的栏位，则仅显示同时与两个运算式相符的项目︰
```shell
journalctl _SYSTEMD_UNIT=avahi-daemon.service _PID=1488
```
如果两个比对参考了同一个栏位，则显示与两个运算式中任意一个相符的所有项目︰
```shell
journalctl _SYSTEMD_UNIT=avahi-daemon.service _SYSTEMD_UNIT=dbus.service
```
可以使用“+”分隔符将两个运算式组合成一个逻辑“OR”。下面的范例将显示来自程序 ID 为 1480 之 Avahi 服务程序的所有讯息，以及来自 D-Bus 服务的所有讯息︰
```shell
journalctl _SYSTEMD_UNIT=avahi-daemon.service _PID=1480 + _SYSTEMD_UNIT=dbus.service
```
