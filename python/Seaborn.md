#

Seaborn is a library for making statistical graphics in Python. It builds on top of matplotlib and integrates closely with pandas data structures.

Seaborn 将 matplotlib 的参数划分为两个独立的组合。第一组是设置绘图的外观风格的，第二组主要将绘图的各种元素按比例缩放的，以至可以嵌入到不同的背景环境中。

操控这些参数的接口主要有两对方法：

- 控制风格：axes_style(), set_style()
  会返回一组字典参数
- 缩放绘图：plotting_context(), set_context()
  会设置matplotlib的默认参数。

Seaborn 绘图函数根据图形层级分为两种类型：

- axes级：绘图函数在单个axes上绘图，函数返回值就是axes对象。
- figure级：绘图函数在figure上绘图，返回一个 FacetGrid 对象，类似 Figure 对象，可以管理 figure。

- relplot(关系绘图)：figure级函数，适合绘制 x,y变量连续型数据
  - scatterplot：axes级函数（散点图）
  - lineplot：axes级函数（折线图）

- displot（分布统计绘图）：figure级函数，适合绘制 x 连续的分段统计
  - histplot：axes级函数（直方图）
  - kdeplot：axes级函数（核密度图）
  - ecdfplot：axes级函数（累积分布图）
  - rugplot：axes级函数（地毯图）
  - distplot：官方已废弃的过时api

- catplot（类别统计绘图）：figure级函数，适合绘制x分类，y连续
  - striplot：axes级函数（分类散点条图）
  - swarmplot：axes级函数（分类散点簇图）
  - boxplot：axes级函数（箱形图），max，min 的 distribtuion
  - violinplot：axes级函数（提琴图），max，min 的 distribtuion
  - barplot：axes级函数（柱形图），aggegation
  - pointplot：axes级函数（分类统计点图），aggegation
  - countplot：axes级函数（数量统计图）
- lmplot（回归统计绘图）：：figure级函数
  - regplot：axes级函数（回归拟合图）
  - residplot：axes级函数（回归拟合误差图）
- clustermap（矩阵绘图）：figure级函数
  - heatmap：axes级函数（热度图）
两个特殊的figure级函数(多图组合)
- jointplot：figure级函数，返回JointGrid对象。同时绘制3个子图，在绘制二维图的基础上，在图行上方和右侧绘制分布图。
- pairplot：figure级函数，返回PairGrid对象。（配对分布统计图）


## 安装

https://seaborn.pydata.org/installing.html

```shell
pip install seaborn
# 高级特性
pip install seaborn[stats]

conda install seaborn
conda install seaborn -c conda-forge
```

## 使用

col 和 row 是用来指定分组的列名或行名。通过这种方式，可以将数据集按照指定的列或行进行分组，并在每个子图中显示该分组的数据。这样可以更直观地显示数据的不同分组之间的关系和差异。

tips 包含列：total_bill、tip、sex、smoker、day、time、size
time 列包含两种值：Dinner、Lunch
```python
# Import seaborn
import seaborn as sns

# Apply the default theme
sns.set_theme()

# Load an example dataset
tips = sns.load_dataset("tips")

# lunch = tips[tips.time=='Lunch']

# Create a visualization
sns.relplot(
    data=tips,
    x="total_bill", y="tip", col="time", hue="smoker", style="smoker", size="size", # smoker 列控制点颜色（hue）和样式样式（style）
)
```

## 主题样式

https://seaborn.pydata.org/tutorial/aesthetics.html

There are five preset seaborn themes: `darkgrid`, `whitegrid`, `dark`, `white`, and `ticks`. 

可以使用 `set_theme` 设置，也可以使用 `set_style` 设置。

```python
import seaborn as sns

# sns.set_theme(style="whitegrid", palette="pastel")
sns.set_style("white")

# Load an example dataset
tips = sns.load_dataset("tips")

sns.relplot(
    data=tips,
    x="total_bill", y="tip", col="time",
    hue="smoker", style="smoker", size="size",
)
```

- darkgrid（默认）：深色网格主题。
```python
# 设置为 darkgrid 主题
sns.set_theme(style="darkgrid")
```

