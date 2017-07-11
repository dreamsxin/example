# encodeing:utf-8
import numpy as np

A = np.array([1,1,1])
B = np.array([2,2,2])
# 对列合并一起，增加行
C = np.vstack((A,B))
# 对行合并，增加列
D = np.hstack((A,B))
print('vertical stack', C)
print('horizontal stack', D)

print('A shape', A.shape)
print('B shape', B.shape)
print('C shape', C.shape)
print('D shape', D.shape)


# 列数或行数
print("A.T", A.T)
print("newaxis A", A[np.newaxis,:])
print("newaxis2 A", A[:, np.newaxis])
print('newaxis2 A.shape', A[:, np.newaxis].shape)

# 增加行
E = np.concatenate((A[:, np.newaxis], B[:, np.newaxis]), axis=0)
print("concatenate", E)
print("hstack", np.vstack((A[:, np.newaxis], B[:, np.newaxis])))
# 增加列
E = np.concatenate((A[:, np.newaxis], B[:, np.newaxis]), axis=1)
print("concatenate", E)

# 分割
A = np.arange(12).reshape((3,4))
print("A",A)
# 对列进行分割
print("split axis=1", np.split(A, 2, axis=1))
print("split axis=1", np.array_split(A, 3, axis=1))

print('vsplit', np.vsplit(A, 3))
print('hsplit', np.hsplit(A, 2))


