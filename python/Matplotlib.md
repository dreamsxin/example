# Matplotlib

```shellid
sudo apt-get install python-matplotlib
```

## 交互模式

默认情况下，Matplotlib使用阻塞模式，要想打开交互模式需要通过下面的几个函数来做操作，下面直接列出要用的核心函数。
```python
plt.ion()  # 打开交互模式
plt.ioff()  # 关闭交互模式
plt.clf()  # 清除当前的Figure对象
plt.pause()  # 暂停GUI功能多少秒
```

```python
import matplotlib.pyplot as plt
import numpy as np


def io_test():
    fig = plt.figure()  # 生成画布
    plt.ion()  # 打开交互模式
    for index in range(50):
        fig.clf()  # 清空当前Figure对象
        fig.suptitle("3d io pic")

        # 生成数据
        point_count = 100  # 随机生成100个点
        x = np.random.random(point_count)
        y = np.random.random(point_count)
        z = np.random.random(point_count)
        color = np.random.random(point_count)
        scale = np.random.random(point_count) * 100
        ax = fig.add_subplot(111, projection="3d")
        # 绘图
        ax.scatter3D(x, y, z, s=scale, c=color, marker="o")
        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.set_zlabel("Z")
        # 暂停
        plt.pause(0.2)

    # 关闭交互模式
    plt.ioff()

    plt.show()


if __name__ == '__main__':
    io_test()

```

```python
import matplotlib.pyplot as plt
import matplotlib.animation as animation

fig, ax = plt.subplots()
x, y, tmp = [], [], []

for i in range(10):
    x.append(i)
    y.append(i+1)
    temp = ax.bar(x, height=y, width=0.3)
    tmp.append(temp)

ani = animation.ArtistAnimation(fig, tmp, interval=200, repeat_delay=1000)
ani.save("bar.gif", writer='pillow')

```

## 动态图

```python
import seaborn as sns
import matplotlib.pyplot as plt
import matplotlib.patheffects as fx

# 绘制图表的函数
def make_plot(year):
    
    # 根据年份来筛选出数据
    df = data[data.Year == year]
        
    # 制作图表
    fig, (ax1, ax2) = plt.subplots(1, 2, sharey = True)
    ax1.invert_xaxis()
    fig.subplots_adjust(wspace = 0) 
    
    ax1.barh(df[df.Sex == 'Male'].AgeGrp, df[df.Sex == 'Male'].percent, label = 'Male')
    ax2.barh(df[df.Sex == 'Female'].AgeGrp, df[df.Sex == 'Female'].percent, label = 'Female', color = 'C1')
    
    country = df.Location.iloc[0]
    if country == 'United States of America': country == 'US'
        
    fig.suptitle(f'......')
    fig.supxlabel('......')
    fig.legend(bbox_to_anchor = (0.9, 0.88), loc = 'upper right')
    ax1.set_ylabel('Age Groups')
    
    return fig

import matplotlib.animation as animation
fig, ax = plt.subplots()
ims = []

for year in years:
    im = ax.imshow(plt.imread(f'{year}.jpeg'), animated = True)
    ims.append([im])

ani = animation.ArtistAnimation(fig, ims, interval=600)
ani.save('us_population.gif')
```

## 画数学函数图形

```python
import matplotlib.pyplot as plt
import numpy as np

# plt.rcParams['font.sans-serif'] = ['SimHei']
# plt.rcParams['axes.unicode_minus'] = False
# %matplotlib inline

x = np.linspace(0, 5, 100)
y1 = 5-np.sqrt(25-x**2)
y2 = np.sqrt(5*x-x**2)

plt.figure(figsize=(5, 5))

plt.plot(x, y1, label='扇形y=5-$\sqrt{25-x^2}$')
plt.plot(x, y2, label='圆形y=$\sqrt{5x-x^2}$')

plt.fill_between(x[y1 <= y2], y1[y1 <= y2], y2[y1 <= y2])

plt.xlim(0, 5)
plt.ylim(0, 5)

plt.axvline(4, ymin=0, ymax=0.4, color="r", ls="--")

plt.legend()

plt.show()
```

## 画布布局
- plt.figure()
  创建一个画布。参数num：相当于给画布定义一个id，如果给出了num，之前没有使用，则创建一个新的画布；如果之前使用了这个num，那么返回那个画布的引用，在之前的画布上继续作图。如果没有给出num, 则每次创建一块新的画布。
- add_subplot()
  向图中加入子图的轴。返回子图的坐标轴axes。可通过 返回的 ax 设置各种图的参数。
- add_subplot()
 向图中加入子图的轴。只影响当前子图