- whitegrid：浅色网格主题。
```python
# 设置为 whitegrid 主题
sns.set_theme(style="whitegrid")
```

- dark：深色主题，没有网格。
```python
# 设置为 dark 主题
sns.set_theme(style="dark")
```

- white：浅色主题，没有网格。
```python
# 设置为 white 主题
sns.set_theme(style="white")
```

- ticks：深色主题，带有刻度标记。
```python
# 设置为 ticks 主题
sns.set_theme(style="ticks")
```

### 模板（Context）

绘图元素比例，可以使用 `set_theme` 设置，也可以使用 `set_context` 设置。

```python
sns.set_context('notebook',font_scale = 1.5, rc = {'lines.linewidth':2.5})
```

**按大小从小到大排列分别为：paper, notebook, talk, poster。**

- paper：适用于小图，具有较小的标签和线条。
```python
# 设置为 paper 模板
sns.set_theme(context="paper")
```

- notebook（默认）：适用于笔记本电脑和类似环境，具有中等大小的标签和线条。
```python
# 设置为 notebook 模板
sns.set_theme(context="notebook")
```

- talk：适用于演讲幻灯片，具有大尺寸的标签和线条。
```python
# 设置为 talk 模板
sns.set_theme(context="talk")
```

- poster：适用于海报，具有非常大的标签和线条。
```python
# 设置为 poster 模板
sns.set_theme(context="poster")
```

## 坐标轴/边框的风格
可以利用 `offset` 参数将轴线进行偏置，另外，当刻度没有完全覆盖整个坐标轴的的范围时，利用 `trim` 参数限制已有坐标轴的范围，只显示必要的部分，‌而不是整个坐标轴的范围。‌这种功能在处理数据可视化时特别有用，‌因为它可以帮助去除图表边缘的干扰元素，‌让数据点更加突出，‌从而提高图表的可读性和美观性。
```python
# 显示
sns.despine(left=False, top=False, right=False, bottom=False)

# 隐藏
sns.despine()
```

## 误差条

https://seaborn.pydata.org/tutorial/error_bars.html

相同 X 轴上，存在多个 Y 值，用到。线条显示的是y的平均值，填充色把y的95%置信区间范围显示出来了。

```python
import seaborn as sns
sns.set_theme(style="darkgrid")

# Load an example dataset with long-form data
fmri = sns.load_dataset("fmri")

# Plot the responses for different events and regions
sns.lineplot(x="timepoint", y="signal",
             hue="region", style="event",
             data=fmri)

sns.relplot(data=fmri,x="timepoint",y="signal",kind="line")
sns.relplot(data=fmri,x="timepoint",y="signal",kind="line", errorbar=None)

plt.show
```
## 图表类型

seaborn 图表一共有5个大类。

### Relational plots 关系类图表

relplot 关系类图表的接口，通过指定kind参数可以画出下面的两种图
- scatterplot 散点图
- lineplot 折线图

relplot 函数为 figure 级函数，返回 FacetGrid 对象。FacetGrid 对象类似于 figure。scatterplot 和 lineplot 函数为 axes 级函数，返回axes对象。
**常用参数简介**
- x,y参数：x,y坐标数据。x,y为数组或者是data的key名称（data为DataFrame时，x,y指列名称）。
- data参数：data为类字典数据。当data不为空时，x,y可以用data的key引用数据。
- kind参数：scatter指绘制散点图，line指绘制折线图。
**分组聚合参数**
- hue参数：用不同颜色对数组分组。hue可以是列表或者data的key。hue的每一个数据绘制一根曲线，给一个图例标签。
- size参数：用不同点大小对数组分组。数组或者data中的key。每个大小给个图例标签。
- style参数：用不同的点样式对数据分组。数组或者data中的key。每个样式给一个图例标签。
- row,col：把row,col指定的数据按照行或列排列到不同的子图。
**分组聚合参数样式**
- palette参数：指定hue分组的每组曲线的颜色。
- markers参数：设置True使用默认点样式。False不显示点。可以用列表为style设置对应点样式(长度必须和style分组数相同)。
- sizes参数：把size映射到一个指定的区间。
折线图专用
- dashes参数：bool设置是否使用默认线样式(实线)。用列表为style分组设置对应线样式(列表长度必须和style分组数相同)
- sort参数：是否对x排序。默认为True。False则按照数组中x的顺序绘图。
- ci 参数（已改名为 errorbar 误差条）：当存在 x 值对应多个 y 值时，用置信区间绘制线条，默认显示 95% 置信区间(confidence intervals)。
  - None：表示不显示置信区间。
  - 'pi'：范围，数据两端的差值 max(s)-min(s)
  - 'sd'：标准差，表示显示标准偏差(standard deviation)而不是置信区间。
  - 'se'：标准误差
  - ci：置信区间，通常取95%
  - 自定义：errorbar：lambda x: (x.min()), x.max())等价于errorbar=('pi', 100)
