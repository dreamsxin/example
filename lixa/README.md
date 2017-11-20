# lixa

LIXA (LIbre XA) 是一个事务管理器，实现了分布式事务处理 XA 规范和 TX(transaction demarcation) 规范。

https://github.com/tiian/lixa

## 拓扑

![Aaron Swartz](https://raw.githubusercontent.com/dreamsxin/example/master/lixa/img/LIXA_Configuration_1.png)

## 编译

```shell
tar xvzf lixa-X.Y.Z.tar.gz
cd lixa-X.Y.Z
./configure
make
sudo make install
```

重建

```shell
 autoreconf -v -f -i
 ```

## Postgresql

```shell
./configure --with-postgresql-include=/usr/include/postgresql --with-postgresql-lib=/usr/lib
```