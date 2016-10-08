

```shell
sudo docker build -t caffe:cpu https://raw.githubusercontent.com/BVLC/caffe/master/docker/standalone/cpu/Dockerfile
```

```shell
git clone https://github.com/yahoo/open_nsfw.git
cd open_nsfw
docker run --volume=$(pwd):/workspace caffe:cpu \
python ./classify_nsfw.py \
--model_def nsfw_model/deploy.prototxt \
--pretrained_model nsfw_model/resnet_50_1by2_nsfw.caffemodel \
test_image.jpg
```