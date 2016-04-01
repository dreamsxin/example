# TreeFrog 安装
```shell
sudo apt-get install -y qt4-qmake libqt4-dev libqt4-sql-mysql libqt4-sql-psql g++
git clone https://github.com/treefrogframework/treefrog-framework.git
cd treefrog-framework/
./configure --enable-debug
cd src && make && sudo make install
cd ../tools && make && sudo make install
```

# 创建一个 blog 应用

## 生成应用目录结构
```shell
tspawn new blogapp
```