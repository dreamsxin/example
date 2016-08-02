openwrt目录结构，其中第一行是原始目录，第二行是编译过程中生成的目录。各目录的作用是：

- tools - 编译时需要一些工具， tools里包含了获取和编译这些工具的命令。里面是一些Makefile，有的可能还有patch。每个Makefile里都有一句 $(eval $(call HostBuild))，表示编译这个工具是为了在主机上使用的。
- toolchain - 包含一些命令去获取kernel headers, C library, bin-utils, compiler, debugger
- target - 各平台在这个目录里定义了firmware和kernel的编译过程。
- package - 包含针对各个软件包的Makefile。openwrt定义了一套Makefile模板，各软件参照这个模板定义了自己的信息，如软件包的版本、下载地址、编译方式、安装地址等。
- include - openwrt的Makefile都存放在这里。

- scripts - 一些perl脚本，用于软件包管理。
- dl - 软件包下载后都放到这个目录里
- build_dir - 软件包都解压到build_dir/里，然后在此编译
- staging_dir - 最终安装目录。tools, toolchain被安装到这里，rootfs也会放到这里。
- feeds -

- bin - 编译完成之后，firmware和各ipk会放到此目录下。


# 下载

```shell
sudo apt-get install build-essential subversion libncurses5-dev zlib1g-dev gawk gcc-multilib flex git-core gettext libssl-dev
```

```shell
git clone git://git.openwrt.org/openwrt.git
#14.07
git clone git://git.openwrt.org/14.07/openwrt.git

cd openwrt
./scripts/feeds update -a
./scripts/feeds install -a

# 配置编译选项
make menuconfig

# 同配置Linux内核类似，几乎每一个设置都有三个选项:y / m / n，分别代表如下含义：
# * `` (按下`y`)这个包会被包含进固件镜像
# * `` (按下`m`)这个包会在生成刷新OpenWrt的镜像文件以后被编译，但是不会被包含进镜像文件 
# * `` (按下`n`)这个包不会被编译

# export HTTP_PROXY=http://127.0.0.1:8787
# export HTTPS_PROXY=http://127.0.0.1:8787

#先编译要用到的工具和库， 编译单个软件，进入编译选项选择要编译的软件后，如果没有make需执行以下：
make prereq
make tools/install
make toolchain/install

# 只编译某个模块
# make package/name/compile
make package/qos/clean
make package/qos/compile
make package/qos/install

# 编译固件，V=99表示输出详细的debug信息
make V=99


# 清除
make dirclean
```

# VirtualBox

- 目标系统（Target System）选择x86，
- Subtarget和Target Profile使用默认的Generic，
- 目标镜像（Tartget Images），选择ext4和VirtualBox image files (VDI)，
- 在内核模块（Kernel modules）的网络设备（Network Devices）中，选择网卡驱动E1000（默认已选），
- 在LuCI（LuCI）的（Translations）中，选择luci-i18n-chinese。

```shell
make menuconfig
```


3. 编译

```shell
make -j4 V=99  
```                                                         

编译成功会生成镜像： bin/x86/openwrt-x86-*.vdi

```shell
VBoxManage convertfromraw --format VDI openwrt-x86-generic-combined-ext4.img openwrt.vdi
```

新建虚拟电脑，选择 Linux 2.6 (32位)，新建硬盘 IDE 加载 VDI。

路由器至少要两块网卡：网络地址转换（NAT）、桥接（用于lan、wlan）芯片类型 Intel PRO 1000

`/etc/config/network`

```config
onfig interface loopback
        option ifname lo
        option proto static
        option ipaddr 127.0.0.2
        option netmask 255.0.0.0

#config globals 'globals'
#       option ula_prefix 'fddd:d3d9:b24b::/48'

config interface 'wan'
        option ifname 'eth0'
        option proto 'dhcp'

config interface 'lan'
        option type 'bridge'
        option ifname 'eth1'
        option proto 'static'
        option ipaddr '192.168.1.118'
        option netmask '255.255.255.0'
        option gateway '192.168.1.108'
        option ip6assign '60'
```

```shell
/etc/init.d/network restart
```

这样就可以进行远程管理
```shell
ssh -lroot 192.168.1.118
```

在openwrt系统中发现没有shutdown命令，可以采用下面的命令关闭系统。

```shell
kill -USR1 1
```

注意：系统会立即关闭

```shell
kill -USR2 1
```

效果等于reboot。

# 安装 LuCI Web 管理界面并设置开机自动启动
```shell
opkg update
opkg install luci
opkg install luci-i18n-base-zh-cn
/etc/init.d/uhttpd start
/etc/init.d/uhttpd enable
```

# 配置 PHP

```shell
opkg install -d udisk php5 php5-cgi php5-mod-ctype php5-mod-hash php5-mod-pdo php5-mod-pdo-pgsql php5-mod-session
```

修改 `/etc/php.ini`
```ini
extension=ctype.so
extension=hash.so
extension=pdo.so
extension=pdo_pgsql.so
extension=pgsql.so
extension=session.so

short_open_tag = On
;doc_root = "/www" 
date.timezone = Asia/Shanghai
```

## 配置 uhttpd

在`/etc/config/uhttpd`最后添加：
```config
config uhttpd web
list listen_http 0.0.0.0:88
option home /var/www
option index_page index.php
list interpreter ".php=/usr/bin/php-cgi"
```