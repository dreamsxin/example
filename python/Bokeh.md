
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
