
https://docs.bokeh.org/en/latest/docs/first_steps/first_steps_1.html

## Creating a simple line chart

```python
from bokeh.plotting import figure, show

# prepare some data
x = [1, 2, 3, 4, 5]
y = [6, 7, 2, 4, 5]

# create a new plot with a title and axis labels
p = figure(title="Simple line example", x_axis_label='x', y_axis_label='y')

# add a line renderer with legend and line thickness to the plot
p.line(x, y, legend_label="Temp.", line_width=2)

# show the results
show(p)
```

## 指定生成文件名

```python
from bokeh.plotting import figure, output_file, show

output_file("bokeh_example.html")

p = figure(title='Bokeh散点图示例')
p.circle([1, 2, 3, 4, 5], [10, 15, 7, 12, 9], size=10)
show(p)
```

## 

```python
from bokeh.plotting import figure
from bokeh.io import output_notebook, push_notebook, show

output_notebook()

plot = figure()
plot.circle([1,2,3], [4,6,5])

handle = show(plot, notebook_handle=True)

# Update the plot title in the earlier cell
plot.title.text = "New Title"
push_notebook(handle=handle)
```

## Legend
```python
from bokeh.models import DatetimeTickFormatter
from bokeh.palettes import Bokeh, Magma, Inferno, Plasma, Viridis, Cividis
from bokeh.plotting import figure, output_notebook,  show

output_notebook()

p = figure(title="UV/PV/IP", x_axis_label='date', y_axis_label='num', x_axis_type="datetime")

# add a line renderer with legend and line thickness to the plot
p.line(df.hour, df.pv, legend_label="PV", line_width=2, color = Bokeh[6][0])
p.line(df.hour, df.uv, legend_label="UV", line_width=2, line_dash=[4, 4], color = Bokeh[6][2])
p.line(df.hour, df.ip, legend_label="IP", line_width=2, line_dash=[4, 4], color = Bokeh[6][4])

# 需要在添加图例之后设置
p.legend.location = "bottom_left"

# show the results
show(p)
```

## Formatter

```python
from bokeh.models import DatetimeTickFormatter
from bokeh.plotting import figure, output_notebook,  show

output_notebook()

p = figure(title="UV/PV/IP", x_axis_label='date', y_axis_label='num', x_axis_type="datetime")

p.xaxis.formatter.context = DatetimeTickFormatter(days="%m-%d %H:%M")
# p.xaxis.formatter.days = '%Hh'

p.line(df.hour, df.pv, legend_label="PV", line_width=2)

# show the results
show(p)
```

## Palette
https://docs.bokeh.org/en/latest/docs/reference/palettes.html
```python
from bokeh.models import DatetimeTickFormatter
from bokeh.palettes import Bokeh, Magma, Inferno, Plasma, Viridis, Cividis
from bokeh.plotting import figure, output_notebook,  show

output_notebook()

p = figure(title="UV/PV/IP", x_axis_label='date', y_axis_label='num', x_axis_type="datetime")

# add a line renderer with legend and line thickness to the plot
p.line(df.hour, df.pv, legend_label="PV", line_width=2, color = Bokeh[3][0])
p.line(df.hour, df.uv, legend_label="UV", line_width=2, line_dash=[4, 4], color = Bokeh[3][1])

# show the results
show(p)
```

## HoverTool
Bokeh中的一个工具，用于在鼠标悬停时显示数据点的信息。要使用HoverTool，首先需要导入HoverTool类，然后将其添加到图形对象的tools属性中。接下来，可以通过传递一个Tooltip对象来自定义悬停时显示的内容。
```python
from bokeh.plotting import figure, show
from bokeh.models import HoverTool, ColumnDataSource

# 创建一个数据源
data = {'x': [1, 2, 3, 4, 5],
        'y': [6, 7, 2, 4, 5],
        'label': ['A', 'B', 'C', 'D', 'E']}
source = ColumnDataSource(data)

# 创建一个图形对象
p = figure(x_axis_label='X', y_axis_label='Y')

# 绘制散点图
p.circle(x='x', y='y', size=10, source=source)

# 创建一个HoverTool，并设置显示内容
hover = HoverTool(tooltips=[('Label', '@label'), ('Value', '($x, $y)')])
p.add_tools(hover)

# 显示图形
show(p)
```

## 布局

