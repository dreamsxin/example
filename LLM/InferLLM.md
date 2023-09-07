## 编译 InferLLM
```shell
git clone https://github.com/dreamsxin/InferLLM.git
cd InferLLM
mkdir build
cd build
cmake ..
make
```

## 下载模型

https://huggingface.co/kewin4933/InferLLM-Model/tree/main

- chatglm2-q4.bin

## 运行

```shell
./alpaca -m chatglm2-q4.bin -t 4
```
