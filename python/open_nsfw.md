
## 安装 Caffe

For Ubuntu (>= 17.04)

```shell
sudo apt install caffe-cpu
# or
sudo apt install caffe-cuda
```

For Ubuntu (< 17.04)

```shell
sudo apt-get install libprotobuf-dev libleveldb-dev libsnappy-dev libopencv-dev libhdf5-serial-dev protobuf-compiler
sudo apt-get install --no-install-recommends libboost-all-dev
sudo apt-get install libatlas-base-dev
sudo apt-get install libhdf5-serial-dev
sudo apt-get install python-dev python-pip
sudo apt-get install libgflags-dev libgoogle-glog-dev liblmdb-dev

git clone https://github.com/BVLC/caffe.git
cd caffe/python
for req in $(cat requirements.txt); do pip install $req; done
```

复制 `Makefile.config.example` 生成新文件 `Makefile.config`，修改内容：
```config
CPU_ONLY := 1
# Whatever else you find you need goes here.
INCLUDE_DIRS := $(PYTHON_INCLUDE) /usr/local/include /usr/include/hdf5/serial
LIBRARY_DIRS := $(PYTHON_LIB) /usr/local/lib /usr/lib /usr/lib/x86_64-linux-gnu/hdf5/serial
```

编译
```shell
make pycaffe
make all
make test
make runtest
```
如果不使用`make install`，需要设置环境变量
```shell
export PYTHONPATH=/path/to/caffe/python:$PYTHONPATH
```

## Docker 镜像

> 需要将 图片放在当前目录下，并指定 `--volume=$(pwd):/workspace`

```shell
# 下载构建 docker 镜像 https://github.com/BVLC/caffe/blob/master/docker/cpu/Dockerfile
docker build -t caffe:cpu https://raw.githubusercontent.com/BVLC/caffe/master/docker/cpu/Dockerfile

# 检测caffe的版本，这里可以看到docker run caffe:cpu 这个语句就是docker的运行语句，后面可以更命令，就和linux其他命令一样
docker run caffe:cpu caffe --version

# 运行模型
cd open_nsfw
docker run --volume=$(pwd):/workspace caffe:cpu \
python ./classify_nsfw.py \
--model_def nsfw_model/deploy.prototxt \
--pretrained_model nsfw_model/resnet_50_1by2_nsfw.caffemodel \
test_image.jpg
```