from sklearn import svm

x=[[2,0],[1,1],[2,3]]
y=[0,0,1]
clf = svm.SVC(kernel="linear")
clf.fit(x,y)

print  clf

print clf.support_vectors_

print clf.support_

print clf.n_support_

print clf.predict([2,5])