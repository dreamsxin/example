# 异常处理(Exception)

## 错误和消息

利用 RAISE 语句报告消息息以及抛出错误。
```sql
RAISE [ level ] 'format' [, expression [, ... ]] [ USING option = expression [, ... ] ];
RAISE [ level ] condition_name [ USING option = expression [, ... ] ];
RAISE [ level ] SQLSTATE 'sqlstate' [ USING option = expression [, ... ] ];
RAISE [ level ] USING option = expression [, ... ];
RAISE ;
```

level选项声明了错误的严重性等级，可能的级别有 DEBUG, LOG, INFO, NOTICE, WARNING, 和EXCEPTION，默认的是 EXCEPTION。
EXCEPTION 会抛出一个错误（强制关闭当前事务），而其他级别仅仅是产生不同优先级的消息。无论是将特定优先级别的消息报告给客户端，还是写到服务器日志，亦或是二者都是， 都是由 log_min_messages 和 client_min_messages 配置变量控制的。 

在 level 之后， 你可以写 format，（这必须是一个简单的字符串文本）。格式字符串声明要报告的错误消息文本。格式字符串后面可以跟着要插入到消息中的可选的参数表达式。 在格式字符串里，`%`被下一个可选参数的外部表现形式代替。 要表示`%`字符必须写成`%%`。

在这个例子里，`v_job_id`的值将代替字符串中的`%`：
```sql
RAISE NOTICE 'Calling cs_create_job(%)', v_job_id;
```

通过写一个后面跟着option = expression项的USING，可以为错误报告附加一些额外信息。每一个expression可以是任意字符串值的表达式。允许的option关键词是：

- MESSAGE
设置错误消息文本。这个选项可以被用于在USING之前包括一个格式字符串的RAISE形式。

- DETAIL
提供一个错误的细节消息。

- HINT
提供一个提示消息。

- ERRCODE
指定要报告的错误代码（SQLSTATE），可以用附录 A中所示的条件名（http://www.postgres.cn/docs/9.6/errcodes-appendix.html），或者直接作为一个五字符的 SQLSTATE 代码。

- COLUMN
- CONSTRAINT
- DATATYPE
- TABLE
- SCHEMA

提供一个相关对象的名称。

这个例子将用给定的错误消息和提示中止事务：
```sql
RAISE EXCEPTION 'Nonexistent ID --> %', user_id
      USING HINT = 'Please check your user ID';
```
这两个例子展示了设置 SQLSTATE 的两种等价的方法：
```sql
RAISE 'Duplicate user ID: %', user_id USING ERRCODE = 'unique_violation';
RAISE 'Duplicate user ID: %', user_id USING ERRCODE = '23505';
```
还有第二种RAISE语法，在其中主要参数是要被报告的条件名或 SQLSTATE，例如：
```sql
RAISE division_by_zero;
RAISE SQLSTATE '22012';
```
在这种语法中，USING能被用来提供一个自定义的错误消息、细节或提示。另一种做前面的例子的方法是
```sql
RAISE unique_violation USING MESSAGE = 'Duplicate user ID: ' || user_id;
```
仍有另一种变体是写RAISE USING或者RAISE level USING并且把所有其他东西都放在USING列表中。

RAISE的最后一种变体根本没有参数。这种形式只能被用在一个BEGIN块的EXCEPTION子句中，它导致当前正在被处理的错误被重新抛出。
如果在一个RAISE EXCEPTION命令中没有指定条件名以及 SQLSTATE，默认是使用RAISE_EXCEPTION (P0001)。如果没有指定消息文本，默认是使用条件名或 SQLSTATE 作为消息文本。 

## 检查断言

ASSERT语句是一种向 PL/pgSQL函数中插入调试检查的方便方法。
```sql
ASSERT condition [ , message ];
```
condition 是一个布尔 表达式，它被期望总是计算为真。如果确实如此，ASSERT语句不会再做什么。但如果结果是假 或者空，那么将发生一个`ASSERT_FAILURE`异常（如果在计算 condition 时发生错误， 它会被报告为一个普通错误）。

如果提供了可选的message，它是一个结果（如果非空）被用来替换默认错误消息文本 "assertion failed"的表达式（如果 condition失败）。message 表达式在 断言成功的普通情况下不会被计算。

通过配置参数 `plpgsql.check_asserts` 可以启用或者禁用断言测试，这个参数接受布尔值且默认为 `on`。如果这个参数为 `off`，则ASSERT语句什么也不做。

注意 `ASSERT` 是为了检测程序的 bug，而不是 报告普通的错误情况。如果要报告普通错误，请使用前面介绍的 `RAISE` 语句。 