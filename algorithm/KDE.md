# 核密度估计 Kernel Density Estimation

概率密度估计方法用于估计一组数据集的概率密度分布，分为参数估计方法和非参数估计方法。

**参数估计方法**
假定样本集符合某一概率分布，然后根据样本集拟合该分布中的参数，例如：似然估计，混合高斯等，由于参数估计方法中需要加入主观的先验知识，往往很难拟合出与真实分布的模型；

**非参数估计法**
非参数估计并不加入任何先验知识，而是根据数据本身的特点、性质来拟合分布，这样能比参数估计方法得出更好的模型。核密度估计就是非参数估计中的一种。

核密度估计 KDE 由 Rosenblatt (1955) 和 Emanuel Parzen(1962) 提出，又名 Parzen window，设数据集包含 N 个样本，对这 N 个样本进行核函数拟合，将这 N 个概率密度函数进行叠加便得到了整个样本集的概率密度函数。

## 概率密度函数 Probability density function
一般使用高斯函数作为核函数。
> 将原始空间中的向量作为输入向量，并返回特征空间（转换后的数据空间,可能是高维）中向量的点积的函数称为核函数，可以认为核函数就是距离函数。

带宽 h 的选择：当选择极小的带宽值，每个点就是一个峰值，那么每个点就是一类，如果选择大的带宽值，那么所有的数据只有一个峰值，只有一类。

选择的高斯核，那么高斯核的方差，也就是h（也叫带宽，也叫窗口，我们这里说的邻域）应该选择多大呢？不同的带宽会导致最后的拟合结果差别很大。同时上面也提到过，理论上h->0的，但h太小，邻域中参与拟合的点就会过少。那么借助机器学习的理论，我们当然可以使用交叉验证选择最好的h。
另外，也有一个理论的推导给你选择h提供一些信息。 
在样本集给定的情况下，我们只能对样本点的概率密度进行计算，那拟合过后的概率密度应该和计算的值更加接近才好，基于这一点，我们定义一个误差函数，然后最小化该误差函数便能为h的选择提供一个大致的方向。选择最小化L2风险函数，即均平方积分误差函数(mean intergrated squared error)

如果带宽不是固定的，其变化取决于估计的位置（balloon estimator）或样本点（逐点估计pointwise estimator），由此可以产生一个非常强大的方法称为自适应或可变带宽核密度估计。

## Histogram

给定一个数据集，需要观察这些样本的分布情况，往往我们会采用直方图的方法来进行直观的展现。该直方图的特点是简单易懂，但缺点在于以下三个方面：（1）密度函数是不平滑的；（2）密度函数受子区间（即每个直方体）宽度影响很大，同样的原始数据如果取不同的子区间范围，那么展示的结果可能是完全不同的。（3）直方图最多只能展示2维数据，如果维度更多则无法有效展示。

直方图展示的分布曲线并不平滑，即在一个bin中的样本具有相等的概率密度，显然，这一点往往并不适合。解决这一问题的办法时增加bins的数量，当bins增到到样本的最大值时，就能对样本的每一点都会有一个属于自己的概率，但同时会带来其他问题，样本中没出现的值的概率为0，概率密度函数不连续，这同样存在很大的问题。如果我们将这些不连续的区间连续起来，那么这很大程度上便能符合我们的要求，其中一个思想就是对于样本中的某一点的概率密度，如果能把邻域的信息利用起来，那么最后的概率密度就会很大程度上改善不连续的问题。


直方图由 Karl Pearson 提出，用以表示样本数据的分布，帮助分析样本数据的众数、中位数等性质，横轴表示变量的取值区间，纵轴表示在该区间内数据出现的频次与区间的长度的比例。

美国人口普查局（The U.S. Census Bureau）调查了 12.4 亿人的上班通勤时间，数据如下：

| 起点 | 组距 | 频次  | 频次/组距 | 频次/组距/总数 |
| :--: | :--: | :---: | :-------: | :------------: |
|  0   |  5   | 4180  |    836    |     0.0067     |
|  5   |  5   | 13687 |   2737    |     0.0221     |
|  10  |  5   | 18618 |   3723    |      0.03      |
|  15  |  5   | 19634 |   3926    |     0.0316     |
|  20  |  5   | 17981 |   3596    |     0.029      |
|  25  |  5   | 7190  |   1438    |     0.0116     |
|  30  |  5   | 16369 |   3273    |     0.0264     |
|  35  |  5   | 3212  |    642    |     0.0052     |
|  40  |  5   | 4122  |    824    |     0.0066     |
|  45  |  15  | 9200  |    613    |     0.0049     |
|  60  |  30  | 6461  |    215    |     0.0017     |
|  90  |  60  | 3435  |    57     |     0.0005     |


