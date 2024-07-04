
```shell
dlv attach <pid>
```

## 可能会发生错误

`Could not attach to pid xxx: this could be caused by a kernel security setting, try writing "0" to /proc/sys/kernel/yama/ptrace_scope`

- `kernel.yama.ptrace_scope = 1`
ptrace 保护是 Ubuntu 10.10 左右首次引入的一项刻意的内核安全措施。
