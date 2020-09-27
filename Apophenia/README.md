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
