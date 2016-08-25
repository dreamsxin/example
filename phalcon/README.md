# 环境

```shell
sudo apt-get install git vim build-essential libpcre3-dev
```

## 安装 nginx

```shell
sudo add-apt-repository ppa:nginx/stable
sudo apt-get update
sudo apt-get install nginx
```

## 安装 php-fpm

```shell
sudo apt-get install python-software-properties
sudo add-apt-repository ppa:ondrej/php
sudo apt-get update
sudo apt-get install php5.6-fpm php5.6-cli php5.6-dev php5.6-dev php5.6-curl php5.6-pgsql php5.6-mcrypt php5.6-mbstring
```

### 开启php-fpm日志报告

编辑 `/etc/php/5.6/fpm/php.ini`

```conf
error_reporting = E_ALL
display_errors = On
# 追加
php_flag[display_errors] = on
```

### 查看php5运行进程

```shell
ps -waux | grep php5
```

### 关闭重启php5进程
```shell
sudo service php5-fpm stop
# or
sudo /etc/init.d/php5-fpm stop

sudo service php5-fpm start
sudo service php5-fpm restart
sudo service php5-fpm status
```

## 安装 phalcon 扩展

See https://github.com/dreamsxin/cphalcon/wiki/%E6%BA%90%E7%A0%81%E7%BC%96%E8%AF%91%E5%AE%89%E8%A3%85%EF%BC%88Linux%EF%BC%89

```shell
git clone https://github.com/dreamsxin/cphalcon.git
cd cphalcon/ext
phpize
./configure
make -j4 && sudo make install
```

编辑文件 `/etc/php/5.6/mods-available/pdo_pgsql.ini`，增加一行
```ini
extension=phalcon.so
```

