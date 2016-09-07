# mysql 使用

## 安装

```shell
sudo apt-get install mysql-server
```

## 命令行

### 登录
```shell
mysql -uroot -p
```

### 创建数据库

```shell
create database phalcon_test charset=utf8 collate=utf8_unicode_ci;
```

### 切换数据库

```shell
use phalcon_test;
```

### 导入sql

```shell
source schemas/mysql/phalcon_test.sql
```

### 退出

```shell
\q
exit
```

## 忘记密码

编辑mysql的配置文件`/etc/mysql/my.cnf`，在`[mysqld]`段下加入一行“skip-grant-tables”。用空密码进入mysql管理命令行，切换到mysql库。

或者使用`/etc/mysql/debian.cnf`里的账户密码登录修改密码。

```conf
# Automatically generated for Debian scripts. DO NOT TOUCH!
[client]
host     = localhost
user     = debian-sys-maint
password = Fcbd8XbszEm0FIbe
socket   = /var/run/mysqld/mysqld.sock
[mysql_upgrade]
host     = localhost
user     = debian-sys-maint
password = Fcbd8XbszEm0FIbe
socket   = /var/run/mysqld/mysqld.sock
basedir  = /usr
```

```shell
mysql -u debian-sys-maint -p
```

```shell
update user set password=PASSWORD("123456") where user='root';
```