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