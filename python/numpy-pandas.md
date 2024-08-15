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

##
```python
# 筛选特定日期的数据，例如筛选所有1月1日的数据
filtered_df = df[df['timestamp'].dt.day == 1]
print(filtered_df)

# 使用fillna()处理缺失值
df['day_filled'] = df['day'].fillna(0)  # 将缺失的day替换为0（或其他合适的值）
print(df)
```

## groupBy

```python
import pandas as pd
 
# 创建示例数据
data = {
    'Category': ['A', 'B', 'A', 'B', 'C', 'A', 'C', 'B'],
    'Value': [10, 20, 15, 25, 35, 40, 45, 50]
}
 
df = pd.DataFrame(data)
 
# 按照Category进行汇总，计算Value的总和
summary = df.groupby('Category').sum()
 
print(summary)
```

## 

```python
import pandas as pd
 
# 创建示例数据
data = {
    'Date': ['2021-01-01', '2021-01-01', '2021-01-02', '2021-01-02'],
    'Value': [10, 20, 30, 40]
}
 
# 创建DataFrame
df = pd.DataFrame(data)
 
# 将'Date'列转换为datetime类型
df['Date'] = pd.to_datetime(df['Date'])
 
# 按日期汇总值的和
summary = df.groupby('Date')['Value'].sum().reset_index()
 
print(summary)
```

```python
import pandas as pd

# 示例DataFrame
df = pd.DataFrame({
    'datetime': pd.to_datetime(['2021-01-01 01:00', '2021-01-01 02:00', '2021-01-02 01:00', '2021-01-02 02:00']),
    'value': [10, 20, 30, 40]
})
 
# 将'datetime'列转为日期和小时
df['date'] = df['datetime'].dt.date
# df['hour'] = df['datetime'].dt.hour
df['hour'] = df['datetime'].dt.strftime('%Y-%m-%d %H')
# 按天和小时进行汇总
summary = df.groupby('hour')['value'].sum()
 
print(summary)
```

## 
```python
import pandas as pd
 
# 示例DataFrame
df = pd.DataFrame({
    'datetime': pd.to_datetime(['2021-01-01 01:00', '2021-01-01 02:00', '2021-01-02 01:00', '2021-01-02 02:00']),
    'value': [10, 20, 30, 40]
})
 
# 将'datetime'列转为日期和小时
df['date'] = df['datetime'].dt.date
df['hour'] = df['datetime'].dt.hour
 
# 按天和小时进行汇总
summary = df.groupby(['date', 'hour'])['value'].sum()
 
print(summary)
```

## resample

按周累计上述数据，重新绘图
```python
weekly = data.resample("W").sum()
weekly.plot(style=[':','--','-'])
plt.ylabel("Weekly bicycle count")
```
计算30天的移动平均值
```python
daily = data.resample("D").sum()

daily.rolling(30, center=True).mean().plot(style=[':','--','-'])
plt.ylabel("mean of 30 days count")
```

## 移动平均

使用移动平均 来平滑曲线

高斯分布时间窗口：窗口宽度=50天 窗口内高斯平滑宽度=10天
```python
daily.rolling(50, center=True,
win_type="gaussian").sum(std=10).plot(style=[':','--','-'])
```
## melt
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

engine = create_engine("postgresql:///yl_stats?sslmode=disable")
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
