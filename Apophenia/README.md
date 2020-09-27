# Apophenia

https://github.com/b-k/apophenia

## 简介 overview

Apophenia fills a space between traditional C libraries and stats packages.

* apop_data
数据集，有很多函数可以操作该数据集，使数据集的使用变得更简单。

* apop_model

模型，它封装了对数据集将要采取的操作类型，如预估模型参数（estimating model）参数 或 预测值（predicting values）。

能方便的把数据库中的数据导入到数据集。

## 实例

* apop_text_to_db
将原始数据读入数据库。

* apop_query_data
将数据从数据库中拉入内存转为数据集 apop_data

* apop_query_to_text
* apop_query_to_mixed_data

* apop_estimate
统计估计。
`apop_model * apop_estimate(apop_data *d,apop_model *m)`
根据预设模型的参数自动填充数据集生成新的模型。

* apop_predict
统计预测，根据统计估计的模型，自动填充。
A prediction supplies E(a missing value | original data, already-estimated parameters, and other supplied data elements ).

* apop_model_print
输出模型

`census.c`
```C
#include <apop.h>
int main(){
    apop_text_to_db(.text_file="ss08pdc.csv", .tabname="dc");
    apop_data *data = apop_query_to_data("select log(pincp+10) as log_income, agep, sex from dc where agep+ pincp+sex is not null and pincp>=0");
    apop_model *est = apop_estimate(data, apop_ols);// apop_estimate(data, apop_probit);
    apop_model_print(est);
}
```

* 编译

```shell
gcc census.c -std=gnu99 -lapophenia -lgsl -lgslcblas -lsqlite3 -o census
```

## 函数接受命名的可选参数。

`apop_data` 有行号和列号，通过列或行名称（或索引）的任意组合来引用单元格，读取具有列名的数据集：
```c
x = apop_data_get(data, 2, 3); //observation 2, column 3
x = apop_data_get(data, .row=2, .colname="sex"); // same
apop_data_set(data, 2, 3, 1);
apop_data_set(data, .colname="sex", .row=2, .val=1);
```

默认值可以让 `apop_data_get`, `apop_data_set`, `apop_data_ptr` 等函数正确的处理矩阵 `matrices`, 向量`vectors`, 标量`scalars` :
```c
//Let v be a hundred-element vector:
apop_data *v = apop_data_alloc(100);
[fill with data here]
double x1 = apop_data_get(v, 10);
apop_data_set(v, 2, .val=x1); // 默认列号0

//A 100x1 matrix behaves like a vector
apop_data *m = apop_data_alloc(100, 1);
[fill with data here]
double m1 = apop_data_get(v, 1);

//let s be a scalar stored in a 1x1 apop_data set:
apop_data *v = apop_data_alloc(1);
double *scalar = apop_data_ptr(v); // 行列号默认都为0
```

* apop_data_alloc/apop_data_calloc
`apop_data * apop_data_alloc(const size_t size1, const size_t size2, const int size3)`

* apop_data_get
`double apop_data_get(const apop_data *data, size_t row, int col, const char *rowname, const char *colname, const char *page)`

## apop_data

实际数据处理中许多都是关于文本、数字或缺失值等常见问题。
数据集结构包含了7个部分：
- 向量 a vector,
- 矩阵 a matrix,
- 文本元素网格a grid of text elements,
- 权重的向量值 a vector of weights,
- 名称: row names, a vector name, matrix column names, text names,
- 下一页数据链接
- 错误标记 an error marker

