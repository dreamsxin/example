#

MindsDB is the platform for customizing AI from enterprise data.

With MindsDB, you can deploy, serve, and fine-tune models in real-time, utilizing data from databases, vector stores, or applications, to build AI-powered apps - using universal tools developers already know.

https://github.com/mindsdb/mindsdb

## 安装

MindsDB 是一个第三方应用程序，通过 SQL 与 MariaDB Server 进行交互，提供机器学习功能。接口是通过Connect存储引擎完成的。

要获得可用的 MariaDB - MindsDB 安装，需要安装以下组件：

- MindsDB：按照项目的官方文档中的说明进行操作。
- Connect Storage Engine 必须启用才能使集成工作。请参阅安装connect存储引擎。

## 连接到 mysql
MindsDB 通过常规用户连接到 MariaDB Server，以设置一个名为 mindsdb 的专用数据库。将使用哪个用户在 MindsDB 的配置文件中指定。

例如，如果 MindsDB 安装在本地，则可以创建一个名为 mindsdb@localhost 的用户。MindsDB 只通过mysql_native_password插件进行身份验证，因此必须为用户设置密码：
```sql
CREATE USER mindsdb@localhost;
SET PASSWORD for mindsdb@localhost=PASSWORD("password");
```
必须授予该用户全局FILE权限和 mindsdb 数据库的所有权限。
```sql
GRANT FILE on *.* to mindsdb@localhost;
GRANT ALL on mindsdb.* to mindsdb@localhost;
```
假设 MindsDB 在 Python 路径中，可以使用以下参数启动 MindsDB：
```sql
python -m mindsdb --config=$CONFIG_PATH --api=http,mysql
```
请确保$CONFIG_PATH指向适当的MindsDB配置文件。

## 用法
MindsDB是一个正在积极开发的项目，请始终参考该项目的官方文档以获取最新的使用场景。

有关逐步示例，您可以参考以下博客文章。

如果MindsDB和MariaDB之间的连接成功，则应该看到mindsdb数据库存在，其中包含两个表：commands和predictors。

作为AutoML框架，MindsDB在训练AI模型方面完成了所有工作。必要的是通过SELECT语句将初始数据传递给它。这可以通过将其插入到predictors表中来完成。
```sql
INSERT INTO `predictors`
       (`name`, `predict`, `select_data_query`)
VALUES ('bikes_model', 'count', 'SELECT * FROM test.bike_data');
```
插入到predictors的值作为命令，指示MindsDB执行以下操作：

训练名为'bikes_model'的模型
从输入数据中学习预测'count'列。
输入数据是通过选择语句 'SELECT * FROM test.bike_data' 生成的。
`select_data_query` 应该是MindsDB可以针对MariaDB运行的有效选择。
