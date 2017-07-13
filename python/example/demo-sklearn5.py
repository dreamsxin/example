from sklearn.datasets import load_iris
from sklearn.cross_validation import train_test_split
from sklearn.cross_validation import cross_val_score
from sklearn.neighbors import KNeighborsClassifier
import matplotlib.pyplot as plt


iris = load_iris()
X = iris.data
y = iris.target

# test train split #
X_train, X_test, y_train, y_test = train_test_split(X, y, random_state=4)

# n_neighbors=5 考虑最近几个值
knn = KNeighborsClassifier(n_neighbors=5)
knn.fit(X_train, y_train)

y_pred = knn.predict(X_test)
print(knn.score(X_test, y_test))

# 交叉验证 this is cross_val_score #
knn = KNeighborsClassifier(n_neighbors=5)
# cv=5 分成5组，scoring='accuracy' 判断准确度
scores = cross_val_score(knn, X, y, cv=5, scoring='accuracy')
print(scores)
# 平均值
print(scores.mean())

# this is how to use cross_val_score to choose model and configs #
k_range = range(1, 31)
k_scores = []
for k in k_range:
    knn = KNeighborsClassifier(n_neighbors=k)
##    loss = -cross_val_score(knn, X, y, cv=10, scoring='mean_squared_error') # for regression
##    k_scores.append(loss.mean())
    scores = cross_val_score(knn, X, y, cv=10, scoring='accuracy') # for classification
    k_scores.append(scores.mean())

plt.plot(k_range, k_scores)
plt.xlabel('Value of K for KNN')
plt.ylabel('Cross-Validated Accuracy')
plt.show()
