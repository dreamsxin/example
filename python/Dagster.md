## 一、Dagster 简介

### 1.1 什么是 Dagster？

Dagster 是一个**面向数据资产（Data Assets）的数据编排平台**，专为数据工程师和机器学习工程师设计。你可以把它理解为**数据工程领域的 Django**——正如 Django 为 Web 开发提供了完整的框架，Dagster 为数据管道的开发、运行和观测提供了端到端的解决方案。

与传统的任务调度器（如 Airflow）不同，Dagster 的核心设计理念是 **“软件定义资产”（Software-Defined Assets）** ——它不只关注“任务何时运行”，更关注“数据资产如何产生、依赖什么、质量如何”。

### 1.2 核心特性

Dagster 为数据团队带来以下关键能力：

| 特性 | 说明 |
|------|------|
| **数据血缘与可观测性** | 自动追踪资产之间的依赖关系，清晰展示数据流向 |
| **声明式编程模型** | 用 Python 装饰器声明资产及其依赖，代码简洁直观 |
| **一流的可测试性** | 支持单元测试、集成测试，提升代码质量 |
| **全生命周期支持** | 从本地开发到生产部署，提供完整的 CI/CD 支持 |
| **灵活的自动化** | 支持定时调度（Schedules）、事件驱动（Sensors）和声明式自动化 |
| **可扩展的架构** | 从单机开发到生产级集群均可平滑扩展 |

### 1.3 核心概念

Dagster 的核心抽象包括：

- **资产（Asset）**：代表一个逻辑数据单元，如表、数据集或机器学习模型。资产可以依赖其他资产，形成完整的数据血缘。通过 `@dg.asset` 装饰器即可定义。

- **作业（Job）**：一组需要一起执行的操作（Ops）或资产的执行单元。调度器和传感器都会触发作业运行。

- **调度器（Schedule）**：按固定时间间隔（如每天、每小时）自动触发作业或资产运行。

- **传感器（Sensor）**：基于外部事件触发运行，例如新文件到达、外部系统状态变更等。

- **资源（Resource）**：可跨多个 Dagster 对象共享的配置对象，如数据库连接、API 客户端等。

- **代码位置（Code Location）**：一组 Dagster 定义的集合，包括资产、作业、调度器、传感器和资源。

### 1.4 架构概览

Dagster 采用**用户代码与系统代码分离**的架构设计。一个典型的 Dagster 部署包含以下组件：

- **Dagster Web UI / GraphQL API**：提供可视化的界面和 API 接口
- **Dagster 守护进程（Daemon）**：负责运行调度器、传感器、运行队列管理等后台任务
- **代码位置服务器（Code Location Server）**：加载用户定义的资产、作业等
- **存储层（Storage）**：存储元数据、运行日志等
- **执行器与运行启动器（Executor & Run Launcher）**：负责实际执行任务

这种架构使得 Dagster 既支持本地开发（Dagster OSS），也支持云托管部署（Dagster+）。

---

## 二、Dagster 快速入门教程

以下教程将带你从零开始，在本地构建并运行你的第一个 Dagster 数据管道。

### 2.1 前置条件

开始之前，请确保你的环境满足以下要求：
- **Python 3.10+**
- 包管理器：推荐使用 `uv`，也可使用 `pip`

### 2.2 步骤一：创建 Dagster 项目

打开终端，使用以下命令创建一个新的 Dagster 项目：

**使用 uv（推荐）：**
```bash
uvx create-dagster@latest project dagster-quickstart
# 提示时输入 y 运行 uv sync
cd dagster-quickstart
source .venv/bin/activate  # Windows: .venv\Scripts\activate
uv add pandas
```

**使用 pip：**
```bash
create-dagster project dagster-quickstart
cd dagster-quickstart
python -m venv .venv
source .venv/bin/activate  # Windows: .venv\Scripts\activate
pip install pandas
pip install --editable . --group dev
```

创建后的项目结构如下：
```
dagster-quickstart/
├── pyproject.toml
├── src/
│   └── dagster_quickstart/
│       ├── __init__.py
│       ├── definitions.py
│       └── defs/
│           └── __init__.py
└── tests/
    └── __init__.py
```

### 2.3 步骤二：定义数据资产

使用 `dg scaffold` 命令生成资产文件：
```bash
dg scaffold defs
```

然后在生成的资产文件中定义一个 ETL 资产。以下是一个完整示例——读取 CSV 文件、转换数据、保存结果：

```python
import dagster as dg
import pandas as pd

@dg.asset
def processed_customers():
    """读取客户数据，添加年龄分组列，并保存处理后的结果"""
    # Extract: 读取原始数据
    df = pd.read_csv("data/customers.csv")
    
    # Transform: 添加年龄分组
    df["age_group"] = pd.cut(
        df["age"],
        bins=[0, 18, 35, 50, 100],
        labels=["Under 18", "18-35", "36-50", "50+"]
    )
    
    # Load: 保存处理后的数据
    df.to_csv("data/processed_customers.csv", index=False)
    
    return df
```

### 2.4 步骤三：启动 Dagster UI

在项目根目录下运行：
```bash
dg dev
```

启动后，在浏览器中打开 `http://localhost:3000`，你将看到 Dagster 的 Web UI。

### 2.5 步骤四：运行管道

