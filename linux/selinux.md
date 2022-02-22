## error
selinux 配置错误导致
```output
Kernel Panic - not syncing: Attempted to kill init!
Pid: 1 comm: init Not tainted 2.6.32-358.6.2.e16.x86_64 #1
Call Trace:
[<ffffffff8150d478>] ? panic+0xs7/0x16f
[<ffffffff81073ae2>] ? do_exit+0x862/0x870
[<ffffffff81182965>] ? fput+0x25/0x30
[<ffffffff81073b48>] ? do_group_exit+0x58/0xd0
[<ffffffff81073bd7>] ? sys_exit_group+0x17/0x20
[<ffffffff8100b072>] ? system_call_fastpath+0x16/0x1b
```
启动时编辑 `grub` 追加 `selinux=0 enforcing=0` 或 `selinux=1 enforcing=0`
