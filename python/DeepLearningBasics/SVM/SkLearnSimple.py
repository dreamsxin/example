from sklearn import svm

x=[[2,0],[1,1],[2,3]]
y=[0,0,1]
clf = svm.SVC(kernel="linear")
clf.fit(x,y)

print  clf

print clf.support_vectors_
# 支持向量所在索引
print clf.support_
# 每个类y（0,1）分别找到几个支持向量
print clf.n_support_

print clf.predict([[2,5],[1,0]])