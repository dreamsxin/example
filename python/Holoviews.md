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

### 例子：地铁站


```python
import pandas as pd
import numpy as np
import holoviews as hv
from holoviews import opts
hv.extension('bokeh')

station_info = pd.read_csv('../assets/station_info.csv')
station_info.head()
```