使用直方图进行数据可视化如下：

![Histogram of travel time (to work), US 2000 census. Area under the curve equals the total number of cases. This diagram uses Q/width from the table.](https://github.com/dreamsxin/example/blob/master/algorithm/img/Travel_time_histogram_total_n_Stata.png?raw=true))

该直方图使用单位间隔的人数（频次/组距）表示为每个矩形的高度，因此每个矩形的面积表示该区间内的人数，矩形的总面积即为 12.4 亿。

而当直方图使用（频次/组距/总数）表示为每个矩形的高度时，数据可视化如下：

![Histogram of travel time (to work), US 2000 census. Area under the curve equals 1. This diagram uses Q/total/width from the table.](https://github.com/dreamsxin/example/blob/master/algorithm/img/Travel_time_histogram_total_1_Stata.png?raw=true))

此时，矩形的面积表示该区间所占的频率，矩形的总面积为 1，该直方图也即**频率直方图**。

频率直方图有以下特点：

1. 矩形面积为该区间的频率；
2. 矩形的高度为该区间的平均频率密度。

## 概率密度函数

**极限思维**

我们使用微分思想，将频率直方图的组距一步步减小，随着组距的减小，矩形宽度越来越小，因此，在极限情况下频率直方图就会变成一条曲线，而这条曲线即为概率密度曲线。

对于概率密度曲线，我们知道：随机变量的取值落在某区域内的概率值为概率密度函数在这个区域的积分（见[百度百科](https://baike.baidu.com/item/%E6%A6%82%E7%8E%87%E5%AF%86%E5%BA%A6%E5%87%BD%E6%95%B0)），即：
$$P(a< x \leq b) = \int\limits_a^b f(x)dx$$。

设累积分布函数为 $$F(x)$$，根据上述定义，则

$$F(x) = \int\limits_{-\infty}^x f(x)dx$$。

根据微分思想，则有：

$$
\begin{align}
f(x_0)
&= \dot{F(x_0)}\\
&= \lim^{}_{h \to 0}\frac{F(x_0+h)-F(x_0 - h)}{2h}
\end{align}
$$

## 核密度估计

根据上述分析，我们应该已经明白核密度估计的目的事实上就是估测所给样本数据的概率密度函数。

### 公式推导

考虑一维数据，有如下 $n$ 个样本数据：
$x_1,x_2,x_3,...,x_n$

假设该样本数据的累积分布函数为 $F(x)$ ，概率密度函数为 $f(x)$ ，则有：

$$F(x_{i-1} < x < x_i) = \int\limits_{x_{i-1}}^{x_i} f(x)dx$$

$$f(x_i) = \lim^{}_{h \to 0}\frac{F(x_i+h)-F(x_i-h)}{2h}$$

引入累积分布函数的[经验分布函数](https://zh.wikipedia.org/wiki/%E7%BB%8F%E9%AA%8C%E5%88%86%E5%B8%83%E5%87%BD%E6%95%B0)：

$$F_n(t) = \frac{1}{n}\sum_{i=1}^n1_{x_i \leq t}$$

该函数大概意思为：使用 $n$ 次观测中 $x_i \leq t$ 出现的次数与 $n$ 的比值来近似描述 $P(x \leq t)$ 。

将该函数代入 $f(x_i)$ ，有：

$$f(x_i) = \lim_{h \to 0}\frac{1}{2nh}\sum_{i=1}^n 1_{x_i-h \leq x_j \leq x_i+h}$$

根据该公式，在实际计算中，必须给定 $h$ 值， $h$ 值不能太大也不能太小，太大不满足 $h \to 0$ 的条件，太小使用的样本数据点太少，误差会很大，因此，关于 $h$ 值的选择有很多研究，该值也被称为核密度估计中的**带宽**。

确定带宽后，我们可以写出 $f(x)$ 的表达式：

$$f(x) = \frac{1}{2nh}\sum_{i=1}^n1_{x-h \leq x_i \leq x+h}$$

### 核函数

$f(x)$ 表达式可变形为：

$$
\begin{align}
f(x) &= \frac{1}{2nh}\sum_{i=1}^n 1_{x-h \leq x_i \leq {x+h}}\\
    &= \frac{1}{2nh}\sum_{i=1}^n K(\frac{|x-x_i|}{h})
\end{align}
$$

其中，令 $t = \frac{\|x-x_i\|}{h}$ ，则当 $0 \leq t \leq 1$ 时，$K(t) = 1$.

且：

$$
\begin{align}
\int f(x)dx
&=\int\frac{1}{2nh}\sum_{i=1}^nK(\frac{|x-x_i|}{h})dx\\
&= \int\frac{1}{2n}\sum_{i=1}^nK(t)dt\\
&= \int\frac{1}{2} K(t)dt
\end{align}
$$

注意，此处的 $\sum^{n}_{i=1}$ 指的是 $n$ 次实验，而不是累计，因此计算值为 $n$。

令 $K_0(t) = \frac{1}{2} K(t)$ ，根据概率密度函数的定义，我们有：

$$\int K_0(t)dt = 1$$

其中当 $0 \leq t \leq 1$ 时， $K_0(t) = \frac{1}{2}$ 。

此时 $K_0(t)$ 就称为**核函数**，常用的核函数有: $uniform,triangular, biweight, triweight, Epanechnikov, normal...$ 。

$f(x)$ 的表达式变为：

$$f(x) = \frac{1}{nh}\sum^{n}_{i=1}K_0(\frac{|x-x_i|}{h})$$

对于二维数据， $f(x)$ 为：

$$f(x, y) = \frac{1}{nh^2}\sum^{n}_{i=1}K_0(\frac{dist((x, y), (x_i, y_i))}{h})$$

## 实验：POI 点核密度分析

### 技术选型

- 栅格数据可视化：**canvas**
- KFC POI 爬取：[POIKit](https://github.com/Civitasv/AMapPoi)

### 核函数、带宽选择

使用 ArcGIS 软件说明文档提供的带宽选择方案，核函数为：

$$K_0(t) = \frac{3}{\pi}(1-t^2)^2$$

概率密度估测函数为：

$$f(x, y) = \frac{1}{n*h^2}\sum_{i=1}^n pop_i K_0(\frac{dist_i}{h})$$

其中， $pop_i$ 为给定的权重字段，若不含有该字段，则取值为 1， $n$ 为 POI 点个数。

带宽为：

$$h = 0.9*min(A,\sqrt{\frac{1}{\ln(2)}}*D_m)*n^{-0.2}$$

参数解释：

- 平均中心：指 $n$ 个 POI 点的平均中心，即经度和纬度分别取平均；
- 加权平均中心：指 $n$ 个 POI 点的加权平均中心，即经度和纬度分别乘以权重再取平均；
- 标准距离计算公式：

$$
SD = \sqrt{\frac{\sum_{i=1}^n(x_i-\bar X)^2}{n}+\frac{\sum_{i=1}^n(y_i-\bar Y)^2}{n}+\frac{\sum_{i=1}^n(z_i-\bar Z)^2}{n}}
$$

  其中：

  - $x_i,y_i,z_i$ 是 POI 的坐标
  - ${\bar X,\bar Y, \bar Z}$ 表示平均中心
  - $n$ 是 POI 总数

- 加权标准距离计算公式：

$$
\begin{align}
SD_w = \sqrt{\frac{\sum_{i=1}^n w_i(x_i-\bar X_w)^2}{\sum_{i=1}^{n}w_i}+\frac{\sum_{i=1}^nw_i(y_i-\bar Y_w)^2}{\sum_{i=1}^{n}w_i}+\frac{\sum_{i=1}^nw_i(z_i-\bar Z_w)^2}{\sum_{i=1}^{n}w_i}}
\end{align}
$$

  其中：

  - $w_i$ 是要素$i$ 的权重
  - ${\bar X_w,\bar Y_w, \bar Z_w}$ 表示加权平均中心
  - $n$ 是 POI 总数

- 若 POI 点**不含有**权重字段，则 $D_m$ 为到平均中心距离的中值， $n$ 是 POI 点数目， $A$ 是标准距离 $SD$ ；
- 若 POI 点**含有**权重字段，则 $D_m$ 为到加权平均中心距离的中值，$n$ 是 POI 点权重字段值的总和， $A$ 是加权标准距离 $SD_w$ 。
