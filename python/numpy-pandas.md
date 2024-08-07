# numpy

数据分析工具，矩阵

```shell
sudo apt-get install python-numpy python-pandas
```

## ndarray

NumPy 提供了对多维数组的支持，与Python原生支持的List类型不同，数组的所有元素必须同样的类型。数组的维度被称为 axes，维数称为 rank。 

Numpy的数组类型为 ndarray， ndarray 的重要属性包括: 

- ndarray.ndim：数组的维数，也称为rank
- ndarray.shape：数组各维的大小tuple 类型，对一个n 行m 列的矩阵来说， shape 为 (n,m)。
- ndarray.size：元素的总数。 
- ndarray.dtype：每个元素的类型，可以是 numpy.int32, numpy.int16, and numpy.float64 等。 
- ndarray.itemsize：每个元素占用的字节数。
- ndarray.data：指向数据内存。

- zeros 函數

可以創建指定長度或形狀的全0的 ndarray 數組

- ones 函數

可以創建指定長度或形狀的全1的 ndarray 數組

- empty 函數

這個函數可以創建一個沒有任何具體值的 ndarray 數組

- 類型轉換

通過 ndarray 的 astype 方法進行強制類型轉換，浮點數轉換為整數時小數部分會被捨棄


# pandas

类似字典，能对行和列重命名

```python
from sqlalchemy import create_engine
import pandas as pd

engine = create_engine("postgresql://?sslmode=disable")
# Connect to an existing database
# with engine.connect() as conn:
df = pd.read_sql('select * from business_users limit 10000', engine)
df
```
```python
from sqlalchemy import create_engine
import pandas as pd

engine = create_engine("postgresql://zzx:S^QYpQCZjJ2pY0IH@pgm-bp13402de6174wnwno.pg.rds.aliyuncs.com:5432/yl_stats?sslmode=disable")
# Connect to an existing database
with engine.connect() as conn:
    df = pd.read_sql('select * from business_users limit 10000', conn)
df
# melt 列转行
melt = df.melt(id_vars='app_id',var_name='sendtype', value_vars=['sendemail','sendnotice'])
pivottable = pd.pivot_table(melt, index='app_id', columns='sendtype',values=['value'], aggfunc=sum)
```

### pivot_table
使用语法：
```python
pd.pivot_table(data,
                     values=None,
                     index=None,
                     columns=None,
                     aggfunc='mean',
                     fill_value=None,
                     margins=False,
                     dropna=True,
                     margins_name='All',
                     observed=False,
                     sort=True)
```
参数解释：

- data -- DataFrame格式数据
- values -- 需要汇总计算的列
- index -- 行分组键
- columns -- 列分组键
- aggfunc -- 聚合函数，或函数列表，默认为平均值
- fill_value -- 缺失值填充
- margins -- 是否添加行列的总计
- dropna -- 如果列的值都为NaN则不计算
- margins_name -- 汇总行列名称
- observed -- 是否显示观测值

```python
pd.pivot_table(df, index='app_id',values='sendemail', aggfunc=sum)
pd.pivot_table(df, index=['app_id', 'business_id'],values='sendemail', aggfunc=sum)
```
**列columns**

```python
pd.pivot_table(df, index='app_id',values='sendemail', columns='business_id', aggfunc=sum)
```

**多维度**
```python
import numpy as np

pd.pivot_table(df, index=['app_id', 'business_id'],values=['sendemail', 'sendsms'], aggfunc=[np.sum, np.mean])
```

**画图**
```python
import matplotlib.pyplot as plt
import seaborn as sns # 简化操作

# 绘制条形图
pivottable = pd.pivot_table(df, index=['app_id', 'business_id'],values=['sendemail', 'sendsms'], aggfunc=[np.sum, np.mean])
pivottable.plot(kind='bar', stacked=True)
plt.show()

# 或者使用seaborn的barplot
sns.barplot(data=pivottable)
bar = sns.barplot(data=pivottable, x=pivottable.index, y="sendemail")
# 显示 值分类
sns.barplot(data=pivottable, x=pivottable.index, y="sendemail", hue="sendemail")
```
