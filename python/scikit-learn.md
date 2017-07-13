# Scikit-learn

http://scikit-learn.org/stable/tutorial/machine_learning_map/index.html

## 覆盖问题领域

分类（classification）、回归（regression）、聚类（clustering）、降维（dimensionality reduction）、模型选择（model selection）、预处理（preprocessing）

## 安装

Scikit-learn requires:

- Python (>= 2.6 or >= 3.3),
- NumPy (>= 1.6.1),
- SciPy (>= 0.9).

* 安装 python 及必要的 packages
```shell
sudo apt-get install python python-numpy python-scipy python-matplotlib
# or install Anaconda
wget https://repo.continuum.io/archive/Anaconda2-4.4.0-Linux-x86_64.sh
chmod a+x Anaconda2-4.4.0-Linux-x86_64.sh
sudo ./Anaconda2-4.4.0-Linux-x86_64.sh
# or
sudo bash Anaconda2-4.4.0-Linux-x86_64.sh
```

- `/usr/lib/python2.7/dist-packages`

*安装 
```shell
sudo apt-get install python-sklearn graphviz
# or
# python < 3.0 需要安装 sudo easy_install pip
pip install -U scikit-learn
# or
git clone https://github.com/scikit-learn/scikit-learn.git
cd scikit-learn
make
sudo python setup.py install
```

- `/home/myleft/.local/lib/python2.7/site-packages`

## 数据集模块 datasets

scikit-learn 的 datasets 模块包含测试数据相关函数，主要包括三类：

- datasets.load_*()：获取小规模数据集。数据包含在 datasets 里
- datasets.fetch_*()：获取大规模数据集。需要从网络上下载，函数的第一个参数是 data_home，表示数据集下载的目录，默认是 ~/scikit_learn_data/。要修改默认目录，可以修改环境变量SCIKIT_LEARN_DATA。数据集目录可以通过datasets.get_data_home()获取。clear_data_home(data_home=None)删除所有下载数据。
- datasets.make_*()：本地生成数据集。

### 数据集格式

- tuple(X, y)
    本地生成数据函数 make_* 和 load_svmlight_* 返回的数据是 tuple(X, y) 格式

- Bunch
    load_*和 fetch_* 函数返回的数据类型是 datasets.base.Bunch，本质上是一个 dict，它的键值对可用通过对象的属性方式访问。主要包含以下属性：
        data：特征数据数组，是 n_samples * n_features 的二维 numpy.ndarray 数组
        target：标签数组，是 n_samples 的一维 numpy.ndarray 数组
        DESCR：数据描述
        feature_names：特征名
        target_names：标签名

### 获取小数据集

- load_boston()：
    房屋特征-房价，用于regression
- load_diabetes()：
    糖尿病数据，用于 regression
- load_linnerud()：
    Linnerud数据集，有多个标签，用于 multilabel regression
- load_iris()：
    鸢尾花特征和类别，用于classification
- load_digits([n_class])：
    手写数字识别
- load_sample_images()：
    载入图片数据集，共两张图
- load_sample_image(name)：
    载入图片数据集中的一张图
- load_files(container_path, description=None, categories=None, load_content=True, shuffle=True, encoding=None, decode_error='strict', random_state=0)：
    从本地目录获取文本数据，并根据二级目录做分类

### 获取大数据集

- load_mlcomp(name_or_id, set_='raw', mlcomp_root=None, **kwargs)：
    从 http://mlcomp.org/ 上下载数据集
- fetch_california_housing(data_home=None, download_if_missing=True)
- fetch_olivetti_faces(data_home=None, shuffle=False, random_state=0, download_if_missing=True)：
    Olivetti 脸部图片数据集
- fetch_lfw_people(data_home=None, funneled=True, resize=0.5, min_faces_per_person=0, color=False, slice_=(slice(70, 195, None), slice(78, 172, None)), download_if_missing=True)：
- fetch_lfw_pairs(subset='train', data_home=None, funneled=True, resize=0.5, color=False, slice_=(slice(70, 195, None), slice(78, 172, None)), download_if_missing=True)：
    Labeled Faces in the Wild (LFW) 数据集，参考 LFW
- fetch_20newsgroups(data_home=None, subset='train', categories=None, shuffle=True, random_state=42, remove=(), download_if_missing=True)
- fetch_20newsgroups_vectorized(subset='train', remove=(), data_home=None)：
    新闻分类数据集，数据集包含 ‘train’ 部分和 ‘test’ 部分。
