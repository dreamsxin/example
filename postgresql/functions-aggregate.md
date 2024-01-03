# 聚合函数

聚集函数从一个输入值的集合计算出一个单一值。

https://www.postgresql.org/docs/9.1/functions-aggregate.html

http://www.postgres.cn/docs/12/functions-aggregate.html

## 通用聚集函数

函数	参数类型	返回类型	部分模式	描述
- array_agg(expression)	任何非数组类型	参数类型的数组	No	输入值（包括空）被连接到一个数组
- array_agg(expression)	任意数组类型	和参数数据类型相同	No	输入数组被串接到一个更高维度的数组中 （输入必须都具有相同的维度并且不能为空或者 NULL）
- avg(expression)	smallint, int, bigint、real、double precision、numeric或interval	对于任何整数类型参数是numeric，对于一个浮点参数是double precision，否则和参数数据类型相同	Yes	所有非空输入值的平均值（算术平均）
- bit_and(expression)	smallint、int、bigint或bit	与参数数据类型相同	Yes	所有非空输入值的按位与，如果没有非空值则结果是空值
- bit_or(expression)	smallint, int, bigint, or bit	与参数数据类型相同	Yes	所有非空输入值的按位或，如果没有非空值则结果是空值
- bool_and(expression)	bool	bool	Yes	如果所有输入值为真则结果为真，否则为假
- bool_or(expression)	bool	bool	Yes	至少一个输入值为真时结果为真，否则为假
- count(*)	 	bigint	Yes	输入的行数
- count(expression)	any	bigint	Yes	expression值非空的输入行的数目
- every(expression)	bool	bool	Yes	等价于bool_and
- json_agg(expression)	any	json	No	将值，包含空值，聚集成一个 JSON 数组
- jsonb_agg(expression)	any	jsonb	No	把值，包含空值，聚合成一个 JSON 数组
- json_object_agg(name, value)	(any, any)	json	No	将名字/值对聚集成一个 JSON 对象，值可以为空，但不能是名字。
- jsonb_object_agg(name, value)	(any, any)	jsonb	No	把名字/值对聚合成一个 JSON 对象，值可以为空，但不能是名字。
- max(expression)	任意数组、数字、串、日期/时间、网络或者枚举类型，或者这些类型的数组	与参数数据类型相同	Yes	所有非空输入值中expression的最大值
- min(expression)	任意数组、数字、串、日期/时间、网络或者枚举类型，或者这些类型的数组	与参数数据类型相同	Yes	所有非空输入值中expression的最小值
- string_agg(expression, delimiter)	(text, text) 或 (bytea, bytea)	与参数数据类型相同	No	非空输入值连接成一个串，用定界符分隔
- sum(expression)	smallint、int、 bigint、real、double precision、numeric、 interval或money	对smallint或int参数是bigint，对bigint参数是numeric，否则和参数数据类型相同	Yes	所有非空输入值的expression的和
- xmlagg(expression)	xml	xml	No	连接 非空XML 值（参见第 9.14.1.7 节）

## 用于统计的聚集函数

函数	参数类型	返回类型	部分模式	描述
- corr(Y, X)	double precision	double precision	Yes	相关系数
- covar_pop(Y, X)	double precision	double precision	Yes	总体协方差
- covar_samp(Y, X)	double precision	double precision	Yes	样本协方差
- regr_avgx(Y, X)	double precision	double precision	Yes	自变量的平均值 （sum(X)/N）
- regr_avgy(Y, X)	double precision	double precision	Yes	因变量的平均值 （sum(Y)/N）
- regr_count(Y, X)	double precision	bigint	Yes	两个表达式都不为空的输入行的数目
- regr_intercept(Y, X)	double precision	double precision	Yes	由（X, Y）对决定的最小二乘拟合的线性方程的 y截距
- regr_r2(Y, X)	double precision	double precision	Yes	相关系数的平方
- regr_slope(Y, X)	double precision	double precision	Yes	由（X, Y）对决定的最小二乘拟合的线性方程的斜率
- regr_sxx(Y, X)	double precision	double precision	Yes	sum(X^2) - sum(X)^2/N（自变量的“平方和”）
- regr_sxy(Y, X)	double precision	double precision	Yes	sum(X*Y) - sum(X) * sum(Y)/N（自变量乘以因变量的“积之合”）
- regr_syy(Y, X)	double precision	double precision	Yes	sum(Y^2) - sum(Y)^2/N（因变量的“平方和”）
- stddev(expression)	smallint、int、 bigint、real、double precision或numeric	浮点参数为double precision，否则为numeric	Yes	stddev_samp的历史别名
- stddev_pop(expression)	smallint、int、 bigint、real、double precision或numeric	浮点参数为double precision，否则为numeric	Yes	输入值的总体标准偏差
- stddev_samp(expression)	smallint、int、 bigint、real、double precision或numeric	浮点参数为double precision，否则为numeric	Yes	输入值的样本标准偏差
- variance(expression)	smallint、int、 bigint、real、double precision或numeric	浮点参数为double precision，否则为numeric	Yes	var_samp的历史别名
- var_pop(expression)	smallint、int、 bigint、real、double precision或numeric	浮点参数为double precision，否则为numeric	Yes	输入值的总体方差（总体标准偏差的平方）
- var_samp(expression)	smallint、int、 bigint、real、double precision或numeric	浮点参数为double precision，否则为numeric	Yes	输入值的样本方差（样本标准偏差的平方）
