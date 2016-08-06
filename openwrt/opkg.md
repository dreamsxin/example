# 搭建自己的opkg软件仓库


## 生成索引文件Packages.gz

生成Packages.gz的指令如下：

```shell
# 进入 ipk 文件目录
cd bin/packages/x86/packages/packages
../../../scripts/ipkg-make-index.sh ./ > ./Packages
gzip -9c ./Packages > ./Packages.gz
```

## 上传到服务器

假设服务器地址为：192.168.1.108

文件地址为：http://192.168.1.108/openwrt/packages/Packages.gz

那对应的opkg设置为：

`src/gz myopkg http://192.168.1.108/openwrt/packages/Packages.gz`

## 把自己的源加入opkg配置中

```shell
find /etc/opkg.d/ -name '*.conf' | xargs sed -i '2isrc/gz myopkg http://192.168.1.108/openwrt/packages/Packages.gz'
opkg update
opkg list | grep nodog
opkg install nodogsplash
```

## 在固件中加入

修改 `package/system/opkg/files/customfeeds.conf`
修改 `package/base-files/image-config.in`

# iPKG包制作（ipkg-build）

ipkg 文件只是一个包含三个其他文件的 `tar` 文件（将扩展名改为`tar`即可正常打开）：

第一个文件，即 `data.tar.gz`，是一个压缩的 tar 文件，其中包含将被安装到正确目录结构中的多个文件。
第二个文件，即 `control.tar.gz`，是一个压缩的 tar 文件，其中包含安装脚本和信息。
第三个文件，即 `debian-binary`，是一个纯文本文件，其中包含字符串“2.0”。该文件目前不真正做任何事情，但是一些工具期望该文件可用。

## 简单的流程

```
mkdir -p /tmp/foo/bin
cp foo /tmp/foo/bin
mkdir -p /tmp/foo/CONTROL

cat >/tmp/foo/CONTROL/control <<EOF
Package: foo
Version: 0.1
Architecture: mipsel
Maintainer: foo bar <foo@bar.com>
Section: base
Priority: optional
Depends: libc
Description: This package provides foo, which does nothing.
Source: http://foo.bar/foo
EOF

ipkg-build -o root -g root /tmp/foo /tmp
```

## 制作一个ipk包

安装该ipk之后会在home文件夹创建一个`README.md`文件，步骤：

1. 创建一个文件夹`test`,并在test下面创建两个目录为：`CONTROL`和`home`
2. 在`home`目录下放一个文件`README.md`
3. 在`CONTROL`目录下创建一个`control`文件內容如下
```txt
Package: readme
Priority: optional 
Section: Misc
Version: 0.0.1 
Architecture: intel 
Maintainer: dreamsxin@qq.com 
Source:https://github.com/dreamsxin
Description: ipkg readme package
```

在`CONTROL`目录下可以新建`preinst`和`postinst`脚本文件，在安装前、安装时、更新、删除、配置时处理一些任务，其各自內容如下
[preinst]
```shell
#!/bin/sh 
echo "I will start installation"

exit 0
```

[postinst]
```shell
#!/bin/sh 
case "$1" in 
update)
;;
install)
echo "I'm in the installation"
;;
configure)
;;
remove)
;;
*)
;;
esac 

exit
```