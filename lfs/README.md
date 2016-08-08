# Linux From Scratch（从零开始构建Linux系统，简称LFS）

## 宿主系统环境监测

```shell
sudo apt-get install build-essential texinfo
sudo apt-get install libgmp-dev
```

检测脚本`version-check.sh`内容
```shell
#!/bin/bash
# Simple script to list version numbers of critical development tools
export LC_ALL=C
bash --version | head -n1 | cut -d" " -f2-4
echo "/bin/sh -> `readlink -f /bin/sh`"
echo -n "Binutils: "; ld --version | head -n1 | cut -d" " -f3-
bison --version | head -n1

if [ -h /usr/bin/yacc ]; then
	echo "/usr/bin/yacc -> `readlink -f /usr/bin/yacc`";
elif [ -x /usr/bin/yacc ]; then
	echo yacc is `/usr/bin/yacc --version | head -n1`
else
	echo "yacc not found"
fi

bzip2 --version 2>&1 < /dev/null | head -n1 | cut -d" " -f1,6-
echo -n "Coreutils: "; chown --version | head -n1 | cut -d")" -f2
diff --version | head -n1
find --version | head -n1
gawk --version | head -n1

if [ -h /usr/bin/awk ]; then
	echo "/usr/bin/awk -> `readlink -f /usr/bin/awk`";
elif [ -x /usr/bin/awk ]; then
	echo yacc is `/usr/bin/awk --version | head -n1`
else
	echo "awk not found"
fi

gcc --version | head -n1
g++ --version | head -n1
ldd --version | head -n1 | cut -d" " -f2- # glibc version
grep --version | head -n1
gzip --version | head -n1
cat /proc/version
m4 --version | head -n1
make --version | head -n1
patch --version | head -n1
echo Perl `perl -V:version`
sed --version | head -n1
tar --version | head -n1
makeinfo --version | head -n1
xz --version | head -n1
echo 'main(){}' > dummy.c && g++ -o dummy dummy.c
if [ -x dummy ]
	then echo "g++ compilation OK";
else
	echo "g++ compilation failed";
fi
rm -f dummy.c dummy
```

执行脚本`version-check.sh`

```shell
bash version-check.sh
```

如果出现错误`/bin/bash^M: 解释器错误`，执行下面的命令替换`\n\r`为`\n`
```shell
sed -i 's/\r$//' version-check.sh
```

`library-check.sh`

```shell
#!/bin/bash
for lib in lib{gmp,mpfr,mpc}.la; do
echo $lib: $(if find /usr/lib* -name $lib|
grep -q $lib;then :;else echo not;fi) found
done
unset lib
```

执行脚本`library-check.sh`

```shell
bash library-check.sh
```

## 分区

```shell
fdisk /dev/sdb
# or
cfdisk /dev/sdb
```

## 格式化：

将sdb2设置为swap分区。

格式化sdb1为ext4文件系统。

```shell
mkswap /dev/sdb2
mkfs -v -t ext4 /dev/sdb1
```

详解：

- mkswap是用于将磁盘分区格式化为交换分区的命令。
- mkfs是用于在设备上创建文件系统的命令。

## 建立“创作基地”

### 创建一个环境变量，并创建一个重要目录（即创作基地），再将sdb1挂在该目录下：

```shell
export LFS=/mnt/lfs
mkdir -pv $LFS
mount -v -t ext4 /dev/sdb1 $LFS
```

详解：

- export命令用于将shell变量输出为环境变量，或者将shell函数输出为环境变量。
- mkdir命令用于创建目录。
- mount命令用于加载文件系统到指定的加载点。此命令的最常用于挂载cdrom，使我们可以访问cdrom中的数据，因为你将光盘插入cdrom中，Linux并不会自动挂载，必须使用Linux mount命令来手动完成挂载。

### 加载交换分区sdb2

```shell
/sbin/swapon -v /dev/sdb2
```

详解：

swapon命令用于激活Linux系统中交换空间，Linux系统的内存管理必须使用交换区来建立虚拟内存。

### 创建源代码编译用的目录并分给其足够的权限：

```shell
mkdir -v $LFS/sources
chmod -v a+wt $LFS/sources
```

详解：

chmod命令用来变更文件或目录的权限。


### 创建工具链目录：

```shell
mkdir -v $LFS/tools
ln -sv $LFS/tools /
```

详解：

ln命令用来为文件创件连接，连接类型分为硬连接和符号连接两种，默认的连接类型是硬连接。如果要创建符号连接必须使用"-s"选项。

### 创建lfs用户并给其相应的权限

```shell
groupadd lfs
useradd -s /bin/bash -g lfs -m -k /dev/null lfs
chown -v lfs $LFS/tools
chown -v lfs $LFS/sources
```

详解：

- groupadd命令用于创建一个新的工作组，新工作组的信息将被添加到系统文件中。
- useradd命令用于Linux中创建的新的系统用户。useradd可用来建立用户帐号。帐号建好之后，再用passwd设定帐号的密码．而可用userdel删除帐号。使用useradd指令所建立的帐号，实际上是保存在/etc/passwd文本文件中。
- chown命令改变某个文件或目录的所有者和所属的组，该命令可以向某个用户授权，使该用户变成指定文件的所有者或者改变文件所属的组。用户可以是用户或者是用户D，用户组可以是组名或组ID。文件名可以使由空格分开的文件列表，在文件名中可以包含通配符。 只有文件主和超级用户才可以便用该命令。