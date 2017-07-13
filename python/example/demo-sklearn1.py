import numpy as np
from sklearn import datasets
from sklearn.cross_validation import train_test_split
from sklearn.neighbors import KNeighborsClassifier

iris = datasets.load_iris();
# 属性
iris_X = iris.data
# 每个实例的分类
iris_Y = iris.target

# 打印2行数据
print(iris_X[0:2,:])
# 打印每个实例的分类
print(iris_Y)

# 所有实例随机分成2份，学习和测试比例7:3
X_train, X_test, Y_train, Y_test = train_test_split(
    iris_X, iris_Y, test_size=0.3)

# 创建分类器
knn = KNeighborsClassifier()
knn.fit(X_train, Y_train)
print(knn.predict(X_test))
print(Y_test)