```python
import numpy as np
from matplotlib import pyplot as plt
from scipy.interpolate import interp1d
 
x=np.linspace(0,10*np.pi,num=20)
y=np.sin(x)
yn=np.cos(x)
f1=interp1d(x,y,kind='linear')#线性插值
f2=interp1d(x,y,kind='cubic')#三次样条插值
x_pred=np.linspace(0,10*np.pi,num=1000)
y1=f1(x_pred)
y2=f2(x_pred)
plt.figure(1)
plt.plot(x_pred,y1,'r',label='linear')
plt.plot(x_pred,y2,'b--',label='cubic')
plt.legend()
# plt.show()
plt.figure(2)
plt.plot(x,yn,label='new')
plt.legend()
```

```python
import matplotlib.pyplot as plt
from numpy import *

x = [1, 2, 3, 4, 5]
y = [1, 4, 9, 16, 20]
fig = plt.figure()

#修改处，原先是 121
fig.add_subplot(211)
plt.scatter(x, y)

#新增 223 
fig.add_subplot(223)
plt.scatter(x, y)

fig.add_subplot(224)
plt.scatter(x, y)
plt.show()
```

```python
import numpy as np
import matplotlib.pyplot as plt
 
# Fixing random state for reproducibility
np.random.seed(19680801)
 
 
x = np.random.rand(10)
y = np.random.rand(10)
z = np.sqrt(x**2 + y**2)
 
plt.subplot(321)
plt.scatter(x, y, s=80, c=z, marker=">")
 
plt.subplot(322)
plt.scatter(x, y, s=80, c=z, marker=(5, 0))
 
verts = np.array([[-1, -1], [1, -1], [1, 1], [-1, -1]])
plt.subplot(323)
plt.scatter(x, y, s=80, c=z, marker=verts)
 
plt.subplot(324)
plt.scatter(x, y, s=80, c=z, marker=(5, 1))
 
plt.subplot(325)
plt.scatter(x, y, s=80, c=z, marker='+')
 
plt.subplot(326)
plt.scatter(x, y, s=80, c=z, marker=(5, 2))
```

### 子图间距位置

`subplots_adjust(left=None, bottom=None, right=None, top=None, wspace=None, hspace=None)`
**函数参数与示例**

| 参数     | 含义                                       |
|--------|------------------------------------------|
| left   | 可选参数，浮点数；子区左边的位置，默认为 0.125，以画布figure为参考系 |
| right  | 可选参数，浮点数；子区右边的位置 ，默认为 0.9，以画布figure为参考系  |
| bottom | 可选参数，浮点数；子区底边的位置，默认为 0.11，以画布figure为参考系  |
| top    | 可选参数，浮点数；子区顶边的位置，默认为 0.88，以画布figure为参考系  |
| wspace | 可选参数，浮点数；子区之间的空白宽度，默认为 0.2，以绘图区的平均宽度为参考  |
| hspace | 可选参数，浮点数；子区之间的空白高度，默认为 0.2，以绘图区的平均宽度为参考  |


```python
import matplotlib.pyplot as plt
import numpy as np
import matplotlib as mpl

mpl.rcParams['font.sans-serif'] = ['KaiTi']
mpl.rcParams['axes.unicode_minus'] = False

x = np.linspace(0, 2 * np.pi, 500)
y1 = np.sin(x) * np.cos(x)
y2 = np.exp(-x)
y3 = np.sqrt(x)
y4 = x / 4

fig, ax = plt.subplots(4, 1, facecolor='beige', sharex=True, subplot_kw=dict(facecolor='seashell'))

fig.subplots_adjust(hspace=0)

ax[0].plot(x, y1, c='r', lw=2)
ax[1].plot(x, y2, c='y', ls="--")
ax[2].plot(x, y3, c='g', ls=":")
ax[3].plot(x, y4, c='m', ls='-.', lw=2)

plt.show()
```

## Windows 中文乱码问题
下载 SimSun https://www.webfontfree.com/cn/download/SimSun
修改 `D:\Program Files\python313\Lib\site-packages\matplotlib\mpl-data\matplotlibrc`

```ini
#font.serif:      SimSun, DejaVu Serif, Bitstream Vera Serif, Computer Modern Roman, New Century Schoolbook, Century Schoolbook L, Utopia, ITC Bookman, Bookman, Nimbus Roman No9 L, Times New Roman, Times, Palatino, Charter, serif
#font.sans-serif: SimSun, DejaVu Sans, Bitstream Vera Sans, Computer Modern Sans Serif, Lucida Grande, Verdana, Geneva, Lucid, Arial, Helvetica, Avant Garde, sans-serif
#font.cursive:    SimSun, Apple Chancery, Textile, Zapf Chancery, Sand, Script MT, Felipa, Comic Neue, Comic Sans MS, cursive
#font.fantasy:    SimSun, Chicago, Charcoal, Impact, Western, xkcd script, fantasy
#font.monospace:  SimSun, DejaVu Sans Mono, Bitstream Vera Sans Mono, Computer Modern Typewriter, Andale Mono, Nimbus Mono L, Courier New, Courier, Fixed, Terminal, monospace
```
