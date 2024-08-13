
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

## 

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
