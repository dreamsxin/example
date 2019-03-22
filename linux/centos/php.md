## CentOS 6 安装 PHP 7

```shell
yum search php
# yum install epel-release yum-utils
# yum install http://rpms.remirepo.net/enterprise/remi-release-7.rpm
yum --enablerepo=remi-safe install php73-php php73-php-common php73-php-fpm php73-php-mysql php73-php-pecl-memcached php73-php-mbstring php73-php-mcrypt php73-php-cli php73-php-pdo
yum remove php73-php php73-php-common php73-php-fpm php73-php-mysql php73-php-pecl-memcached php73-php-mbstring php73-php-mcrypt php73-php-cli php73-php-pdo
# 替换默认 PHP 链接，不建议
# source /opt/remi/php71/enable
```
配置文件
- `/etc/opt/remi/php73/php-fpm.conf`
- `/etc/opt/remi/php73/php-fpm.d/www.conf`

```shell
service php73-php-fpm restart
```

## 编译 PHP 扩展

```shell
# 命令自动补全
yum install bash-completion
# 升级 autoconf
yum -y install autoconf268
sudo ln -s /usr/bin/autoconf268 /usr/bin/autoconf
sudo ln -s /usr/bin/autoscan268 /usr/bin/autoscan
sudo ln -s /usr/bin/autom4te268 /usr/bin/autom4te
sudo ln -s /usr/bin/autoheader268 /usr/bin/autoheader

/opt/remi/php73/root/usr/bin/phpize
./configure --with-php-config=/opt/remi/php73/root/usr/bin/php-config
```

## 站点配置

`/etc/nginx/conf.d/open2.conf`

```shell
sudo service php73-php-fpm reload
sudo service nginx reload
```