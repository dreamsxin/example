# Gradio 教学

1. **Gradio 的使用**：如何搭建一个包含多标签页、文件上传、聊天界面的交互式应用。
2. **Agent 开发流程与关键技术**：如何从零构建一个带文档库的 LLM Agent，系统性地掌握 Prompt、Embedding、Function Calling、上下文压缩管理等核心环节。

## 教学目标概述

最终你会得到一个 **LLM Wiki Agent**，它能：
- 接收你上传的 PDF/TXT 文档，自动分割并建立向量知识库；
- 在对话中调用工具搜索知识库；
- 根据知识库内容，为你生成结构化的 Wiki 页面。

整个开发过程会分成 **6 个教学模块**，每个模块都聚焦一个具体知识点。

---

将模块1的内容拆解为“快速上手 `gr.Interface`”和“用 `gr.Blocks` 搭建灵活布局”两部分，并把常见组件串进去，方便你一次了解全貌。

---

### 1. 快速上手：用 `gr.Interface` 做一个文本反转工具

```python
import gradio as gr

def reverse_text(text):
    return text[::-1]

demo = gr.Interface(
    fn=reverse_text,         # 处理函数
    inputs="text",           # 输入组件（简写形式，等价于 gr.Textbox()）
    outputs="text"           # 输出组件
)

demo.launch(share=True)      # share=True 生成公网临时链接，方便分享
```

- `gr.Interface` 是 Gradio 最高层的封装，**适合只有一个输入和一个输出的简单场景**。
- `inputs` 和 `outputs` 可直接写字符串 `"text"`，也可以传入组件实例（如 `gr.Textbox()`）来定制样式和参数。
- 启动后会自动打开浏览器，页面上一个文本框输入，点击提交即显示反转结果。

---

### 2. 为什么还需要 `gr.Blocks`？

`gr.Interface` 虽然简单，但无法实现复杂布局，比如：

- 多个标签页切换
- 并排的多列布局
- 自定义 CSS 样式
- 复杂的多输入多输出交互

这些都需要 `gr.Blocks` 来实现。它像搭积木一样，可以自由放置组件。

---

### 3. 用 `gr.Blocks` 构建多标签页应用（界面骨架）

下面用 `gr.Blocks` + `gr.Tab` 做一个“LLM Wiki Agent”的空壳，提前熟悉组件组织方式。

```python
import gradio as gr

with gr.Blocks(title="LLM Wiki Agent") as demo:
    gr.Markdown("# 📚 LLM Wiki Agent")          # 标题

    with gr.Tab("📁 文档上传"):                 # 第一个标签页
        file_input = gr.File(label="选择 PDF 或 TXT 文件")
        upload_btn = gr.Button("开始处理并入库")
        upload_msg = gr.Textbox(label="处理状态", interactive=False)

    with gr.Tab("💬 Wiki 对话"):                 # 第二个标签页
        chatbot = gr.Chatbot(label="对话记录")
        msg = gr.Textbox(label="输入你的问题或指令")
        clear = gr.Button("清空对话")

        # 事件绑定（暂时用 lambda 占位）
        upload_btn.click(fn=lambda f: "上传功能未实现", inputs=file_input, outputs=upload_msg)
        msg.submit(fn=lambda m, h: ("", h + [[m, "回复功能未实现"]]), inputs=[msg, chatbot], outputs=[msg, chatbot])
        clear.click(fn=lambda: [], outputs=chatbot)

if __name__ == "__main__":
    demo.launch()
```

运行这个脚本，你就会看到两个可以切换的标签页，按钮点击时显示占位文字。**功能尚未真正实现，但骨架已经搭好。**

---

### 4. 骨架中的常用组件解析

