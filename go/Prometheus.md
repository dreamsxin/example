# Prometheus

Prometheus 所有采集的监控数据均以指标（metric）的形式保存在内置的时间序列数据库当中（TSDB）：属于同一指标名称，同一标签集合的、有时间戳标记的数据流。
除了存储的时间序列，Prometheus 还可以根据查询请求产生临时的、衍生的时间序列作为返回结果。

- metric
指标名称和描述当前样本特征的 labelsets

格式如下：

`<metric name>{<label name>=<label value>, ...}`
例如：
`http_requests_total{host="192.10.0.1", method="POST", handler="/messages"}`
这里的 `http_requests_total` 是指标名，host、method、handler 是三个标签(label)，也就是三个维度

- timestamp
时间戳一个精确到毫秒的时间戳；

- value
样本值一个 folat64 的浮点型数据表示当前样本的值。

## 安装

```shell
sudo useradd --no-create-home --shell /bin/false prometheus
sudo useradd --no-create-home --shell /bin/false node_exporter
sudo mkdir /etc/prometheus
sudo mkdir /var/lib/prometheus
sudo chown prometheus:prometheus /etc/prometheus
sudo chown prometheus:prometheus /var/lib/prometheus
# 下载
curl -LO https://github.com/prometheus/prometheus/releases/download/v2.0.0/prometheus-2.0.0.linux-amd64.tar.gz
# 校检
sha256sum prometheus-2.0.0.linux-amd64.tar.gz
tar xvf prometheus-2.0.0.linux-amd64.tar.gz
sudo cp prometheus-2.0.0.linux-amd64/prometheus /usr/local/bin/
sudo cp prometheus-2.0.0.linux-amd64/promtool /usr/local/bin/
sudo chown prometheus:prometheus /usr/local/bin/prometheus
sudo chown prometheus:prometheus /usr/local/bin/promtool
sudo cp -r prometheus-2.0.0.linux-amd64/consoles /etc/prometheus
sudo cp -r prometheus-2.0.0.linux-amd64/console_libraries /etc/prometheus
sudo chown -R prometheus:prometheus /etc/prometheus/consoles
sudo chown -R prometheus:prometheus /etc/prometheus/console_libraries
# 从主目录中删除剩余文件
rm -rf prometheus-2.0.0.linux-amd64.tar.gz prometheus-2.0.0.linux-amd64
# 修改配置文件
sudo nano /etc/prometheus/prometheus.yml
```

* 检查配置文件语法

要在不启动Prometheus服务器的情况下快速检查规则文件在语法上是否正确，请安装并运行Prometheus的promtool命令行实用工具：

```shell
go get github.com/prometheus/prometheus/cmd/promtool
promtool check rules /path/to/example.rules.yml
```

* 安装运行
```shell
wget http://labfile.oss.aliyuncs.com/courses/980/05/assets/prometheus-2.2.1.linux-amd64.tar.gz
tar xvfz prometheus-2.2.1.linux-amd64.tar.gz
cd prometheus-2.2.1.linux-amd64
./prometheus
```

## 配置

配置 Prometheus 监控其自身

Prometheus 服务本身也通过路径 /metrics 暴露了其内部的各项度量指标，我们只需要把它加入到监控目标里就可以。
```yaml
global:
  # 全局默认抓取间隔
  scrape_interval: 15s

scrape_configs:
  # 任务名
  - job_name: 'prometheus'
    # 本任务的抓取间隔，覆盖全局配置
    scrape_interval: 5s
    static_configs:
      # 抓取地址同 Prometheus 服务地址，路径为默认的 /metrics
      - targets: ['localhost:9090']
```
在global设置中，定义抓取指标的默认时间间隔。请注意，除非单个导出器自己的设置覆盖全局变量，否则Prometheus会将这些设置应用于每个导出器。
Prometheus 使用`job_name`在标签和图表上标记出口商，因此请务必在此处选择描述性内容。
由于Prometheus会导出可用于监控性能和调试的重要数据，因此我们将全局scrape_interval指令从15秒重写为5秒，以便更频繁地进行更新。

* 运行

```shell
sudo -u prometheus /usr/local/bin/prometheus \
    --config.file /etc/prometheus/prometheus.yml \
    --storage.tsdb.path /var/lib/prometheus/ \
    --web.console.templates=/etc/prometheus/consoles \
    --web.console.libraries=/etc/prometheus/console_libraries
```

* 自启动服务
```shell
sudo nano /etc/systemd/system/prometheus.service
# 重新加载systemd
sudo systemctl daemon-reload
# 启动服务
sudo systemctl start prometheus
# 查看服务状态
sudo systemctl status prometheus
# 启动开机服务
sudo systemctl enable prometheus
```
`prometheus.service` 内容
```conf
[Unit]
Description=Prometheus
Wants=network-online.target
After=network-online.target

[Service]
User=prometheus
Group=prometheus
Type=simple
ExecStart=/usr/local/bin/prometheus \
    --config.file /etc/prometheus/prometheus.yml \
    --storage.tsdb.path /var/lib/prometheus/ \
    --web.console.templates=/etc/prometheus/consoles \
    --web.console.libraries=/etc/prometheus/console_libraries

[Install]
WantedBy=multi-user.target
```

Prometheus支持两种类型的规则，这些规则可以定期配置，然后定期评估：记录规则和警报规则。
要在Prometheus中包含规则，请创建包含必要规则语句的文件，并让Prometheus通过Prometheus配置中的rule_files字段加载文件， 规则文件使用YAML。

通过将SIGHUP发送到Prometheus进程，可以在运行时重新加载规则文件。 仅当所有规则文件格式正确时才会应用更改。

