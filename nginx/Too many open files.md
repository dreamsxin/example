# Too many open files

查看系统

```shell
ulimit -a
```

需要调大限制
```shell
ulimit -u 65535
ulimit -n 65535
```

```shell
lsof|grep tomcat|wc -l
```


/etc/profile：
```shell
ulimit -u 65535
ulimit -SHn 65535 
```