### 工具栏位置
```python
from bokeh.plotting import figure, show

p = figure(width=400, height=400,
           title=None, toolbar_location="below")

p.scatter([1, 2, 3, 4, 5], [2, 5, 8, 2, 7], size=10)

show(p)
```

### 图例位置互动操作
https://docs.bokeh.org/en/latest/docs/user_guide/interaction/legends.html
```python
# 需要在添加图例之后设置图例位置
p.legend.location = "bottom_left"
```
```python
import pandas as pd

from bokeh.palettes import Spectral4
from bokeh.plotting import figure, show
from bokeh.sampledata.stocks import AAPL, GOOG, IBM, MSFT

p = figure(width=800, height=250, x_axis_type="datetime")
p.title.text = 'Click on legend entries to hide the corresponding lines'

for data, name, color in zip([AAPL, IBM, MSFT, GOOG], ["AAPL", "IBM", "MSFT", "GOOG"], Spectral4):
    df = pd.DataFrame(data)
    df['date'] = pd.to_datetime(df['date'])
    p.line(df['date'], df['close'], line_width=2, color=color, alpha=0.8, legend_label=name)

p.legend.location = "top_left"
# 隐藏
p.legend.click_policy="hide"

show(p)
```

```python
import pandas as pd

from bokeh.palettes import Spectral4
from bokeh.plotting import figure, show
from bokeh.sampledata.stocks import AAPL, GOOG, IBM, MSFT

p = figure(width=800, height=250, x_axis_type="datetime")
p.title.text = 'Click on legend entries to mute the corresponding lines'

for data, name, color in zip([AAPL, IBM, MSFT, GOOG], ["AAPL", "IBM", "MSFT", "GOOG"], Spectral4):
    df = pd.DataFrame(data)
    df['date'] = pd.to_datetime(df['date'])
    p.line(df['date'], df['close'], line_width=2, color=color, alpha=0.8,
           muted_color=color, muted_alpha=0.2, legend_label=name)

p.legend.location = "top_left"
# 静音/半透明效果
p.legend.click_policy="mute"

show(p)
```

## 坐标轴位置
`y_axis_location`
```python
from bokeh.layouts import gridplot
from bokeh.models import ColumnDataSource
from bokeh.plotting import figure, show
from bokeh.sampledata.penguins import data
from bokeh.transform import factor_cmap

SPECIES = sorted(data.species.unique())

TOOLS = "box_select,lasso_select,help"

source = ColumnDataSource(data)

left = figure(width=300, height=400, title=None, tools=TOOLS,
              background_fill_color="#fafafa")
left.scatter("bill_length_mm", "body_mass_g", source=source,
             color=factor_cmap('species', 'Category10_3', SPECIES))

right = figure(width=300, height=400, title=None, tools=TOOLS,
               background_fill_color="#fafafa", y_axis_location="right")
right.scatter("bill_depth_mm", "body_mass_g", source=source,
              color=factor_cmap('species', 'Category10_3', SPECIES))

show(gridplot([[left, right]]))
```

## Column layout
多个图例列式分开显示
```python
from bokeh.layouts import column
from bokeh.plotting import figure, show

x = list(range(11))
y0 = x
y1 = [10 - i for i in x]
y2 = [abs(i - 5) for i in x]

# create three plots
s1 = figure(width=250, height=250, background_fill_color="#fafafa")
s1.scatter(x, y0, size=12, color="#53777a", alpha=0.8)

s2 = figure(width=250, height=250, background_fill_color="#fafafa")
s2.scatter(x, y1, size=12, marker="triangle", color="#c02942", alpha=0.8)

s3 = figure(width=250, height=250, background_fill_color="#fafafa")
s3.scatter(x, y2, size=12, marker="square", color="#d95b43", alpha=0.8)

# put the results in a column and show
show(column(s1, s2, s3))
```
## 多图示链接操作

### 坐标范围

```python
from bokeh.layouts import gridplot
from bokeh.plotting import figure, show

x = list(range(21))
y0 = x
y1 = [20-xx for xx in x]
y2 = [abs(xx-10) for xx in x]

# create a new plot
s1 = figure(width=250, height=250, title=None)
s1.scatter(x, y0, size=10, color="navy", alpha=0.5)

# create a new plot and share both ranges
s2 = figure(width=250, height=250, x_range=s1.x_range, y_range=s1.y_range, title=None)
s2.scatter(x, y1, size=10, marker="triangle", color="firebrick", alpha=0.5)

# create a new plot and share only one range
s3 = figure(width=250, height=250, x_range=s1.x_range, title=None)
s3.scatter(x, y2, size=10, marker="square", color="olive", alpha=0.5)

p = gridplot([[s1, s2, s3]], toolbar_location=None)

show(p)
```

