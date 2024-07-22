• 平均负载升高的主要原因
• 多个进程争抢CPU、等待IO、CPU上下文切换
• 命令：mpstats（全局）多核CPU性能分析程序，实时查看每个CPU的性能指标和全部CPU的平均性能指标
• 命令：pidstats（局部）实时查看进程的CPU、内存、I/O、上下文切换等指标
• 命令：vmstats（全局）实时查看系统的上下文切换（跨进程间，同个进程里多个子线程）、系统中断次数

```shell
pidstat
sudo apt install sysstat -y
sudo systemctl enable sysstat
sudo systemctl start sysstat
```

##
`/etc/default/sysstat`
Set `ENABLED` to `true` as below:

##

```shell
#CPU
pidstat -u | sort -k 8 -r -n | head -n 11
pidstat -r | sort -k 8 -r -n | head -n 11
```

##

```shell
*/1 * * * * /usr/bin/pidstat -u | sort -k 8 -r -n | head -n 2 >> /var/log/pidstat.log
*/1 * * * * /usr/bin/pidstat -r | sort -k 8 -r -n | head -n 2 >> /var/log/pidstat.log
```