- estimator参数：聚合设置，默认为平均值
  - estimator=None：不使用聚合，x对应多个y就每个x坐标绘制多个y点。
  - estimator=func：聚合函数，比如mean，sum等。
**其他参数**
- legend参数：图例显示方式，默认 True。False不显示图例。brief 则 hue 和 size 的分组都取等间距样本作为图例。full 则把分组内所有数值都显示为图例。auto 则自动选择 brief 或 full。
- height参数：每个子图的高度(单位inch)
- aspect参数：宽度=aspect×高度

> hue,style,size类似，都是用于分组，hue根据颜色分组，style根据点先样式分组，size根据点大小或线粗细分组。每个组在曲线图中就是绘制一根单独的曲线。

**tips 数据集示例散点图**
数据集为一个餐厅的小费数据。
total_bill为总账单、tip为小费金额；
sex为消费者性别，smoker为是否抽烟，size为用餐人数。
day、time分别为日期、时间。

```python
import seaborn as sns

tips = sns.load_dataset("tips")

sns.relplot(data=tips,x='total_bill',y='tip') #观察账单和小费的关系，绘制散点图
sns.relplot(data=tips,x='total_bill',y='tip',kind='line') #观察账单和小费的关系，绘制折线图

sns.relplot(data=tips,x='total_bill',y='tip',hue='day') #分别观察每天的账单和小费的关系。
sns.relplot(data=tips,x='total_bill',y='tip',hue='smoker') #分别观察抽烟和不抽烟人群的账单和小费的关系。
sns.relplot(data=tips,x='total_bill',y='tip',hue='size') #根据就餐人数，分别观察人群的账单和小费的关系。

# palette
sns.relplot(data=tips,x='total_bill',y='tip',hue='day',palette='coolwarm')  #用colormap指定颜色
sns.relplot(data=tips,x='total_bill',y='tip',hue='day',palette='ch:start=2,rot=.5') #用cubehelix参数指定颜色
sns.relplot(data=tips,x='total_bill',y='tip',hue='day',palette=('r','g','b','gray')) #用列表参数指定颜色

# style分组
sns.relplot(data=tips,x='total_bill',y='tip',style="smoker")  #抽烟的人一个样式，不抽烟的人一个样式。
sns.relplot(data=tips,x='total_bill',y='tip',hue='smoker',style="smoker")  #组合样式
sns.relplot(data=tips,x='total_bill',y='tip',hue='smoker',style="time") #组合样式

# style+marker
sns.relplot(data=tips,x='total_bill',y='tip',style="time",markers=["o","*"]) #用 markers 参数指定 style 分组点样式

# size分组
sns.relplot(data=tips,x='total_bill',y='tip',size="smoker")#size分组(根据是否抽烟，分组)
sns.relplot(data=tips,x='total_bill',y='tip',hue='smoker',size="size")#hue+size
sns.relplot(data=tips,x='total_bill',y='tip',hue='day',style="size",size="smoker")#hue+style+size

# 设置大小数值 size数据的大小不太适合显示，比如都特别小，或者差异不明显不便于观看，用 sizes 参数可以把 size 映射到一个指定的区间
sns.relplot(data=tips,x='total_bill',y='tip',hue='day',size="size",sizes=(10,200)) #把size映射到10-200区间，用于显示大小

# 多子图
sns.relplot(data=tips,x='total_bill',y='tip',col='smoker')     #不同子图绘制smoker不同的图形，绘制在不同的行 
sns.relplot(data=tips,x='total_bill',y='tip',row='smoker',col='time') #用不同的行绘制子图区分smoker，不同的列子图区分time
sns.relplot(data=tips,x='total_bill',y='tip',hue='smoker',col='time') #根据time绘制多列子图，每个子图用hue分组
sns.relplot(data=tips,x='total_bill',y='tip',hue='smoker',col='day',col_wrap=2) #每列最多2个子图，超过2个自动换行
```

