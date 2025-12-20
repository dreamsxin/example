# Cilium

- https://github.com/cilium/cilium

## 一、Cilium 核心架构

```
+---------------------------+
|    Cilium Agent (Daemon)  |
|   (每个节点运行)           |
+---------------------------+
|     eBPF 程序             |  ← 内核空间
|    • 网络转发             |
|    • 安全策略             |
|    • 负载均衡             |
+---------------------------+
|    Cilium Operator        |  ← 集群级管理
+---------------------------+
```

## 二、安装配置

### 1. 准备工作
```bash
# 系统要求
- Kubernetes 1.16+
- Linux 内核 4.19+（推荐 5.10+）
- 禁用其他 CNI 插件

# 检查内核版本
uname -r
cat /proc/version

# 启用内核参数
echo "net.ipv4.ip_forward=1" | sudo tee -a /etc/sysctl.conf
echo "net.core.rmem_max=134217728" | sudo tee -a /etc/sysctl.conf
sudo sysctl -p
```

### 2. Helm 安装（推荐）
```yaml
# values.yaml 配置文件示例
helm repo add cilium https://helm.cilium.io/
helm repo update

# 基础配置
cat > cilium-values.yaml << EOF
kubeProxyReplacement: strict  # 替换 kube-proxy
k8sServiceHost: API_SERVER_IP
k8sServicePort: 6443

ipam:
  mode: kubernetes  # IP 分配模式

routingMode: native  # 路由模式
tunnel: disabled     # 禁用隧道（需要网络支持）

loadBalancer:
  mode: dsr          # 直接服务器返回模式

hubble:
  enabled: true      # 启用可观测性
  metrics:
    enabled: [dns,drop,tcp,flow,port-distribution,icmp,http]
  relay:
    enabled: true
  ui:
    enabled: true
EOF

# 安装命令
helm install cilium cilium/cilium \
  --namespace kube-system \
  --values cilium-values.yaml
```

## 三、核心配置参数

### 1. 网络模式配置
```yaml
# 隧道模式（VXLAN/Geneve）
tunnel: vxlan
# 或 native（BGP/直接路由）
routingMode: native
autoDirectNodeRoutes: true

# IPAM 配置
ipam:
  mode: "kubernetes"  # 或 "cluster-pool", "azure", "aws"
  operator:
    clusterPoolIPv4PodCIDR: "10.0.0.0/8"
    clusterPoolIPv4MaskSize: 24
```

### 2. 负载均衡配置
```yaml
loadBalancer:
  # 算法模式
  algorithm: "maglev"  # 或 "random"
  
  # DSR 配置
  mode: "dsr"
  acceleration: "native"  # XDP 加速
  
  # 服务类型
  serviceTypes:
    - "ClusterIP"
    - "NodePort"
    - "LoadBalancer"
    - "ExternalIPs"
```

### 3. 安全策略配置
```yaml
# NetworkPolicy 增强
enableL7Proxy: true
enableEnvoyConfig: true

# 加密配置
enableEncryption: true
encryption:
  type: "wireguard"  # 或 "ipsec"
  nodeEncryption: false
```

## 四、CiliumNetworkPolicy 配置示例

### 1. L3/L4 策略
```yaml
apiVersion: "cilium.io/v2"
kind: CiliumNetworkPolicy
metadata:
  name: "web-allow-api"
spec:
  endpointSelector:
    matchLabels:
      app: web
  ingress:
  - fromEndpoints:
    - matchLabels:
        app: api
    toPorts:
    - ports:
      - port: "80"
        protocol: TCP
```

### 2. L7 应用层策略
```yaml
apiVersion: "cilium.io/v2"
kind: CiliumNetworkPolicy
metadata:
  name: "api-http-policy"
spec:
  endpointSelector:
    matchLabels:
      app: api
  ingress:
  - toPorts:
    - ports:
      - port: "8080"
        protocol: TCP
      rules:
        http:
        - method: "GET"
          path: "/api/v1/*"
        - method: "POST"
          path: "/api/v1/users"
```

### 3. DNS 策略
```yaml
apiVersion: "cilium.io/v2"
kind: CiliumNetworkPolicy
metadata:
  name: "dns-policy"
spec:
  endpointSelector:
    matchLabels:
      app: frontend
  egress:
  - toEndpoints:
    - matchLabels:
        "k8s:io.kubernetes.pod.namespace": kube-system
        "k8s:k8s-app": kube-dns
    toPorts:
      - ports:
        - port: "53"
          protocol: UDP
        rules:
          dns:
            - matchPattern: "*.cluster.local"
```