### 工具栏
```python
from bokeh.layouts import gridplot
from bokeh.models import ColumnDataSource
from bokeh.plotting import figure, show
from bokeh.sampledata.penguins import data
from bokeh.transform import factor_cmap

SPECIES = sorted(data.species.unique())

TOOLS = "box_select,lasso_select,help"

source = ColumnDataSource(data)

left = figure(width=300, height=400, title=None, tools=TOOLS,
              background_fill_color="#fafafa")
left.scatter("bill_length_mm", "body_mass_g", source=source,
             color=factor_cmap('species', 'Category10_3', SPECIES))

right = figure(width=300, height=400, title=None, tools=TOOLS,
               background_fill_color="#fafafa", y_axis_location="right")
right.scatter("bill_depth_mm", "body_mass_g", source=source,
              color=factor_cmap('species', 'Category10_3', SPECIES))

show(gridplot([[left, right]]))
```

## 回调
### JavaScript callback
```python
from bokeh.io import show
from bokeh.models import Button, SetValue

button = Button(label="Foo", button_type="primary")
callback = SetValue(obj=button, attr="label", value="Bar")
button.js_on_event("button_click", callback)

show(button)
```

### python 回调
```python
def my_text_input_handler(attr, old, new):
    print("Previous label: " + old)
    print("Updated label: " + new)

text_input = TextInput(value="default", title="Label:")
text_input.on_change("value", my_text_input_handler)
```

### Tooltips

```python
from bokeh.models import DatetimeTickFormatter
from bokeh.palettes import Bokeh, Magma, Inferno, Plasma, Viridis, Cividis
from bokeh.plotting import figure, output_notebook,  show

output_notebook()

p = figure(title="UV/PV/IP", x_axis_label='date', y_axis_label='num', x_axis_type="datetime")

# add a line renderer with legend and line thickness to the plot
p.line(df.hour, df.pv, legend_label="PV", line_width=2, color = Bokeh[6][0])
p.line(df.hour, df.uv, legend_label="UV", line_width=2, line_dash=[4, 4], color = Bokeh[6][2])
p.line(df.hour, df.ip, legend_label="IP", line_width=2, line_dash=[4, 4], color = Bokeh[6][4])

p.add_tools(HoverTool(tooltips=[("date", "@x{%F}")], formatters={"@x": "datetime"}))

# show the results
show(p)
```

https://docs.bokeh.org/en/latest/docs/user_guide/interaction/tools.html#ug-interaction-tools-formatting-tooltip-fields

- $index
index of selected point in the data source

- $glyph_view
a reference to the glyph view for the glyph that was hit

- $name
value of the name property of the hovered glyph renderer

- $x
x-coordinate under the cursor in data space

- $y
y-coordinate under the cursor in data space

- $sx
x-coordinate under the cursor in screen (canvas) space

- $sy
y-coordinate under the cursor in screen (canvas) space

- $snap_x
x-coordinate where the tooltip is anchored in data space

- $snap_y
y-coordinate where the tooltip is anchored in data space

- $snap_sx
x-coordinate where the tooltip is anchored in screen (canvas) space

- $snap_sy
y-coordinate where the tooltip is anchored in screen (canvas) space

- $color
colors from a data source, with the syntax: $color[options]:field_name. The available options are: hex (to display the color as a hex value), swatch (color data from data source displayed as a small color box).

- $swatch
color data from data source displayed as a small color box.

Additionally, certain glyphs may report additional data that is specific to that glyph

- $indices
indices of all the selected points in the data source

- $segment_index
segment index of a selected sub-line (multi-line glyphs only)

- $image_index
pixel index into an image array (image glyphs only)

```python
from bokeh.io import show
from bokeh.layouts import column
from bokeh.models import TextInput, Tooltip
from bokeh.models.dom import HTML

plaintext_tooltip = Tooltip(content="plain text tooltip", position="right")
html_tooltip = Tooltip(content=HTML("<b>HTML</b> tooltip"), position="right")

input_with_plaintext_tooltip = TextInput(value="default", title="Label:", description=plaintext_tooltip)
input_with_html_tooltip = TextInput(value="default", title="Label2:", description=html_tooltip)

show(column(input_with_plaintext_tooltip, input_with_html_tooltip))
```

