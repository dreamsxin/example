# mysql 使用

## 安装

```shell
sudo apt-get install mysql-server
```

## 修改 root 验证方式

```shell
# 查看密码
sudo cat /etc/mysql/debian.cnf
mysql -udebian-sys-maint -p
#查看root验证方式
use mysql;
select user,plugin from user;
#修改验证方式
update user set plugin='mysql_native_password' where user='root'; # 修改其密码格式
select user,plugin from user;
#修改密码
alter user 'root'@'localhost' identified by 'lgl000820';
flush privileges;
```

## 命令行

### 登录
```shell
export MYSQL_PWD=your_password
mysql -uroot -p
```

### 免密码登录
`/home/xxxx/.my.cnf`
```conf
[client]
user=your_username
password=your_password
host=your_host
```

### 显示所有库和表

```sql
show databases;
show tables;
desc tablename;
# 显示当前所在库
SELECT DATABASE();
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
ALTER TABLE person ADD INDEX idx_birthday (birthday);
INSERT INTO person (name, birthday) VALUES ('A', '2019-08-01 00:00:00'), ('B', '2019-08-01 23:59:59'), ('C', '2019-08-02 00:00:00');
```

## 创建用户授权

```sql
use mysql;
CREATE USER IF NOT EXISTS "prestashop"@"localhost" IDENTIFIED BY "1234567";
GRANT ALL PRIVILEGES ON *.* TO 'prestashop'@'localhost' WITH GRANT OPTION;
GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, ALTER ON *.* TO 'prestashop'@'localhost';
FLUSH PRIVILEGES;
```
**可能错误**
`ERROR 1396 (HY000): Operation xxx failed for user`
```sql
-- Check if a user exists
SELECT User, Host FROM mysql.user WHERE User = 'username' AND Host = 'hostname';

-- Creating users
CREATE USER IF NOT EXISTS 'username'@'hostname' IDENTIFIED BY 'password';

-- Dropping users
DROP USER IF EXISTS 'username'@'hostname';

-- List all instances of the user with different hosts
SELECT User, Host FROM mysql.user WHERE User = 'username';

-- Then use the exact user and host combination
DROP USER 'username'@'192.168.1.%';  -- Instead of 'username'@'%'

-- After direct modifications to mysql tables
FLUSH PRIVILEGES;

SELECT EXISTS(SELECT 1 FROM mysql.user
             WHERE User = 'username' AND Host = 'hostname') AS user_exists;
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

## windows

```shell
net start mysql
net stop mysql

services.msc
```

## 远程连接

在Ubuntu上配置MySQL允许远程连接需完成以下步骤，确保操作准确性与安全性：

### 一、修改MySQL配置文件
1. **调整监听地址**  
   打开配置文件（路径通常为`/etc/mysql/mysql.conf.d/mysqld.cnf`）：
   ```bash
   sudo nano /etc/mysql/mysql.conf.d/mysqld.cnf
   ```
   找到`bind-address = 127.0.0.1`改为`bind-address = 0.0.0.0`（允许所有IP访问）。



2. **保存并退出**  
   按`Ctrl+O`保存，`Ctrl+X`退出编辑器。

### 二、创建远程访问用户并授权
1. **登录MySQL**  
   ```bash
   sudo mysql -u root -p
   ```

2. **创建远程用户**  
   替换`username`和`password`：
   ```sql
   CREATE USER 'username'@'%' IDENTIFIED BY 'password';
   ```

3. **授予全局权限**  
   ```sql
   GRANT ALL PRIVILEGES ON *.* TO 'username'@'%' WITH GRANT OPTION;
   FLUSH PRIVILEGES;  -- 刷新权限生效
   ```

### 三、配置防火墙
1. **开放3306端口**  
   ```bash
   sudo ufw allow 3306/tcp  # 允许TCP流量通过
   sudo ufw reload          # 重载规则
   ```



2. **验证防火墙状态**  
   ```bash
   sudo ufw status  # 检查3306端口是否允许
   ```

### 四、重启MySQL服务
```bash
sudo systemctl restart mysql  # 应用配置修改
```

### 五、验证远程连接
在远程主机使用命令测试：
```bash
mysql -h [服务器IP] -u username -p
```
输入密码后成功登录即配置完成。

---

#### ️ 安全建议
- 避免直接授权`root`用户远程访问，改用普通用户。
- 限制IP范围：将`'%'`替换为具体IP（如`'192.168.1.%'`）。
- 云服务器需在安全组中开放3306端口。

