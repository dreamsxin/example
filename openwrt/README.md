# 下载

```shell
sudo apt-get install build-essential subversion libncurses5-dev zlib1g-dev gawk gcc-multilib flex git-core gettext libssl-dev
```

```shell
git clone git://git.openwrt.org/openwrt.git
cd openwrt
./scripts/feeds update -a
./scripts/feeds install -a
make menuconfig
make defconfig
make prereq

# 只编译某个模块
make package/qos/clean
make package/qos/compile
make package/qos/install

# 编译固件，V=99表示输出详细的debug信息
make V=99


# 表示编译所有
make world
```