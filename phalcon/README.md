## 安装 php

```shell
# PPA 安装

```shell
sudo apt-get install python-software-properties
sudo add-apt-repository ppa:ondrej/php
sudo apt-get update
sudo apt-get install php5.6-cli php5.6-dev php5.6-dev php5.6-curl php5.6-pgsql php5.6-mcrypt php5.6-mbstring
```

## 安装 phalcon 扩展

See https://github.com/dreamsxin/cphalcon/wiki/%E6%BA%90%E7%A0%81%E7%BC%96%E8%AF%91%E5%AE%89%E8%A3%85%EF%BC%88Linux%EF%BC%89

```shell
sudo apt-get install git gcc make git libpcre3-dev 
git clone https://github.com/dreamsxin/cphalcon.git
cd cphalcon/ext
phpize
./configure
make -j4 && sudo make install
```