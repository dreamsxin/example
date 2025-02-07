# Ollama

https://www.ollama.com/

Ollama 是一个开源的人工智能（AI）和机器学习（ML）工具平台，特别设计用于简化大型语言模型（LLM）的部署和使用流程。

- https://github.com/ollama/ollama
- https://github.com/ollama/ollama/blob/main/docs/linux.md

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

## 

```shell
ollama run deepseek-r1
pip install open-webui
open-webui serve
```

