# coding:utf-8
import numpy as np
import pylab as pl # sudo apt-get install python-matplotlib
from sklearn import svm

np.random.seed(0)

X = np.r_[np.random.randn(20, 2) - [2, 2], np.random.randn(20, 2) + [2, 2]]
Y = [0] * 20 + [1] * 20
print "X:",X
print "Y:",Y
clf = svm.SVC(kernel="linear")
clf.fit(X, Y)

# get the separating hyperplane
w = clf.coef_[0]
# 斜率
a = -w[0] / w[1]
# 产生连续的值
xx = np.linspace(-5, 5)
# 截距 (clf.intercept_[0]) / w[1]
# w0x + w1y + w3 = 0 => y = -(w0/w1)x + w3/w1
yy = a * xx - (clf.intercept_[0]) / w[1]

# 下面的线，取第一个支持向量，取得截距
b = clf.support_vectors_[0]
yy_down = a * xx + (b[1] - a * b[0])
# 取第二个支持向量，取得截距
b = clf.support_vectors_[-1]
yy_up = a * xx + (b[1] - a * b[0])

print "w:",w
print "a:",a

print "support_vectors_:",clf.support_vectors_
print "clf.coef_:",clf.coef_


pl.plot(xx,yy,'k-')
pl.plot(xx,yy_down,'k--')
pl.plot(xx,yy_up,'k--')

pl.scatter(clf.support_vectors_[:,0],clf.support_vectors_[:,1],s=80,facecolors = 'none')
pl.scatter(X[:,0],X[:,1],c=Y,cmap=pl.cm.Paired)

pl.axis('tight')
pl.show()
