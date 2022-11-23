

## CPU环境镜像（版本号：1.0.2）：

`registry.cn-hangzhou.aliyuncs.com/modelscope-repo/modelscope:ubuntu20.04-py37-torch1.11.0-tf1.15.5-1.0.2`

## GPU环境镜像（版本号：1.0.2）：
`registry.cn-hangzhou.aliyuncs.com/modelscope-repo/modelscope:ubuntu20.04-cuda11.3.0-py37-torch1.11.0-tf1.15.5-1.0.2`

## Python环境配置

安装配置`Anaconda`环境。 安装完成后，执行如下命令为`modelscope library`创建对应的python环境。
```shell
conda create -n modelscope python=3.7
conda activate modelscope
```

## 安装深度学习框架
**安装pytorc**
```shell
pip3 install torch torchvision torchaudio
pip3 install torch torchvision torchaudio -i https://pypi.tuna.tsinghua.edu.cn/simple
```
**安装Tensorflow参考链**
```shell
pip install --upgrade tensorflow==1.15 # 仅支持 CPU 的版本
pip install --upgrade tensorflow-gpu==1.15 # GPU 版
```

## ModelScope Library 安装

如仅需体验CV领域模型，可执行如下命令安装依赖：
```shell
pip install "modelscope[cv]" -f https://modelscope.oss-cn-beijing.aliyuncs.com/releases/repo.html
```

如仅需体验NLP领域模型，可执行如下命令安装依赖：
```shell
pip install "modelscope[nlp]" -f https://modelscope.oss-cn-beijing.aliyuncs.com/releases/repo.html
```

如仅需体验多模态领域模型，可执行如下命令安装依赖：
```shell
pip install "modelscope[multi-modal]" -f https://modelscope.oss-cn-beijing.aliyuncs.com/releases/repo.html
```
如仅需体验语音领域模型，请执行如下命令：
```shell
pip install "modelscope[audio]" -f https://modelscope.oss-cn-beijing.aliyuncs.com/releases/repo.html
```
** 注意：语音相关的功能仅支持 python3.7,tensorflow1.15.4的Linux环境使用**

如仅需体验科学计算领域模型，请执行如下命令：
```shell
pip install "modelscope[science]" -f https://modelscope.oss-cn-beijing.aliyuncs.com/releases/repo.html
```
** 注意：语音相关的功能仅支持 python3.7,tensorflow1.15.4的Linux环境使用**

如果您所有领域的模型功能都想体验，可执行如下命令
```shell
pip install "modelscope[audio,cv,nlp,multi-modal,science]" -f https://modelscope.oss-cn-beijing.aliyuncs.com/releases/repo.html
```
注：

如果您已经安装过ModelScope，但是需要升级使用新版发布的Library，可以使用
```shell
pip install "modelscope" --upgrade -f https://modelscope.oss-cn-beijing.aliyuncs.com/releases/repo.html
```

## 安装验证

安装成功后，即可使用对应领域模型进行推理，训练等操作。这里我们以NLP领域为例。安装后，可执行如下命令，运行中文分词任务，来验证安装是否正确：
```shell
python -c "from modelscope.pipelines import pipeline;print(pipeline('word-segmentation')('今天天气不错，适合 出去游玩'))"
```
