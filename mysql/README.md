# mysql 使用

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