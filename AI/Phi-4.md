
## Phi-4-reasoning-plus

- https://huggingface.co/microsoft/Phi-4-reasoning-plus?local-app=vllm

### 下载模型

```shell
pip install huggingface_hub 
huggingface-cli download microsoft/Phi-4-reasoning-plus
# or
git lfs install git clone https://huggingface.co/microsoft/Phi-4-reasoning-plus
```

### 

```shell
pip install tensorflow==2.13.0  # 指定稳定版本
pip install tensorflow-gpu==2.13.0  # 需提前安装匹配的CUDA和cuDNN驱动:ml-citation{ref="8" data="citationList"}

pip install torch torchvision torchaudio
# 根据CUDA版本选择命令（以CUDA11.8为例）
pip install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu118:ml-citation{ref="1" data="citationList"}

```

```python
# Use a pipeline as a high-level helper
from transformers import pipeline

messages = [
    {"role": "user", "content": "Who are you?"},
]
pipe = pipeline("text-generation", model="microsoft/Phi-4-reasoning-plus")
pipe(messages)
```

```python
# Load model directly
from transformers import AutoTokenizer, AutoModelForCausalLM

tokenizer = AutoTokenizer.from_pretrained("microsoft/Phi-4-reasoning-plus")
model = AutoModelForCausalLM.from_pretrained("microsoft/Phi-4-reasoning-plus")
```

###

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
