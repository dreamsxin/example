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

### 登录lfs用户并设置环境变量：

#### 登录lfs：

```shell
passwd lfs
```

详解：

设置一个密码，以便于登陆。注意设置空密码可能会报错，这里密码设置自己随意了。

passwd命令用于设置用户的认证信息，包括用户密码、密码过期时间等。系统管理者则能用它管理系统用户的密码。只有管理者可以指定用户名称，一般用户只能变更自己的密码。

```shell
su - lfs
```

详解：

su命令用于切换当前用户身份到其他用户身份，变更时须输入所要变更的用户帐号与密码。
重要：这里一定要有“-”,作用是使用lfs登录，采用新的环境变量和用户身份；如果没有“-”，就会使用root的环境变量，只不过是换做lfs身份罢了。

#### 设置环境变量：

```shell
cat > ~/.bash_profile << "EOF"
exec env -i HOME=$HOME TERM=$TERM PS1='\u:\w\$ ' /bin/bash
```

```shell
cat > ~/.bashrc << "EOF"
set +h
umask 022
LFS=/mnt/lfs
LC_ALL=POSIX
LFS_TGT=$(uname -m)-lfs-linux-gnu
PATH=/tools/bin:/bin:/usr/bin
export LFS LC_ALL LFS_TGT PATH
EOF
source ~/.bash_profile
```

现在我们已经完成了，环境变量的设置，那么来检查一下：

```shell
export
```

输出结果应该如下，就OK了：
```out
declare -x HOME="/home/lfs"
declare -x LC_ALL="POSIX"
declare -x LFS="/mnt/lfs"
declare -x LFS_TGT="x86_64-lfs-linux-gnu"
declare -x OLDPWD
declare -x PATH="/tools/bin:/bin:/usr/bin"
declare -x PS1="\\u:\\w\\\$ "
declare -x PWD="/home/lfs"
declare -x SHLVL="1"
declare -x TERM="linux"
```

## 进入工作目录，并放入LFS-7.6的压缩包：

### 进入工作目录：

```shell
cd $LFS/sources
```

详解：

cd命令用来切换工作目录至dirname。 其中dirName表示法可为绝对路径或相对路径。若目录名称省略，则变换至使用者的home directory(也就是刚login时所在的目录)。另外，~也表示为home directory的意思，.则是表示目前所在的目录，..则表示目前目录位置的上一层目录。

### 放入LFS7.6所需要的包：

这个时候要使用SecureCRT的clone session的功能，并在root用户下执行下面的代码：

```shell
apt-get install lrzsz
```

- yum命令是在Fedora和RedHat以及SUSE中基于rpm的软件包管理器，它可以使系统管理人员交互和自动化地更细与管理RPM软件包，能够从指定的服务器自动下载RPM包并且安装，可以自动处理依赖性关系，并且一次安装所有依赖的软体包，无须繁琐地一次次下载、安装。 yum提供了查找、安装、删除某一个、一组甚至全部软件包的命令，而且命令简洁而又好记。
- lrzsz：该安装包包含了rz，sz的功能。rz将文件下载到当前目录，sz将文件上传到本地。

```shell
rz #等待窗口选择文件
```

详解：选择下载好的lfs-7.6的压缩包，下载到当前目录。

```shell
tar xvf lfs-packages-7.6.tar
rm -rf lfs-packages-7.6.tar 
mv -v 7.6/* .
rm -rf 7.6
```

详解：

- tar命令可以为linux的文件和目录创建档案。利用tar，可以为某一特定文件创建档案（备份文件），也可以在档案中改变文件，或者向档案中加入新的文件。tar最初被用来在磁带上创建档案，现在，用户可以在任何设备上创建档案。利用tar命令，可以把一大堆的文件和目录全部打包成一个文件，这对于备份文件或将几个文件组合成为一个文件以便于网络传输是非常有用的。
- rm命令可以删除一个目录中的一个或多个文件或目录，也可以将某个目录及其下属的所有文件及其子目录均删除掉。对于链接文件，只是删除整个链接文件，而原有文件保持不变。
- mv命令用来对文件或目录重新命名，或者将文件从一个目录移到另一个目录中。source表示源文件或目录，target表示目标文件或目录。如果将一个文件移到一个已经存在的目标文件中，则目标文件的内容将被覆盖。

## 从现在开始重启后如何恢复工作目录（建议使用虚拟机的挂起功能）

使用SecureCRT连接root用户，执行下面代码恢复工作目录：

```shell
export LFS=/mnt/lfs
mount -v -t ext4 /dev/sdb1 $LFS
/sbin/swapon /dev/sdb2
su - lfs
cd $LFS/sources
```

详解：前面已经说得很清楚了，这里不再赘述。