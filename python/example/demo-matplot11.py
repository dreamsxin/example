import matplotlib.pyplot as plt

plt.figure()

# 两行 两列，选择第一格
## plt.subplot(2,2,1)
## plt.plot([0,1],[0,1])

# 两行 两列，选择第二格
## plt.subplot(2,2,2)
## plt.plot([0,1],[0,2])

# 两行 一列，选择第一格
plt.subplot(2,1,1)
plt.plot([0,1],[0,1])

# 两行 两列，选择第三格
plt.subplot(223)
plt.plot([0,1],[0,3])

# 两行 两列，选择第四格
plt.subplot(224)
plt.plot([0,1],[0,4])

plt.show()
