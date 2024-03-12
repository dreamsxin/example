## CHECK 语法
下面是 PostgreSQL CHECK 的语法：

`[CONSTRAINT constraint_name] CHECK(expr)`

### 解释说明：

- constraint_name 是约束的名字。 CONSTRAINT constraint_name 是可选的，只有您需要指定约束名称的时候，才使用此子句。
- expr 是一个布尔表达式。如果表达式结算结果为真，则 PostgreSQL 允许将输入写入到表中，否则 PostgreSQL 拒绝写入数据。

您可以在一个列上或者一个表上使用 CHECK 约束。如果你为一个列使用 CHECK 约束，则 CHECK 表达式只能使用此列。如果你为一个表使用 CHECK 约束，则 CHECK 表达式可以使用表上的所有列。

下面以 age 列需要大于 0 为例，使用不同的方法添加此约束：

**在创建表的时候在列定义中使用 CHECK 约束**
```sql
CREATE TABLE users (
    id INTEGER PRIMARY KEY,
    age INTEGER NOT NULL CHECK(age > 0)
);
```
**在创建表的时候在约束定义中使用 CHECK 约束**
```sql
CREATE TABLE users (
    id INTEGER PRIMARY KEY,
    age INTEGER NOT NULL,
    CONSTRAINT users_age_check CHECK(age > 0)
);
```
**使用修改表语句的添加 CHECK 约束**
```sql
ALTER TABLE users
ADD CONSTRAINT users_age_check CHECK(age > 0);
```
在查询向表中插入一条记录之前，CHECK约束会评估CHECK约束定义中指定的条件。如果条件评估为TRUE，那么查询中指定的值将被插入或更新到表中。如果条件评估为FALSE，那么查询将返回一个违反检查条件的错误并终止查询。

## CHECK 约束实例
通过下面的例子，你会很容易理解 PostgreSQL CHECK 约束的用法和作用。

假设，您需要一个 users 表存储用户的名称，登录名，密码，且需要符合以下要求：

用户的名称不能为空。
登录名的长度不少于 4 个字符。
密码的长度不少于 8 个字符。
密码不能和登录名相同。
注意，在实际的应用中，您不应该将密码的明文存放在数据库中，这是不安全的。

使用以下的 CREATE TABLE 语句创建表：

```sql
CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    name VARCHAR(45) NOT NULL,
    login_name VARCHAR(45) NOT NULL CHECK(length(login_name) >= 4),
    password VARCHAR(45) NOT NULL CHECK(length(password) >= 8),
    CHECK(login_name <> password)
);
```

### 自动转为小写
```sql
ALTER TABLE your_table
  ADD CONSTRAINT your_table_the_column_lowercase_ck
  CHECK (the_column = lower(the_column));
```
也可以使用索引解决大小写问题
```sql
CREATE UNIQUE INDEX idx_lower_unique 
   ON your_table (lower(the_column));
```
