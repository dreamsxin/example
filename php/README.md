Ubuntu 编译 PHP7
----------------

```shell
 ./buildconf --force

./configure --prefix=/usr/local/php --with-config-file-path=/usr/local/php/etc --with-fpm-user=www-data --with-fpm-group=www-data --with-pdo-pgsql --with-pdo-mysql --with-pdo-sqlite  --with-iconv-dir --with-freetype-dir --with-jpeg-dir --with-png-dir --with-zlib --with-libxml-dir=/usr --enable-xml --disable-rpath --enable-bcmath --enable-shmop --enable-sysvsem --enable-inline-optimization --with-curl --enable-mbregex --enable-mbstring --with-mcrypt --enable-ftp --with-gd --enable-gd-native-ttf --with-openssl --with-mhash --enable-pcntl --enable-sockets --with-xmlrpc --enable-zip --enable-soap --without-pear --with-gettext --disable-fileinfo --enable-maintainer-zts --enable-phpdbg-debug --enable-debug

make -j4

sudo make install

```

安装扩展

```shell
/usr/local/php/bin/phpize
./configure CFLAGS="-g3 -O0 -std=gnu90 -Wall -Werror -Wno-error=uninitialized" --with-php-config=/usr/local/php/bin/php-config
```

解压文件乱码
------------

在 Ubuntu 下，当PHP的执行用户为 www-data 时，调用 shell 命令进行解压时，有可能乱码，因为 www-data 用户语言环境没有设置。正确的方法如下：
```shell
putenv('LANG=en_US.UTF-8');
exec('7za x test.7z -r -y -o ./test');
# or
exec('LANG=en_US.UTF-8 7za x test.7z -r -y -o ./test');
```

Valgrind 内存泄漏检查

```shell
export USE_ZEND_ALLOC=0; valgrind --tool=memcheck --num-callers=30 --log-file=memcache.log /usr/local/php/bin/php test.php
# or
setenv USE_ZEND_ALLOC 0
valgrind --leak-check=full /usr/local/php/bin/php test.php
 ```


# 覆盖安装
```shell
php-config
```

