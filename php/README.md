Ubuntu 编译 PHP7
----------------

```shell
 ./buildconf --force

./configure --prefix=/usr/local/php --with-config-file-path=/usr/local/php/etc --with-fpm-user=www-data --with-fpm-group=www-data --with-pdo-pgsql --with-pdo-mysql --with-pdo-sqlite  --with-iconv-dir --with-freetype-dir --with-jpeg-dir --with-png-dir --with-zlib --with-libxml-dir=/usr --enable-xml --disable-rpath --enable-bcmath --enable-shmop --enable-sysvsem --enable-inline-optimization --with-curl --enable-mbregex --enable-mbstring --with-mcrypt --enable-ftp --with-gd --enable-gd-native-ttf --with-openssl --with-mhash --enable-pcntl --enable-sockets --with-xmlrpc --enable-zip --enable-soap --without-pear --with-gettext --disable-fileinfo --enable-maintainer-zts 

make -j4

sudo make install
```