```python
from bokeh.models import ColumnDataSource
from bokeh.plotting import figure, show

source = ColumnDataSource(data=dict(
    x=[1, 2, 3, 4, 5],
    y=[2, 5, 8, 2, 7],
    desc=['A', 'b', 'C', 'd', 'E'],
))

TOOLTIPS = [
    ("index", "$index"),
    ("(x,y)", "($x, $y)"), # $ 开头对应特殊字段，例如鼠标在数据或屏幕空间中的坐标 以@开头的字段名与ColumnDataSource中的列相关联。
    ("desc", "@desc"),
]

p = figure(width=400, height=400, tooltips=TOOLTIPS,
           title="Mouse over the dots")

p.scatter('x', 'y', size=20, source=source)

show(p)
```

## ColumnDataSource

```python
from bokeh.plotting import figure
from bokeh.models import ColumnDataSource

# create a Python dict as the basis of your ColumnDataSource
data = {'x_values': [1, 2, 3, 4, 5],
        'y_values': [6, 7, 2, 3, 6]}

# create a ColumnDataSource by passing the dict
source = ColumnDataSource(data=data)

# create a plot using the ColumnDataSource's two columns
p = figure()
p.circle(x='x_values', y='y_values', source=source)
```

```python
from bokeh.models import ColumnDataSource
# from bokeh.models import DatetimeTickFormatter
# from bokeh.palettes import Bokeh, Magma, Inferno, Plasma, Viridis, Cividis
# from bokeh.plotting import figure, output_notebook,  show

source = ColumnDataSource(data=df)

output_notebook()

p = figure(title="UV/PV/IP", x_axis_label='date', y_axis_label='num', x_axis_type="datetime")

# add a line renderer with legend and line thickness to the plot
p.line(x='hour', y="pv", source=source, legend_label="PV", line_width=2, color = Bokeh[6][0])
p.line(x='hour', y="uv", source=source, legend_label="UV", line_width=2, line_dash=[4, 4], color = Bokeh[6][2])
p.line(x='hour', y="ip", source=source, legend_label="IP", line_width=2, line_dash=[4, 4], color = Bokeh[6][4])

p.add_tools(HoverTool(tooltips=[("date", "@hour{%F}")], formatters={"@hour": "datetime"}))

# show the results
show(p)
```

## 主题

内置5种主题：caliber, dark_minimal, light_minimal, night_sky, and contrast.
```python
from bokeh.io import curdoc
from bokeh.plotting import figure, output_file, show

x = [1, 2, 3, 4, 5]
y = [6, 7, 6, 4, 5]

output_file("dark_minimal.html")

curdoc().theme = 'dark_minimal'

p = figure(title='dark_minimal', width=300, height=300)
p.line(x, y)

show(p)
```

### 自定义主题
```python
from bokeh.themes import Theme
curdoc().theme = Theme(filename="./theme.yml")
```
`theme.yml`
```yml
attrs:
    figure:
        background_fill_color: '#2F2F2F'
        border_fill_color: '#2F2F2F'
        outline_line_color: '#444444'
    Axis:
        axis_line_color: !!null
    Grid:
        grid_line_dash: [6, 4]
        grid_line_alpha: .3
    Title:
        text_color: "white"
```

## 散点图

https://docs.bokeh.org/en/latest/docs/reference/plotting/figure.html=

默认 `marker = 'circle'`

### 圆形

```python
from bokeh.plotting import figure, show

p = figure(width=400, height=400)

# add a scatter circle renderer with a size, color, and alpha
p.scatter([1, 2, 3, 4, 5], [6, 7, 2, 4, 5], size=20, color="navy", alpha=0.5)

# show the results
show(p)
```
效果等同
```python
from bokeh.plotting import figure, show

plot = figure(width=400, height=400)
plot.circle(x=[1, 2, 3, 4, 5], y=[6, 7, 2, 4, 5], size=20, color="navy", alpha=0.5) # radius=0.2 占用数据空间的百分比

show(plot)
```

### 正方形

