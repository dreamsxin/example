# 命令行使用

输入`help`可以看到基本操作命令：
```shell
mongo
> help
```

- show dbs: 显示数据库列表
- show collections：显示当前数据库中的集合（类似关系数据库中的表）
- show users：显示用户

- use <db name>：切换当前数据库，这和MS-SQL里面的意思一样
- db.help()：显示数据库操作命令，里面有很多的命令
- db.foo.help()：显示集合操作命令，同样有很多的命令，foo指的是当前数据库下，一个叫foo的集合，并非真正意义上的命令
- db.foo.find()：对于当前数据库中的foo集合进行数据查找（由于没有条件，会列出所有数据）
- db.foo.find( { a : 1 } )：对于当前数据库中的foo集合进行查找，条件是数据中有一个属性叫a，且a的值为1

## 查找所有记录

数据库 `phalcon_test` 存在集合 `caches`。

```shell
use phalcon_test;
show collections;
db.caches.find();
```
