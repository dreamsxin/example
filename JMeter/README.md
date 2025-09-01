#

- https://jmeter.apache.org/download_jmeter.cgi

1. JMeter 安装与部署

JMeter 是一款基于 Java 的开源性能测试工具，能够对 Web 应用程序、FTP 服务器、数据库等进行负载和性能测试。它的跨平台特性使得在 Windows、Linux 和 macOS 上都能运行。

1.1 环境准备：安装 Java

JMeter 需要 Java 运行环境 (JRE) 或 Java 开发工具包 (JDK)。请确保你的系统已安装 Java 8 或更高版本。

检查Java是否已安装： 打开终端（Linux/macOS）或命令提示符（Windows），输入：

```bash
java -version
```

如果显示版本信息（如 java version "1.8.0_281"），说明已安装。如果未安装或版本过低，请先安装。

安装Java（参考命令）：

· Linux (CentOS/RedHat):
```bash
sudo yum install java-1.8.0-openjdk-devel  # 安装JDK[citation:1][citation:4]
```
· Linux (Ubuntu/Debian):
```bash
sudo apt update
sudo apt install openjdk-11-jdk
```
· Windows/macOS: 请从 Oracle JDK 或 OpenJDK 官网下载安装包并安装。

安装完成后，建议配置 JAVA_HOME 环境变量指向你的Java安装路径。

1.2 下载与安装 JMeter

1. 访问官网下载：从 Apache JMeter 官方下载页面 获取最新稳定版本的二进制包（例如 apache-jmeter-5.6.3.zip 用于 Windows，apache-jmeter-5.6.3.tgz 用于 Linux/macOS）。
2. 解压安装包：将下载的压缩包解压到你选择的目录。
· Windows: 可以使用解压软件解压到任意目录（如 C:\jmeter），路径中最好不要包含中文或空格。
· Linux/macOS: 在终端中使用 tar 命令解压：
 ```bash
 tar -xzf apache-jmeter-5.6.3.tgz -C /usr/local  # 解压到/usr/local目录
 ```

1.3 配置环境变量（可选但推荐）

为了方便地从任意位置启动JMeter，建议将其bin目录添加到系统的PATH环境变量中。

**· Windows:**
`jmeter.bat`

1. 右键点击“此电脑” -> “属性” -> “高级系统设置” -> “环境变量”。
2. 在“系统变量”部分，找到并选中Path变量，点击“编辑”。
3. 点击“新建”，添加JMeter的bin目录路径（例如 C:\jmeter\apache-jmeter-5.6.3\bin）。
4. 你也可以新建一个 JMETER_HOME 变量，值为JMeter的安装目录（例如 C:\jmeter\apache-jmeter-5.6.3），然后在Path中添加 %JMETER_HOME%\bin。
**· Linux/macOS:**
编辑当前用户的shell配置文件（如 ~/.bashrc, ~/.zshrc），在文件末尾添加以下行：
```bash
export JMETER_HOME=/path/to/your/jmeter  # 替换为你的JMeter解压路径，例如 /usr/local/apache-jmeter-5.6.3
export PATH=$JMETER_HOME/bin:$PATH
```
保存文件后，运行 source ~/.bashrc（或 source ~/.zshrc）使配置立即生效。

1.4 验证安装

打开终端或命令提示符，输入：

```bash
jmeter -v
```

如果安装和配置成功，你将看到JMeter的版本信息。

2. JMeter HTTP 压力测试说明

JMeter 可以通过图形界面（GUI）创建测试计划，但进行实际压力测试时，强烈建议在非GUI（命令行）模式下运行，以减少资源开销。

2.1 启动 JMeter

· GUI模式（用于创建测试计划）：
· Windows: 进入JMeter的bin目录，双击 jmeter.bat。
· Linux/macOS: 在终端中输入 jmeter。
· 非GUI模式（用于执行压力测试）：使用命令行。

2.2 创建基本的 HTTP 压力测试计划

