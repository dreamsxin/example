# Holoviews

一个超级简洁的 python 可视化工具，自动根据数据使用 Numpy、Pandas、bokeh、matplotlib、datashader 等库生成可视化图形。

> Stop plotting your data - annotate your data and let it visualize itself.

https://github.com/holoviz/holoviews

## 安装

```shell
conda install holoviews
# or
pip install "holoviews[recommended]"
# or
#从GitHub克隆HoloViews
git clone git://github.com/holoviz/holoviews.git
cd holoviews
pip install -e .
```

## 入门

- https://holoviews.org/getting_started/Introduction.html
- https://github.com/holoviz/holoviews/tree/main/examples/assets

### 例子：表格数据-地铁站

**散点图**
```python
import pandas as pd
import numpy as np
import holoviews as hv
from holoviews import opts
hv.extension('bokeh')

station_info = pd.read_csv('../assets/station_info.csv')
station_info.head()

scatter = hv.Scatter(station_info, 'services', 'ridership')
print(scatter)
scatter
```

**合成layout：增加直方图**
```python
layout = scatter + hv.Histogram(np.histogram(station_info['opened'], bins=24), kdims=['opened'])
print(layout)
layout
```

### 例子：数组数据-出租车下车点

```python
taxi_dropoffs = {hour:arr for hour, arr in np.load('../assets/hourly_taxi_data.npz').items()}
print('Hours: {hours}'.format(hours=', '.join(taxi_dropoffs.keys())))
print('Taxi data contains {num} arrays (one per hour).\nDescription of the first array:\n'.format(num=len(taxi_dropoffs)))
np.info(taxi_dropoffs['0'])
```

**地点覆盖图**
```python
# 限制数值范围 (x0,y0,x1,y1)
bounds = (-74.05, 40.70, -73.90, 40.80)
image = hv.Image(taxi_dropoffs['0'], ['lon','lat'], bounds=bounds)
image
```
**增加点图**
```python
points = hv.Points(station_info, ['lon','lat']).opts(color="red")
image * points
```

**合成覆盖图+覆盖图点图**
```python
points = hv.Points(station_info, ['lon','lat']).opts(color="red")
image + image * points
```