## 五、Hubble 可观测性配置

### 1. Hubble 部署
```yaml
# values.yaml 追加
hubble:
  enabled: true
  metrics:
    enableOpenMetrics: true
    enabled:
      - "flow:source=workload;destination=workload"
      - "http:source=workload;destination=workload"
      - "dns:query;source=workload;destination=workload"
      - "drop:source=workload;destination=workload"
  
  # UI 配置
  ui:
    enabled: true
    ingress:
      enabled: true
      annotations:
        kubernetes.io/ingress.class: nginx
      hosts:
        - hubble.example.com
```

### 2. 流量监控示例
```bash
# 查看实时流量
hubble observe

# 过滤特定命名空间
hubble observe -n kube-system

# 查看 HTTP 流量
hubble observe --type l7 --protocol http

# 查看策略拒绝的流量
hubble observe --verdict DROPPED
```

## 六、高级功能配置

### 1. BGP 配置（用于 MetalLB）
```yaml
bgp:
  enabled: true
  announce:
    loadbalancerIP: true
    podCIDR: true
  peers:
  - peerAddress: "192.168.1.1"
    peerASN: 64512
    myASN: 64513
```

### 2. 服务网格集成
```yaml
# Cilium Service Mesh
mesh:
  enabled: true
  # 与 Istio 集成
  sidecar:
    enabled: false  # 使用无 sidecar 模式
  
  # 透明代理
  transparent:
    enabled: true
    redirectType: "proxy"
```

### 3. 集群网格（多集群）
```yaml
clusterMesh:
  enabled: true
  useAPIServer: true
  config:
    enabled: true
  # 集群连接配置
  serviceType: LoadBalancer
```

## 七、性能调优配置

### 1. eBPF 优化
```yaml
# 资源限制
resources:
  requests:
    cpu: 100m
    memory: 512Mi
  limits:
    cpu: 1000m
    memory: 2Gi

# eBPF 映射大小
bpf:
  mapDynamicSizeRatio: "0.0025"  # 系统内存的 0.25%
  
# 连接跟踪
conntrack:
  gcInterval: "30s"
```

### 2. XDP 加速
```yaml
bandwidthManager:
  enabled: true
  
bpf:
  masquerade: true
  hostLegacyRouting: false
  
xdp:
  enabled: true
  acceleration: native
```

## 八、故障排查配置

### 1. 调试模式
```yaml
debug:
  enabled: true
  # 日志级别
  logLevel: "debug"
  
  # eBPF 验证
  bpfVerifier: "verbose"
  
# Prometheus 指标
prometheus:
  enabled: true
  port: 9090
  serviceMonitor:
    enabled: true
```

### 2. 诊断命令
```bash
# 检查安装状态
cilium status --verbose

# 检查网络连接
cilium connectivity test

# 查看端点状态
cilium endpoint list

# 查看策略状态
cilium policy get

# 生成诊断包
cilium bugtool
```

## 九、配置验证清单

1. **基础检查**
   - 所有 Cilium Pod 处于 Running 状态
   - 集群网络策略正常工作
   - Pod 间通信正常

2. **性能检查**
   - eBPF 程序加载成功
   - 连接跟踪正常
   - 负载均衡生效

3. **安全检查**
   - 网络策略正确执行
   - 加密通信正常
   - DNS 安全策略生效

4. **可观测性检查**
   - Hubble 流量可见
   - 指标正常收集
   - Grafana 仪表板正常

## 十、最佳实践建议

1. **生产环境建议**
   - 使用 Helm 进行版本管理
   - 启用高可用模式
   - 配置资源限制
   - 定期备份 Cilium 配置

2. **升级策略**
   - 先升级次要版本
   - 测试环境验证
   - 使用 Helm 回滚功能

3. **监控告警**
   - 监控 eBPF map 使用率
   - 设置 Hubble 丢弃流量告警
   - 监控策略执行情况

Cilium 的配置非常灵活，建议根据实际使用场景选择合适的配置。详细配置请参考 [官方文档](https://docs.cilium.io)。