| 组件 | 作用 | 关键参数 |
|------|------|----------|
| `gr.Markdown` | 渲染 Markdown 文字，可做标题、说明 | 直接传字符串 |
| `gr.Tab` | 创建标签页，用 `with` 包裹其中的组件 | `label` 指定标签名 |
| `gr.File` | 文件上传，支持单文件或多文件 | `label`, `file_count` |
| `gr.Button` | 按钮，绑定 `click` 事件 | `value` 按钮文字 |
| `gr.Textbox` | 文本输入框，可单行或多行 | `label`, `lines`, `interactive` |
| `gr.Chatbot` | 聊天显示区，接受 `[[user, bot], ...]` 格式的列表 | `label`, `height` |

---

### 5. 更多常用组件速览

除了上面这些，还有大量现成的输入/输出组件：

```python
import gradio as gr

def show_result(text, num, option, img):
    return f"文本：{text}\n数字×2：{num*2}\n选择：{option}", img

with gr.Blocks() as demo:
    gr.Markdown("## 组件展示")
    with gr.Row():                          # Row 将内部组件水平排列
        with gr.Column():
            text = gr.Textbox(label="输入文本")
            num = gr.Slider(1, 10, step=1, label="选择数字")
            option = gr.Dropdown(["A", "B", "C"], label="下拉选项")
            img = gr.Image(label="上传图片", type="pil")
            btn = gr.Button("提交")
        with gr.Column():
            out_text = gr.Textbox(label="输出结果")
            out_img = gr.Image(label="图片预览")
    btn.click(show_result, [text, num, option, img], [out_text, out_img])

demo.launch()
```

一些常用组件说明：

- **`gr.Slider`**：滑块，可设置最小值、最大值、步长。
- **`gr.Dropdown`**：下拉选择，传入选项列表。
- **`gr.Radio` / `gr.Checkbox`**：单选/多选。
- **`gr.Image`**：图片上传与显示，`type="pil"` 会传入 PIL 图像对象。
- **`gr.DataFrame`**：表格数据，处理 pandas DataFrame。
- **`gr.Row` / `gr.Column`**：布局组件，控制横向/纵向排列。

---

### 6. 事件绑定的核心逻辑

无论是 `click`、`submit` 还是 `change`，本质都是：

```
组件.事件类型(fn=处理函数, inputs=[...], outputs=[...])
```

- **`fn`**：当事件发生时被调用的函数，其参数顺序与 `inputs` 中的组件顺序一一对应。
- **`inputs`**：传入的组件或组件列表，函数会收到这些组件的当前值。
- **`outputs`**：接收函数返回值的组件，返回值顺序要与 `outputs` 中的顺序一致。如果只想更新部分组件，其他组件可以返回 `gr.update()` 占位。

---

### 7. 小结

通过模块1，你已经学会：

- 用 `gr.Interface` 快速搭建单功能应用
- 用 `gr.Blocks` 和 `gr.Tab` 构建多标签页复杂布局
- 放置、配置常见组件，并理解事件绑定规则
- 熟悉了 `Textbox`、`File`、`Chatbot`、`Slider`、`Dropdown`、`Image` 等常用组件

后续模块会在此基础上逐步填充真实功能，例如读取 PDF 文本、调用大模型、维护对话历史等。

---

## 模块 2：文档向量化 — 从文件到知识库

### 我们要学什么
- 文档加载（`PyPDFLoader`, `TextLoader`）
- 文本分块（`RecursiveCharacterTextSplitter`）——为什么需要分块？怎样设置块大小和重叠？
- 向量嵌入（`OpenAIEmbeddings`）——什么是 Embedding？为什么用它？
- 向量数据库（`Chroma`）——持久化存储，快速检索。

### 关键概念：为什么需要整个管道？
LLM 的输入长度有限，无法直接“阅读”一整本书；而且直接塞大段文本成本高、准确度低。因此我们需要将文档拆成小块，用 Embedding 模型变成向量（高维空间中的坐标），存入向量数据库。这样当用户提问时，我们可以快速找到语义最相近的文档块。

### 代码 (DocumentProcessor 类)

