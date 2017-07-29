

## 安装

```shell
sudo aptitude install zabbix-server-pgsql zabbix-frontend-php php7.1-pgsql libapache2-mod-php7.1
```

## 解压 zabbix-server-pgsql 自带压缩包

```shell
sudo gunzip /usr/share/zabbix-server-pgsql/*.gz
```

## 创建 zabbix 数据库用户和数据库

```shell
sudo -u postgres psql
```

```sql
create user zabbix with password 'zabbix';
create database zabbix owner zabbix;
```
使用 zabbix 用户登录
```shell
psql -U zabbix -d zabbix
```
导入schema、image、data
```sql
\c zabbix
\i /usr/share/zabbix-server-pgsql/schema.sql
\i /usr/share/zabbix-server-pgsql/images.sql
\i /usr/share/zabbix-server-pgsql/data.sql
```

## 修改 zabbix-server 配置

`/etc/zabbix/zabbix_server.conf`

默认情况下 `DBName` 和 `DBUser` 已经设置好，只需要添加DBpassword就可以了，最后复制原有的行，然后修改：
```shell
DBName=zabbix
DBUser=zabbix
DBPassword=zabbix
```

## 拷贝已有的zabbix配置模板

```shell
sudo cp /usr/share/doc/zabbix-frontend-php/examples/zabbix.conf.php.example /etc/zabbix/zabbix.conf.php
```
拷贝已有的apache配置模板
```shell
sudo cp /usr/share/doc/zabbix-frontend-php/examples/apache.conf /etc/apache2/conf-available/zabbix.conf
```

## 修改zabbix配置模板

`/etc/zabbix/zabbix.conf.php`

默认是"MYSQL"的，改为`POSTGRESQL`，修改数据库名以及账户和密码。

## 使apache配置生效

```shell
sudo a2enconf zabbix.conf
sudo a2enmod alias
sudo service apache2 restart
```

## 修改php参数

`/etc/php/7.1/apache2/php.ini`
```ini
post_max_size = 16M
max_execution_time = 300
max_input_time = 300
date.timezone = Asia/Shanghai
```

## 配置 zabbix server 启动

`/etc/default/zabbix-server`

修改项：

`START=yes`

## 重启 zabbix server

```shell
sudo service zabbix-server restart
```

访问

http://xxx.xxx.xxx.xxx/zabbix

缺省的账户：

- Username = admin
- Password = zabbix

## 本机监控

```shell
sudo apt-get install zabbix-agent
sudo service zabbix-agent restart
```

## 客户端配置

`/etc/zabbix/zabbix_agentd.conf`

调整项
```conf
Server=127.0.0.1		# 修改为 zabbix server 服务器的IP，如果有网关或被监控机为虚拟机也加上母机的IP
ServerActive=127.0.0.1	# 修改为 zabbix server 服务器的IP
Hostname=Zabbix server	# 修改为网页里面添加的Hostname，需要保持一致。
```
