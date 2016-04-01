# TreeFrog 安装

## 下载源码安装

```shell
sudo apt-get install -y qt4-qmake libqt4-dev libqt4-sql-mysql libqt4-sql-psql g++
git clone https://github.com/treefrogframework/treefrog-framework.git
cd treefrog-framework/
./configure --enable-debug
cd src && make && sudo make install
cd ../tools && make && sudo make install
```

## 安装数据库驱动
```shell
sudo apt-get install libqt5sql5-mysql libqt5sql5-psql
```

## 显示支持的数据库驱动
```shell
tspawn --show-drivers
```

# 创建一个 blog 应用

## 生成应用目录结构
```shell
tspawn new blogapp
```

## 创建数据库
```shell
cd blogapp
sqlite3 db/blogdb
sqlite> CREATE TABLE blog (id INTEGER PRIMARY KEY AUTOINCREMENT, title VARCHAR(20), body VARCHAR(200), created_at TIMESTAMP, updated_at TIMESTAMP, lock_revision INTEGER);
sqlite> .quit
```

## 修改配置文件 `config/database.ini`
```ini
[dev]
DriverType=QSQLITE
DatabaseName=db/blogdb
```

## 测试数据库
```shell
tspawn --show-tables
```

## 设置模版引擎 `config/development.ini`
```ini
TemplateSystem=ERB
```

## 根据数据库自动生成代码
```shell
tspawn scaffold blog
```