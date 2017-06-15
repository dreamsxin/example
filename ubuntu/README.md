# 快捷键

## 打开终端

可以使用快捷键 `ctrl + alt + t` 打开一个新终端。
也可以按下 alt 然后输入 terminal

## 打开应用搜索菜单

按下 `supper` 键

## 最小化所有窗口

按下 `supper + ctrl + d` 键

## 打开新窗口

在 Nautilus 打开新的 Tab，浏览器中打开新的页面窗口。

`ctrl + t`

## 关闭当前窗口

在 Nautilus 关闭当前 Tab，浏览器中关闭当前页面窗口。

`ctrl + w`

## 返回 Home 目录

按下 `alt + home` 键

# 安装中文拼音

```shell
sudo apt-get install ibus-sunpinyin
```

## 重启 ibus

```shell
killall ibus-daemon
ibus-daemon -d
# or
ibus-daemon -d -x -r
```

## 设置快捷键 和 添加输入法

# use gcc 5.x on ubuntu 14.04 

```shell
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install gcc-5 g++-5

sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5
```

# denyhosts 使用

重要配置信息
```ini
SECURE_LOG = /var/log/auth.log
HOSTS_DENY = /etc/hosts.deny
```

## 启动与停止
```shell
sudo service denyhosts start
sudo service denyhosts stop
```

## 清除日志
```shell
sudo rm /var/log/auth.log
```

## 警告: 磁盘描述表明物理块大小为 2048 字节，但 Linux 认为它是 512 字节。  (udisks-error-quark, 0)

```shell
sudo dd if=/dev/zero of=/dev/sdb bs=2048 count=32
```

## 系统编码

```shell
apt-get install -y language-pack-en-base
```

`/etc/default/locale`
```ini
LANG="en_US.UTF-8"
LANGUAGE="en_US:en"
LC_ALL="en_US.UTF-8"
```

```shell
sudo locale-gen
# locale-gen zh_CN.UTF-8
sudo dpkg-reconfigure locales
```

## 遇到 Hash Sum mismatch 错误
```shell
sudo apt-get update --fix-missing
```

# syslog 日志

`/etc/rsyslog.conf`
`/etc/rsyslog.d/50-default.conf`

# 记录 crontab 日志

## 修改 rsyslog

`/etc/rsyslog.d/50-default.conf`

将 cron 前面的注释符去掉
```shell
cron.*                          /var/log/cron.log
```

## 任务

分时日月周
```shell
0 0 * * * /usr/bin/php /usr/share/nginx/www/apps/cli.php --task=report
```

## 重启 rsyslog

```shell
sudo service rsyslog restart
```

## Rhythmbox 乱码

编辑 `~/.profile`

```shell
export GST_ID3_TAG_ENCODING=GBK:UTF-8:GB18030
export GST_ID3V2_TAG_ENCODING=GBK:UTF-8:GB18030
```

保存导入

```shell
source ~/.profile
```

## 双网卡 NAT 配置

```shell
sudo iptables -F
sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE #打开NAT
sudo iptables -t nat -A POSTROUTING -s 192.168.1.0/24 -o eth1 -j SNAT --to-source xxx.xxx.xxx.xxx
sudo iptables -t nat -A POSTROUTING -s 192.168.1.0/24 -d ! 192.168.1.0/24 -j SNAT --to-source xxx.xxx.xxx.xxx
sudo iptables -A FORWARD -s 192.168.1.0/24 -p tcp -m tcp --tcp-flags SYN,RST SYN -j TCPMSS --set-mss 1200
#sudo ifconfig eth1 mtu 1500
#sudo ifconfig eth0 mtu 1500
```

