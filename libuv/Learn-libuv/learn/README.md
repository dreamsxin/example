# libuv v1.x 源码学习

> Based on OS X

## 准备工作

### 克隆项目

```bash
git clone https://github.com/Jiahonzheng/Learn-libuv.git
```

### 安装 GYP

[GYP: Generate Your Projects](https://gyp.gsrc.io/)

```bash
cd Learn-libuv/
git clone https://chromium.googlesource.com/external/gyp.git build/gyp
cd build/gyp
sudo ./setup.py install
```

可能在克隆 `gyp` 时，会出现无法解析域名的错误，解决办法有两种：[科学上网](https://jiahonzheng.github.io/2018/03/14/Mac%20Terminal%20%E4%BB%A3%E7%90%86%E9%85%8D%E7%BD%AE/)和[下载压缩包](https://github.com/svn2github/gyp)。

### 生成 Xcode 项目

```bash
cd Learn-libuv/
./gyp_uv.py -f xcode
```

执行上述命令后，执行 `out` 文件夹下的 `uv.xcodeproj` ，即可得到生成的 Xcode 项目。

### 生成开发文档

#### 安装 `sphinx`

我们需要 `sphinx` 编译官方提供的开发文档。

```bash
brew install sphinx-doc
```

#### 设置环境变量

```bash
echo "export PATH=/usr/local/opt/sphinx-doc/bin:$PATH" >> ~/.bash_profile
```

#### 编译

进入 `docs/` 文件夹，使用 `make` 生成特定格式。

```bash
cd Learn-libuv/docs/
make epub
```

### 编译 `libuv` 到 `OS X` 

我们通过 `autotools` 工具，将 `libuv` 编译到 `OS X` 。

#### 安装 `autotools` 

```bash
brew install autoconf
brew install automake
brew install libtool
```

#### 编译

```bash
cd Learn-libuv/
sh autogen.sh
./configure
make
make check
make install
```

当执行 `make check` 时，可能会出现部分报错，可忽视。