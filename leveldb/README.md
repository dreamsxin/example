# leveldb

## 1.20 安装

```shell
unzip leveldb-1.20.zip 
cd leveldb
make
# 拷贝头文件
sudo cp -r include/leveldb /usr/local/include
# 拷贝动态库
sudo cp out-shared/libleveldb.so.1.20 /usr/local/lib
cd /usr/local/lib
sudo ln -s libleveldb.so.1.20 libleveldb.so.1
sudo ln -s libleveldb.so.1 libleveldb.so
# 将动态库加入缓存中
sudo ldconfig
```
