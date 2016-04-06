批量使用文件MD5值替换文件名

```shell
md5sum * | sed -e 's/\([^ ]*\) \(.*\(\..*\)\)$/mv -v \2 \1\3/' | sh
```

ISO 制作
```shell
mkisofs -r -o iso1.iso iso1/
```

# 查看连接数
```shell
netstat -n | awk '/^tcp/ {++S[$NF]} END {for(a in S) print a, S[a]}'
```

# 查看 apache 进程数
```shell
ps -ef | grep apache | wc -l
```