```shell
./configure --prefix=/usr --with-apxs2=/usr/bin/apxs2 --with-config-file-path=/etc/php5/apache2 --with-config-file-scan-dir=/etc/php5/apache2/conf.d --build=x86_64-linux-gnu --host=x86_64-linux-gnu --sysconfdir=/etc --localstatedir=/var --mandir=/usr/share/man --disable-debug --with-regex=php --disable-rpath --disable-static --with-pic --with-layout=GNU --without-pear --enable-calendar --enable-sysvsem --enable-sysvshm --enable-sysvmsg --enable-bcmath --with-bz2 --enable-ctype --with-db4 --with-qdbm=/usr --without-gdbm --with-iconv --enable-exif --enable-ftp --with-gettext --enable-mbstring --with-onig=/usr --with-pcre-regex=/usr --enable-shmop --enable-sockets --enable-wddx --with-libxml-dir=/usr --with-zlib --with-kerberos=/usr --with-openssl=/usr --enable-soap --enable-zip --with-mhash=yes --with-system-tzdata --with-mysql-sock=/var/run/mysqld/mysqld.sock --enable-dtrace --without-mm --with-curl=shared,/usr --with-enchant=shared,/usr --with-zlib-dir=/usr --with-gd=shared,/usr --enable-gd-native-ttf --with-gmp=shared,/usr --with-jpeg-dir=shared,/usr --with-xpm-dir=shared,/usr/X11R6 --with-png-dir=shared,/usr --with-freetype-dir=shared,/usr --with-vpx-dir=shared,/usr --with-imap=shared,/usr --with-imap-ssl --enable-intl=shared --without-t1lib --with-ldap=shared,/usr --with-ldap-sasl=/usr --with-mcrypt=shared,/usr --with-mysql=shared,/usr --with-mysqli=shared,/usr/bin/mysql_config --with-pspell=shared,/usr --with-unixODBC=shared,/usr --with-recode=shared,/usr --with-xsl=shared,/usr --with-snmp=shared,/usr --with-sqlite3=shared,/usr --with-mssql=shared,/usr --with-tidy=shared,/usr --with-xmlrpc=shared --with-pgsql=shared,/usr PGSQL_INCLUDE=/usr/include/postgresql --enable-pdo=shared --without-pdo-dblib --with-pdo-mysql=shared,/usr --with-pdo-odbc=shared,unixODBC,/usr --with-pdo-pgsql=shared,/usr/bin/pg_config --with-pdo-sqlite=shared,/usr --with-pdo-dblib=shared,/usr --with-interbase=shared,/usr --with-pdo-firebird=shared,/usr build_alias=x86_64-linux-gnu host_alias=x86_64-linux-gnu 


./configure --enable-fpm --prefix=/usr --with-config-file-path=/etc/php5/fpm --with-config-file-scan-dir=/etc/php5/fpm/conf.d --with-fpm-user=www-data --with-fpm-group=www-data --with-pdo-pgsql --with-pdo-mysql --with-pdo-sqlite  --with-iconv-dir --with-freetype-dir --with-jpeg-dir --with-png-dir --with-zlib --with-libxml-dir=/usr --enable-xml --disable-rpath --enable-bcmath --enable-shmop --enable-sysvsem --enable-inline-optimization --with-curl --enable-mbregex --enable-mbstring --with-mcrypt --enable-ftp --with-gd --enable-gd-native-ttf --with-openssl --with-mhash --enable-pcntl --enable-sockets --with-xmlrpc --enable-zip --enable-soap --without-pear --with-gettext --disable-fileinfo --enable-maintainer-zts --enable-phpdbg-debug
--enable-debug

./configure --enable-fpm --prefix=/usr --with-config-file-path=/etc/php5/fpm --with-config-file-scan-dir=/etc/php5/fpm/conf.d --with-fpm-user=www-data --with-fpm-group=www-data  --build=x86_64-linux-gnu --host=x86_64-linux-gnu --sysconfdir=/etc --localstatedir=/var --mandir=/usr/share/man --disable-debug --with-regex=php --disable-rpath --disable-static --with-pic --with-layout=GNU --with-pear=/usr/share/php --without-gdbm --with-iconv --enable-exif --enable-ftp --with-gettext --enable-mbstring --with-pcre-regex=/usr --enable-shmop --enable-sockets --with-libxml-dir=/usr --with-zlib --enable-zip --without-mm --with-curl=shared,/usr --with-zlib-dir=/usr --enable-gd-native-ttf --with-jpeg-dir=shared,/usr --with-png-dir=shared,/usr --enable-intl=shared --without-t1lib --with-libdir=/lib/x86_64-linux-gnu --enable-maintainer-zts --enable-pdo --with-pdo-mysql --with-pdo-pgsql --with-pdo-sqlite --with-pgsql --disable-dom --disable-xml --without-pear --disable-simplexml


./configure --prefix=/usr --with-apxs2=/usr/bin/apxs2 --with-config-file-path=/etc/php5/apache2 --with-config-file-scan-dir=/etc/php5/apache2/conf.d --build=x86_64-linux-gnu --host=x86_64-linux-gnu --sysconfdir=/etc --localstatedir=/var --mandir=/usr/share/man --disable-debug --with-regex=php --disable-rpath --disable-static --with-pic --with-layout=GNU --with-pear=/usr/share/php --without-gdbm --with-iconv --enable-exif --enable-ftp --with-gettext --enable-mbstring --with-pcre-regex=/usr --enable-shmop --enable-sockets --with-libxml-dir=/usr --with-zlib --enable-zip --without-mm --with-curl=shared,/usr --with-zlib-dir=/usr --enable-gd-native-ttf --with-jpeg-dir=shared,/usr --with-png-dir=shared,/usr --enable-intl=shared --without-t1lib --with-libdir=/lib/x86_64-linux-gnu --enable-maintainer-zts --enable-pdo --with-pdo-mysql --with-pdo-pgsql --with-pdo-sqlite --with-pgsql --disable-dom --disable-xml --without-pear --disable-simplexml

sudo apt-get install apache2-prefork-dev

./configure --prefix=/usr --with-apxs2=/usr/bin/apxs2 --with-config-file-path=/etc/php5/apache2 --with-config-file-scan-dir=/etc/php5/apache2/conf.d  --sysconfdir=/etc --localstatedir=/var --mandir=/usr/share/man --disable-debug --with-regex=php --disable-rpath --disable-static --with-pic --with-layout=GNU --with-pear=/usr/share/php --without-gdbm --with-iconv --enable-exif --enable-ftp --with-gettext --enable-mbstring --enable-shmop --enable-sockets --with-zlib --enable-zip --without-mm --with-curl --enable-gd-native-ttf --with-jpeg-dir=shared,/usr --with-png-dir=shared,/usr --enable-intl=shared --enable-maintainer-zts --enable-pdo --with-pdo-mysql --with-pdo-pgsql --with-pdo-sqlite --with-pgsql --disable-dom --disable-xml --without-pear --disable-simplexml --disable-xmlreader
```