```python
from langchain.document_loaders import PyPDFLoader, TextLoader
from langchain.text_splitter import RecursiveCharacterTextSplitter
from langchain_openai import OpenAIEmbeddings
from langchain.vectorstores import Chroma
import os

class DocumentProcessor:
    def __init__(self, persist_dir="./chroma_db"):
        self.persist_dir = persist_dir
        self.embeddings = OpenAIEmbeddings(model="text-embedding-3-small")
        # 如果已有向量库就加载，否则后续创建
        if os.path.exists(persist_dir) and os.listdir(persist_dir):
            self.vectorstore = Chroma(
                persist_directory=persist_dir,
                embedding_function=self.embeddings
            )
        else:
            self.vectorstore = None

    def load_and_split(self, file_path):
        if file_path.endswith(".pdf"):
            loader = PyPDFLoader(file_path)
        elif file_path.endswith(".txt"):
            loader = TextLoader(file_path, encoding="utf-8")
        else:
            raise ValueError("仅支持 PDF 或 TXT")
        docs = loader.load()
        splitter = RecursiveCharacterTextSplitter(
            chunk_size=800,
            chunk_overlap=100,
            separators=["\n\n", "\n", "。", ".", " ", ""]
        )
        return splitter.split_documents(docs)

    def ingest(self, file_path):
        chunks = self.load_and_split(file_path)
        if self.vectorstore is None:
            self.vectorstore = Chroma.from_documents(
                documents=chunks,
                embedding=self.embeddings,
                persist_directory=self.persist_dir
            )
        else:
            self.vectorstore.add_documents(chunks)
        self.vectorstore.persist()
        return f"✅ 已处理并存储 {len(chunks)} 个文档块"
```

### 设计意图讲解
- `RecursiveCharacterTextSplitter` 的 `chunk_overlap=100` 确保相邻块间有重叠，避免重要信息被切断在边界。
- 分块大小 800 是根据通用经验值，能让一个块包含足够上下文，又不至于让检索返回过多无关内容。
- 持久化到 `./chroma_db` 目录，重启应用后知识库依然存在。

---

## 模块 3：上下文压缩检索 — 精准获取信息

### 我们要学什么
- 基础检索器：`vectorstore.as_retriever()` 如何工作。
- 为什么需要**上下文压缩**？当检索到的文档块很长时，会包含大量无关文本，浪费 LLM 的注意力并增加成本。
- `ContextualCompressionRetriever` + `LLMChainExtractor` 如何通过 LLM 自动提取出只与问题相关的片段。

### 关键概念
`LLMChainExtractor` 内部会调用一个轻量级 LLM，对每个检索到的文档块执行：
> “Given the question and context, extract only the part that helps answer the question.”

压缩后的内容更紧凑，让 Agent 能更高效地推理。

### 代码

```python
from langchain.retrievers import ContextualCompressionRetriever
from langchain.retrievers.document_compressors import LLMChainExtractor
from langchain_openai import ChatOpenAI

def create_compressed_retriever(vectorstore):
    base_retriever = vectorstore.as_retriever(search_kwargs={"k": 6})
    llm = ChatOpenAI(model="gpt-4o-mini", temperature=0)  # 轻量模型做提取
    compressor = LLMChainExtractor.from_llm(llm)
    return ContextualCompressionRetriever(
        base_compressor=compressor,
        base_retriever=base_retriever
    )
```

### 设计意图讲解
- `k=6`：先取 6 个最相关块，然后压缩，确保最终输入 LLM 的信息量足够且不冗余。
- 使用 `gpt-4o-mini` 做压缩，速度很快且成本极低，适合这种“清洗”任务。

---

## 模块 4：定义工具 — Function Calling 的实现

### 我们要学什么
- 什么是 **Function Calling**（工具调用）？为什么 Agent 需要它？
- 如何使用 `@tool` 装饰器将 Python 函数声明为 LLM 可用的工具。
- 工具函数的 **docstring** 非常重要：它会被自动转化为 OpenAI 的 function schema，告诉模型什么时候该调用哪个工具、需要什么参数。

