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

### 显示所有库和表

```sql
show database;
show tables;
```

### 创建数据库

```sql
create database phalcon_test charset=utf8 collate=utf8_unicode_ci;
```

### 切换数据库

```sql
use phalcon_test;
```

### 创建表

```sql
CREATE TABLE person(
    id int not null auto_increment,
    name varchar(8),
    birthday datetime,
    constraint pk__person primary key(id)
);
INSERT INTO person (name, birthday) VALUES ('A', '2019-08-01 00:00:00'), ('B', '2019-08-01 23:59:59'), ('C', '2019-08-02 00:00:00');
```

## 创建用户授权

```sql
use mysql;
CREATE USER "prestashop"@"localhost" IDENTIFIED BY "1234567";
GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER ON "prestashop".* TO "prestashop"@"localhost";
FLUSH PRIVILEGES;
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

```sql
update user set password=PASSWORD("123456") where user='root';
```

## MATCH AGAINST

```SQL
SELECT pages.*,
       MATCH (head, body) AGAINST ('some words') AS relevance,
       MATCH (head) AGAINST ('some words') AS title_relevance
FROM pages
WHERE MATCH (head, body) AGAINST ('some words')
ORDER BY title_relevance DESC, relevance DESC

-- alternatively:
ORDER BY title_relevance + relevance DESC
```

## error

*  Access denied for user 'root'@'localhost' 

```sql
grant all privileges on *.* to 'root'@'localhost' identified by 'root' with grant option;
```

## 批量删除数据表

```shell
for table_name in `mysql -uroot --password=123456 -e 'use prestashop2; show tables' | grep ps2_`
do
  mysql -uroot  --password=123456 -e 'use prestashop2; drop table if exists $table_name '
done
```