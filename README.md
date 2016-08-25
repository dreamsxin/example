example
=======

各种代码实例


双网卡NAT
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

# Ubuntu

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

在Linux中通过locale来设置程序运行的不同语言环境，locale由ANSI C提供支持。locale的命名规则为<语言>_<地区>.<字符集编码>，如zh_CN.UTF-8，zh代表中文，CN代表大陆地区，UTF-8表示字符集。在locale环境中，有一组变量，代表国际化环境中的不同设置：

1.    LC_COLLATE
定义该环境的排序和比较规则

2.    LC_CTYPE
用于字符分类和字符串处理，控制所有字符的处理方式，包括字符编码，字符是单字节还是多字节，如何打印等。是最重要的一个环境变量。

3.    LC_MONETARY
货币格式

4.    LC_NUMERIC
非货币的数字显示格式

5.    LC_TIME
时间和日期格式

6.    LC_MESSAGES
提示信息的语言。另外还有一个LANGUAGE参数，它与LC_MESSAGES相似，但如果该参数一旦设置，则LC_MESSAGES参数就会失效。LANGUAGE参数可同时设置多种语言信息，如LANGUANE="zh_CN.GB18030:zh_CN.GB2312:zh_CN"。

7.    LANG
LC_*的默认值，是最低级别的设置，如果LC_*没有设置，则使用该值。类似于 LC_ALL。

8.    LC_ALL
它是一个宏，如果该值设置了，则该值会覆盖所有LC_*的设置值。注意，LANG的值不受该宏影响。