### 代码 (定义两个工具)

```python
from langchain.tools import tool

compressed_retriever = None  # 稍后注入
wiki_llm = ChatOpenAI(model="gpt-4o", temperature=0.3)

@tool
def search_knowledge_base(query: str) -> str:
    """
    在已上传的文档知识库中搜索与 query 最相关的内容。
    当用户询问具体事实或需要从文档中查找信息时使用此工具。
    """
    docs = compressed_retriever.get_relevant_documents(query)
    if not docs:
        return "未找到相关文档。"
    return "\n\n".join([d.page_content for d in docs])

@tool
def generate_wiki(topic: str) -> str:
    """
    根据已上传的文档，自动生成关于指定主题的 Wiki 页面（Markdown 格式）。
    当用户明确要求“生成Wiki”或“创建百科页面”时调用此工具。
    """
    docs = compressed_retriever.get_relevant_documents(topic)
    if not docs:
        return "知识库中没有相关内容，无法生成 Wiki。"
    context = "\n\n".join([d.page_content for d in docs])
    prompt = f"""你是一个专业的百科编辑。请根据以下参考资料，为“{topic}”创建一个结构完整的 Wiki 页面。
要求：
- Markdown 格式，包含概述、核心概念、详细说明等。
- 只依据参考资料，不要编造。
- 语言简洁专业。

参考资料：
{context}
"""
    return wiki_llm.invoke(prompt).content
```

### 设计意图讲解
- 工具 docstring 的第一行是核心描述，LangChain 会用它生成 function schema。
- `generate_wiki` 内部会检索资料后，用另一个专门提示词让模型生成 Markdown，这是典型的 Prompt Engineering 实践。
- `compressed_retriever` 是全局变量，我们会在创建 Agent 前赋值，确保工具内部能访问。

---

## 模块 5：创建 Agent 与上下文压缩记忆

### 我们要学什么
- **Agent 构造流程**：`create_openai_tools_agent` 如何把模型、工具、提示模板组合起来。
- **对话记忆管理**：为什么需要记忆压缩？`ConversationSummaryBufferMemory` 如何工作——保留最近几轮完整对话，超出的用 LLM 自动总结成摘要。
- `AgentExecutor` 负责驱动 ReAct 循环（思考-行动-观察）。

### 代码

```python
from langchain.memory import ConversationSummaryBufferMemory
from langchain.agents import AgentExecutor, create_openai_tools_agent
from langchain_core.prompts import ChatPromptTemplate, MessagesPlaceholder

def create_agent(vectorstore):
    global compressed_retriever
    compressed_retriever = create_compressed_retriever(vectorstore)

    tools = [search_knowledge_base, generate_wiki]
    llm = ChatOpenAI(model="gpt-4o", temperature=0)

    # 记忆：保留最近 2000 token 的完整对话，超出部分自动摘要
    memory = ConversationSummaryBufferMemory(
        llm=ChatOpenAI(model="gpt-4o-mini", temperature=0),
        max_token_limit=2000,
        memory_key="chat_history",
        return_messages=True
    )

    system_prompt = """你是一个 Wiki 助手，能够搜索已上传的文档并生成 Wiki。
规则：
1. 总是优先使用工具查找信息。
2. 未找到相关资料时，如实告知。
3. 生成 Wiki 必须基于文档内容。
"""
    prompt = ChatPromptTemplate.from_messages([
        ("system", system_prompt),
        MessagesPlaceholder(variable_name="chat_history"),
        ("user", "{input}"),
        MessagesPlaceholder(variable_name="agent_scratchpad"),
    ])

    agent = create_openai_tools_agent(llm, tools, prompt)
    return AgentExecutor(agent=agent, tools=tools, memory=memory, verbose=True, max_iterations=5)
```

