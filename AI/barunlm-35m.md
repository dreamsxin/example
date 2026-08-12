## 安装

```shell
git clone https://github.com/harrrshall/barunlm-35m.git
cd barunlm-35m
python -m venv .venv
source .venv/bin/activate
pip install -e .
python examples/generate.py \
  --prompt "The future of efficient language models is" \
  --max-new-tokens 48 \
  --temperature 0.8
```

windows:
```shell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -e .
$env:PYTHONUTF8=1
python examples\generate.py --prompt "The future of efficient language models is" --max-new-tokens 48 --temperature 0.8
python examples\generate.py `
  --prompt "The future of efficient language models is" `
  --max-new-tokens 48 `
  --temperature 0.8
```

基于 `BarunLM-35M` 微调浏览器 Agent 语义路由，完整的训练流程如下。所有目录名、文件名和数据格式均参考 `BarunAction-35M` 的官方实现。

---

## 📁 项目目录结构

```
~/barun-router-training/
├── models/                          # 模型存放目录
│   └── BarunLM-35M/                 # 基础模型（从 Hugging Face 下载）
│       ├── config.json
│       ├── model.safetensors
│       ├── tokenizer.json
│       └── SHA256SUMS
├── data/                            # 数据集目录
│   ├── train.jsonl                  # 训练集（7,937 条样本）
│   └── dev.jsonl                    # 开发集（756 条样本）
├── src/                             # 训练源码
│   ├── train.py                     # 主训练脚本
│   ├── dataset.py                   # 数据加载与格式化
│   └── utils.py                     # 工具函数
├── outputs/                         # 训练输出
│   └── checkpoint-final/            # 最终模型检查点
├── configs/                         # 配置文件
│   └── training_config.json         # 训练超参数
└── scripts/
    └── run_train.sh                 # 一键训练脚本
```

---

## 📊 数据格式

### 训练数据格式（`data/train.jsonl`）

每行为一个 JSON 对象，采用 **Response-only SFT** 格式：

```json
{
  "messages": [
    {
      "role": "system",
      "content": "You are a browser agent that converts natural language requests into structured actions. Given a user request, tool schemas, and context, output strict Action IR JSON."
    },
    {
      "role": "user",
      "content": "打开百度首页"
    },
    {
      "role": "assistant",
      "content": "{\"calls\": [{\"args\": {\"url\": \"https://www.baidu.com\"}, \"tool\": \"navigate\"}], \"decision\": \"CALL\", \"mode\": \"SINGLE\"}"
    }
  ]
}
```

### 数据样本示例（覆盖各类浏览器操作）

| 类型 | 用户请求 (user) | 期望输出 (assistant) |
|------|----------------|---------------------|
| 导航 | `"打开百度首页"` | `{"calls":[{"args":{"url":"https://www.baidu.com"},"tool":"navigate"}],"decision":"CALL","mode":"SINGLE"}` |
| 点击 | `"点击页面上的登录按钮"` | `{"calls":[{"args":{"selector":"button.login"},"tool":"click"}],"decision":"CALL","mode":"SINGLE"}` |
| 输入 | `"在搜索框输入 Python 教程"` | `{"calls":[{"args":{"selector":"input.search","text":"Python 教程"},"tool":"type"}],"decision":"CALL","mode":"SINGLE"}` |
| 信息提取 | `"获取当前页面标题"` | `{"calls":[{"args":{},"tool":"get_title"}],"decision":"CALL","mode":"SINGLE"}` |
| 多操作 | `"搜索天气并打开第一个结果"` | `{"calls":[{"args":{"query":"天气"},"tool":"search"},{"args":{"index":0},"tool":"click_result"}],"decision":"CALL","mode":"SERIAL"}` |
| 拒识（负样本） | `"今天天气真好"` | `{"decision":"ABSTAIN","mode":"SINGLE"}` |

### 数据规模参考

- **训练集**：约 **7,937** 条样本
- **开发集**：约 **756** 条样本
- **训练 Epoch**：**1 个 epoch**（所有样本过一遍）
- **优化步数**：约 **126 步**

---

## 🚀 完整训练流程

### Step 1: 环境准备

```bash
# 1. 克隆 BarunLM-35M 仓库（包含训练基础设施）
git clone https://github.com/harrrshall/barunlm-35m.git
cd barunlm-35m

