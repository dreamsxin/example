import matplotlib.pyplot as plt
import numpy as np

n = 1024
# 方差为1的随机值
X = np.random.normal(0,1,n)
Y = np.random.normal(0,1,n)
T = np.arctan2(Y,X); # for color value

plt.scatter(X,Y, s=75, c=T,alpha=0.5)
#plt.scatter(np.arange(5), np.arange(5)) # 一条线上的点
plt.xlim((-1.5,1.5))
plt.ylim((-1.5,1.5))
# 隐藏 ticks
plt.xticks(())
plt.yticks(())

plt.show()
