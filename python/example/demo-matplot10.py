import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

fig = plt.figure()
ax = Axes3D(fig)

# X, Y value
X = np.arange(-4, 4, 0.25)
Y = np.arange(-4, 4, 0.25)
print 'X:',X
print 'Y:',Y
X, Y = np.meshgrid(X, Y)    # x-y 平面的网格
print 'X:',X
print 'Y:',Y
R = np.sqrt(X ** 2 + Y ** 2)
# height value
Z = np.sin(R)

# rstride
ax.plot_surface(X, Y, Z, rstride=1, cstride=1, cmap=plt.get_cmap('rainbow'))
# zdir='z' 从哪个轴投影，offset投影的位置
ax.contourf(X, Y, Z, zdir='z', offset=-2, cmap=plt.get_cmap('rainbow'))
ax.set_zlim(-2, 2)

plt.show()
