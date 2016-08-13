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

# wget 断点续传

```shell
# -O 指定文件名保存 -c 继续执行上次终端的任务
sudo wget -c -O 保存到本地的文件名 "下载地址"
```

压缩图片

```shell
sudo apt-get install optipng
optipng xxx.png
find ./images/ -iname *.png -print0 |xargs -0 optipng -o7
find . -name '*.png' | xargs optipng -nc -nb -o7 -full
find . -type f -name "*.png" -exec optipng {} \;

sudo apt-get install jpegoptim
jpegoptim xxx.jpg
# 用50%质量压缩图片:
jpegoptim -m50 xxx.jpg
find . -type f -name "*.jpg" -exec jpegoptim {} \;
```

# 查找指定大小的文件，并按大小排序
```shell
find . -type f -size +100M  -print0 | xargs -0 du  | sort -nr
```

# 查找占用空间最大的目录，并按大小排序
```shell
du -hm --max-depth=2 | sort -nr | head -12
```