# 2. 创建虚拟环境
python -m venv .venv
source .venv/bin/activate  # Windows: .venv\Scripts\activate

# 3. 安装依赖
pip install -e .
pip install torch datasets accelerate transformers
```

### Step 2: 下载基础模型

```bash
# 下载 BarunLM-35M 基础模型并校验
hf download harrrshall/BarunLM-35M --local-dir ./models/BarunLM-35M
sha256sum -c ./models/BarunLM-35M/SHA256SUMS
```

### Step 3: 准备数据集

将你的中文路由数据集放入 `data/` 目录：

```bash
# 你的数据集应预先准备好
ls -la data/
# train.jsonl  (7,937 条)
# dev.jsonl    (756 条)
```

### Step 4: 训练脚本（`src/train.py`）

```python
import json
import torch
from transformers import AutoModelForCausalLM, AutoTokenizer, Trainer, TrainingArguments
from datasets import load_dataset

# ============ 配置 ============
MODEL_PATH = "./models/BarunLM-35M"
TRAIN_DATA = "./data/train.jsonl"
DEV_DATA = "./data/dev.jsonl"
OUTPUT_DIR = "./outputs/checkpoint-final"

# ============ 加载模型和 Tokenizer ============
tokenizer = AutoTokenizer.from_pretrained(MODEL_PATH)
model = AutoModelForCausalLM.from_pretrained(MODEL_PATH)

# BarunLM-35M 上下文长度为 2048
tokenizer.pad_token = tokenizer.eos_token
tokenizer.padding_side = "right"

# ============ 加载数据 ============
def format_chat_template(example):
    """将 messages 格式化为模型输入"""
    # BarunLM-35M 是 base model，需要按续写方式 prompt
    prompt = ""
    for msg in example["messages"]:
        if msg["role"] == "system":
            prompt += f"System: {msg['content']}\n"
        elif msg["role"] == "user":
            prompt += f"User: {msg['content']}\n"
        elif msg["role"] == "assistant":
            prompt += f"Assistant: {msg['content']}"
    return {"text": prompt}

def tokenize_function(examples):
    """Response-only SFT: 只对 assistant 部分计算损失"""
    # 简化实现：完整序列训练
    return tokenizer(
        examples["text"],
        truncation=True,
        max_length=2048,
        padding=False,
    )

# 加载数据集
dataset = load_dataset("json", data_files={"train": TRAIN_DATA, "dev": DEV_DATA})
dataset = dataset.map(format_chat_template)
dataset = dataset.map(tokenize_function, batched=True, remove_columns=["messages", "text"])

# ============ 训练配置 ============
# 参考 BarunAction-35M 的超参数
training_args = TrainingArguments(
    output_dir=OUTPUT_DIR,
    overwrite_output_dir=True,
    
    # 训练步数：7,937 / 63 ≈ 126 步
    num_train_epochs=1,
    per_device_train_batch_size=8,
    per_device_eval_batch_size=8,
    gradient_accumulation_steps=8,  # 有效 batch size = 8 * 8 = 64（接近 63）
    
    # 优化器：Muon（需额外安装）或使用 AdamW 替代
    # 若用 Muon: pip install muon
    # optim="muon" if available else "adamw_torch",
    optim="adamw_torch",
    learning_rate=1e-4,  # 预训练阶段峰值 LR
    weight_decay=0.1,    # 预训练阶段 weight decay
    
    # 评估
    eval_strategy="steps",
    eval_steps=20,
    save_steps=20,
    save_total_limit=2,
    load_best_model_at_end=True,
    metric_for_best_model="eval_loss",
    
    # 日志
    logging_steps=5,
    report_to="none",
    
    # 随机种子
    seed=17,
    
    # 硬件
    bf16=True if torch.cuda.is_available() else False,
    dataloader_num_workers=4,
)

