# Apophenia

https://github.com/b-k/apophenia

## 简介 overview

Apophenia fills a space between traditional C libraries and stats packages.

* apop_data
数据集，有很多函数可以操作该数据集，使数据集的使用变得更简单。

* apop_model

模型，它封装了对数据集将要采取的操作类型，如预估模型参数（estimating model）参数 或 预测值（predicting values）。

能方便的把数据库中的数据导入到数据集。

代码规则：
- 宏定义
第一个字母大写

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
- 文本数据网格a grid of text elements,
- 权重的向量值 a vector of weights,
- 名称: row names, a vector name, matrix column names, text names,
- 下一页数据链接 more
- 错误标记 an error marker

* 现实世界中数据集（加权OLS回归 a weighted OLS regression）

<table frame=box>
<tr>
<td>Rowname</td><td>Vector</td><td> Matrix</td><td> Text</td><td>Weights</td>
</tr><tr valign=bottom>
<td align=center>
<table frame=box>
<tr><td> </td></tr>
<tr>
<td>"Steven"</td>
</tr><tr>
<td>"Sandra"</td>
</tr><tr>
<td>"Joe"</td><td>
</tr> 
</table>
</td><td align=center>
<table frame=box>
<tr>
<th>Outcome</th>
</tr> <tr>
<td align=center>1</td>
</tr><tr>
<td align=center>0</td>
</tr><tr>
<td align=center>1</td>
</tr> 
</table>
</td><td align=center>
<table frame=box>
<tr>
<th> Age</th><th> Weight (kg)</th><th> Height (cm)</th>
</tr> <tr>
<td> 32</td><td> 65</td><td> 175</td>
</tr><tr>
<td> 41</td><td> 61</td><td> 165</td>
</tr><tr>
<td> 40</td><td> 73</td><td> 181</td>
</tr> 
</table>
</td><td align=center>
<table frame=box>
<tr>
<th> Sex</th><th> State</th>
</tr>
<tr>
<td> Male</td><td> Alaska</td><td>
</tr><tr>
<td> Female</td><td> Alabama</td>
</tr><tr>
<td> Male</td><td> Alabama</td>
</tr> 
</table>
</td><td align=center>
<table frame=box>
<tr><td> </td></tr>
<tr>
<td>1</td>
</tr><tr>
<td>3.2</td>
</tr><tr>
<td>2.4</td>
</tr> 
</table>
</td></tr>
</table>

数据集中向量vector在-1列：
`apop_data_get(sample_set, .row=0, .col=-1) == 1`

### 子集 Subsets

数据：
```text
Y, X_1, X_2, X_3
2,3,4,5
1,2,9,3
4,7,9,0
2,4,8,16
1,4,2,9
9,8,7,6
```

```c
#ifdef Datadir
#define DATADIR Datadir
#else
#define DATADIR "."
#endif
#include <apop.h>
int main(){
    apop_table_exists( DATADIR "/" "data" , 'd');
    apop_data *d = apop_text_to_data( DATADIR "/" "data" );
    //tally row zero of the data set's matrix by viewing it as a vector:
    gsl_vector *one_row = Apop_rv(d, 0);
    double sigma = apop_vector_sum(one_row);
    printf("Sum of row zero: %g\n", sigma);
    assert(sigma==14);
    //view column zero as a vector; take its mean
    double mu = apop_vector_mean(Apop_cv(d, 0));
    printf("Mean of col zero: %g\n", mu);
    assert(fabs(mu - 19./6)<1e-5);
    //get a sub-data set (with names) of two rows beginning at row 3; print to screen
    apop_data *six_elmts = Apop_rs(d, 3, 2);
    apop_data_print(six_elmts);
}
```

### 基本操作 Basic manipulations

- apop_data_listwise_delete
Quick-and-dirty removal of observations with NaNs

- apop_data_split / apop_data_stack
- apop_data_sort
Sort all elements by a single column.

### Apply and map

如果要对数据集中每个元素进行操作，使用该方法(多核使用OpenMP's，调用 `omp_set_num_threads`)。

### text grid

数据集中的 `your_data->textsize[0]` 表示文本网格的行数， `your_data->textsize[1]` 表示列数，读取元素 `your_data->text[i][j]`。

- apop_text_alloc 调整 `apop_data` 中 text grid 大小。
- apop_text_set

### Errors

当函数调用出现错误时，会设置 `your_data->error`，您可以通过该属性判断停止程序或采取纠正措施：
```c
apop_data *the_data = apop_query_to_data("select * from d");
Apop_stopif(!the_data || the_data->error, exit(1), 0, "Trouble querying the data");
```
