mlpack是一个C++的机器学习库，它重点在于其扩展性、高速性和易用性。它的目的是让新用户通过简单、一致的API使用机器学习，同时为专业用户提供C++的高性能和最大灵活性。

# 编译安装

mlpack依赖于一下类库：

- Armadillo >= 3.6.0 (with LAPACK support)
- LibXML2 >= 2.6.0
- Boost (math_c99, program_options, unit_test_framework, random)

Armadillo C++ Library 是一种C++的线性代数库，包含一些矩阵和向量的运算，可以选用高效的 LAPACK 和 BLAS 进行加速。

## 安装依赖库

### 安装 Armadillo 依赖的库

```shell
sudo apt-get install liblapack-dev
sudo apt-get install libblas-dev
sudo apt-get install libboost-dev
```

### 安装 Armadillo

```shell
sudo apt-get install libarmadillo-dev
```

### 安装 LibXML2、Boost

```shell
sudo apt-get install libboost-math-dev libboost-program-options-dev libboost-random-dev libboost-test-dev libxml2-dev
```

```shell
https://github.com/mlpack/mlpack/archive/mlpack-2.0.2.tar.gz
tar zxvf mlpack-2.0.2.tar.gz
cd mlpack-2.0.2
mkdir build
cd build
cmake ../
make -j4 && sudo make install
```

# 使用

教程中包含的算法有：

- 近邻搜索（NeighborSearch）
- 范围搜索（RangeSearch）
- 线性回归（LinearRegression）
- 欧几里德最小生成树（The Euclidean Minimum Spanning Tree）
- K-均值（K-Means）
- FastMKS（Fast max-kernel search）

mlpack提供了大量的类或API供程序调用，同时还提供了很多可执行程序供不懂C++的用户使用。
这些可执行文件包括：

allkfn, allknn, emst, gmm, hmm_train, hmm_loglik, hmm_viterbi, hmm_generate, kernel_pca, kmeans, lars, linear_regression, local_coordinate_coding, mvu, nbc, nca, pca, radical, sparse_coding。

C++ 程序中使用，需要加链接 `-lmlpack -larmadillo`

## range_search

采用的是欧几里得度量（euclidean metric）也称欧氏距离(Euclidean Distance)。

欧氏距离是最易于理解的一种距离计算方法，源自欧氏空间中两点间的距离公式。

执行以下命令会输出范围内点之间的距离，以及对应所在的行数（从0开始）。

```shell
mlpack_range_search --min=0.25 --max=1.05 --reference_file=csv/test_data_3_1000.csv --distances_file=out/distances.csv --neighbors_file=out/neighbors.csv
mlpack_range_search --min=1 --max=4 --reference_file=csv/test.csv --distances_file=out/test_distances.csv --neighbors_file=out/test_neighbors.csv
```

## 

mlpack_knn -r dataset.csv -n neighbors_out.csv -d distances_out.csv -k 5 -v