* 简单的示例规则文件： 
```yaml
groups:
  - name: example
    rules:
    - record: job:http_inprogress_requests:sum
      expr: sum(http_inprogress_requests) by (job)
```

## 安装Node Exporter

使用节点导出器生成有关我们服务器资源的指标，提供有关系统的详细信息，包括CPU，磁盘和内存使用情况。
```shell
curl -LO https://github.com/prometheus/node_exporter/releases/download/v0.15.1/node_exporter-0.15.1.linux-amd64.tar.gz
tar xvf node_exporter-0.15.1.linux-amd64.tar.gz
sudo cp node_exporter-0.15.1.linux-amd64/node_exporter /usr/local/bin
sudo chown node_exporter:node_exporter /usr/local/bin/node_exporter
rm -rf node_exporter-0.15.1.linux-amd64.tar.gz node_exporter-0.15.1.linux-amd64
sudo nano /etc/systemd/system/node_exporter.service
```
`node_exporter.service`
```conf
[Unit]
Description=Node Exporter
Wants=network-online.target
After=network-online.target

[Service]
User=node_exporter
Group=node_exporter
Type=simple
ExecStart=/usr/local/bin/node_exporter

[Install]
WantedBy=multi-user.target
```
默认的收集器列表 https://github.com/prometheus/node_exporter/blob/master/README.md#enabled-by-default
如果您需要覆盖默认的收集器列表，可以使用--collectors.enabled标志，如：
`ExecStart=/usr/local/bin/node_exporter --collectors.enabled meminfo,loadavg,filesystem`

* 配置 Prometheus 获取节点导出器信息

在`scrape_configs`块的末尾，添加一个名为`node_exporter`的新条目。

```yml
global:
  scrape_interval: 15s

scrape_configs:
  - job_name: 'prometheus'
    scrape_interval: 5s
    static_configs:
      - targets: ['localhost:9090']
  - job_name: 'node_exporter'
    scrape_interval: 5s
    static_configs:
      - targets: ['localhost:9100']  
```

## metric 指标类型

- Counter 只增不减的累加指标
一种累加的 metric，典型的应用如：请求的个数，结束的任务数，出现的错误数等。随着客户端不断请求，数值越来越大。
例如api_http_requests_total{method="POST", handler="/messages"}

- Gauge 可增可减的测量指标
一种常规的 metric，典型的应用如：温度，运行的 goroutines 的个数。返回的数值会上下波动。
例如go_goroutines{instance="172.17.0.2", job="Prometheus"}

- Histogram 自带buckets区间用于统计分布的直方图
可以理解为柱状图，典型的应用如：请求持续时间，响应大小。可以对观察结果采样，分组及统计。
例如设置一个name为web_request_duration_seconds的Histogram 的metrics,并设置区间值为[0.1,0.5,1]

- Summary 数据分布统计图
类似于 Histogram, 典型的应用如：请求持续时间，响应大小。
主要做统计用,设置分位数的值，会实时返回该分位数上的值。

## 使用

```shell
go get github.com/prometheus/client_golang/prometheus/promhttp
```

```go
package main

github.com/prometheus/client_golang/prometheus
import github.com/prometheus/client_golang/prometheus/promhttp

func main() {

  // expose prometheus metrics接口
  http.Handle("/metrics", promhttp.Handler())

  log.Fatal(http.ListenAndServe(":8080", nil))
}
```

启动应用访问 `http://localhost:8080/metrics`，会返回应用中`goroutine`、`thread`、`gc`、`mem`的基本信息。

* 创建指标并注册到 `prometheus`

```go
// 初始化 web_reqeust_total， counter类型指标， 表示接收http请求总次数
var WebRequestTotal = prometheus.NewCounterVec(
  prometheus.CounterOpts{
    Name: "web_reqeust_total",
    Help: "Number of hello requests in total",
  },
  // 设置两个标签 请求方法和 路径 对请求总次数在两个
  []string{"method", "endpoint"},
)

// web_request_duration_seconds，Histogram类型指标，bucket代表duration的分布区间
var WebRequestDuration = prometheus.NewHistogramVec(
  prometheus.HistogramOpts{
    Name: "web_request_duration_seconds",
    Help: "web request duration distribution",
    Buckets: []float64{0.1, 0.3, 0.5, 0.7, 0.9, 1},
  },
  []string{"method", "endpoint"},
)

func init() {
  // 注册监控指标
  prometheus.MustRegister(WebRequestTotal)
  prometheus.MustRegister(WebRequestDuration)
}
```

* 在适当切入点对指标进行记录变更
```go
// 包装 handler function,不侵入业务逻辑
func Monitor(h http.HandlerFunc) http.HandlerFunc {
  return func(w http.ResponseWriter, r *http.Request) {
    start := time.Now()
    h(w, r)
    duration := time.Since(start)
    // counter类型 metric的记录方式
    WebRequestTotal.With(prometheus.Labels{"method": r.Method, "endpoint": r.URL.Path}).Inc()
    // Histogram类型 meric的记录方式
    WebRequestDuration.With(prometheus.Labels{"method": r.Method, "endpoint": r.URL.Path}).Observe(duration.Seconds())
  }
}

// query
func Query(w http.ResponseWriter, r *http.Request) {
  //模拟业务查询耗时0~1s
  time.Sleep(time.Duration(rand.Intn(1000)) * time.Millisecond)
  _,_ = io.WriteString(w, "some results")

}

func main() {
  // expose prometheus metrics接口
  http.Handle("/metrics", promhttp.Handler())
  http.HandleFunc("/query", monitor.Monitor(Query))

  log.Fatal(http.ListenAndServe(":8080", nil))
}
```
