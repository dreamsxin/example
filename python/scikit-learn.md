# Scikit-learn

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
