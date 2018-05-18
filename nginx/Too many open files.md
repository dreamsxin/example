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

前两句是修改参数 最后一句是生效
修改内核参数
```shell
/etc/sysctl.conf  
echo "fs.file-max=65536" >> /etc/sysctl.conf  
sysctl -p
```

`/etc/security/limits.conf`
```shell
*        soft    noproc 65535
*        hard    noproc 65535
*        soft    nofile 65535
*        hard    nofile 65535
```