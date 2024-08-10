
https://docs.bokeh.org/en/latest/docs/first_steps/first_steps_1.html

```python
from bokeh.plotting import figure, output_file, show
 
# 创建一个Bokeh图表
output_file("bokeh_example.html")
p = figure(title='Bokeh散点图示例')
p.circle([1, 2, 3, 4, 5], [10, 15, 7, 12, 9], size=10)
show(p)
```