```python
from bokeh.plotting import figure, show

p = figure(width=400, height=400)

# add a square scatter renderer with a size, color, and alpha
p.scatter([1, 2, 3, 4, 5], [6, 7, 2, 4, 5], marker="square", size=20, color="olive", alpha=0.5)

# show the results
show(p)
```

### 其他类型
https://github.com/bokeh/bokeh/blob/branch-3.6/examples/basic/scatters/markertypes.py
```python
from bokeh.core.enums import MarkerType
from bokeh.plotting import figure, show

bases = [
    "asterisk",
    "circle",
    "cross",
    "dash",
    "diamond",
    "dot",
    "hex",
    "inverted_triangle",
    "plus",
    "square",
    "star",
    "triangle",
    "x",
    "y",
]

kinds = [
    "",
    "cross",
    "dot",
    "pin",
    "x",
    "y",
]

data = []

for base in bases:
    for kind in kinds:
        name = f"{base}_{kind}" if kind else base
        if name in MarkerType:
            data.append((name, base, kind))

marker, base, kind = zip(*data)

p = figure(
    x_range=kinds,
    y_range=list(reversed(bases)),
    toolbar_location=None,
    x_axis_location="above",
)
p.grid.grid_line_color = None
p.axis.major_tick_line_color = None

p.scatter(
    marker=marker,
    x=kind,
    y=base,
    size=25,
    fill_alpha=0.4,
    fill_color="orange",
)

show(p)
```

## 交互

https://github.com/bokeh/bokeh/blob/branch-3.6/examples/output/jupyter/push_notebook/Jupyter%20Interactors.ipynb

```python
from ipywidgets import interact
import numpy as np

from bokeh.io import push_notebook, show, output_notebook
from bokeh.plotting import figure

output_notebook()

x = np.linspace(0, 2*np.pi, 2000)
y = np.sin(x)

p = figure(title="simple line example", height=300, width=600, y_range=(-5,5),
           background_fill_color='#efefef')
r = p.line(x, y, color="#8888cc", line_width=1.5, alpha=0.8)

def update(f, w=1, A=1, phi=0):
    if   f == "sin": func = np.sin
    elif f == "cos": func = np.cos
    r.data_source.data['y'] = A * func(w * x + phi)
    push_notebook()

show(p, notebook_handle=True)

interact(update, f=["sin", "cos"], w=(0,50), A=(1,10), phi=(0, 20, 0.1))
```
## 定时更新

```python
# 实时更新图表
def update_data():
    new_data = {'x': [new_x_value], 'y': [new_y_value]}
    source.stream(new_data)

# 设置定时器，每秒更新一次数据
curdoc().add_periodic_callback(update_data, 1000)
```

```python

from bokeh.io import curdoc
from bokeh.plotting import figure
from bokeh.models import ColumnDataSource
import random
from datetime import datetime
from functools import partial
from tornado import gen
from tornado.ioloop import PeriodicCallback
 
# 准备数据源
source = ColumnDataSource(data=dict(x=[], y=[]))
 
# 创建图表
plot = figure(plot_height=300, plot_width=800, title="实时数据可视化",
              x_axis_type="datetime", y_range=[0, 100])
 
# 添加折线图
line = plot.line(x='x', y='y', source=source, line_width=2)
 
# 更新数据
@gen.coroutine
def update():
    new_data = dict(
        x=[datetime.now()],
        y=[random.randint(0, 100)]
    )
    source.stream(new_data)
 
# 定时更新数据
callback = PeriodicCallback(partial(update), 1000)
callback.start()
 
# 显示图表
curdoc().add_root(plot)
```

## 数学符号

建议在 Jupyter Lab 中使用，Jupyter notebook 中 title 无法正常显示数学符号。

https://docs.bokeh.org/en/latest/docs/user_guide/styling/mathtext.html

```python
from numpy import arange, pi, sin

from bokeh.models.annotations.labels import Label
from bokeh.plotting import figure, show

x = arange(-2*pi, 2*pi, 0.1)
y = sin(x)

p = figure(height=250, title=r"$$\sin(x)$$ for \[x\] between \(-2\pi\) and $$2\pi$$")
p.scatter(x, y, alpha=0.6, size=7)

label = Label(
    text=r"$$y = \sin(x)$$",
    x=150, y=130,
    x_units="screen", y_units="screen",
)
p.add_layout(label)

p.yaxis.axis_label = r"\(\sin(x)\)"
p.xaxis.axis_label = r"\[x\pi\]"

show(p)
```
