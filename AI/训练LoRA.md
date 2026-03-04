# 训练一个中文古诗词生成 LoRA

我们将使用 Hugging Face Transformers 和 PEFT 库，以预训练的中文 GPT-2 模型为基础，在古诗词数据集上进行低秩微调，使其能够生成符合格律和风格的古诗。

---

## 1. 环境准备

安装必要的 Python 库：

```bash
pip install transformers datasets peft accelerate torch
```

如果希望使用更快的训练（如支持 DeepSpeed 或 FSDP），可额外安装 `deepspeed`，但本示例不涉及。

---

## 2. 数据准备

我们使用开源的中文古诗词数据集，例如 [chinese-poetry](https://github.com/chinese-poetry/chinese-poetry) 或 Hugging Face 上的 [shibing624/poetry](https://huggingface.co/datasets/shibing624/poetry)。这里以 `shibing624/poetry` 为例，该数据集包含唐诗、宋词等，每首诗词有标题、作者和内容。

为了简化，我们只使用诗词内容（纯文本），并将每首诗作为一个训练样本。模型将通过自回归方式学习生成诗句。

```python
from datasets import load_dataset

dataset = load_dataset("shibing624/poetry", split="train")  # 约 7.7 万首
print(dataset[0]["content"])  # 查看一首诗的内容
```

数据清洗：诗词内容可能包含空格、换行，我们将其标准化为一行文本，诗句之间用逗号或句号分隔。但 GPT 训练通常保留换行作为文本结构的一部分，我们保持原样即可。

为了加快训练，可以只取部分数据（例如前 1 万首），或者全部使用。

---

## 3. 加载基础模型和分词器

选择一个适合中文的预训练语言模型。推荐使用 **`uer/gpt2-chinese-poem`**，它是一个专门在古诗词上继续预训练的 GPT-2 模型，生成效果较好。如果没有，也可以使用通用的中文 GPT-2 如 `uer/gpt2-chinese-cluecorpussmall`。

```python
from transformers import AutoTokenizer, AutoModelForCausalLM

model_name = "uer/gpt2-chinese-poem"  # 或 "uer/gpt2-chinese-cluecorpussmall"
tokenizer = AutoTokenizer.from_pretrained(model_name)
model = AutoModelForCausalLM.from_pretrained(model_name)

# GPT-2 分词器默认没有 pad_token，设置 eos_token 作为 pad_token
if tokenizer.pad_token is None:
    tokenizer.pad_token = tokenizer.eos_token
```

---

## 4. 配置 LoRA

使用 PEFT 库的 `LoraConfig`。我们需要指定要应用 LoRA 的目标模块。对于 GPT-2，注意力层的参数名称通常是 `c_attn`（包含 Q、K、V）和 `c_proj`（输出投影）。但不同模型命名可能不同，需查看模型结构。`uer/gpt2-chinese-poem` 基于 GPT-2 架构，模块名如下：

- `attn.c_attn`：合并的 Q、K、V 投影（in_proj）
- `attn.c_proj`：输出投影

通常 LoRA 只应用到 `c_attn` 就足够。也可以扩展到所有线性层。

```python
from peft import LoraConfig, get_peft_model, TaskType

lora_config = LoraConfig(
    task_type=TaskType.CAUSAL_LM,        # 因果语言建模
    r=8,                                  # 秩
    lora_alpha=32,                        # 缩放因子
    lora_dropout=0.1,                      # Dropout
    target_modules=["c_attn"],             # GPT-2 的注意力合并投影
)

peft_model = get_peft_model(model, lora_config)
peft_model.print_trainable_parameters()
# 输出示例: trainable params: 294,912 || all params: 102,471,936 || trainable%: 0.2879
```

若想更精确控制，可以指定多个模块，如 `["c_attn", "c_proj"]`。

---

## 5. 数据预处理

将诗词文本分词并整理为适合因果语言建模的格式。我们使用 `tokenizer` 将文本转换为 input_ids，并构建 labels（与 input_ids 相同，表示模型需预测下一个 token）。同时使用 `padding` 和 `truncation`。

```python
def preprocess_function(examples):
    # 假设每个样本的 "content" 字段是诗词内容
    texts = examples["content"]
    # 在文本末尾添加 eos_token，表示结束
    texts = [text + tokenizer.eos_token for text in texts]
    tokenized = tokenizer(
        texts,
        truncation=True,
        max_length=512,          # 根据显存调整
        padding="max_length",
        return_tensors=None,
    )
    tokenized["labels"] = tokenized["input_ids"].copy()
    return tokenized

tokenized_dataset = dataset.map(
    preprocess_function,
    batched=True,
    remove_columns=dataset.column_names,
)

# 划分训练集和验证集（可选）
split_dataset = tokenized_dataset.train_test_split(test_size=0.01, seed=42)
train_dataset = split_dataset["train"]
eval_dataset = split_dataset["test"]
```

---

## 6. 训练参数与训练器

使用 Transformers 的 `Trainer` 简化训练过程。需设置 `TrainingArguments`。

```python
from transformers import Trainer, TrainingArguments, DataCollatorForLanguageModeling

# 数据收集器：动态 padding 或使用已 padding 的数据
data_collator = DataCollatorForLanguageModeling(
    tokenizer=tokenizer,
    mlm=False,                    # 非掩码语言建模，即因果LM
)

training_args = TrainingArguments(
    output_dir="./poetry-lora",
    overwrite_output_dir=True,
    num_train_epochs=3,
    per_device_train_batch_size=4,
    per_device_eval_batch_size=4,
    gradient_accumulation_steps=4,   # 若显存小，可增大此值
    learning_rate=2e-4,
    warmup_steps=100,
    logging_steps=50,
    evaluation_strategy="steps",
    eval_steps=500,
    save_steps=500,
    load_best_model_at_end=True,
    save_total_limit=2,
    fp16=True,                       # 如果 GPU 支持半精度
    remove_unused_columns=False,
)

trainer = Trainer(
    model=peft_model,
    args=training_args,
    train_dataset=train_dataset,
    eval_dataset=eval_dataset,
    data_collator=data_collator,
    tokenizer=tokenizer,
)

# 开始训练
trainer.train()

# 保存 LoRA 权重
peft_model.save_pretrained("./poetry-lora-final")
tokenizer.save_pretrained("./poetry-lora-final")
```

---

## 7. 加载 LoRA 进行推理

训练完成后，可以使用以下方式加载微调后的模型并生成古诗。

```python
from peft import PeftModel
import torch

# 加载基础模型
base_model = AutoModelForCausalLM.from_pretrained("uer/gpt2-chinese-poem")
tokenizer = AutoTokenizer.from_pretrained("uer/gpt2-chinese-poem")

# 加载 LoRA 权重
lora_model = PeftModel.from_pretrained(base_model, "./poetry-lora-final")
lora_model.eval()
lora_model.to("cuda")

# 生成函数
def generate_poem(prompt, max_length=64, temperature=0.8):
    inputs = tokenizer(prompt, return_tensors="pt").to("cuda")
    with torch.no_grad():
        outputs = lora_model.generate(
            **inputs,
            max_new_tokens=max_length,
            temperature=temperature,
            do_sample=True,
            top_p=0.9,
            repetition_penalty=1.2,
            eos_token_id=tokenizer.eos_token_id,
        )
    return tokenizer.decode(outputs[0], skip_special_tokens=True)

# 测试
print(generate_poem("床前明月光，"))
```

---

## 8. 注意事项与调优建议

- **目标模块**：不同模型的结构命名可能不同，请先打印模型结构 `print(model)` 确认线性层名称。例如，某些模型使用 `q_proj`、`v_proj` 等。
- **秩 r**：对于诗歌生成，r=8 通常足够；若想增强表达能力可设为 16 或 32。
- **学习率**：LoRA 的学习率一般比全量微调高，2e-4 ~ 5e-4 是常见范围。
- **数据量**：如果有更多古诗词数据（如数十万首），效果会更好。
- **文本格式**：训练时保持每首诗作为独立文本，并添加 eos_token，模型将学会在诗结束后停止。
- **生成参数**：生成时可调整 `temperature`（温度）控制随机性，`repetition_penalty` 避免重复。

---

## 9. 完整代码汇总

将以上步骤整合为一个 Python 脚本 `train_poetry_lora.py`：

```python
# train_poetry_lora.py
import torch
from datasets import load_dataset
from transformers import (
    AutoTokenizer,
    AutoModelForCausalLM,
    Trainer,
    TrainingArguments,
    DataCollatorForLanguageModeling,
)
from peft import LoraConfig, get_peft_model, TaskType, PeftModel

# 1. 加载数据集
dataset = load_dataset("shibing624/poetry", split="train")
# 可选：取子集加快测试
# dataset = dataset.select(range(5000))

# 2. 加载模型和分词器
model_name = "uer/gpt2-chinese-poem"
tokenizer = AutoTokenizer.from_pretrained(model_name)
if tokenizer.pad_token is None:
    tokenizer.pad_token = tokenizer.eos_token

model = AutoModelForCausalLM.from_pretrained(model_name)

# 3. 配置 LoRA
lora_config = LoraConfig(
    task_type=TaskType.CAUSAL_LM,
    r=8,
    lora_alpha=32,
    lora_dropout=0.1,
    target_modules=["c_attn"],   # GPT-2 的注意力合并层
)
peft_model = get_peft_model(model, lora_config)
peft_model.print_trainable_parameters()

# 4. 数据预处理
def preprocess(examples):
    texts = [text + tokenizer.eos_token for text in examples["content"]]
    tokenized = tokenizer(
        texts,
        truncation=True,
        max_length=256,
        padding="max_length",
    )
    tokenized["labels"] = tokenized["input_ids"].copy()
    return tokenized

tokenized_dataset = dataset.map(preprocess, batched=True, remove_columns=dataset.column_names)
split_dataset = tokenized_dataset.train_test_split(test_size=0.01, seed=42)

# 5. 训练参数
training_args = TrainingArguments(
    output_dir="./poetry-lora",
    num_train_epochs=3,
    per_device_train_batch_size=4,
    gradient_accumulation_steps=4,
    learning_rate=2e-4,
    warmup_steps=100,
    logging_steps=50,
    evaluation_strategy="steps",
    eval_steps=500,
    save_steps=500,
    load_best_model_at_end=True,
    save_total_limit=2,
    fp16=True,
    remove_unused_columns=False,
)

data_collator = DataCollatorForLanguageModeling(tokenizer, mlm=False)

trainer = Trainer(
    model=peft_model,
    args=training_args,
    train_dataset=split_dataset["train"],
    eval_dataset=split_dataset["test"],
    data_collator=data_collator,
    tokenizer=tokenizer,
)

# 6. 训练并保存
trainer.train()
peft_model.save_pretrained("./poetry-lora-final")
tokenizer.save_pretrained("./poetry-lora-final")

# 7. 推理示例（可选）
base_model = AutoModelForCausalLM.from_pretrained(model_name)
lora_model = PeftModel.from_pretrained(base_model, "./poetry-lora-final")
lora_model.to("cuda")
lora_model.eval()

prompt = "床前明月光，"
inputs = tokenizer(prompt, return_tensors="pt").to("cuda")
outputs = lora_model.generate(
    **inputs,
    max_new_tokens=64,
    temperature=0.8,
    do_sample=True,
    top_p=0.9,
    repetition_penalty=1.2,
)
print(tokenizer.decode(outputs[0], skip_special_tokens=True))
```

运行脚本：
```bash
python train_poetry_lora.py
```

---

## 10. 可能遇到的问题及解决方案

- **显存不足**：减小 `per_device_train_batch_size`，增大 `gradient_accumulation_steps`；或降低 `max_length`。
- **生成内容重复**：调高 `repetition_penalty`（如 1.2～1.5）或降低 `temperature`。
- **不押韵或格律错误**：基础模型本身可能不具备强格律知识，可使用更专业的诗词预训练模型，或在数据中增加格律标注。
- **LoRA 目标模块名称错误**：打印模型结构，找到对应线性层名称。例如对于 LLaMA 架构，通常是 `q_proj`, `v_proj` 等。

通过上述流程，你可以在约 1-2 小时（根据数据量和硬件）内得到一个能够生成中文古诗词的 LoRA 模型。祝你训练顺利！