1. 添加线程组：
· 启动JMeter GUI后，你会看到一个新的测试计划。
· 右键点击“Test Plan” -> “Add” -> “Threads (Users)” -> “Thread Group”。
· 线程组决定了模拟的用户数量和行为：
 · Number of Threads (users): 并发用户数（如 100）。
 · Ramp-up period (seconds): 在多长时间内启动所有用户（如 10秒，即每秒启动10个用户）。
 · Loop Count: 每个用户的请求迭代次数（勾选“Infinite”可持续运行）。
2. 添加 HTTP 请求采样器：
· 右键点击“Thread Group” -> “Add” -> “Sampler” -> “HTTP Request”。
· 配置你的Web服务器详细信息：
 · Protocol: http 或 https。
 · Server Name or IP: 目标服务器地址（如 www.example.com 或 192.168.1.1）。
 · Port Number: 端口号（如 80 或 443）。
 · HTTP Request: 请求方法（GET, POST等）和请求路径（如 /api/users）。
 · 对于POST请求，你可以在“Body Data”选项卡中填写请求体（如JSON数据）。
3. 添加监听器（查看结果）：
· 右键点击“Thread Group” -> “Add” -> “Listener”。
· 常用的监听器有：
 · View Results Tree: 查看每个请求的详细结果，包括请求和响应数据。注意：压力测试时不要使用此监听器，因为它会消耗大量内存，仅用于调试。
 · Summary Report: 提供聚合的摘要报告。
 · Aggregate Report: 提供更详细的聚合数据，包括平均值、中位数、90%百分位等响应时间。
 · Response Time Graph: 以图表形式展示响应时间变化。

2.3 执行压力测试并获取结果

在GUI中配置好测试计划后，将其保存为.jmx文件（例如 test_plan.jmx）。

然后，打开终端或命令提示符，切换到JMeter的bin目录，使用非GUI模式运行测试：

```bash
jmeter -n -t /path/to/your/test_plan.jmx -l /path/to/results.jtl
```

· -n: 表示非GUI模式运行。
· -t: 指定测试计划文件（.jmx）的路径。
· -l: 指定结果日志文件（.jtl）的路径，测试结果将保存于此。

测试完成后，你可以通过JMeter GUI中的监听器（如Aggregate Report）来加载生成的 .jtl 文件并分析结果。

2.4 关键性能指标解读

在聚合报告或摘要报告中，重点关注以下指标：

· # Samples: 总请求数。
· Average (ms): 平均响应时间。
· Median (ms): 响应时间中位数。
· 90% Line (ms): 90%的请求响应时间低于此值。
· 95% Line (ms): 95%的请求响应时间低于此值（更严格）。
· 99% Line (ms): 99%的请求响应时间低于此值（极端情况）。
· Min (ms) / Max (ms): 最小/最大响应时间。
· Error %: 错误请求的百分比。
· Throughput (requests/sec): 每秒处理的请求数（吞吐量），是衡量系统性能的关键指标。
· Received KB/sec / Sent KB/sec: 每秒接收/发送的数据量。

2.5 压力测试注意事项

· 循序渐进：不要一开始就使用极高的并发数，逐步增加负载，观察系统性能变化和瓶颈。
· 监控系统资源：在压力测试过程中，同时监控测试机和目标服务器的CPU、内存、磁盘I/O和网络带宽使用情况（可使用 top, vmstat, nmon 等命令或专业监控工具）。
· 避开生产环境：压力测试应在测试环境或预生产环境进行，避免对生产系统造成影响。
· 思考时间（Timer）：为了模拟更真实的用户行为，可以在线程组中添加随机延迟等定时器。
· 参数化：如果需要对不同的用户使用不同的数据（如登录名），可以使用CSV Data Set Config组件进行参数化。

3. 其他压力测试工具简介

除了JMeter，还有其他一些常用的HTTP压力测试工具：

· ab (ApacheBench)：Apache HTTP 服务器自带的一个简单易用的工具，适合快速进行基准测试。
```bash
ab -n 1000 -c 100 http://example.com/
```
· wrk：一个现代的HTTP基准测试工具，能用少量的线程压出巨大的并发量。
· Locust：一个使用Python编写的开源负载测试工具，它允许你使用Python代码定义用户行为，并支持分布式运行。
