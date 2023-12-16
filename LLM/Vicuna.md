## Vicuna介绍
版本：Vicuna-13B和Vicuna-7B,
所需内存：单个GPU Vicuna-7B：14GB Vicuna-13B：28GB
训练数据：ShareGPT（目前已禁止爬取）收集的用户70K共享对话进行微调
基础模型：LLaMA
效果：Vicuna-13B在OpenAI ChatGPT和Google Bard的质量方面达到了90%以上
花费：训练耗费300美元

## 构建虚拟环境
```shell
conda create --name vicuna python=3.10
conda activate vicuna  # 激活环境
```
### 安装FastChat
```shell
# 方法一
pip3 install fschat

# 方法二
git clone https://github.com/lm-sys/FastChat.git
cd FastChat
pip3 install --upgrade pip  # enable PEP 660 support
pip3 install -e .
```

## 重下载和合并
根据LLaMA模型的许可声明，LLaMA原始权重和微调产生delta权重需要分别下载进行合并。

如果使用lfs下载huggingface的模型，需要先进行安装

`git lfs install`

### 原始权重下载
```shell
# 7b
git clone https://huggingface.co/decapoda-research/llama-7b-hf
# 13b
git clone https://huggingface.co/eachadea/vicuna-13b-1.1
```
### vicuna delta weights下载
```shell
# 7b
git clone https://huggingface.co/lmsys/vicuna-7b-delta-v1.1
# 13b
git clone https://huggingface.co/lmsys/vicuna-13b-delta-v1.1
```

### 权重合并
调用脚本合并权重
```shell
# 合并权重
python -m fastchat.model.apply_delta \ --base ./model/llama-7b-hf \ --delta ./model/vicuna-7b-delta-v1.1 \ --target ./model/vicuna-7b-all-v1.1
```

## 部署推理
### 命令行部署推理
**单GPU推理**
# 指定GPU CUDA_VISIBLE_DEVICES=1 
`CUDA_VISIBLE_DEVICES=1 python3 -m fastchat.serve.cli --model-path model/vicuna-13b-all-v1.1`
**多GPU推理**
# 指定GPU CUDA_VISIBLE_DEVICES=1,2
`CUDA_VISIBLE_DEVICES=1,2 python3 -m fastchat.serve.cli --model-path model/vicuna-13b-all-v1.1 --num-gpus 2`
**CPU推理**
`python3 -m fastchat.serve.cli --model-name /path/to/vicuna/weights --device cpu`
## Web GUI部署推理
使用web UI提供服务，需要三个主要组件：

- 与用户接口的web服务器；
- 托管一个或多个模型的模型worker；
- 协调web服务器和模型worker的控制器。操作步骤如下：

### 启动控制器
`python3 -m fastchat.serve.controller`

### 启动model work
`CUDA_VISIBLE_DEVICES=1 python3 -m fastchat.serve.model_worker --model-path model/vicuna-13b-all-v1.1`

### 发送测试消息
`python3 -m fastchat.serve.test_message --model-name vicuna-13b`

感觉报错了，但是最后还是启动成功了

### 启动gradio
`python3 -m fastchat.serve.gradio_web_server`

在浏览器打开对应网址就可以使用了
