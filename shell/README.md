批量使用文件MD5值替换文件名

```shell
md5sum * | sed -e 's/\([^ ]*\) \(.*\(\..*\)\)$/mv -v \2 \1\3/' | sh
```

ISO 制作
```shell
mkisofs -r -o iso1.iso iso1/
sudo dd if=/dev/sr0 of=动物功夫系列.iso
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

# ssh 代理

```shell
ssh -C -v -N -D 127.0.0.1:7070 xxx@x.x.x.x -p 22022 -pw 密码
ssh -qTfnN -D 7070 xxx@x.x.x.x -p 22
ssh x.x.x.x -l username -p 22 -D 7070
```

# cat 

## cat 创建文件

```shell
cat > hello.txt << EOF
hello world
EOF
```

## cat 查看文件

```shell
cat hello.txt
```

## cat 合并文件

```shell
cat hello1.txt hello2.txt hello3.txt > helloall.txt
```

# more

more [参数选项] [文件]

参数如下：
- +num   从第num行开始显示；
- -num   定义屏幕大小，为num行；
- +/pattern   从pattern 前两行开始显示；
- -c   从顶部清屏然后显示；
- -d   提示Press space to continue, 'q' to quit.（按空格键继续，按q键退出），禁用响铃功能；
- -l    忽略Ctrl+l （换页）字符；
- -p    通过清除窗口而不是滚屏来对文件进行换页。和-c参数有点相似；
- -s    把连续的多个空行显示为一行；
- -u    把文件内容中的下划线去掉退出more的动作指令是q 

```shell
more -dc /etc/profile		// 显示提示，并从终端或控制台顶部显示；
more +4 /etc/profile		// 从profile的第4行开始显示；
more -4 /etc/profile		// 每屏显示4行；
more +/MAIL /etc/profile	// 从profile中的第一个MAIL单词的前两行开始显示；
```

## 修改用户主分组

```shell
usermod -g groupname username
```

## 修改用户次分组

```shell
usermod -G groupname username
```

## 追加用户次分组

```shell
gpasswd -a username groupname
usermod -a -G groupname username
```

## 改变文件所有者和组

```shell
chown username:groupname file
chown username file
```

## 改变文件权限

```shell
chmod u+x file			// 给file的属主增加执行权限
chmod 751 file			// 给file的属主分配读、写、执行(7)的权限，给file的所在组分配读、执行(5)的权限，给其他用户分配执行(1)的权限
chmod u=rwx,g=rx,o=x file	// 上例的另一种形式
chmod =r file			// 为所有用户分配读权限
chmod 444 file			// 同上例
chmod a-wx,a+r   file		// 同上例
chmod -R u+r directory		// 递归地给directory目录下所有文件和子目录的属主分配读的权限
chmod 4755			// 设置用ID，给属主分配读、写和执行权限，给组和其他用户分配读、执行的权限。
```