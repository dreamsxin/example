
```shell
sudo apt-get install bison
sudo apt-get install libxml2-dev
sudo apt-get install libsqlite3-dev
sudo apt-get install zlib1g-dev
sudo apt-get install libcurl-ocaml-dev
sudo apt-get install libonig-dev #oniguruma

wget https://github.com/php/php-src/archive/refs/tags/php-8.0.7.tar.gz
tar zxvf php-8.0.7.tar.gz
cd php-8.0.7
./buildconf --force
# --with-pdo-pgsql --with-pdo-mysql 
./configure --prefix=/usr/local/php --with-config-file-path=/usr/local/php/etc --with-fpm-user=www-data --with-fpm-group=www-data --with-pdo-sqlite --with-zlib --enable-xml --disable-rpath --enable-bcmath --enable-shmop --enable-sysvsem --with-curl --enable-mbregex --enable-mbstring --enable-ftp --with-openssl --with-mhash --enable-pcntl --enable-sockets --without-pear --with-gettext --disable-fileinfo --enable-phpdbg-debug --enable-debug
make -j4
sudo make install

sudo ln -s /usr/local/php/bin/php /usr/local/bin/php8
sudo ln -s /usr/local/php/bin/phpize /usr/local/bin/phpize8
sudo ln -s /usr/local/php/bin/php-config /usr/local/bin/php-config8
```