在 Dagster UI 中：
1. 在左侧导航栏找到 **Assets** 页面
2. 找到你定义的 `processed_customers` 资产
3. 点击 **Materialize** 按钮手动执行
4. 在 **Runs** 页面查看运行日志和结果

### 2.6 步骤五：添加自动化

要让管道按计划自动运行，可以添加一个调度器：

```python
from dagster import ScheduleDefinition

# 每天凌晨 2 点运行
daily_schedule = ScheduleDefinition(
    job=processed_customers.to_job(),
    cron_schedule="0 2 * * *",
)
```

将调度器添加到 `definitions.py` 中即可生效。

### 2.7 下一步

完成本教程后，你可以：
- 阅读官方 [Concepts 文档](https://docs.dagster.io/getting-started/concepts) 深入了解核心概念
- 学习如何[部署到生产环境](https://docs.dagster.io/deployment)
- 探索 [dagster-dbt](https://docs.dagster.io/integrations/dbt) 等集成方案

## 有向无环图（DAG）

在 Dagster 中，资产（Asset）之间的依赖关系通过**有向无环图（DAG）** 来组织。这种关系也被称为**上下游关系**：被依赖的资产是**上游资产（Upstream Asset）**，依赖别人的是**下游资产（Downstream Asset）**。

定义这种依赖关系主要有两种方式，你可以根据具体场景选择。

### 方式一：通过函数参数隐式定义（最推荐）

这是最直观、最常用的方法。你只需将上游资产的**函数名**作为下游资产函数的**参数**即可。

Dagster 会自动识别参数名对应的上游资产，并建立依赖关系。这种方式也清晰地表明了数据的流向。

```python
import dagster as dg

# 1. 定义上游资产：原始客户数据
@dg.asset
def raw_customers():
    # ... 从源系统加载原始数据 ...
    return df

# 2. 定义下游资产：清洗后的客户数据
#    通过将 'raw_customers' 作为参数，隐式声明依赖
@dg.asset
def cleaned_customers(raw_customers):
    # raw_customers 是上游资产 'raw_customers' 返回的数据
    cleaned_df = raw_customers.dropna()
    # ... 保存清洗后的数据 ...
    return cleaned_df
```

### 方式二：通过 `deps` 参数显式定义（适用于特殊场景）

当你需要声明依赖，但**下游资产的函数并不直接使用上游数据**时（例如，只做元数据跟踪），可以使用 `@asset` 装饰器的 `deps` 参数。

```python
import dagster as dg

# 上游资产
@dg.asset
def upstream_table():
    # ... 创建或更新一张表 ...
    pass

# 下游资产，声明依赖但不使用数据
@dg.asset(deps=[upstream_table]) # 或 deps=["upstream_table"]
def downstream_report():
    # 这个函数不接收 'upstream_table' 作为参数
    # ... 生成一份报告，该报告逻辑上依赖上游表的存在 ...
    pass
```

---

### 复杂场景：多依赖与多资产

*   **一个资产依赖多个上游资产**：在函数中定义多个参数，或在 `deps` 列表中指定多个资产即可。
    ```python
    # 通过函数参数
    @dg.asset
    def final_report(customers, orders, payments):
        # customers, orders, payments 是三个上游资产
        # ...
        pass
    ```

*   **一个操作生成多个资产 (`@multi_asset`)**：当一个函数（如从同一个API获取数据）需要生成多个资产时，可使用 `@multi_asset` 装饰器。这些子资产间的依赖关系也可通过 `deps` 在 `AssetSpec` 中定义。

---

### 进阶：依赖管理的最佳实践与考量

1.  **分区资产 (Partitioned Assets) 的依赖**
    对于按时间等维度分区的资产，Dagster 允许你精细控制分区间的依赖。
    *   **默认规则**：如果上下游分区定义相同，则对应分区相互依赖。
    *   **自定义映射**：你可以通过 `PartitionMapping` 来覆盖默认规则。例如，让一个每日汇总表依赖于前一天的原始数据。
    ```python
    @dg.asset(
        deps=[
            dg.AssetDep(
                "daily_events",
                partition_mapping=dg.TimeWindowPartitionMapping(start_offset=-1, end_offset=-1)
            )
        ]
    )
    def yesterday_summary():
        # ...
    ```

2.  **跨代码位置 (Code Location) 的依赖**
    当资产定义在不同的 Dagster 项目（代码位置）中时，不能直接通过函数参数传递数据。
    *   **仅跟踪血缘**：使用 `deps=["upstream_asset_key"]` 来声明依赖关系，用于在UI中展示血缘。
    *   **传递数据**：需要使用 `AssetSpec` 配合 I/O 管理器来跨位置加载数据。
    *   **触发下游运行**：可以使用**传感器 (Sensor)** 或**声明式自动化 (Declarative Automation)** 来监听上游资产的变化并触发下游任务的执行。

### 总结

*   **首选方法**：使用**函数参数**来定义依赖，这是最清晰、最符合数据流直觉的方式。
*   **特殊场景**：当不涉及数据传递时，使用 `deps` 参数。
*   **复杂场景**：处理分区或跨项目依赖时，请参考上述进阶实践。

Dagster 会根据你定义的这些依赖关系，自动构建出完整的资产血缘图（Asset Graph），并在UI中清晰展示。
