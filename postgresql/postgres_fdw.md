* 安装fdw扩展

```sql
CREATE EXTENSION postgres_fdw;
```

* 查询拓展

```sql
SELECT * FROM pg_available_extensions; 
```

* 创建远程 server

`server_remote` 是给这个远程 `server` 取得名字。`options` 中是连接远程 `server` 需要的信息，包括地址、端口和需要连接的数据库名。

```sql
CREATE SERVER fdw_server 
	FOREIGN DATA WRAPPER postgres_fdw 
	OPTIONS(host '192.168.1.2', port '5432', dbname 'phalcon_test');
```

* 查看所有远程 server

```sql
SELECT * from pg_foreign_server;
```

* 创建用户匹配信息

表示在 fdw_server 下为角色 postgres 创建一个用户匹配信息，options 里是用户名和密码。

```sql
CREATE USER MAPPING FOR POSTGRES SERVER fdw_server OPTIONS(user 'postgres', password 'yourpassword');
```

* 在本地创建外部表

用到的表为`remote_table`和`remote_table2`。

>> 注：本地 local_table 表中字段列数可以少于远程 remote_table 表，但是字段名必须完成一致，否则会提示错误。

```sql
CREATE FOREIGN TABLE local_table (
	id SERIAL,
	name TEXT,
) SERVER fdw_server OPTIONS (table_name 'remote_table');

CREATE FOREIGN TABLE local_table2 (
	id SERIAL,
	name TEXT
) SERVER fdw_server OPTIONS (table_name 'remote_table2');
```

创建完后，可以看到本地也生成了`t_resource`表和`t_permission`表。

* 自动导入所有外部表到指定模式

```sql
CREATE SCHEMA fdw;

-- Import the schema
IMPORT FOREIGN SCHEMA public
  FROM SERVER fdw_server
  INTO fdw;
```
也可以指定表：
```sql
IMPORT FOREIGN SCHEMA public LIMIT TO (remote_table, remote_table_2)
  FROM SERVER fdw_server
  INTO fdw;
```

* 外部表更新选项

```sql
ALTER SERVER fdw_server  
	OPTIONS (ADD updatable 'false')
```
限制单个外部表不可更新
```sql
ALTER FOREIGN TABLE remote_table  
	OPTIONS (ADD updatable 'false');  
```