# HPA

Horizontal Pod Autoscaler(HPA) 是根据CPU使用率或自定义指标 (metric) 自动对 Pod 进行扩/缩容

控制器每隔30s查询metrics的资源使用情况

支持三种metrics类型：①预定义metrics(比如Pod的CPU)以利用率的方式计算 ②自定义的Pod metrics，以原始值(raw value)的方式计算 ③自定义的object metrics

实现cpu或内存的监控，首先有个前提条件是该对象必须配置了resources.requests.cpu或resources.requests.memory才可以，可以配置当cpu/memory达到上述配置的百分比后进行扩容或缩容

## HPA实例操作

我们有下面这个 `nginx_deploy.yaml` 文件：
```yml
apiVersion: apps/v1
kind: Deployment
metadata:
  labels:
    app: nginx-deploy
  name: nginx-deploy
  namespace: default
spec:
  replicas: 1
  revisionHistoryLimit: 10
  selector:
    matchLabels:
      app: nginx-deploy
  strategy:
    rollingUpdate:
      maxSurge: 25%
      maxUnavailable: 25%
    type: RollingUpdate
  template:
    metadata:
      labels:
        app: nginx-deploy
    spec:
      containers:
      - image: nginx:1.7.9
        imagePullPolicy: IfNotPresent
        name: nginx
        resources:
          limits: 
            cpu: 200m # 1000m是用满一个cpu核，200m就是用了0.2的一个cpu核
            memory: 128Mi
          requests:
            cpu: 10m
            memory: 128Mi # 1000m是用满一个cpu核，100m就是用了0.1的一个cpu核
      restartPolicy: Always
      terminationGracePeriodSeconds: 30
```
然后运行这个文件：
`kubectl create -f nginx_deploy.yaml`

查看一下
`kubectl get po`

然后执行我们的自动更新命令：
`kubectl autoscale deploy nginx-deploy --cpu-percent=20 --min=2 --max=5`
查看一下现在的数量
```shell
kubectl get deploy
kubectl get rs
kubectl get po
```

在 Kubernetes 中，Horizontal Pod Autoscaler (HPA) 不仅支持基于 CPU 和内存的扩缩容，还支持多种自定义指标，使扩缩容策略更贴合业务需求。以下是 Kubernetes HPA 支持的主要自定义指标类型及其应用场景：

---

### ⚙️ **一、Pods 指标（Pods Metrics）**  
**定义**：基于单个 Pod 的业务指标（如请求率、队列长度）。  
**核心机制**：  
- 指标由 Pod 暴露（如通过 Prometheus 采集）。  
- HPA 计算所有 Pod 指标的平均值，并与目标值对比。  
**典型场景**：  
- **QPS（每秒请求数）**：当每个 Pod 的 QPS 超过阈值时触发扩容。  
- **消息队列消费速率**：如 Kafka 消费者组的消息积压量。  
- **自定义业务指标**：如订单处理速率、在线用户数等。  

**配置示例**：  
```yaml
metrics:
- type: Pods
  pods:
    metric:
      name: http_requests_per_second  # 指标名称
    target:
      type: AverageValue
      averageValue: 100  # 目标 QPS 为 100
```

---

### 🔗 **二、Object 指标（Object Metrics）**  
**定义**：基于 Kubernetes 对象（如 Ingress、Service）的指标。  
**核心机制**：  
- 指标与特定对象关联（如 Ingress 的请求延迟）。  
- HPA 直接查询该对象的指标值。  
**典型场景**：  
- **Ingress 请求延迟**：当平均延迟超过阈值时扩容后端 Pod。  
- **Service 错误率**：基于 5xx 错误率触发扩缩容。  
- **ConfigMap/Secret 变更频率**（需自定义控制器支持）。  

**配置示例**：  
```yaml
metrics:
- type: Object
  object:
    describedObject:
      kind: Ingress
      name: my-ingress
    metric:
      name: request_latency_ms
    target:
      type: AverageValue
      averageValue: 200  # 目标延迟 ≤ 200ms
```

---

### 🌐 **三、External 指标（External Metrics）**  
**定义**：来自集群外部系统的指标（如云服务、数据库）。  
**核心机制**：  
- 通过 Prometheus Adapter 或云厂商插件（如 AWS CloudWatch）集成外部数据源。  
- HPA 直接使用外部系统提供的指标值。  
**典型场景**：  
- **云服务负载**：如 AWS SQS 队列积压消息数。  
- **数据库性能**：如 MySQL 连接池使用率、Redis 内存占用。  
- **混合云指标**：跨集群的全局负载均衡。  

**配置示例**：  
```yaml
metrics:
- type: External
  external:
    metric:
      name: sqs_queue_messages_visible  # AWS SQS 队列积压量
    target:
      type: AverageValue
      averageValue: 50  # 目标积压消息数 ≤ 50
```

---

### 🧩 **四、多指标组合策略**  
HPA 支持同时配置多个指标，按 **"最激进扩缩原则"** 决策（任一指标触发即扩缩）。  
**典型组合方案**：  
1. **CPU + QPS**：防止高并发请求导致 CPU 不足。  
2. **队列积压 + 处理速率**：确保消息及时消费（如 Kafka 消费者组）。  
3. **外部指标 + 资源指标**：混合云场景下的全局弹性。  

**配置示例**：  
```yaml
metrics:
- type: Resource  # CPU 指标
  resource:
    name: cpu
    target:
      type: Utilization
      averageUtilization: 70
- type: Pods      # QPS 指标
  pods:
    metric:
      name: http_requests_per_second
    target:
      type: AverageValue
      averageValue: 200
```

---

### ⚠️ **五、注意事项与最佳实践**  
1. **指标采集依赖**：  
   - 自定义指标需部署 **Prometheus Adapter** 或 **Metrics Server** 扩展组件。  
   - 验证指标可用性：`kubectl get --raw "/apis/custom.metrics.k8s.io/v1beta1"`。  
2. **避免指标冲突**：  
   - 若多指标目标值矛盾（如 CPU 需扩容但 QPS 需缩容），HPA 优先执行扩容。  
3. **冷启动优化**：  
   - 配置 `behavior.scaleUp` 的步长（如每分钟最多扩容 4 个 Pod），防止突发流量过载。  
4. **指标类型选择**：  
   | **指标类型**   | **适用场景**                     | **采集复杂度** |  
   |----------------|----------------------------------|---------------|  
   | Pods           | 业务级实时指标（QPS/延迟）       | 中（需暴露接口） |  
   | Object         | 网关/中间件性能监控              | 高（需对象关联） |  
   | External       | 混合云或第三方服务集成           | 高（需适配器）  |  

---

### 💎 **总结**  
Kubernetes HPA 的自定义指标能力大幅提升了扩缩容的灵活性：  
- **Pods 指标**：适用于业务感知（如 QPS、错误率）。  
- **Object 指标**：优化网关和中间件性能。  
- **External 指标**：实现跨云/混合云弹性。  
**生产建议**：优先使用 **Prometheus + Adapter** 统一采集业务和资源指标，通过多指标组合规避单点瓶颈，并结合 **Cluster Autoscaler** 实现节点层弹性。
