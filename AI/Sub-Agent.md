## Sub-Agent 的常见架构模式

### 1. 委托模式（Delegation）

主 Agent 作为路由器，根据任务类型分发给不同 Sub-Agent：

```
User → Main Agent → 判断任务类型
                   ├→ Code Agent（写代码）
                   ├→ Research Agent（搜索调研）
                   └→ Review Agent（审查结果）
```

### 2. 管道模式（Pipeline）

任务按阶段串行流经多个 Sub-Agent：

```
User → Agent A（拆解需求）→ Agent B（执行）→ Agent C（校验）→ 输出
```

### 3. 协作模式（Collaboration）

多个 Sub-Agent 并行工作，主 Agent 汇总结果：

```
User → Main Agent → ├→ Agent 1 ─┐
                    ├→ Agent 2 ──┼→ 汇总 → 输出
                    └→ Agent 3 ─┘
```

---

## 几个主流框架的实现方式

### LangChain / LangGraph

```python
from langgraph.prebuilt import create_react_agent

# 定义 sub-agent
code_agent = create_react_agent(model, tools=[code_tools], name="code_agent")
search_agent = create_react_agent(model, tools=[search_tools], name="search_agent")

# 主 agent 把 sub-agent 当作 tool 调用
main_agent = create_react_agent(
    model,
    tools=[
        code_agent.as_tool(name="write_code", description="写代码用"),
        search_agent.as_tool(name="search", description="搜索信息用"),
    ]
)
```

### AutoGen (AG2)

```python
from autogen import AssistantAgent, UserProxyAgent

sub_agent_1 = AssistantAgent(name="researcher", system_message="你负责调研...")
sub_agent_2 = AssistantAgent(name="coder", system_message="你负责写代码...")

main_agent = AssistantAgent(
    name="coordinator",
    system_message="你是协调者，把任务分配给 researcher 或 coder"
)

# 群聊模式自动协调
from autogen import GroupChat, GroupChatManager
group_chat = GroupChat(agents=[main_agent, sub_agent_1, sub_agent_2])
manager = GroupChatManager(groupchat=group_chat)
```

### CrewAI

```python
from crewai import Agent, Task, Crew

researcher = Agent(role="研究员", goal="调研信息", tools=[search_tool])
coder = Agent(role="程序员", goal="写代码", tools=[code_tool])

crew = Crew(agents=[researcher, coder], tasks=[task1, task2])
crew.kickoff()
```

### 自己实现（纯 Python）

```python
class SubAgent:
    def __init__(self, name, tools, llm):
        self.name = name
        self.tools = tools
        self.llm = llm

    def run(self, task: str) -> str:
        # 调用 LLM + tools 执行
        ...

class MainAgent:
    def __init__(self, sub_agents: list[SubAgent], llm):
        self.sub_agents = {a.name: a for a in sub_agents}
        self.llm = llm

    def run(self, user_input: str) -> str:
        # 1. 让 LLM 决定调用哪个 sub-agent
        decision = self.llm.call(
            f"可用 agent: {list(self.sub_agents.keys())}\n"
            f"用户任务: {user_input}\n"
            f"选择哪个 agent 并给出子任务："
        )
        # 2. 调用对应 sub-agent
        agent_name, sub_task = parse_decision(decision)
        result = self.sub_agents[agent_name].run(sub_task)
        return result
```

---

## 设计 Sub-Agent 时的关键考虑

| 要点 | 说明 |
|---|---|
| **职责边界** | 每个 sub-agent 只做一件事，职责清晰 |
| **工具隔离** | 不同 agent 持有不同的 tools，避免权限混乱 |
| **上下文传递** | 主 agent 需要把必要的上下文传给 sub-agent，但不要传无关信息 |
| **结果聚合** | 定义好 sub-agent 的输出格式，方便主 agent 汇总 |
| **错误处理** | sub-agent 失败时主 agent 如何重试或降级 |
| **成本控制** | sub-agent 调用会产生额外 token，注意控制调用链深度 |

---

你是在用某个具体的框架，还是在自己搭建 agent 系统？告诉我你的技术栈和场景，我可以给出更精准的方案。
