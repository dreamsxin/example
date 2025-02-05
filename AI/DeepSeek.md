
## install
```shell
git clone https://github.com/deepseek-ai/DeepSeek-V3.git
cd inference
pip install -r requirements.txt
python fp8_cast_bf16.py --input-fp8-hf-path /path/to/fp8_weights --output-bf16-hf-path /path/to/bf16_weights
```

### vLLM
```shell
pip install vllm
vllm serve deepseek-ai/DeepSeek-R1-Distill-Qwen-32B --tensor-parallel-size 2 --max-model-len 32768 --enforce-eager
```

### SGLang
```shell
pip install --upgrade pip
pip install sgl-kernel --force-reinstall --no-deps
pip install "sglang[all]" --find-links https://flashinfer.ai/whl/cu124/torch2.5/flashinfer/
```
