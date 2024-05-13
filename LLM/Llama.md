## 📌 如何使用Llama模型?


你可以选择下面的快速上手的任一种方式，开始使用 Llama 系列模型。推荐使用[中文预训练对话模型](#llama2中文预训练模型atom-7b)进行使用，对中文的效果支持更好。


### 快速上手-使用Anaconda

第 0 步：前提条件
- 确保安装了 Python 3.10 以上版本。

第 1 步：准备环境

如需设置环境，安装所需要的软件包，运行下面的命令。
```bash
git clone https://github.com/LlamaFamily/Llama-Chinese.git
cd Llama-Chinese
pip install -r requirements.txt
```

第 2 步：下载模型

你可以从以下来源下载Atom-7B-Chat模型。
- [HuggingFace](https://huggingface.co/FlagAlpha)
- [ModelScope](https://modelscope.cn/organization/FlagAlpha)
- [WideModel](https://wisemodel.cn/models/FlagAlpha/Atom-7B-Chat)

第 3 步：进行推理

使用Atom-7B-Chat模型进行推理
创建一个名为 quick_start.py 的文件，并将以下内容复制到该文件中。
```python
import torch
from transformers import AutoTokenizer, AutoModelForCausalLM
device_map = "cuda:0" if torch.cuda.is_available() else "auto"
model = AutoModelForCausalLM.from_pretrained('FlagAlpha/Atom-7B-Chat',device_map=device_map,torch_dtype=torch.float16,load_in_8bit=True,trust_remote_code=True,use_flash_attention_2=True)
model =model.eval()
tokenizer = AutoTokenizer.from_pretrained('FlagAlpha/Atom-7B-Chat',use_fast=False)
tokenizer.pad_token = tokenizer.eos_token
input_ids = tokenizer(['<s>Human: 介绍一下中国\n</s><s>Assistant: '], return_tensors="pt",add_special_tokens=False).input_ids
if torch.cuda.is_available():
  input_ids = input_ids.to('cuda')
generate_input = {
    "input_ids":input_ids,
    "max_new_tokens":512,
    "do_sample":True,
    "top_k":50,
    "top_p":0.95,
    "temperature":0.3,
    "repetition_penalty":1.3,
    "eos_token_id":tokenizer.eos_token_id,
    "bos_token_id":tokenizer.bos_token_id,
    "pad_token_id":tokenizer.pad_token_id
}
generate_ids  = model.generate(**generate_input)
text = tokenizer.decode(generate_ids[0])
print(text)
```

运行 quick_start.py 代码。
```bash
python quick_start.py
```

### 快速上手-使用Docker

详情参见：[Docker部署](https://github.com/LlamaFamily/Llama-Chinese/blob/main/docs/chat_gradio_guide.md)

第 1 步：准备docker镜像，通过docker容器启动[chat_gradio.py](../examples/chat_gradio.py)
```bash
git clone https://github.com/LlamaFamily/Llama-Chinese.git

cd Llama-Chinese

docker build -f docker/Dockerfile -t flagalpha/llama2-chinese:gradio .
```

第 2 步：通过docker-compose启动chat_gradio
```bash
cd Llama-Chinese/docker
docker-compose up -d --build
```

### 快速上手-使用llama.cpp
详情参见：[使用llama.cpp](https://github.com/LlamaFamily/Llama-Chinese/blob/main/inference-speed/CPU/ggml/README.md)

### 快速上手-使用gradio
基于gradio搭建的问答界面，实现了流式的输出，将下面代码复制到控制台运行，以下代码以Atom-7B-Chat模型为例，不同模型只需修改一下面的model_name_or_path对应的模型名称就好了😊
```
python examples/chat_gradio.py --model_name_or_path FlagAlpha/Atom-7B-Chat
```

### 快速上手-构建API服务
使用FastChat构建和OpenAI一致的推理服务接口。

<details>
第 0 步：前提条件

安装fastchat
```bash
pip3 install "fschat[model_worker,webui]"
```
第 1 步：启动Restful API

开启三个控制台分别执行下面的三个命令
- 首先启动controler
```bash
python3 -m fastchat.serve.controller \
--host localhost \
--port 21001
```

- 启动模型
```bash
CUDA_VISIBLE_DEVICES="0" python3 -m fastchat.serve.model_worker --model-path /path/Atom-7B-Chat \
--host localhost \
--port 21002 \
--worker-address "http://localhost:21002" \
--limit-worker-concurrency 5 \
--stream-interval 2 \
--gpus "1" \
--load-8bit
```

- 启动RESTful API 服务
```bash
python3 -m fastchat.serve.openai_api_server \
--host localhost \
--port 21003 \
--controller-address http://localhost:21001
```

第 2 步：测试api服务

执行下面的python代码测试上面部署的api服务
```python
# coding=utf-8
import json
import time
import urllib.request
import sys
import requests

def test_api_server(input_text):
    header = {'Content-Type': 'application/json'}

    data = {
          "messages": [{"role": "system", "content": ""}, {"role": "user", "content": input_text}],
          "temperature": 0.3, 
          "top_p" : 0.95, 
          "max_tokens": 512, 
          "model": "LLama2-Chinese-13B",
          "stream" : False,
          "n" : 1,
          "best_of": 1, 
          "presence_penalty": 1.2, 
          "frequency_penalty": 0.2,           
          "top_k": 50, 
          "use_beam_search": False, 
          "stop": [], 
          "ignore_eos" :False,
          "logprobs": None
    }
    response = requests.post(
        url='http://127.0.0.1:21003/v1/chat/completions',
        headers=header,
        data=json.dumps(data).encode('utf-8')
    )

    result = None
    try:
        result = json.loads(response.content)
        print(json.dumps(data, ensure_ascii=False, indent=2))
        print(json.dumps(result, ensure_ascii=False, indent=2))

    except Exception as e:
        print(e)

    return result

if __name__ == "__main__":
    test_api_server("如何去北京?")
```

</details>

## Llama.cpp

https://github.com/ggerganov/llama.cpp/blob/master/examples/main/README.md#common-options

Llama.cpp提供的 main工具允许你以简单有效的方式使用各种 LLaMA 语言模型。 它专门设计用于与 llama.cpp 项目配合使用。

Llama.cpp的工具 main提供简单的 C/C++ 实现，具有可选的 4 位量化支持，可实现更快、更低的内存推理，并针对桌面 CPU 进行了优化。 该程序可用于使用 LLaMA 模型执行各种推理任务，包括根据用户提供的提示生成文本以及使用反向提示进行类似聊天的交互。

```shell
.\main.exe -m .\Llama3-xxx.GGUF --interactive-first
```

### 1、快速开始

要立即开始，请运行以下命令，确保使用你拥有的模型的正确路径：

基于 Unix 的系统（Linux、macOS 等）：

./main -m models/7B/ggml-model.bin --prompt "Once upon a time"
Windows：

main.exe -m models\7B\ggml-model.bin --prompt "Once upon a time"
要获得交互式体验，请尝试以下命令：

基于Unix的系统（Linux、MacOS等）：

./main -m models/7B/ggml-model.bin -n -1 --color -r "User:" --in-prefix " " -i -p \
'User: Hi
AI: Hello. I am an AI chatbot. Would you like to talk?
User: Sure!
AI: What would you like to talk about?
User:'
Windows：

main.exe -m models\7B\ggml-model.bin -n -1 --color -r "User:" --in-prefix " " -i -e -p "User: Hi\nAI: Hello. I am an AI chatbot. Would you like to talk?\nUser: Sure!\nAI: What would you like to talk about?\nUser:"
以下命令从启动提示生成“无限”文本（可以使用 Ctrl-C 来停止它）：

基于Unix的系统（Linux、MacOS等）：

./main -m models/7B/ggml-model.bin --ignore-eos -n -1 --random-prompt
Windows：

main.exe -m models\7B\ggml-model.bin --ignore-eos -n -1 --random-prompt

### 2、常用选项
在本节中，我们将介绍使用 LLaMA 模型运行主程序的最常用选项：

-m FNAME, --model FNAME：指定 LLaMA 模型文件的路径，例如  models/7B/ggml-model.bin。
-i, --interactive：以交互模式运行程序，允许你直接提供输入并接收实时响应。
-ins, --instruct：以指令模式运行程序，这在处理Alpaca模型时特别有用。
-n N, --n-predict N：设置生成文本时要预测的标记数量。 调整此值可以影响生成文本的长度。
-c N, --ctx-size N：设置提示上下文的大小。 默认值为 512，但 LLaMA 模型是在 2048 的上下文中构建的，这将为较长的输入/推理提供更好的结果。

### 3、输入提示

Llama.cpp的main程序提供了多种使用输入提示与 LLaMA 模型交互的方法：

--prompt PROMPT：直接提供提示作为命令行选项。
--file FNAME：提供包含一个或多个提示的文件。
--interactive-first：以交互模式运行程序并立即等待输入。 下面有更多相关内容。
--random-prompt：以随机提示开始。

### 4、与模型交互

Llama.cpp的 main程序提供了与 LLaMA 模型交互的无缝方式，允许用户参与实时对话或提供特定任务的说明。 可以使用各种选项触发交互模式，包括 --interactive、 --interactive-first 和 --instruct。

在交互模式下，用户可以通过在过程中注入输入来参与文本生成。 用户可以随时按  Ctrl+C 插入并键入输入，然后按 Return 将其提交到 LLaMA 模型。 要提交其他行而不最终确定输入，用户可以使用反斜杠 ( \) 结束当前行并继续输入。

### 4.1 交互选项

-i, --interactive：以交互模式运行程序，允许用户进行实时对话或向模型提供特定指令。
--interactive-first：以交互模式运行程序，并在开始文本生成之前立即等待用户输入。
-ins, --instruct：以指令模式运行程序，该模式专门设计用于擅长根据用户指令完成任务的 Alpaca 模型。
--color：启用彩色输出，以在视觉上区分提示、用户输入和生成的文本。
通过理解和利用这些交互选项，你可以使用 LLaMA 模型创建引人入胜的动态体验，根据你的特定需求定制文本生成过程。

### 4.2 反向提示

反向提示是一种通过在遇到特定文本字符串时暂停文本生成来使用 LLaMA 模型创建类似聊天体验的强大方法：

-r PROMPT, --reverse-prompt PROMPT：指定一个或多个反向提示以暂停文本生成并切换到交互模式。 例如，每当轮到用户说话时，-r“User:”可用于跳回对话。 这有助于创造更具互动性和对话性的体验。 但是，当以空格结尾时，反向提示不起作用。
要克服此限制，可以使用 --in-prefix 标志在反向提示符后添加空格或任何其他字符。

### 4.3 In-Prefix

--in-prefix 标志用于向输入添加前缀，主要用于在反向提示后插入空格。 以下是如何将 --in-prefix 标志与 --reverse-prompt 标志结合使用的示例：

./main -r "User:" --in-prefix " "

### 4.4  In-Suffix

--in-suffix 标志用于在输入后添加后缀。 这对于在用户输入后添加“Assistant:”提示非常有用。 它自动添加到用户输入末尾的换行符 ( \n) 之后。 以下是如何将 --in-suffix 标志与 --reverse-prompt 标志结合使用的示例：

./main -r "User:" --in-prefix " " --in-suffix "Assistant:"

### 4.5 指令模式

指令模式在使用 Alpaca 模型时特别有用，这些模型旨在遵循用户指令来执行特定任务：

-ins、--instruct：启用指令模式，以利用 Alpaca 模型的功能，根据用户提供的指令完成任务。
技术细节：用户的输入在内部以反向提示符为前缀（或 默认 ### instruction：），后跟 ### Response:（除非你在没有任何输入的情况下按 Return 键，以继续生成更长的响应 ）。

通过理解和利用这些交互选项，可以使用 LLaMA 模型创建引人入胜的动态体验，根据你的特定需求定制文本生成过程。

### 5、上下文管理
在文本生成过程中，LLaMA 模型的上下文大小有限，这意味着它们只能考虑输入和生成文本中的一定数量的标记。 当上下文填满时，模型会在内部重置，可能会丢失对话或指令开始时的一些信息。 上下文管理选项有助于在这些情况下保持连续性和连贯性。

### 5.1 上下文大小
--ctx-size 选项允许你设置 LLaMA 模型在文本生成过程中使用的提示上下文的大小。 较大的上下文大小有助于模型更好地理解和生成较长输入或对话的响应。

-c N, --ctx-size N：设置提示上下文的大小（默认值：512）。 LLaMA 模型是在 2048 上下文构建的，这将在较长的输入/推理上产生最佳结果。 然而，将上下文大小增加到超过 2048 可能会导致不可预测的结果。
### 5.2 扩展的上下文大小
一些微调模型通过缩放 RoPE 来延长上下文长度。 例如，如果原始预训练模型的上下文长度（最大序列长度）为 4096 (4k)，而微调模型的上下文长度为 32k。 这是一个 8 的缩放因子，应该通过将上面的 --ctx-size 设置为 32768 (32k) 并将 --rope-scale 设置为 8 来工作。

--rope-scale N：其中 N 是微调模型使用的线性缩放因子。
### 5.3 保持提示
--keep 选项允许用户在模型运行脱离上下文时保留原始提示，确保保持与初始指令或对话主题的连接。

--keep N：指定模型重置其内部上下文时要保留的初始提示中的标记数量。 默认情况下，该值设置为 0（意味着不保留令牌）。 使用 -1 保留初始提示中的所有标记。
通过利用 --ctx-size 和 --keep 等上下文管理选项，你可以与 LLaMA 模型保持更加连贯一致的交互，确保生成的文本与原始提示或对话保持相关。

### 6、生成相关选项
以下选项允许你控制文本生成过程，并根据你的需要微调生成文本的多样性、创造力和质量。 通过调整这些选项并尝试不同的值组合，你可以找到适合你的特定用例的最佳设置。

### 6.1 要预测的令牌数量
-n N, --n-predict N：设置生成文本时要预测的令牌数量（默认值：128，-1 = 无穷大，-2 = 直到上下文填充）
--n-predict 选项控制模型响应输入提示生成的标记数量。 通过调整此值，你可以影响生成文本的长度。 较高的值将生成较长的文本，而较低的值将生成较短的文本。

即使我们有有限的上下文窗口， -1 的值也将启用无限文本生成。 当上下文窗口已满时，一些较早的令牌（ --n-keep之后的令牌的一半）将被丢弃。 然后必须重新评估上下文，然后才能恢复生成。 在大型模型和/或大型上下文窗口上，这将导致输出显著暂停。

如果不希望出现暂停，则值 -2 将在上下文填满时立即停止生成。

需要注意的是，如果遇到序列结束 ( EOS) 标记或反向提示，生成的文本可能会短于指定的令牌数量。 在交互模式下，文本生成将暂停，控制权将返回给用户。 在非交互模式下，程序将结束。 在这两种情况下，文本生成可能会在达到指定的 n 预测值之前停止。 如果你希望模型继续运行而不自行产生序列结束，则可以使用 --ignore-eos 参数。

6.2 温度
--temp N：调整生成文本的随机性（默认值：0.8）。
温度是一个超参数，控制生成文本的随机性。 它影响模型输出标记的概率分布。 较高的温度（例如 1.5）使输出更加随机和创造性，而较低的温度（例如 0.5）使输出更加集中、确定性和保守。 默认值为 0.8，它提供了随机性和确定性之间的平衡。 在极端情况下，温度为 0 时将始终选择最有可能的下一个标记，从而在每次运行中产生相同的输出。

用法示例： --temp 0.5

6.3 重复惩罚
--repeat-penalty N：控制生成文本中标记序列的重复（默认值：1.1）。
--repeat-last-n N：用于惩罚重复的最后 n 个标记（默认值：64，0 = 禁用，-1 = ctx-size）。
--no-penalize-nl：应用重复惩罚时禁用换行标记的惩罚。
重复惩罚选项有助于防止模型生成重复或单调的文本。 较高的值（例如，1.5）将对重复进行更严厉的惩罚，而较低的值（例如，0.9）将更宽松。 默认值为 1.1。

Repeat-last-n 选项控制历史记录中要考虑惩罚重复的标记数量。 较大的值将在生成的文本中进一步查找以防止重复，而较小的值将仅考虑最近的标记。 值为 0 会禁用惩罚，值为 -1 会将标记数量设置为等于上下文大小 (ctx-size)。

使用 --no-penalize-nl 选项在应用重复惩罚时禁用换行惩罚。 此选项对于生成聊天对话、对话、代码、诗歌或换行符在结构和格式中发挥重要作用的任何文本特别有用。 禁用换行惩罚有助于在这些特定用例中保持自然流程和预期格式。

用法示例： --repeat-penalty 1.15 --repeat-last-n 128 --no-penalize-nl

6.4 Top-K 采样
--top-k N：将下一个标记选择限制为 K 个最可能的标记（默认值：40）。
Top-k 采样是一种文本生成方法，仅从模型预测的前 k 个最有可能的标记中选择下一个标记。 它有助于降低生成低概率或无意义令牌的风险，但也可能限制输出的多样性。 top-k 的较高值（例如 100）将考虑更多标记并导致更多样化的文本，而较低值（例如 10）将关注最可能的标记并生成更保守的文本。 默认值为 40。

用法示例： --top-k 30

6.5 Top-P 采样
--top-p N：将下一个标记选择限制为累积概率高于阈值 P（默认值：0.9）的标记子集。
Top-p 采样，也称为核心采样，是另一种文本生成方法，它从累积概率至少为 p 的标记子集中选择下一个标记。 该方法通过考虑令牌的概率和采样的令牌数量来提供多样性和质量之间的平衡。 top-p 的较高值（例如 0.95）将导致文本更加多样化，而较低的值（例如 0.5）将生成更加集中和保守的文本。 默认值为 0.9。

用法示例： --top-p 0.95

6.6 无尾采样 (TFS)
--tfs N：使用参数 z 启用无尾采样（默认值：1.0，1.0 = 禁用）。
无尾采样 (TFS) 是一种文本生成技术，旨在减少不太可能的标记对输出的影响，这些标记可能不太相关、不太连贯或无意义。 与 Top-P 类似，它尝试动态确定大部分最可能的标记。 但 TFS 根据概率的二阶导数过滤掉 logits。 当二阶导数之和达到参数 z 后，停止添加标记。 简而言之：TFS 查看令牌的概率下降的速度，并使用参数 z 切断不太可能的令牌的尾部。 z 的典型值在 0.9 至 0.95 范围内。 值 1.0 将包括所有令牌，从而禁用 TFS 的效果。

使用示例： --tfs 0.95

6.6 局部典型抽样
--typical N：使用参数 p 启用本地典型采样（默认值：1.0，1.0 = 禁用）。
局部典型采样通过对基于周围上下文的典型或预期的标记进行采样，促进上下文连贯且多样化的文本的生成。 通过将参数 p 设置在 0 和 1 之间，你可以控制生成局部连贯和多样化的文本之间的平衡。 接近 1 的值将促进上下文更加一致的标记，而接近 0 的值将促进更加多样化的标记。 等于 1 的值将禁用本地典型采样。

使用示例： --typical 0.9

6.7 Microstat采样
--mirostat N：启用 Mirostat 采样，控制文本生成过程中的困惑（默认值：0，0 = 禁用，1 = Mirostat，2 = Mirostat 2.0）。
--mirostat-lr N：设置Mirostat学习率，参数eta（默认：0.1）。
--mirostat-ent N：设置Mirostat目标熵，参数tau（默认值：5.0）。
Mirostat 是一种在文本生成过程中主动将生成文本的质量保持在所需范围内的算法。 它的目的是在连贯性和多样性之间取得平衡，避免由于过度重复（无聊陷阱）或不连贯（混乱陷阱）而导致低质量的输出。

--mirostat-lr 选项设置 Mirostat 学习率 (eta)。 学习率影响算法对生成文本反馈的响应速度。 较低的学习率将导致调整速度较慢，而较高的学习率将使算法更具响应性。 默认值为 0.1。

--mirostat-ent 选项设置 Mirostat 目标熵 (tau)，它表示生成的文本所需的困惑度值。 调整目标熵可以让你控制生成文本的连贯性和多样性之间的平衡。 较低的值将导致文本更加集中和连贯，而较高的值将导致文本更加多样化且可能不太连贯。 默认值为 5.0。

用法示例：  --mirostat 2 --mirostat-lr 0.05 --mirostat-ent 3.0

6.8 Logits偏差
-l TOKEN_ID(+/-)BIAS, --logit-bias TOKEN_ID(+/-)BIAS：修改令牌出现在生成的文本完成中的可能性。
logit 偏差选项允许你手动调整特定标记出现在生成文本中的可能性。 通过提供令牌 ID 以及正偏差值或负偏差值，可以增加或减少生成该令牌的概率。

例如，使用 --logit-bias 15043+1 增加标记“Hello”的可能性，或使用 --logit-bias 15043-1 降低其可能性。 使用负无穷大的值， --logit-bias 15043-inf 确保永远不会生成令牌  Hello。

更实际的用例可能是通过使用  -l 29905-inf 将 \ 令牌 (29905) 设置为负无穷大来防止生成  \code{begin} 和  \code{end}。 这是由于 LLaMA 模型推理中出现的 LaTeX 代码普遍存在。

用法示例： --logit-bias 29905-inf

6.9 RNG种子
-s SEED, --seed SEED：设置随机数生成器 (RNG) 种子（默认值：-1，-1 = 随机种子）。
RNG 种子用于初始化影响文本生成过程的随机数生成器。 通过设置特定的种子值，你可以使用相同的输入和设置在多次运行中获得一致且可重复的结果。 这有助于测试、调试或比较不同选项对生成文本的影响，以了解它们何时出现分歧。 如果种子设置为小于 0 的值，则将使用随机种子，这将导致每次运行产生不同的输出。

7、性能调整和内存选项
这些选项有助于提高 LLaMA 模型的性能和内存使用率。 通过调整这些设置，你可以微调模型的行为，以更好地适应你的系统功能，并为特定用例实现最佳性能。

7.1 线程数
-t N, --threads N：设置计算期间使用的线程数。 为了获得最佳性能，建议将此值设置为系统具有的物理 CPU 核心数（而不是逻辑核心数）。 使用正确数量的线程可以极大地提高性能。
7.2 内存锁定模型
--mlock：将模型锁定在内存中，防止在内存映射时被换出。 这可以提高性能，但会需要更多 RAM 来运行，并且在模型加载到 RAM 时可能会减慢加载时间，从而牺牲了内存映射的一些优势。
7.3 无内存映射
--no-mmap：不要对模型进行内存映射。 默认情况下，模型被映射到内存中，这允许系统根据需要仅加载模型的必要部分。 但是，如果模型大于 RAM 总量，或者系统可用内存不足，则使用 mmap 可能会增加页面调出的风险，从而对性能产生负面影响。 禁用 mmap 会导致加载时间变慢，但如果不使用 --mlock，则可能会减少页面输出。 请注意，如果模型大于 RAM 总量，关闭 mmap 将根本无法加载模型。
7.4 NUMA 支持
--numa：尝试优化，以帮助某些具有非统一内存访问的系统。 目前，这包括将相等比例的线程固定到每个 NUMA 节点上的核心，并禁用 mmap 的预取和预读。 后者会导致映射页面在第一次访问时出现故障，而不是一次性全部出现故障，并且结合将线程固定到 NUMA 节点，更多页面最终出现在使用它们的 NUMA 节点上。
请注意，如果模型已经在系统页面缓存中，例如由于先前运行时没有此选项，则除非你先删除页面缓存，否则这不会产生什么影响。 这可以通过重新启动系统或在 Linux 上以 root 身份将“3”写入 /proc/sys/vm/drop_caches来完成。

7.5 内存浮点32位
--memory-f32：对于内存键+值，使用 32 位浮点数而不是 16 位浮点数。 这使上下文内存要求和缓存的提示文件大小增加了一倍，但似乎并没有以可测量的方式提高生成质量。 不建议。
7.6 批大小
-b N, --batch-size N：设置提示处理的批处理大小（默认值：512）。 这种大的批有利于在构建期间安装并启用 BLAS 的用户。 如果你没有启用 BLAS（“BLAS=0”），则可以使用较小的数字（例如 8）来查看在某些情况下评估时的提示进度。
7.7 提示缓存
--prompt-cache FNAME：指定一个文件来缓存初始提示后的模型状态。 当使用较长的提示时，这可以显着加快启动时间。 该文件在第一次运行期间创建，并在后续运行中重用和更新。 注意：恢复缓存的提示并不意味着恢复会话保存时的确切状态。 因此，即使指定特定种子，也不能保证你获得与原始生成相同的令牌序列。
7.8 语法
--grammar GRAMMAR、--grammar-file FILE：指定语法（内联或在文件中定义）以将模型输出限制为特定格式。 例如，你可以强制模型输出 JSON 或仅使用表情符号说话。 有关语法的详细信息，请参阅 GBNF 指南。
7.8 量化
有关 4 位量化的信息，可以显着提高性能并减少内存使用，请参阅 llama.cpp 的主README。

8、其他选项
这些选项在运行 LLaMA 模型时提供额外的功能和自定义：

-h, --help：显示帮助消息，其中显示所有可用选项及其默认值。 这对于检查最新选项和默认值特别有用，因为它们可能会经常更改，并且本文档中的信息可能会过时。
--verbose-prompt：在生成文本之前打印提示。
--mtest：通过运行一系列测试来测试模型的功能，以确保其正常工作。
-ngl N, --n-gpu-layers N：当使用适当的支持（当前是 CLBlast 或 cuBLAS）进行编译时，此选项允许将某些层卸载到 GPU 进行计算。 通常会提高性能。
-mg i, --main-gpu i：当使用多个 GPU 时，此选项控制哪个 GPU 用于小张量，对于小张量，在所有 GPU 上分割计算的开销是不值得的。 相关 GPU 将使用稍多的 VRAM 来存储临时结果的暂存缓冲区。 默认情况下使用 GPU 0。 需要 cuBLAS。
-ts SPLIT, --tensor-split SPLIT：当使用多个 GPU 时，此选项控制应在所有 GPU 上分割多大的张量。 SPLIT 是一个以逗号分隔的非负值列表，用于指定每个 GPU 应按顺序获取的数据比例。 例如，“3,2”会将 60% 的数据分配给 GPU 0，将 40% 的数据分配给 GPU 1。默认情况下，数据按 VRAM 的比例进行分割，但这可能不是最佳的性能。 需要 cuBLAS。
-lv, --low-vram：不分配 VRAM 暂存缓冲区来保存临时结果。 以牺牲性能（尤其是即时处理速度）为代价来减少 VRAM 使用。 需要 cuBLAS。
--lora FNAME：将 LoRA（低阶适应）适配器应用于模型（意味着 --no-mmap）。 这使你可以使预训练模型适应特定任务或领域。
--lora-base FNAME：可选模型，用作 LoRA 适配器修改的层的基础。 该标志与 --lora 标志结合使用，并指定适应的基本模型。
