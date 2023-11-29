# journal

Systemd 统一管理所有 Unit 的启动日志。带来的好处就是，可以只用 `journalctl` 一个命令，查看所有日志（内核日志和应用日志）。
默认情况下，journal 日志在大多数 Linux 发行版中是启用的，但它将日志数据存储在 `/run/log/journal/` 中，重启时会被删除。如果要使日志永久保存，请执行以下步骤，这些步骤将自动创建 `/var/log/journal/` 目录。

以 root 用户身份打开 `/etc/systemd/journald.conf` 文件并取消注释 “Storage=auto” 的行并将其更改为 “Storage=persistent” 。或者，您可以使用 `sed` 命令替换文件中的匹配字符串。

```shell
sed -i '/Storage/ c\Storage=persistent' /etc/systemd/journald.conf
```

## Storage 参数设置

- persistent
- 将日志存储在 /var/log/journal 目录中，该目录在重新启动后仍然存在。
- volatile
- 将日记存储在 volatile/run/log/journal 目录中，这不会导致系统重启。
- auto
rsyslog 将确定使用持久性存储(persistent)还是易失性存储(volatile)，如果存在/var/log/journal目录，则rsyslog使用持久性存储，否则使用易失性存储。

更改后，重新启动systemd-journald服务以使配置更改生效：
```shell
systemctl restart systemd-journald
```
确认服务已重新启动并正在运行：
```shell
systemctl status systemd-journald
```

##  常用的选项
在使用 Journalctl 工具之前，可以了解一些常用的选项：

- -f : 实时显示最近的10条日志。
- -e : 跳转到日志末尾以显示最新事件。
- -r : 按时间倒序打印日志消息
- -k : 只显示内核日志。
- -u : 只显示指定systemd Unit的消息。
- -b : 显示来自特定引导的消息，如果不包括特定引导会话，则显示当前引导消息。
- –list-boots : 显示引导编号(相对于当前引导)、它的id以及与引导有关的第一个和最后一个消息的时间戳。
- –utc : 以UTC时间表示。
- -p, –priority= : 按消息优先级过滤输出。
- -S, –since= : 根据开始时间过滤日志
- -U, –until= : 根据结束时间过滤日志
- –disk-usage : 显示所有日志文件的当前磁盘使用情况。

# 查看所有日志（默认情况下 ，只保存本次启动的日志）
```shell
journalctl
```

# 查看内核日志（不显示应用日志）
```shell
journalctl -k
```

# 查看系统本次启动的日志
```shell
journalctl -b
journalctl -b -0
```

# 查看上一次启动的日志（需更改设置）
```shell
journalctl -b -1
```
-b参数筛选日记。没有任何争论，-b选项仅显示上次引导后的消息。它以负数作为参数，显示以前引导的日志。
```shell
journalctl -b -1 -p err
```
```shell
journalctl --disk-usage
journalctl --vacuum-time=2d
journalctl --vacuum-size=500M
```
