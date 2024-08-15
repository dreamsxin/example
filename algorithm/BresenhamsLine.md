# Bresenham's line

*Bresenham Line Drawing Algorithm*

Bresenham 直线算法是一种用于将两点之间的线段绘制在屏幕上的算法。它的特点是只用基本的加法、减法和比较操作就可以完成，是一种高效的绘线算法。是计算机图形学领域使用最广泛的直线扫描转换算法，其核心思想是由误差项符号决定下一个像素点取右边的一个点还是右上的一个点。

计算机显示设备是由一个个像素组成的，该算法的目的即为“点亮”由起始像素点 $(x_0, x_0)$ 至结束像素点 $(x_1, y_1)$ 的线段上的像素点，当分辨率足够大，像素点足够小，看起来就会像一条直线。

举例：

![example](https://github.com/dreamsxin/example/blob/master/algorithm/img/bresenham_example.png?raw=true)

为简化后续讨论，设置如下限定条件：

1. 起始点在结束点左下方；
2. $x_0 < x_1$ 且 $y_0 <= y_1$ ；
3. 线段的斜率 $0\le k\le 1$ ；
4. 对于区间 $[x_0,x_1]$ 内的所有 $x$ ，该位置仅存在一个像素点。

设置以上限定条件之后，我们可以得到一个十分粗糙的算法：

```py
def easyway(x0,y0,x1,y1):
  m = (y2-y1)/(x2-x1)
  for x in range(x0, x1+1, 1):
    # find y (integer)
    y = round(mx+b)
    colorify(x,y)
```

该算法是可行的，但在循环的每一步都需要计算 $mx+b$，执行速度较慢，因此，我们需要寻找一个快速得到 $y$ 值的方法。

## 推导

由于线段的斜率在0到1之间（斜率小于 1 则沿 x 轴递增来计算每个 x 值的对应 y 值；否则，沿 y 轴递增来计算每个 y 值的对应 x 值。），因此对于点 $(x_k,y_k)$ ，下个点只可能是 $(x_k+1,y_k)$ 或 $(x_k+1,y_k+1)$ ，其中 $k$ 表示横坐标 $x$ 移动的次数，如图所示。

![next point](https://github.com/dreamsxin/example/blob/master/algorithm/img/bresenham_next-point.png?raw=true)

因此，我们需要从 $y_k$ 和 $y_k+1$ 中获取离线段较近的值，假设直线方程为 $y = mx + b$ ，设 $y$ 为直线上实际的值， $d_1(k)$ 和 $d_2(k)$ 分别为距离 $y_k$ 和 $y_k+1$ 的距离，则有：

$$
y = m(x_k+1)+b \\
d_1(k) = y-y_k \\
d_2(k) = (y_k+1) - y
$$

比较 $d_1(k)$ 和 $d_2(k)$ 的值，即可判断下个点的具体位置：

1. 若 $d_1(k) \le d_2(k)$ ，认为下个点应为 $(x_k+1, y_k)$
2. 反之，认为下个点应为 $(x_k+1, y_k+1)$

因此，我们只需要判断 $d_1(k) - d_2(k)$ 的正负性即可：

$$
\begin{aligned}
  d_1(k) - d_2(k) &= (y-y_k) - ((y_k+1)-y) \\
  & = 2y - y_k - y_k -1 \\
  & = 2(m(x_k+1)+b) - 2y_k - 1 \\
  & = 2m(x_k+1) - 2y_k + 2b - 1 \\
\end{aligned}
$$

同时，我们有：

$$b = y_0 - mx_0$$

$$m = \frac{y_1-y_0}{x_1-x_0} = \frac{dy}{dx}$$

因此，进一步简化得到：

$$
\begin{aligned}
  d_1(k) - d_2(k) &= 2\frac{dy}{dx}(x_k+1) - 2y_k+2(y_0-\frac{dy}{dx}x_0) -1 \\
  &=2\frac{dy}{dx}x_k-2y_k+c
\end{aligned}
$$

其中 $c = 2\frac{dy}{dx}+2y_0-2\frac{dy}{dx}-1$ 。

设 $p(k) = d_1(k) - d_2(k)$，则:

$$
\begin{aligned}
p(0) &= d_1(0)-d_2(0) \\
& = 2\frac{dy}{dx}(x_0+1)-2y_0+2(y_0-\frac{dy}{dx}x_0)-1 \\
& = 2\frac{dy}{dx}-1
\end{aligned}
$$

对 $p(k+1)$ 推导如下：

$$
\begin{aligned}
  p(k+1) &= 2\frac{dy}{dx}x_{k+1}-2y_{k+1}+c\\
   &= 2\frac{dy}{dx}(x_k+1)-2y_{k+1}+c
\end{aligned}
$$

所以：

$$
\begin{aligned}
  p(k+1) - p(k) & = 2\frac{dy}{dx} - 2(y_{k+1}-y_k)
\end{aligned}
$$

因此：

1. 当 $p_k \le 0$ 即 $d_1(k) \le d_2(k)$ 时，$y_{k+1} = y_k$，则 $p(k+1) = p(k)+2\frac{dy}{dx}$
2. 反之，$p(k+1) = p(k) + 2\frac{dy}{dx}-2$

因此，综合以上，我们的代码即为:

```py
def pk(x0,y0,x1,y1):
  dy = y1-y0
  dx = x1-x0
  p = 2*dy/dx - 1

  base_increment = 2*dy/dx
  colorify(x0,y0)
  while x0 <= x1:
    x0 += 1
    if p > 0:
      y0 += 1
      p -= 2
    p += base_increment
    colorify(x0,y0)
```

## 整数化

上述算法仍然涉及浮点数的计算，下面我们考虑如何彻底取消浮点相关操作。

观察 $p(k)$ 的表达式，涉及浮点操作的仅为 $\frac{dy}{dx}$ ，因此，考虑将等式两侧同乘 $dx$，注意 $dx>0$ ，因此 $P(k)$ 与 $p(k)$ 正负性相同，有：

$$
P(k) = dx * p(k) = 2dy * x_k-2dx * y_k+c \\
P(0) = 2dy - dx \\
P(k+1)-P(k) = 2dy - 2dx*(y_{k+1}-y_k)
$$

同样的：

1. 当 $P_k \le 0$ 即 $d_1(k) \le d_2(k)$ 时，$y_{k+1} = y_k$，则 $P(k+1) = P(k)+2dy$
2. 反之，$P(k+1) = P(k) + 2dy-2dx$

代码即变为：

```py
def pk(x0,y0,x1,y1):
  dy = y1-y0
  dx = x1-x0
  p = 2*dy - dx

  base_increment = 2*dy
  colorify(x0,y0)
  while x0 <= x1:
    x0 += 1
    if p > 0:
      y0 += 1
      p -= 2*dx
    
    p += base_increment
    colorify(x0,y0)
```

## 一般化

之前的讨论是基于文首给出的限定条件的：

1. 起始点在结束点左下方；
2. $x_0<x_1$ 且 $y_0<=y_1$ ；
3. 线段的斜率 $0\le k\le 1$ ；
4. 对于区间 $[x_0,x_1]$ 内的所有 $x$ ，该位置仅存在一个像素点。

下面我们将该算法一般化。

### 垂直线

当 $x_0 = x_1$ 时，线段的斜率不存在，此时只需要使纵坐标 $y$ 每次递增单位长度，然后染色即可。

```py
if dx == 0:
  while y1!=y2:
    y1 += stepy
    colorify(x1,y1)
```

### dy<0, dx<0

当 dy<0 时，如下图：

![dy<0](https://github.com/dreamsxin/example/blob/master/algorithm/img/bresenham_dy.png?raw=true)

此时我们将点 $(x_1,y_1)$ 映射为 $(x_1,y_1')$，其中虚线为两条线段的垂直平分线，那么 $y_1'-y_0 = y_0 - y_1$，接着我们对虚线上侧的线段按上述算法计算需要绘制的点即可，但在实际绘制时，对需要递增 $y$ 的情况，我们递减 $y$，则可以对称的绘制出原需要绘制的直线。

同理，当 dx < 0 时，如下图：

![dx<0](/img/in-post/Bresenham/dx.png)

同样的，此时我们将点 $(x_1,y_1)$ 映射为 $(x_1',y_1)$，其中虚线为两条线段的垂直平分线，那么 $x_1'-x_0 = x_0 - x_1$，接着我们对虚线右侧的线段按上述算法计算需要绘制的点即可，但在实际绘制时，对需要递增 $x$ 的情况，我们递减 $x$，则可以对称的绘制出原需要绘制的直线。

当 dx 和 dy 均小于0时，需要对原线段作两次变换。

```py
dx = x1-x0
dy = y1-y0
if dy < 0:
    dy = -dy
    stepy = -1
else:
    stepy = 1

if dx < 0:
    dx = -dx
    stepx = -1
else:
    stepx = 1

#  ....

while x0 != x1:
    x0 += stepx

    if p >= 0:
        y0 += stepy
        # ....
    
    # ....

# ....
```

### m > 1

当直线斜率大于 1 时，即 dy>dx，原直线方程为 $y=mx+b$ ，可以变换为 $x = \frac{1}{m}y-\frac{b}{m}$，则 $\frac{1}{m} < 1$，因此只需要对该新直线应用上述算法即可。

### Put it together

```py
def bresenham(p1,p2):
    x0, y0 = p1
    x1, y1 = p2
    dx = x1-x0
    dy = y1-y0
    if dy < 0:
        dy = -dy
        stepy = -1
    else:
        stepy = 1

    if dx < 0:
        dx = -dx
        stepx = -1
    else:
        stepx = 1
    dy <<= 1
    dx <<= 1
    colorify(x0, y0)
    if dx == 0:
        while y0 != y1:
            y0 += stepy
            colorify(x0, y0)
    if dx > dy:
        fraction = dy-(dx >> 1)
        while x0 != x1:
            x0 += stepx

            if fraction >= 0:
                y0 += stepy
                fraction -= dx

            fraction += dy
            colorify(x0, y0)
    else:
        fraction = dx - (dy >> 1)
        while y0 != y1:
            if fraction >= 0:
                x0 += stepx
                fraction -= dy
            y0 += stepy
            fraction += dx
            colorify(x0, y0)
```

时间复杂度为 $O(x_2-x_1)$，空间复杂度为 $O(1)$

## 实际应用

使用 python 中的 `matplotlib` 模块对该算法进行可视化。

**(1,1) -> (2,10)：**

![example1](https://github.com/dreamsxin/example/blob/master/algorithm/img/bresenham_example1.png?raw=true)

**(1,1) -> (10,1)：**

![example2](https://github.com/dreamsxin/example/blob/master/algorithm/img/bresenham_example2.png?raw=true)

**(10,8) -> (1,1)：**

![example3](https://github.com/dreamsxin/example/blob/master/algorithm/img/bresenham_example3.png?raw=true)

## 源码

```python
import matplotlib.pyplot as plt


def rasterify_line(rect, p1, p2):
    '''Bresenham 线栅格化算法'''
    def in_area(x, y):
        return x >= 0 and x < width and y >= 0 and y < height

    def colorify(x, y):
        if in_area(x, y):
            raster[y][x] = 1

    def bresenham():
        x0, y0 = p1
        x1, y1 = p2
        dx = x1-x0
        dy = y1-y0
        if dy < 0:
            dy = -dy
            stepy = -1
        else:
            stepy = 1

        if dx < 0:
            dx = -dx
            stepx = -1
        else:
            stepx = 1
        dy <<= 1
        dx <<= 1
        colorify(x0, y0)
        if dx == 0:
            while y0 != y1:
                y0 += stepy
                colorify(x0, y0)
        if dx > dy:
            fraction = dy-(dx >> 1)
            while x0 != x1:
                x0 += stepx

                if fraction >= 0:
                    y0 += stepy
                    fraction -= dx

                fraction += dy
                colorify(x0, y0)
        else:
            fraction = dx - (dy >> 1)
            while y0 != y1:
                if fraction >= 0:
                    x0 += stepx
                    fraction -= dy
                y0 += stepy
                fraction += dx
                colorify(x0, y0)

    gap = 1
    minx, maxx, miny, maxy = rect
    width = int((maxx-minx) // gap + 1)
    height = int((maxy-miny) // gap + 1)
    raster = [[0]*width for _ in range(height)]
    bresenham()

    return raster


def plot(arr):
    _, ax = plt.subplots()
    ax.imshow(arr, cmap="gray", origin="lower")


rect = (0, 14, 0, 14)
raster = rasterify_line(rect, (10, 8), (1, 1))

plot(raster)
plt.xticks(range(0, 14, 2))
plt.yticks(range(0, 14, 2))
plt.show()
```

## 参考

<https://csustan.csustan.edu/~tom/Lecture-Notes/Graphics/Bresenham-Line/Bresenham-Line.pdf>

<https://en.wikipedia.org/wiki/Bresenham's_line_algorithm>
