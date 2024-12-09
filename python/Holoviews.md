# Holoviews

一个超级简洁的 python 可视化工具，自动根据数据使用 Numpy、Pandas、bokeh、matplotlib、datashader 等库生成可视化图形。

> Stop plotting your data - annotate your data and let it visualize itself.

- https://github.com/holoviz/holoviews
- https://holoviews.org/reference/elements/matplotlib/Spikes.html

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
npfiledata = np.load('assets/hourly_taxi_data.npz')
# 转为 csv
npp = pd.DataFrame(data=npfiledata['0'])
npp.to_csv("data.csv")
# 根据键值对视图，创建字典
taxi_dropoffs = {hour:arr for hour, arr in np.load('../assets/hourly_taxi_data.npz').items()}
print('Hours: {hours}'.format(hours=', '.join(taxi_dropoffs.keys())))
print('Taxi data contains {num} arrays (one per hour).\nDescription of the first array:\n'.format(num=len(taxi_dropoffs)))
np.info(taxi_dropoffs['0'])
```

**地点覆盖图**
```python
# 限制数值范围 (x0,y0,x1,y1)
bounds = (-74.05, 40.70, -73.90, 40.80)
# 画出 0 点时的地点覆盖
image = hv.Image(taxi_dropoffs['0'], ['lon','lat'], bounds=bounds)
image
# 打印数据
image.data
```
**增加点图**
```python
points = hv.Points(station_info, ['lon','lat']).opts(color="red")
image * points
```

**点图选择**
```python
hotspot = points.select(lon=(-73.99, -73.96), lat=(40.75,40.765))
image * hotspot
# x 轴标签旋转90度，设置点的形状和尺寸
composition = image * hotspot
composition.opts(
    opts.Image(xrotation=90),
    opts.Points(color='red', marker='v', size=6))
```

**合成覆盖图+覆盖图点图**
```python
points = hv.Points(station_info, ['lon','lat']).opts(color="red")
image + image * points
```

**将每个小时的数据都生成覆盖图**
```python
dictionary = {int(hour):hv.Image(arr, ['lon','lat'], bounds=bounds) 
              for hour, arr in taxi_dropoffs.items()}
holomap = hv.HoloMap(dictionary, kdims='Hour')
holomap
```

**选择要显示的 Image 对象合成 Layout**
```python
holomap.select(Hour={3,6,9}).layout()
```

**组合**
```python
hotspot = points.select(lon=(-73.99, -73.96), lat=(40.75,40.765))
composition = holomap * hotspot

composition.opts(
    opts.Image(xrotation=90),
    opts.Points(color='red', marker='v', size=6))
```

**保存**
```python
hv.save(composition, 'holomap.html')
```

## 例子：神经元放电数据（神经元在受到刺激时产生的电信号脉冲）

```python
import pandas as pd
import holoviews as hv
from holoviews import opts

spike_train = pd.read_csv('../assets/spike_train.csv.gz')
spike_train.head(n=3)
```

**曲线和**
```python
curve  = hv.Curve( spike_train, 'milliseconds', 'Hertz', label='Firing Rate')
spikes = hv.Spikes(spike_train, 'milliseconds', [],      label='Spike Train')

layout = curve + spikes
layout
```

**自定义外观**
```python
layout.opts(
    opts.Curve( height=200, width=900, xaxis=None, line_width=1.50, color='red', tools=['hover']),
    opts.Spikes(height=150, width=900, yaxis=None, line_width=0.25, color='grey')).cols(1)
```
