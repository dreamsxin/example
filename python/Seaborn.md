#

Seaborn is a library for making statistical graphics in Python. It builds on top of matplotlib and integrates closely with pandas data structures.

Seaborn 将 matplotlib 的参数划分为两个独立的组合。第一组是设置绘图的外观风格的，第二组主要将绘图的各种元素按比例缩放的，以至可以嵌入到不同的背景环境中。

操控这些参数的接口主要有两对方法：

- 控制风格：axes_style(), set_style()
  会返回一组字典参数
- 缩放绘图：plotting_context(), set_context()
  会设置matplotlib的默认参数。

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

### 按列分组显示图表

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
    x="total_bill", y="tip", col="time",
    hue="smoker", style="smoker", size="size",
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
