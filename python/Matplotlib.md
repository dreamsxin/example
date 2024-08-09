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
