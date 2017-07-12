# coding:utf-8
from time import time
import logging
import matplotlib.pyplot as plt

#from sklearn.cross_validation import train_test_split
from sklearn.model_selection import train_test_split
from sklearn.datasets import fetch_lfw_people
#from sklearn.grid_search import GridSearchCV
from sklearn.model_selection import GridSearchCV
from sklearn.metrics import classification_report
from sklearn.metrics import confusion_matrix
from sklearn.decomposition import RandomizedPCA
from sklearn.svm import SVC
from sklearn.utils import class_weight

print(__doc__)

# 输出日志信息
logging.basicConfig(level=logging.INFO, format='%(asctime)s %(message)s')

# 加载数据 data_home="./scikit_learn_data",
lfw_people = fetch_lfw_people(min_faces_per_person=70, resize=0.4,
                              download_if_missing=True)

# 图片的数量及大小
n_samples, h, w = lfw_people.images.shape

# 每张图片中的特征数
X = lfw_people.data
n_features = X.shape[1]

# 对应的人名信息
y = lfw_people.target
target_names = lfw_people.target_names
n_classes = target_names.shape[0];

# 输出相应的信息
print('Total dataset size:')
print('n_samples %d' % n_samples)
print('n_featurs %d' % n_features)
print('n_calsses %d' % n_classes)

#################################################
# 构造训练集合测试集

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.25)

################################################
# PCA降维

n_components = 150;
print('Extracting the top %d eigenfaces from %d faces' % (n_components, X_train.shape[0]))

t0 = time()

# 进行pca降维，得到特征图
pca = RandomizedPCA(n_components=n_components, whiten=True).fit(X_train)
print('PCA finished in %0.3fs' % (time() - t0))
eigenfaces = pca.components_.reshape((n_components, h, w))

print('Projecting input data on the eigenfaces orthnormal basis')
t0 = time()
X_train_pca = pca.transform(X_train)
X_test_pca = pca.transform(X_test)
print('Projecting finished in %0.3fs' % (time() - t0))

###########################################
# 训练模型

print('Fitting a classifier to the training set')
t0 = time()

# 用来找最佳核函数参数
param_grid = {'C': [1e3, 5e3, 1e4, 5e4, 1e5],
              'gamma': [0.0001, 0.0005, 0.001, 0.005, 0.01, 0.01], }

clf = GridSearchCV(SVC(kernel='rbf', class_weight='balanced'), param_grid)
clf = clf.fit(X_train_pca, y_train)

print('Fitting finished in %0.3fs' % (time() - 0))
print('Best estinmator found by grid search:')
print(clf.best_estimator_)

##############################################
# 进行预测，在测试集上看效果
print("Predicting people's name on test data")
t0 = time()

y_pred = clf.predict(X_test_pca)
print("predict finished in %0.3fs" % (time() - t0))

print(classification_report(y_test, y_pred, target_names=target_names))
print(confusion_matrix(y_test, y_pred, labels=range(n_classes)))


#################################################
# 输出图像的函数

def plot_gallery(images, titles, h, w, n_row=3, n_col=4):
    plt.figure(figsize=(1.8 * n_col, 2.4 * n_row))
    plt.subplots_adjust(bottom=0, left=.01, right=.99, top=.90, hspace=.35)
    for i in range(n_row * n_col):
        plt.subplot(n_row, n_col, i + 1)
        plt.imshow(images[i].reshape((h, w)), cmap=plt.cm.gray)
        plt.title(titles[i], size=12)
        plt.xticks()
        plt.yticks()


####################################################
# 输出预测结果图

def title(y_pred, y_test, target_names, i):
    pred_name = target_names[y_pred[i]].rsplit(' ', 1)[-1]
    true_name = target_names[y_test[i]].rsplit(' ', 1)[-1]
    return 'predicted: %s\ntrue: %s' % (pred_name, true_name)


prediction_titles = [title(y_pred, y_test, target_names, i)
                     for i in range(y_pred.shape[0])]

plot_gallery(X_test, prediction_titles, h, w)

####################################################
# 输出特征图

eigenface_titles = ["eigenface: %d" % i for i in range(eigenfaces.shape[0])]
plot_gallery(eigenfaces, eigenface_titles, h, w)

plt.show()