**随机数据集示例折线图**
```python
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import pandas as pd

# 随机生成 500x2 （500行，2列）的数组
df=pd.DataFrame(np.random.randn(500, 2).cumsum(axis=0), columns=["x", "y"])

sns.relplot(data=df,x="x",y="y",kind="line")
sns.relplot(data=df,x="x",y="y",kind="line",sort=False)
plt.show()
```
**fmri 数据集示例折线图**
fmri 是一组事件相关功能核磁共振成像数据。
subject 为14个测试者。
timpoint为时间点。
event为事件，分为刺激stim和暗示cue。
region为区域，分为额叶和前叶。
signal为信号。

**使用 scatterplot**
scatterplot 是 axes 级函数

返回值是一个axes，可以用ax指定axes。不指定ax时，会在默认的axes中绘图。不指定ax，多次调用scatterplot绘图都会在默认（同一个）的axes中绘图。
```python
import matplotlib.pyplot as plt
import seaborn as sns

tips=sns.load_dataset('tips')

fig=plt.figure()
ax1=fig.add_subplot(121) # 参数 121 表示 将画布分割成 1 行 2 列，且坐标图位于从左到右，从上到下的第一个位置
ax2=fig.add_subplot(122) # 参数 122 表示 将画布分割成 1 行 2 列，且坐标图位于从左到右，从上到下的第二个位置
#ax1,ax2=fig.subplots(1,2)

sns.scatterplot(data=tips,x="total_bill",y="tip",hue='smoker',ax=ax1)
sns.scatterplot(data=tips,x="total_bill",y="tip",hue='smoker',size='sex',ax=ax2)
```
**使用 lineplot**
```python
import matplotlib.pyplot as plt
import seaborn as sns

fmri=sns.load_dataset('fmri')

fig=plt.figure()

(ax1,ax2),(ax3,ax4)=fig.subplots(2,2)

sns.lineplot(data=fmri,x="timepoint",y="signal",ax=ax1)
sns.lineplot(data=fmri,x="timepoint",y="signal",hue='region',ax=ax2)
sns.lineplot(data=fmri,x="timepoint",y="signal",hue='region',style='event',ax=ax3)
sns.lineplot(data=fmri,x="timepoint",y="signal",hue='region',size='event',ax=ax4)

plt.show()
```

### Categorical plots 分类图表
catplot 分类图表的接口，通过指定kind参数可以画出下面的八种图
- stripplot 分类散点图
- swarmplot 能够显示分布密度的分类散点图
- boxplot 箱图
- violinplot 小提琴图
- boxenplot 增强箱图
- pointplot 点图
- barplot 条形图
- countplot 计数图

**kind**
**分类散点**
- strip 散点条图：数据按x分组，所有x相同的y值，在x坐标上方绘制散点，散点形成条状。
  - hue参数：分组按不同颜色绘图
  - dodge参数：是否将不同的组分开绘制
  - jitter参数：设置抖动量，False表示散点条绘制在一条线上。默认为True。
- swarm 散点群图：类似散点条图，但是y值相同的点显示为树状。
**分类分布图**
- box 箱型图图：每一组数据分别统计中位数、25%位置数、75%位置数，以及高位和低位值。
  - hue参数：分组按不同颜色绘图
  - dodge参数：是否将不同的组分开绘制
  - orient参数：“v”|“h”，设置box图方向
  - whis参数：设置低位和高位点系数，高位、低位数公式：中位数±whis*(75%位数-25%位数)，whis设置为np.inf时，低位和高位值取min和max。
- boxen 增强型箱型图：绘制更多的分位数
  - hue,分组按不同颜色绘图
  - dodge,是否将不同的组分开绘制
  - orient,“v”|“h”，设置boxen图方向
  - k_depth,箱形图箱数设置默认值tukey，还可以是“proportion”, “trustworthy”, “full”，都是内置的确定方法。当为数字是表示具体箱形框数量。

