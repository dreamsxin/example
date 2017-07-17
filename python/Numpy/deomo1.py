# coding: utf-8
import numpy as np

mylist = [1,2,3,4,5]
a = 10
mymatrix = np.mat(mylist)
mymatrix2 = np.array(mylist)
print 'mat'
print mymatrix
print mymatrix.I
print a*mymatrix

mymatrix = np.array(mylist)
print 'array'
print mymatrix
print a*mymatrix

print 'zeros'
myZero = np.zeros([3,5]) # 3*5的全零矩阵
print myZero
print 'ones'
myOnes = np.ones([3,5]) # 3*5的全1矩阵
print myOnes
print 'random.rand(3,4)'
myRand = np.random.rand(3,4) # 3行4列的0~1之间的随机数矩阵
print myRand

print 'eye'
myEye = np.eye(3) # 3*3的单位阵
print myEye

mymatrix = np.mat( [[1,2,3],[4,5,6],[7,8,9]])
print mymatrix
print 'np.sum'
print np.sum(mymatrix)
print 'sum'
print sum(mymatrix)

mymatrix = np.mat( [[1,2,3],[4,5,6],[7,8,9]])
mymatrix2 = 1.5*np.ones([3,3])
print mymatrix
print mymatrix2
print 'np.multiply'
print np.multiply(mymatrix,mymatrix2)

print 'np.power'
print np.power(mymatrix,2)

mymatrix = np.mat([[1,2,3],[4,5,6],[7,8,9]])
mymatrix2 = np.mat([[1],[2],[3]])
print '*'
print mymatrix*mymatrix2 # 1*1 + 2*2 + 3*3 = 14

print 'transpose'
print mymatrix.T # 矩阵的转置
print mymatrix.transpose() # 矩阵的转置

mymatrix = np.mat([[1,2,3],[4,5,6],[7,8,9]])
[m,n]= np.shape(mymatrix) # 矩阵的行列数
[m,n]= mymatrix.shape
print "矩阵的行数和列数:",m,n

myscl1 = mymatrix[0] # 按行切片
print "按行切片:",myscl1
myscl2 = mymatrix.T[0] # 按列切片
print "按列切片:",myscl2

# n阶方阵的行列式运算
A = np.mat( [[1,2,4,5,7,],[9,12,11,8,2,],[6,4,3,2,1,],[9,1,3,4,5],[0,2,3,4,1]])
print "det(A):",np.linalg.det(A); # 方阵的行列式

print 'inv'
print np.linalg.inv(mymatrix) # 矩阵的逆

A = np.mat([[1,2,4,5,7,],[9,12,11,8,2,],[6,4,3,2,1,],[9,1,3,4,5],[0,2,3,4,1]])
print np.linalg.matrix_rank(A) #矩阵的秩

A = np.mat([[1,2,4,5,7,],[9,12,11,8,2,],[6,4,3,2,1,],[9,1,3,4,5],[0,2,3,4,1]])
b = [1,0,1,0,1]
S = np.linalg.solve(A,np.array(b))
print S

# 向量范数的运算
A = [8,1,6]
# 手工计算
modA = np.sqrt(sum(np.power(A,2)))
print "modA:",modA
# 库函数
normA = np.linalg.norm(A)
print "norm(A):",normA

# 实现欧式距离公式的：
vector1 = np.mat([1,2,3])
vector2 = np.mat([4,5,6])
print np.sqrt((vector1-vector2)*((vector1-vector2).T))

# 曼哈顿距离
vector1 = np.mat([1,2,3])
vector2 = np.mat([4,5,6])
print np.sum(np.abs(vector1-vector2))

# 切比雪夫距离
vector1 = np.mat([1,2,3])
vector2 = np.mat([4,7,5])
print np.abs(vector1-vector2).max()

# 夹角余斜
vector1 = np.array([1,2,3])
vector2 = np.array([4,7,5])
cosV12 = np.dot(vector1,vector2)/(np.linalg.norm(vector1)*np.linalg.norm(vector2))
print cosV12

# 汉明距离
matV = np.mat([[1,1,0,1,0,1,0,0,1],[0,1,1,0,0,0,1,1,1]])
smstr = np.nonzero(matV[0]-matV[1]);
print smstr[0].shape
print np.shape(smstr[0])
print np.shape(smstr[0])[0]

# 杰卡德距离
import scipy.spatial.distance as dist # 导入scipy 距离公式
matV = np.mat([[1,1,0,1,0,1,0,0,1],[0,1,1,0,0,0,1,1,1]])
print "dist.jaccard:", dist.pdist(matV,'jaccard')