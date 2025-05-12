
## Phi-4-reasoning-plus

- https://huggingface.co/microsoft/Phi-4-reasoning-plus?local-app=vllm

```shell
# Install vLLM from pip:
pip install vllm
```

```shell
# Load and run the model:
vllm serve "microsoft/Phi-4-reasoning-plus"
```

```shell
# Call the server using curl:
curl -X POST "http://localhost:8000/v1/chat/completions" \
	-H "Content-Type: application/json" \
	--data '{
		"model": "microsoft/Phi-4-reasoning-plus",
		"messages": [
			{
				"role": "user",
				"content": "What is the capital of France?"
			}
		]
	}'
```
