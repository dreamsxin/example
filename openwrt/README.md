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