# ============ Trainer ============
trainer = Trainer(
    model=model,
    args=training_args,
    train_dataset=dataset["train"],
    eval_dataset=dataset["dev"],
    tokenizer=tokenizer,
)

# ============ 开始训练 ============
trainer.train()
trainer.save_model(OUTPUT_DIR)
tokenizer.save_pretrained(OUTPUT_DIR)

print(f"✅ 训练完成！模型保存在: {OUTPUT_DIR}")
```

### Step 5: 一键训练脚本（`scripts/run_train.sh`）

```bash
#!/bin/bash
cd ~/barun-router-training

# 激活环境
source .venv/bin/activate

# 运行训练
python src/train.py 2>&1 | tee training.log

# 验证模型
python -c "
from transformers import AutoModelForCausalLM, AutoTokenizer
model = AutoModelForCausalLM.from_pretrained('./outputs/checkpoint-final')
tokenizer = AutoTokenizer.from_pretrained('./outputs/checkpoint-final')
print('✅ 模型加载成功！')
"
```

```bash
chmod +x scripts/run_train.sh
./scripts/run_train.sh
```

### Step 6: 推理验证

训练完成后，用以下代码测试模型的中文路由能力：

```python
from transformers import AutoModelForCausalLM, AutoTokenizer

model_path = "./outputs/checkpoint-final"
tokenizer = AutoTokenizer.from_pretrained(model_path)
model = AutoModelForCausalLM.from_pretrained(model_path)

prompt = """System: You are a browser agent that converts natural language requests into structured actions.
User: 帮我打开百度首页
Assistant:"""

inputs = tokenizer(prompt, return_tensors="pt")
outputs = model.generate(
    **inputs,
    max_new_tokens=128,
    temperature=0,  # 确定性解码
    do_sample=False,
)
response = tokenizer.decode(outputs[0], skip_special_tokens=True)
print(response)
```

---

## 📋 关键超参数总结

| 参数 | 值 | 来源 |
|------|-----|------|
| 基础模型 | `harrrshall/BarunLM-35M` |  |
| 微调方式 | 全参数、Response-only SFT |  |
| Epoch | 1 |  |
| 训练样本数 | 7,937 |  |
| 有效 Batch Size | 63 |  |
| 优化步数 | 126 |  |
| 优化器 | Muon（或 AdamW） |  |
| 学习率 | 1e-4 |  |
| Weight Decay | 0.1 |  |
| 上下文长度 | 2,048 |  |
| 随机种子 | 17 |  |
| 解码方式 | 确定性 Greedy |  |

---

## ⚠️ 注意事项

1. **BarunLM-35M 是 Base Model**：没有经过指令微调，需要按**续写模型**的方式构造 prompt。

2. **Response-only SFT**：训练时只对 assistant 部分的输出计算损失，system 和 user 部分不参与 loss 计算。上述简化脚本中为完整序列训练，生产环境建议实现 `ignore_index` 机制。

3. **数据量适配**：如果您的数据集规模与 7,937 不同，需要相应调整 `gradient_accumulation_steps` 以保持有效 batch size ≈ 63。

4. **Muon 优化器**：BarunLM 预训练使用 Muon 优化器，如需使用可安装 `pip install muon` 并在 `TrainingArguments` 中设置 `optim="muon"`。

5. **硬件要求**：原模型在单张 H200 上完成训练，35M 参数量极小，消费级 GPU（如 RTX 3090/4090）完全可以胜任。

6. **中文适配**：由于 BarunLM-35M 预训练数据以英文为主，建议在数据集中**增加中文样本的多样性**和**覆盖度**，以弥补预训练阶段中文能力的不足。