### 设计意图讲解
- `ConversationSummaryBufferMemory` 兼顾了对话完整性和 Token 消耗：既不会丢失久远对话的要点，又严格控制了上下文长度。
- 使用单独的 `gpt-4o-mini` 做记忆总结，把成本降到最低。
- `agent_scratchpad` 是 LangChain 内部用来存放工具调用中间过程的变量，你只需在模板中占位即可。

---

## 模块 6：将所有零件组装进 Gradio

现在我们把前面所有功能绑定到 Gradio 界面上。`upload_file` 函数调用 `DocumentProcessor.ingest`，并重新创建 Agent。`chat` 函数调用 `agent.invoke` 并返回回复。

```python
import gradio as gr

# 全局变量
vectorstore = None
agent = None

def upload_file(file):
    global vectorstore, agent
    if file is None:
        return "❌ 请先选择文件"
    try:
        processor = DocumentProcessor()
        result = processor.ingest(file.name)
        vectorstore = processor.vectorstore
        agent = create_agent(vectorstore) if vectorstore else None
        return f"{result}"
    except Exception as e:
        return f"❌ 处理失败：{str(e)}"

def chat(message, history):
    global agent, vectorstore
    if vectorstore is None:
        return "知识库为空，请先上传文档！"
    if agent is None:
        agent = create_agent(vectorstore)
    try:
        result = agent.invoke({"input": message})
        return result['output']
    except Exception as e:
        return f"❌ 出错：{str(e)}"

# Gradio 界面
with gr.Blocks(title="LLM Wiki Agent") as demo:
    gr.Markdown("# 📚 LLM Wiki Agent\n上传文档 → 提问/生成 Wiki")

    with gr.Tab("📁 文档上传"):
        file_input = gr.File(label="选择 PDF 或 TXT")
        upload_btn = gr.Button("入库")
        upload_msg = gr.Textbox(label="状态", interactive=False)
        upload_btn.click(fn=upload_file, inputs=file_input, outputs=upload_msg)

    with gr.Tab("💬 Wiki 对话"):
        chatbot = gr.Chatbot(label="对话记录")
        msg = gr.Textbox(label="输入你的问题")
        clear = gr.Button("清空对话")

        def respond(message, chat_history):
            bot_reply = chat(message, chat_history)
            chat_history.append((message, bot_reply))
            return "", chat_history

        msg.submit(respond, [msg, chatbot], [msg, chatbot])
        clear.click(lambda: [], None, chatbot)

if __name__ == "__main__":
    demo.launch()
```

---

## 完整代码整合与运行

将上述所有代码块按顺序组合成一个 `app.py`（记得导入所需的库，并在开头配置 API 密钥）。启动命令：

```bash
python app.py
```

Gradio 会在本地启动一个 Web 服务，打开浏览器即可使用。

---

## 学习检验：你能回答这些问题吗？

完成项目后，请自我检查你是否真正掌握了关键知识：

1. **Gradio**：`gr.Blocks` 与 `gr.Tab` 的作用是什么？如何绑定事件并更新 `Chatbot` 的对话列表？
2. **文档向量化**：为什么需要分割文档？`chunk_overlap` 的作用是什么？
3. **上下文压缩**：`ContextualCompressionRetriever` 解决了什么问题？它在 Agent 流程中的位置在哪里？
4. **Function Calling**：`@tool` 的 docstring 为什么至关重要？Agent 是如何决定调用哪个工具的？
5. **记忆管理**：`ConversationSummaryBufferMemory` 与普通的 `ConversationBufferMemory` 有什么区别？为什么说它实现了“上下文压缩”？

---

## 下一步深造方向

- 尝试将后端换成本地模型（使用 `Ollama` + `ChatOllama`），实现完全离线运行。
- 在 `generate_wiki` 中引入多 Agent 协作：让一个 Agent 规划大纲，另一个 Agent 填充内容。
- 把 Gradio 部署到 Hugging Face Spaces，让其他人也能在线体验。
