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
