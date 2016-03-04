批量使用文件MD5值替换文件名

```shell
md5sum * | sed -e 's/\([^ ]*\) \(.*\(\..*\)\)$/mv -v \2 \1\3/' | sh
```

ISO 制作
```shell
mkisofs -r -o iso1.iso iso1/
```