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
./configure --prefix=/usr --with-apxs2=/usr/bin/apxs2 --with-config-file-path=/etc/php5/apache2 --with-config-file-scan-dir=/etc/php5/apache2/conf.d --build=x86_64-linux-gnu --host=x86_64-linux-gnu --sysconfdir=/etc --localstatedir=/var --mandir=/usr/share/man --disable-debug --with-regex=php --disable-rpath --disable-static --with-pic --with-layout=GNU --with-pear=/usr/share/php --without-gdbm --with-iconv --enable-exif --enable-ftp --with-gettext --enable-mbstring --with-pcre-regex=/usr --enable-shmop --enable-sockets --with-libxml-dir=/usr --with-zlib --enable-zip --without-mm --with-curl=shared,/usr --with-zlib-dir=/usr --enable-gd-native-ttf --with-jpeg-dir=shared,/usr --with-png-dir=shared,/usr --enable-intl=shared --without-t1lib --with-libdir=/lib/x86_64-linux-gnu --enable-maintainer-zts --enable-pdo --with-pdo-mysql --with-pdo-pgsql --with-pdo-sqlite --with-pgsql --disable-dom --disable-xml --without-pear --disable-simplexml