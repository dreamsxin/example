import matplotlib.pyplot as plt
import numpy as np

x = np.linspace(-3,3,50)
y1 = 2*x+1
y2 = x**2

plt.figure()

# 设置可见区域范围
plt.xlim((-1,2))
plt.ylim((-2,3))
# label
plt.xlabel('i am x')
plt.ylabel('i am y')
# 设置单位ticks
new_ticks = np.linspace(-1,2,5)
print(new_ticks)
plt.xticks(new_ticks)
plt.yticks([-2, -1.8, -1, 1.22, 3],
           ['$really\ bad$', r'$bad\ \alpha$', '$normal$', '$good$', '$really\ good$']
        )
# legend
line_up, = plt.plot(x, y2, label="up")
line_down, = plt.plot(x, y1, color="red",linewidth=1.0,linestyle='--',label="down")
# loc = upper right center 
# plt.legend()
plt.legend(handles=[line_up,line_down],labels=['aaa', 'bbb'],loc='best')
plt.show()