- fetch_rcv1(data_home=None, subset='all', download_if_missing=True, random_state=None, shuffle=False)：
    路透社新闻语聊数据集
- fetch_mldata(dataname, target_name='label', data_name='data', transpose_data=True, data_home=None)：
    从 mldata.org 中下载数据集。参考 PASCAL network
- mldata_filename(dataname)：
    将 mldata 的数据集名转换为下载的数据文件名
- fetch_covtype(data_home=None, download_if_missing=True, random_state=None, shuffle=False)
    Forest covertypes 数据集

### 本地生成数据

#### 回归(regression)

- make_regression(n_samples=100, n_features=100, n_informative=10, n_targets=1, bias=0.0, effective_rank=None, tail_strength=0.5, noise=0.0, shuffle=True, coef=False, random_state=None)
- make_sparse_uncorrelated(n_samples=100, n_features=10, random_state=None)
- make_friedman1(n_samples=100, n_features=10, noise=0.0, random_state=None)
- make_friedman2(n_samples=100, noise=0.0, random_state=None)
- make_friedman3(n_samples=100, noise=0.0, random_state=None)

####分类(classification)

* 单标签

- make_classification(n_samples=100, n_features=20, n_informative=2, n_redundant=2, n_repeated=0, n_classes=2, n_clusters_per_class=2, weights=None, flip_y=0.01, class_sep=1.0, hypercube=True, shift=0.0, scale=1.0, shuffle=True, random_state=None)：
    生成 classification 数据集。包含所有的设置，可以包含噪声，偏斜的数据集
- make_blobs(n_samples=100, n_features=2, centers=3, cluster_std=1.0, center_box=(-10.0, 10.0), shuffle=True, random_state=None)：
    生成 classification 数据集。数据服从高斯分布
- centers 可以是整数，表示中心点个数，或者用列表给出每个中心点的特征值
- cluster_std 可以是浮点数或浮点数列表
- random_state 可以是整数，表示随机起始 seed，或者 RandomState 对象，默认使用 np.random
- make_gaussian_quantiles(mean=None, cov=1.0, n_samples=100, n_features=2, n_classes=3, shuffle=True, random_state=None)：
- make_hastie_10_2(n_samples=12000, random_state=None)：
- make_circles(n_samples=100, shuffle=True, noise=None, random_state=None, factor=0.8)：

- make_moons(n_samples=100, shuffle=True, noise=None, random_state=None)：

* 多标签

- make_multilabel_classification(n_samples=100, n_features=20, n_classes=5, n_labels=2, length=50, allow_unlabeled=True, sparse=False, return_indicator='dense', return_distributions=False, random_state=None)：
    生成 multilabel classification 数据集。

* 双聚类(bicluster)

- make_biclusters(shape, n_clusters, noise=0.0, minval=10, maxval=100, shuffle=True, random_state=None)：
- make_checkerboard(shape, n_clusters, noise=0.0, minval=10, maxval=100, shuffle=True, random_state=None)：

* 流形学习(manifold learning)

- make_s_curve(n_samples=100, noise=0.0, random_state=None)
- make_swiss_roll(n_samples=100, noise=0.0, random_state=None)、

* 可降维(decomposition)数据

- make_low_rank_matrix(n_samples=100, n_features=100, effective_rank=10, tail_strength=0.5, random_state=None)
- make_sparse_coded_signal(n_samples, n_components, n_features, n_nonzero_coefs, random_state=None)
- make_spd_matrix(n_dim, random_state=None)
- make_sparse_spd_matrix(dim=1, alpha=0.95, norm_diag=False, smallest_coef=0.1, largest_coef=0.9, random_state=None)

* 处理 svmlight / libsvm 格式数据

提供 svmlight / libsvm 格式数据的导入或导出。

- load_svmlight_file(f, n_features=None, dtype=numpy.float64, multilabel=False, zero_based='auto', query_id=False)：
- 返回 (X, y, [query_id])，其中 X 是 scipy.sparse matrix，y 是 numpy.ndarray
- load_svmlight_files(files, n_features=None, dtype=numpy.float64, multilabel=False, zero_based='auto', query_id=False)
- dump_svmlight_file(X, y, f, zero_based=True, comment=None, query_id=None, multilabel=False)