```shell
./configure --prefix=/usr --with-apxs2=/usr/bin/apxs2 --with-config-file-path=/etc/php5/apache2 \
	--with-config-file-scan-dir=/etc/php5/apache2/conf.d --build=x86_64-linux-gnu \
	--host=x86_64-linux-gnu --sysconfdir=/etc --localstatedir=/var --mandir=/usr/share/man \
	--disable-debug --with-regex=php --disable-rpath --disable-static --with-pic --with-layout=GNU \
	--with-pear=/usr/share/php --without-gdbm --with-iconv --enable-exif --enable-ftp --with-gettext \
	--enable-mbstring --with-pcre-regex=/usr --enable-shmop --enable-sockets --enable-wddx \
	--with-libxml-dir=/usr --with-zlib --enable-zip --without-mm --with-curl=shared,/usr \
	--with-zlib-dir=/usr --with-gd=shared,/usr --enable-gd-native-ttf --with-jpeg-dir=shared,/usr \
	--with-png-dir=shared,/usr --enable-intl=shared --without-t1lib --with-libdir=/lib/x86_64-linux-gnu \
	--enable-maintainer-zts --enable-pdo --with-pdo-mysql --with-pdo-pgsql --with-pdo-sqlite --with-pgsql \
	--disable-dom --disable-simplexml --disable-xml --without-pear
```
```shell
./configure --prefix=/usr --with-apxs2=/usr/bin/apxs2 --with-config-file-path=/etc/php5/apache2 --with-config-file-scan-dir=/etc/php5/apache2/conf.d --build=x86_64-linux-gnu --host=x86_64-linux-gnu --sysconfdir=/etc --localstatedir=/var --mandir=/usr/share/man --disable-debug --with-regex=php --disable-rpath --disable-static --with-pic --with-layout=GNU --with-pear=/usr/share/php --without-gdbm --with-iconv --enable-exif --enable-ftp --with-gettext --enable-mbstring --with-pcre-regex=/usr --enable-shmop --enable-sockets --with-libxml-dir=/usr --with-zlib --enable-zip --without-mm --with-curl=shared,/usr --with-zlib-dir=/usr --enable-gd-native-ttf --with-jpeg-dir=shared,/usr --with-png-dir=shared,/usr --enable-intl=shared --without-t1lib --with-libdir=/lib/x86_64-linux-gnu --enable-maintainer-zts --enable-pdo --with-pdo-mysql --with-pdo-pgsql --with-pdo-sqlite --with-pgsql --disable-dom --disable-xml --without-pear --disable-simplexml
```

# 设置语言环境

```shell
apt-get install -y language-pack-en-base
vi /etc/default/locale
-- LANG="en_US.UTF-8"
-- LANGUAGE="en_US:en"
-- LC_ALL="en_US.UTF-8"
sudo locale-gen
# locale-gen zh_CN.UTF-8
sudo dpkg-reconfigure locales
apt-get install software-properties-common
add-apt-repository ppa:ondrej/php5
```

在Linux中通过locale来设置程序运行的不同语言环境，locale由ANSI C提供支持。
locale的命名规则为<语言>_<地区>.<字符集编码>，如zh_CN.UTF-8，zh代表中文，CN代表大陆地区，UTF-8表示字符集。在locale环境中，有一组变量，代表国际化环境中的不同设置：

1. LC_COLLATE
定义该环境的排序和比较规则

2. LC_CTYPE
用于字符分类和字符串处理，控制所有字符的处理方式，包括字符编码，字符是单字节还是多字节，如何打印等。是最重要的一个环境变量。

3. LC_MONETARY
货币格式

4. LC_NUMERIC
非货币的数字显示格式

5. LC_TIME
时间和日期格式

6. LC_MESSAGES
提示信息的语言。另外还有一个LANGUAGE参数，它与LC_MESSAGES相似，但如果该参数一旦设置，则LC_MESSAGES参数就会失效。LANGUAGE参数可同时设置多种语言信息，如LANGUANE="zh_CN.GB18030:zh_CN.GB2312:zh_CN"。

7. LANG
LC_*的默认值，是最低级别的设置，如果LC_*没有设置，则使用该值。类似于 LC_ALL。

8. LC_ALL
它是一个宏，如果该值设置了，则该值会覆盖所有LC_*的设置值。注意，LANG的值不受该宏影响。