
https://github.com/ollama/ollama

```shell
docker pull ollama/ollama
```

## CPU only
```shell
docker run -d -v ollama:/root/.ollama -p 11434:11434 --name ollama ollama/ollama
```

```shell
docker run -d --gpus=all -v ollama:/root/.ollama -p 11434:11434 --name ollama ollama/ollama
```

## 运行模型

```shell
docker exec -it ollama ollama run llama3
```
