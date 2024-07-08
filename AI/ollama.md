# Ollama

Ollama 是一个开源的人工智能（AI）和机器学习（ML）工具平台，特别设计用于简化大型语言模型（LLM）的部署和使用流程。

- https://github.com/ollama/ollama
- https://github.com/ollama/ollama/blob/main/docs/linux.md

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
docker exec -it ollama ollama run mistral
```

## 查看已有模型

```shell
ollama list
```

## REST API
```shell
curl http://localhost:11434/api/generate -d '{
  "model": "qwen:14b",
  "prompt":"你好"
}'
```
Chat
```shell
curl http://localhost:11434/api/chat -d '{
  "model": "qwen:14b",
  "messages": [
    { "role": "user", "content": "你好" }
  ]
}'
```
