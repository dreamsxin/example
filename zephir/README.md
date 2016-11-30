# Zephir 安装

```shell
git clone https://github.com/phalcon/zephir
cd zephir
./install -c
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
