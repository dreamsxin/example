## 编译 InferLLM
```shell
git clone https://github.com/MegEngine/InferLLM.git
cd InferLLM
mkdir build
cd build
cmake ..
make
```

## 下载模型

https://huggingface.co/kewin4933/InferLLM-Model/tree/main
- chinese-alpaca-7b-q4.bin
- chatglm-q4.bin

## 运行

```shell
./alpaca -m chinese-alpaca-7b-q4.bin -t 4
./chatglm -m chatglm-q4.bin -t 4
```
