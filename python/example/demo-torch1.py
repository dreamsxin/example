import torch
import numpy as np

# abs
data = [-1, -2, 1, 2]
tensor = torch.FloatTensor(data) # 32bit

print(np.sin(data))
print(torch.sin(tensor))
print(torch.abs(tensor))
print(torch.mean(tensor))

data = [[1,2],[3,4]]
data = np.array(data)
tensor = torch.FloatTensor(data)

# 矩阵相乘
print(
    '\nnumpy:', np.matmul(data, data),
    '\nnumpy:', np.dot(data),
    '\ntorch:', torch.mm(tensor, tensor)
    '\ntorch:', torch.dot(tensor) # 1x1 + 2x2 + 3x3 + 4x4 = 30
)
