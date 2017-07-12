import matplotlib.pyplot as plt
import numpy as np

def f(x,y):
    return (1 -x/2 + x**5 + y**3) * np.exp(-x**2 - y**2)

n = 256
x = np.linspace(-3,3,n)
y = np.linspace(-3,3,n)
# 网格
X,Y = np.meshgrid(x,y)

# use plt.contourf to filling contours
# X, Y and value for (X,Y) point
# 颜色 plt.cm.cool， 8 代表等高线分成几种
plt.contourf(X, Y, f(X, Y), 8, alpha=0.75, cmap=plt.cm.hot)

# use plt.contour to add contour lines
# 画等高线
C = plt.contour(X, Y, f(X,Y), 8, colors='black',linewidth=.5)

#adding label
plt.clabel(C, inline=True, fontsize=10)

plt.xticks(())
plt.yticks(())

plt.show()
