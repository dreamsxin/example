# 编译

```shell
cd ext/openssl/
mv config0.m4 config.m4
phpize && ./configure
```

```shell
cd ext/mysqlnd
cp config9.m4 config.m4
sed -ie "s{ext/mysqlnd/php_mysqlnd_config.h{config.h{" mysqlnd_portability.h
phpize && ./configure
```
