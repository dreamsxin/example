# ClickHouse

## 安装

```shell
sudo apt-get install clickhouse-server-common clickhouse-client -y
```

## 运行

```shell
sudo service clickhouse-server start
# or
clickhouse-server --config-file=/etc/clickhouse-server/config.xml
```

## 配置

`/etc/clickhouse-server/config.xml`

## 组件介绍

- clickhouse-server
ClickHouse 的Server端，也就是CK数据库的核心程序

- clickhouse-client
ClickHouse 自带的client端，提供命令行的交互操作方式，来连接服务端


* clickhouse-client 参数介绍：

- --host, -h：目标服务器名，默认为 localhost
- --port：目标端口，默认为 9000
- --user, -u：连接用户，默认为 default
- --password：连接用户密码，默认为空字符串
- --query, -q：非交互模式下执行的命令
- --database, -d：当前操作的数据库，默认选择配置文件配置的值（默认为 default 库）
- --multiline, -m：如果设定，允许多行查询
- --multiquery, -n：如果指定,允许处理由分号分隔的多个查询。只有在非交互式模式工作。
- --format, -f：使用指定的默认格式输出结果
- --vertical, -E：如果指定，默认使用垂直格式输出结果，等同于 --format=Vertical。在这种格式中，每个值可在单独的行上，显示宽表时很有用。
- --time, -t：如果指定，在 stderr 中输出查询执行时间的非交互式模式下。
- --stacktrace：如果指定，如果发生异常，也会输出堆栈跟踪。
- --config-file：配置文件的名称，额外的设置或改变了上面列出的设置默认值。

默认情况下，配置文件的搜索顺序如下：

- ./clickhouse-client.xml
- ~/.clickhouse-client/config.xml
- /etc/clickhouse-client/config.xml

如果三个文件同时存在N个，则以找到的第一个配置文件为准。

这个客户端还可以连接到一个远程服务端：

clickhouse-client --host=example.com

还可以指定将用于处理查询的任何设置，如：clickhouse-client --max_threads=1，表示查询处理线程的最大数量为 1。

## 集群

* 如何实现分布式的

ClickHouse 的分布式，完全依赖配置文件，即每个节点，都共享同样的配置文件，这个配置文件里，写了我跟谁是一个cluster的，我自己的名字是啥

