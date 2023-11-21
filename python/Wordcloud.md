## 安装

```shell
pip install wordcloud #这是WordCloud的库
pip install matplotlib #显示图像
```

```shell
pip install numpy
```

## 一个单词构造 WordCloud
```py
import numpy as np
import matplotlib.pyplot as plt

from wordcloud import WordCloud
text = "square" #输入你要的单词
x, y = np.ogrid[:300, :300] #快速产生一对数组

# 产生一个以(150,150)为圆心,半径为130的圆形mask
mask = (x - 150) ** 2 + (y - 150) ** 2 > 130 ** 2 #此时mask是bool型
mask = 255 * mask.astype(int) #变量类型转换为int型

wc = WordCloud(
   background_color="white", #背景颜色为“白色”
   repeat=True, #单词可以重复
   mask=mask #指定形状，就是刚刚生成的圆形
)
wc.generate(text) #从文本生成wordcloud
plt.axis("off") #把作图的坐标轴关掉
plt.imshow(wc, interpolation="bilinear")
plt.show()
```
