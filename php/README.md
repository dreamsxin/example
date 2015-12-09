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