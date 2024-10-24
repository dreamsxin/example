## 源
22.04 配置地址为 `/etc/apt/sources.list.d/ubuntu.sources`
```txt
# 阿里云
Types: deb
URIs: http://mirrors.aliyun.com/ubuntu/
Suites: noble noble-updates noble-security
Components: main restricted universe multiverse
Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg
```

# 使用

```shell
命令	说明
apt-get update	更新源
apt-get upgrade	更新所有已安装的包
apt-get install <pkg>	安装软件包<pkg>，多个软件包用空格隔开
apt-get install –reinstall <pkg>	重新安装软件包<pkg>
apt-get install -f <pkg>	修复安装（破损的依赖关系）软件<pkg>
apt-get remove <pkg>	删除软件包<pkg>（不包括配置文件）
apt-get purge <pkg>	删除软件包<pkg>（包括配置文件）
apt-get clean	清除缓存(/var/cache/apt/archives/{,partial}下)
中所有已下载的包
apt-cache stats	显示系统软件包的统计信息
apt-cache search <pkg>	使用关键字pkg搜索软件包
apt-cache show	显示软件包pkg_name的详细信息
apt-cache depends <pkg>	查看pkg所依赖的软件包
apt-cache rdepends <pkg>	查看pkg被那些软件包所依赖
apt-get build-dep <pkg>	构建pkg源码包的编译依赖
(这条命令很神奇，一步搞定所有编译依赖)
```


## E: Sub-process /usr/bin/dpkg returned an error code (1)

解决方法：

1.现将info文件夹更名

```shell
sudo mv /var/lib/dpkg/info /var/lib/dpkg/info.bk
```

2.新建一个新的info文件夹

```shell
sudo mkdir /var/lib/dpkg/info
```

3.安装修复

```shell
sudo apt-get update
sudo apt-get install -f
```

4.执行完上一步操作后，在info文件夹下生成一些文件，现将这些文件全部移到info.bk文件夹下

```shell
sudo mv /var/lib/dpkg/info/* /var/lib/dpkg/info.bk
```

5.把自己新建的info文件夹删掉

```shell
sudo rm -rf /var/lib/dpkg/info
```

6.恢复原有info文件夹，修改名字

```shell
sudo mv /var/lib/dpkg/info.bk /var/lib/dpkg/info
```

```shell
journalctl -u jenkins -f
systemctl status jenkins.service
sudo dpkg --configure -a
```

## 代理

```shell
sudo apt -o Acquire::http::proxy="http://127.0.0.1:1090/" install xxx

cho -e "Acquire::http::Proxy \"http://127.0.0.1:1090\";" | sudo tee -a /etc/apt/apt.conf > /dev/null
echo -e "Acquire::https::Proxy \"http://127.0.0.1:1090\";" | sudo tee -a /etc/apt/apt.conf > /dev/null

export ALL_PROXY="socks5://127.0.0.1080"
export all_proxy="socks5://127.0.0.1080"

git config --global http.proxy "socks5://127.0.0.1080"
git config --global https.proxy "socks5://127.0.0.1080"
npm config set proxy "http://127.0.0.1090"
npm config set https-proxy "http://127.0.0.1090"
yarn config set proxy "http://127.0.0.1090"
yarn config set https-proxy "http://127.0.0.1090"
```
