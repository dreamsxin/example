
# 安装
```shell
sudo apt-get install postgresql-16-mysql-fdw
```

## 使用

```psql
-- load extension first time after install
CREATE EXTENSION mysql_fdw;

-- create server object
CREATE SERVER mysql_server FOREIGN DATA WRAPPER mysql_fdw OPTIONS (host '127.0.0.1', port '3306');

-- create user mapping
DROP USER MAPPING IF EXISTS FOR postgres SERVER mysql_server;
CREATE USER MAPPING FOR postgres
	SERVER mysql_server
	OPTIONS (username 'foo', password 'bar');

-- create foreign table
CREATE FOREIGN TABLE warehouse
	(
		warehouse_id int,
		warehouse_name text,
		warehouse_created timestamp
	)
	SERVER mysql_server
	OPTIONS (dbname 'db', table_name 'warehouse');
```

## 授权

```psql
GRANT USAGE ON FOREIGN TABLE warehouse TO postgres;
```