- violin 小提琴图，越宽的地方表示y值密度越大，小提琴图由轮廓和内部图形组成：
  轮廓是kde核密度估计曲线。
  内部是箱型图的四分位数。
**分类估计图**
- bar：柱形图应用函数获取估计值（默认采用平均值），然后绘制成柱形图。柱形图上方的短线为置信区间
- point：点图绘制平均值曲线和置信区间，和lineplot非常像，显示的线条和短线和lineplot也完全一致。
- count：计数图对指定x或y坐标中每个值出现的次数进行统计，绘制成柱形图。柱形高度坐标值统计个数。
```python
import matplotlib.pyplot as plt
import seaborn as sns

tips = sns.load_dataset("tips")

sns.catplot(data=tips, x="day", y="total_bill", kind="strip")
sns.catplot(data=tips, x="day", y="total_bill", kind="swarm")
sns.catplot(data=tips, x="day", y="total_bill", kind="box")
sns.catplot(data=tips, x="day", y="total_bill", kind="violin")

plt.show()
```

### Distribution plot 分布图
分布就是了解数据变量的分布规律的。seaborn中“分布”绘图函数共5个：
- displot：通过kind参数指定为 hist,kde,ecdf。默认为hist。为 figure 级函数，返回 FacetGrid 对象，类似于figure。
  - histplot：直方图
  - kdeplot：核密度分布
  - ecdfplot：累积分布
  - rugplot：地毯图(用短线密度表示分布)。rugplot可以在其他图形（hist,kde,ecdf）中显示。不能使用figure级函数displot绘制。
```python
fig, ax =plt.subplots(1,5,constrained_layout=True, figsize=(15, 3))
_ = sns.histplot(penguins, x="flipper_length_mm", stat="count", ax=ax[0])      # count, 也是默认值
_ = sns.histplot(penguins, x="flipper_length_mm", stat="frequency", ax=ax[1])  # frequency 
_ = sns.histplot(penguins, x="flipper_length_mm", stat="probability", ax=ax[2])# probability
_ = sns.histplot(penguins, x="flipper_length_mm", stat="percent", ax=ax[3])    # percent
_ = sns.histplot(penguins, x="flipper_length_mm", stat="density", ax=ax[4])    # density
```
**常用参数简介**

| data | data为类字典数据或DataFrame。数组为参数时不需要指定data。  |
|--------|----------------------------------------|
| x,y  | 只指定x时，表示统计单变量x的分布。x,y都指定时表示双变量分布。      |
| kind | 绘图类型。“hist”,直方图，"kde"核密度图，"ecdf"累积分布图。 |


**分组聚合参数**

| hue     | 用不同颜色对数组分组。hue可以是列表或者data的key。hue的每一个数据绘制一组分布图，给一个图例标签。 |
|-----------|---------------------------------------------------------|
| row,col   | 把row,col指定的数据按照行或列排列到不同的子图。                             |
| palette | 指定hue分组的每组曲线的颜色。                                        |

```python
sns.displot(tips, x="total_bill")
sns.displot(tips, x="total_bill", kind="kde", fill=True, cmap="mako")

penguins = sns.load_dataset("penguins")
sns.displot(penguins, x="flipper_length_mm", kind="kde")

sns.displot(penguins, x="bill_length_mm", y="bill_depth_mm", hue="species", kind="kde")
```

**按照日期分布图**
```python
import datetime

# dflogin['date'] = np.array([d.toordinal() for d in dflogin['trace_time']])
dflogin['date'] = np.array([d.timestamp() for d in dflogin['trace_time']])
sns.displot(dflogin, x="date", kind="kde")

ticks_locations, _ = plt.xticks();
# Get the labels for those positions
#labels = [datetime.datetime.fromordinal(int(t)).strftime("%Y-%m-%d %H:%M") for t in ticks_locations]
labels = [datetime.datetime.fromtimestamp(int(t)).strftime("%Y-%m-%d %H:%M") for t in ticks_locations]

plt.xticks(ticks_locations, labels, rotation=90)
plt.title("Date distribution");
```

