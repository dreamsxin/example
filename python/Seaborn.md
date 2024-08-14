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

- relplot(关系绘图)：figure级函数
  - scatterplot：axes级函数（散点图）
  - lineplot：axes级函数（折线图）
- displot（分布统计绘图）：figure级函数
  - histplot：axes级函数（直方图）
  - kdeplot：axes级函数（核密度图）
  - ecdfplot：axes级函数（累积分布图）
  - rugplot：axes级函数（地毯图）
  - distplot：官方已废弃的过时api
- catplot（类别统计绘图）：figure级函数
  - striplot：axes级函数（分类散点条图）
  - swarmplot：axes级函数（分类散点簇图）
  - boxplot：axes级函数（箱形图）
  - violinplot：axes级函数（提琴图）
  - barplot：axes级函数（柱形图）
  - pointplot：axes级函数（分类统计点图）
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
**其他参数**
- legend参数：图例显示方式。False不现实图例。brief则hue和size的分组都取等间距样本作为图例。full则把分组内所有数值都显示为图例。auto则自动选择brief或full。
- height参数：每个子图的高度(单位inch)
- aspect参数：宽度=aspect×高度

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

### Distribution plot 分布图
- jointplot 双变量关系图
- pairplot 变量关系组图
- distplot 直方图，质量估计图
- kdeplot 核函数密度估计图
- rugplot 将数组中的数据点绘制为轴上的数据

### Regression plots 回归图

- lmplot 回归模型图
- regplot 线性回归图
- residplot 线性回归残差图

### Matrix plots 矩阵图
- heatmap 热力图
- clustermap 聚集图

