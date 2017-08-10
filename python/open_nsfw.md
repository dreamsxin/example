
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

git clone https://github.com/BVLC/caffe.git
cd /home/wangqi/caffe/python
```

```shell
# 从url下载docker命名问caffe:cpu https://github.com/BVLC/caffe/blob/master/docker/cpu/Dockerfile
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