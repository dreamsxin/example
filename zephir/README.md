# Zephir 安装

* composer

```shell
composer config -g repo.packagist composer https://packagist.phpcomposer.com
# or
composer config repo.packagist composer https://packagist.phpcomposer.com
```

```shell
sudo apt-get install git gcc make re2c php php-json php-dev libpcre3-dev build-essential
sudo apt-get install composer libgmp-dev

git clone https://github.com/phalcon/php-zephir-parser.git
cd php-zephir-parser
phpize
./configure
make
sudo make install

git clone https://github.com/phalcon/zephir
cd zephir
composer install --dev
./zephir
```

## Mac 安装 re2c

```shell
brew install re2c
```

# 创建项目

```shell
zephir init mytest
cd mytest
```

## 生成 C 代码

```shell
zephir generate
cd ext
phpize
./configure
make
make install
```

## 编译项目

```shell
zephir compile
```

## 安装项目

```shell
zephir install
```

上面的三个命令也可以合成一个命令｀zephir build｀ or `zephir builddev`
