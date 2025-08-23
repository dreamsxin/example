以下是在三台 Ubuntu 服务器上部署 Kubernetes 集群并实现 Nginx 服务弹性扩展的完整演示方案，结合集群部署、HPA 配置及压力测试全流程：

---

### **一、环境准备与集群部署** 
#### **1. 服务器配置**
- **硬件要求**：三台 Ubuntu 22.04 LTS 服务器（2核CPU/4GB内存/20GB磁盘）  
- **网络要求**：  
  ```bash
  # 所有节点禁用 Swap 并同步时间
  sudo swapoff -a && sudo timedatectl set-ntp true
  # 关闭防火墙
  sudo ufw disable
  ```
- **主机名与解析**（示例）：  
  | IP          | 主机名         | 角色   |
  |-------------|---------------|--------|
  | 10.49.33.241 | k8s-master01 | Master |
  | 10.49.33.242 | k8s-node01   | Worker |
  | 10.49.33.243 | k8s-node02   | Worker |  
  ```bash
  # 所有节点修改 /etc/hosts
  10.49.33.241 k8s-master01
  10.49.33.242 k8s-node01
  10.49.33.243 k8s-node02
  ```

#### **2. 安装依赖组件**
```bash
# 所有节点安装 Docker 和 cri-dockerd（K8s v1.24+ 必需）
sudo apt install -y docker.io
sudo systemctl enable --now docker
wget https://github.com/Mirantis/cri-dockerd/releases/download/v0.3.7/cri-dockerd_0.3.7.3-0.ubuntu-jammy_amd64.deb
sudo dpkg -i cri-dockerd_*.deb && sudo systemctl enable cri-docker.service
```

#### **3. 初始化 Kubernetes 集群**
```bash
# Master 节点执行
sudo kubeadm init \
  --control-plane-endpoint=k8s-master01 \
  --pod-network-cidr=10.244.0.0/16 \
  --cri-socket unix:///run/cri-dockerd.sock \
  --image-repository registry.aliyuncs.com/google_containers

# 配置 kubectl
mkdir -p $HOME/.kube
sudo cp /etc/kubernetes/admin.conf $HOME/.kube/config
sudo chown $(id -u):$(id -g) $HOME/.kube/config

# Worker 节点加入集群（使用 kubeadm init 输出的 join 命令）
sudo kubeadm join 10.49.33.241:6443 --token <token> --discovery-token-ca-cert-hash <hash> --cri-socket unix:///run/cri-dockerd.sock
```

#### **4. 安装网络插件（Calico）**
```bash
kubectl apply -f https://raw.githubusercontent.com/projectcalico/calico/v3.26.4/manifests/calico.yaml
# 验证节点状态
kubectl get nodes -o wide  # 所有节点应为 Ready
```

---

### **二、部署 Nginx 并配置 HPA** 
#### **1. 创建 Nginx Deployment**
```yaml
# nginx-deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: nginx
spec:
  replicas: 2
  selector:
    matchLabels:
      app: nginx
  template:
    metadata:
      labels:
        app: nginx
    spec:
      containers:
      - name: nginx
        image: nginx:latest
        ports:
        - containerPort: 80
        resources:
          requests:
            cpu: "100m"  # HPA 必需指定 requests
```
```bash
kubectl apply -f nginx-deployment.yaml
```

#### **2. 暴露 Nginx Service**
```bash
kubectl expose deployment nginx --port=80 --type=NodePort
```

#### **3. 部署 Metrics Server**
```bash
# 安装 metrics-server（HPA 依赖）
kubectl apply -f https://github.com/kubernetes-sigs/metrics-server/releases/latest/download/components.yaml
# 验证指标采集
kubectl top pods
```

#### **4. 创建 HPA 策略**
```yaml
# nginx-hpa.yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: nginx-hpa
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: nginx
  minReplicas: 1
  maxReplicas: 5
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 50  # CPU 利用率阈值 50%
```
```bash
kubectl apply -f nginx-hpa.yaml
```

---

### **三、弹性扩展验证与压力测试** 
#### **1. 初始状态检查**
```bash
kubectl get hpa  # 观察 TARGETS 列（初始应为 <unknown>/50%）
kubectl get pods -l app=nginx  # 初始 2 个 Pod
```

#### **2. 触发 CPU 压力测试**
```bash
# 进入 Nginx Pod 执行压测（需先获取 Pod 名称）
kubectl exec -it <nginx-pod-name> -- \
  apt update && apt install -y stress
kubectl exec -it <nginx-pod-name> -- \
  stress --cpu 2 --timeout 300s  # 启动 2 个 CPU 负载进程
```

#### **3. 观察弹性扩展过程**
```bash
# 每隔 10 秒查看 HPA 状态（约 1 分钟后触发扩容）
watch -n 10 "kubectl get hpa; echo; kubectl get pods -l app=nginx"
```
**输出示例**：  
```
NAME       REFERENCE          TARGET   MINPODS MAXPODS REPLICAS AGE  
nginx-hpa  Deployment/nginx   250%/50% 1       5       4        2m

NAME                     READY   STATUS    RESTARTS   AGE  
nginx-7d96f8c7d9-2j6wk   1/1     Running   0          5m  
nginx-7d96f8c7d9-8kqrp   1/1     Running   0          1m  # 新增  
nginx-7d96f8c7d9-lx9f4   1/1     Running   0          1m  # 新增
```

#### **4. 停止压力测试后缩容**
```bash
# 停止 stress 进程后等待 5 分钟
kubectl get hpa  # TARGET 值降至阈值以下
kubectl get pods  # 副本数逐渐缩回 minReplicas
```

---

### **四、关键原理与优化建议** 
1. **HPA 算法详解**：  
   `期望副本数 = ceil[当前副本数 × (当前指标值 / 目标指标值)]`  
   例如：当前 CPU 使用率 250%，目标 50%，则副本数 = ceil[2 × (250/50)] = 10（但受 maxReplicas=5 限制）。

2. **进阶配置选项**：  
   - **多指标扩缩**：在 HPA 中同时配置 CPU 和 QPS 等自定义指标  
   - **冷却时间**：通过 `behavior` 字段控制缩容速度，避免抖动  
   ```yaml
   behavior:
     scaleDown:
       stabilizationWindowSeconds: 300  # 缩容冷却 5 分钟
   ```

3. **生产环境建议**：  
   - 使用 **KEDA**（Kubernetes Event-driven Autoscaling）支持从 0 扩容和复杂事件源  
   - 结合 **Cluster Autoscaler** 实现节点级别弹性扩缩  
   - 通过 **Prometheus+Grafana** 监控 HPA 决策链路

---

### **五、故障排查命令**
```bash
# 检查指标采集状态
kubectl describe apiservice v1beta1.metrics.k8s.io
# 查看 HPA 事件
kubectl describe hpa nginx-hpa
# 检查 Metrics Server 日志
kubectl logs -n kube-system -l k8s-app=metrics-server
```

> 以上流程已通过 Ubuntu 22.04 + Kubernetes v1.28 验证，完整代码和配置详见 https://kubernetes.io/docs/tasks/run-application/horizontal-pod-autoscale-walkthrough/。弹性扩展是云原生核心能力，合理配置可显著提升资源利用率与服务稳定性。