### Regression plots 回归图

- lmplot 回归模型图
  - regplot 线性回归图，axes级函数。绘制线性回归拟合。
  - residplot 线性回归残差图，axes级函数。绘制线性回归的误差图。（不能用lmplot绘制resid图）

| hue         | 分系列用不同的颜色绘制                                                  |
|-------------|-----------------------------------------------------------------------|
| col         | 按照某列值进行分组。                                                    |
| row         | 按照某行值进行分组。                                                    |
| c           | 置信区间的大小，取值0-100                                               |
| order       | 指定拟合多项式阶数                                                     |
| scatter     | 是否绘制散点图                                                         |
| x_jitter    | 为x变量添加随机噪点。会导致绘制的散点移动，不会改变原始数据。                               |
| y_jitter    | 为y变量添加随机噪点。会导致绘制的散点移动，不会改变原始数据。                               |
| x_estimator | 参数值为函数，如np.mean。对每个x值的所有y值用函数计算，绘制得到的点，并绘制误差线。          |
| x_bins      | 当x不是离散值时x_estimator可以配合x_bins指定计算点和误差线数量                      |
| robust      | 对异常值降低权重                                                                 |
| logistic    | logistic=True时，假设y取值只有2个比如True和False，并用statsmodels中的逻辑回归模型回归。 |

```python
import seaborn as sns

sns.set_style("white")

tips = sns.load_dataset("tips")

sns.lmplot(data=tips, x="total_bill", y="tip")
sns.residplot(data=tips, x="total_bill", y="tip")
```

### Matrix plots 矩阵图
- heatmap 热力图
| annot      | 在单元格内显示数据。       |
|------------|------------------|
| fmt        | 设置annot参数数据显示格式。 |
| cbar       | 是否显示颜色条。         |
| cmap       | 设置colormap。      |
| square     | 单元格是否方形。         |
| linewidths | 设置单元格线条宽度。       |
| linecolor  | 设置单元格线条颜色。       |

- clustermap 聚集图


```python
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

data = np.random.rand(10, 10)

sns.heatmap(data=data, annot=True, fmt=".2f")
```


## FacetGrid

relplot、catplot、lmplot等，这些函数可以通过col、row等在一个Figure中绘制多个图。这些函数之所以有这些功能，是因为他们的底层使用了FacetGrid来组装这些图形。今天我们就来学习FacetGrid的使用。

先创建一个FacetGrid对象，然后再调用这个对象的map方法。其中map方法的第一个参数是一个函数，后续map将调用这个函数来绘制图形。后面的参数就是传给这个函数的参数。示例代码如下：

```python
# 在该网格上可视化数据的主要方法是使用该FacetGrid.map()方法。
# 为其提供绘图功能以及要绘制的数据框中的变量名称。让我们使用直方图来
# 查看每个子集中tip的分布。
g = sns.FacetGrid(tips, col="time")
g.map(plt.hist, "tip");
```

| 参数                          | 描述          | 对应使用了FacetGrid函数            |
|-----------------------------|-------------|-----------------------------|
| plt.plot/sns.lineplot       | 绘制折线图       | sns.relplot(kind="line")    |
| plt.hexbin                  | 绘制六边形图形     | sns.jointplot(kind="hex")   |
| plt.hist                    | 绘制直方图       | sns.distplot                |
| plt.scatter/sns.scatterplot | 绘制散点图       | sns.relplot(kind="scatter") |
| sns.stripplot               | 绘制分类散点图     | sns.catplot(kind="strip")   |
| sns.swarmplot               | 绘制散开来的分类散点图 | sns.catplot(kind="swarm")   |
| sns.boxplot                 | 绘制箱线图       | sns.catplot(kind="box")     |
| sns.violinplot              | 绘制小提琴图      | sns.catplot(kind="violin")  |
| sns.pointplot               | 绘制点线图       | sns.catplot(kind="point")   |
| sns.barplot                 | 绘制条形图       | sns.catplot(kind="bar")     |
| sns.countplot               | 绘制数量柱状图     | sns.catplot(kind="count")   |
| sns.regplot                 | 绘制带有回归线的散点图 | sns.lmplot                  |

