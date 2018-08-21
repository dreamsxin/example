# SQLite3

Command 	Description
- .show 	Displays current settings for various parameters
- .databases 	Provides database names and files
- .quit 	Quit sqlite3 program
- .tables 	Show current tables
- .schema 	Display schema of table
- .header 	Display or hide the output table header
- .mode 	Select mode for the output table
- .dump 	Dump database in SQL text format

## 创建数据库

```shell
sqlite3 ./phalcon_test.sqlite < "schemas/sqlite/phalcon_test.sql"
```

## 打开或创建

```shell
sqlite3 ./phalcon_test.sqlite
```

## 显示表名

```shell
.tables
```

## 显示数据库结构

```shell
.schema
.schema 表名
```

## 退出

```shell
.quit
.exit
```