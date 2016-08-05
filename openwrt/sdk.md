# 编译 SDK

在`make menuconfig`时，选择 `[*] Build the OpenWrt SDK`

编译完成之后会在 `bin/xxx/` 下生产SDK文件 `OpenWrt-SDK-xxx-for-linux-xxx.tar.bz2`

# 安装 SDK

然后解压之前生成的文件，此目录结构跟openwrt的目录结构基本一致

以后的应用程序代码都放在package目录下，且要遵循相应的目录结构规范

最后在SDK顶层目录下make，就会自动在顶层目录下自动生成bin目录，bin/ramips/packages/base/下就会有ipk文件

# 创建 helloworld 项目

在package目录下，创建helloword项目。

```shell
cd package
mkdir helloword
cd helloword
touch Makefile
```

这里的 Makefile 文件跟一般的 Makefile 格式还不一样，详见OpenWrt上的说明文档：OpenWrt官网Packages说明。
我们要描述 helloworld 包的信息，比如：如何配置、如何编译、如何打包、安装等等信息。
```makefile
include $(TOPDIR)/rules.mk

PKG_NAME:=helloworld
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/helloworld
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Helloworld -- prints a snarky message
endef

define Package/helloworld/description
	It's my first package demo.
endef

define Build/Prepare
	echo "Here is Package/Prepare"
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/helloworld/install
	echo "Here is Package/install"
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/helloworld $(1)/bin/
endef

$(eval $(call BuildPackage,helloworld))
```

## 创建 src 目录
```shell
mkdir src
cd src
touch helloworld.c Makefile
```

`helloworld.c`：
```c
#include <stdio.h>

int main()
{
    printf("This is my hello word!\n");
    return 0;
}
```

`Makefile` 内容
```makefile
helloworld : helloworld.o
    $(CC) $(LDFLAGS) helloworld.o -o helloworld

helloworld.o : helloworld.c
    $(CC) $(CFLAGS) -c helloworld.c

clean :
    rm *.o helloworld
```

## 编译

```shell
make -j1 V=s
# 成功执行将会将会生成文件 bin/x86/OpenWrt-SDK-x86-generic_gcc-5.3.0_musl-1.1.14.Linux-x86_64.tar.bz2
```

## 拷贝到openwrt系统

```shell
scp bin/xxx/packages/base/helloworld_xxx.ipk root@192.168.1.118:/
```

## 安装

```shell
ssh root@192.168.1.118
opkg install helloworld_xxx.ipk
```

## Makefile 说明

```Makefile
#------ OPENWRT集成非官方包之Makefile规则
include $(TOPDIR)/rules.mk
PKG_NAME:=[软件包名字 和文件夹名称一样]
PKG_VERSION:=[软件包版本 自己写个]
PKG_RELEASE:=1
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=utils
	CATEGORY:=[软件包在 menuconfig 里的位置 比如Utilities]
	DEPENDS:=[依赖包 两个之间通过空格分隔 前面加+为默认显示 选中该软件包自动选中依赖包 不加+为默认不显示 选中依赖包才显示]
	TITLE:=[标题]
	PKGARCH:=[支持的平台，全部写all]
	MAINTAINER:=[作者]
endef

define Package/$(PKG_NAME)/description
	[软件包简介]
endef

# 非本目录下的源码文件, 拷贝到此相应目录下.
# 如../../xucommon/xucommon.c, 则将 xucommon.c 拷贝到此目录下的源码的 ../../

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/conffiles
	[升级时保留文件/备份时备份文件 一个文件一行]
endef

define Package/$(PKG_NAME)/install
	$(CP) ./files/* $(1)/
endef

define Package/$(PKG_NAME)/preinst
	[安装前执行的脚本 记得加上#!/bin/sh 没有就空着]
	#!/bin/sh
	uci -q batch <<-EOF >/dev/null
	delete ucitrack.@aria2[-1]
	add ucitrack aria2
	set ucitrack.@aria2[-1].init=aria2
	commit ucitrack
	EOF
	exit 0
endef

define Package/$(PKG_NAME)/postinst
	[安装后执行的脚本 记得加上#!/bin/sh 没有就空着]
	#!/bin/sh
	rm -f /tmp/luci-indexcache
	exit 0
endef

Package/$(PKG_NAME)/prerm
	[删除前执行的脚本 记得加上#!/bin/sh 没有就空着]
endef

Package/$(PKG_NAME)/postrm
	[删除后执行的脚本 记得加上#!/bin/sh 没有就空着]
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
```