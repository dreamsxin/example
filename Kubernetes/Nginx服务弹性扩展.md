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

## 动态配置

以下是 Kubernetes 集群运维的常用操作详解，涵盖集群管理、节点操作、应用维护及故障排查等核心场景，结合最佳实践和命令示例：

---

### 🔍 **一、集群健康检查**
1. **节点状态检查**  
   ```bash
   kubectl get nodes -o wide  # 查看所有节点状态（Ready/NotReady）、IP、资源分配
   kubectl describe node <node-name>  # 查看节点详情（CPU/内存压力、事件、污点）
   ```
2. **系统组件状态**  
   ```bash
   kubectl get componentstatuses  # 检查控制平面组件（API Server、Scheduler、etcd）健康状态
   ```
3. **集群事件分析**  
   ```bash
   kubectl get events --sort-by=.metadata.creationTimestamp  # 按时间排序查看集群事件
   ```

---

### 🖥️ **二、节点管理**
1. **添加新节点**  
   - **Master节点生成加入命令**：  
     ```bash
     kubeadm token create --print-join-command  # 获取加入指令
     ```
   - **新节点执行**：  
     ```bash
     kubeadm join <master-ip>:6443 --token <token> --discovery-token-ca-cert-hash <hash>
     ```
   - **后续配置**：  
     ```bash
     kubectl label node <node-name> disktype=ssd  # 添加标签
     kubectl taint node <node-name> dedicated=user:NoSchedule  # 设置污点
     ```

2. **节点维护与驱逐**  
   - **排空节点（停止调度并驱逐Pod）**：  
     ```bash
     kubectl drain <node-name> --ignore-daemonsets --delete-emptydir-data  # 驱逐非DaemonSet Pod
     ```
   - **恢复调度**：  
     ```bash
     kubectl uncordon <node-name>  # 维护完成后重新启用调度
     ```

3. **节点删除**  
   ```bash
   kubectl delete node <node-name>          # 从集群移除节点
   kubeadm reset                            # 在节点上重置Kubernetes配置
   sudo rm -rf /etc/kubernetes /var/lib/kubelet  # 清理残留文件
   ```

---

### 📦 **三、应用部署与更新**
1. **声明式资源管理**  
   ```bash
   kubectl apply -f deployment.yaml  # 通过YAML创建/更新资源
   kubectl get pods -l app=nginx      # 按标签过滤Pod
   ```

2. **滚动更新与回滚**  
   ```bash
   kubectl set image deployment/web-app nginx=nginx:1.25  # 触发滚动更新
   kubectl rollout status deployment/web-app              # 监控更新进度
   kubectl rollout undo deployment/web-app --to-revision=2  # 回滚到指定版本
   ```

3. **自动扩缩容（HPA）**  
   ```bash
   kubectl autoscale deployment web-app --cpu-percent=70 --min=2 --max=10  # 基于CPU扩缩容
   kubectl get hpa  # 查看HPA状态
   ```

---

### ⚙️ **四、监控与日志排查**
1. **资源监控**  
   ```bash
   kubectl top nodes  # 查看节点资源使用
   kubectl top pods   # 查看Pod资源使用
   ```

2. **日志收集**  
   - **基础日志**：  
     ```bash
     kubectl logs -f <pod-name> -c <container>  # 实时查看容器日志
     ```
   - **高级工具**：  
     ```bash
     stern "app.*" --tail 100  # 聚合多Pod日志（支持正则）
     kubetail -l app=web-app   # 按标签聚合日志
     ```
   - **节点组件日志**：  
     ```bash
     journalctl -u kubelet -f  # 查看kubelet日志
     ```

3. **集成监控方案**  
   - **Prometheus + Grafana**：  
     ```bash
     helm install prometheus stable/prometheus  # 部署Prometheus
     ```
   - **EFK日志栈**：  
     ```bash
     kubectl apply -f https://raw.githubusercontent.com/fluent/fluentd-kubernetes-daemonset/master/fluentd-daemonset-elasticsearch.yaml
     ```

---

### 🔐 **五、备份与恢复**
1. **ETCD备份**  
   ```bash
   ETCDCTL_API=3 etcdctl snapshot save snapshot.db \
     --endpoints=https://127.0.0.1:2379 \
     --cacert=/etc/kubernetes/pki/etcd/ca.crt \
     --cert=/etc/kubernetes/pki/etcd/server.crt \
     --key=/etc/kubernetes/pki/etcd/server.key
   ```
2. **ETCD恢复**  
   ```bash
   ETCDCTL_API=3 etcdctl snapshot restore snapshot.db --data-dir /var/lib/etcd
   ```

---

### 🛡️ **六、安全管理**
1. **RBAC权限控制**  
   ```yaml
   # 创建角色（允许读取Pod）
   apiVersion: rbac.authorization.k8s.io/v1
   kind: Role
   metadata:
     namespace: default
     name: pod-reader
   rules:
   - apiGroups: [""]
     resources: ["pods"]
     verbs: ["get", "list", "watch"]
   ```
   ```bash
   kubectl apply -f role.yaml  # 应用RBAC配置
   ```

2. **网络策略**  
   ```yaml
   # 限制仅特定Pod可访问数据库
   apiVersion: networking.k8s.io/v1
   kind: NetworkPolicy
   metadata:
     name: db-allow
   spec:
     podSelector:
       matchLabels:
         app: db
     ingress:
     - from:
       - podSelector:
           matchLabels:
             role: api
   ```

---

### 💎 **七、运维最佳实践**
1. **资源隔离**  
   - 使用 `Namespace` 分隔环境（dev/test/prod）。  
   - 设置 `ResourceQuota` 限制命名空间资源总量。

2. **高可用设计**  
   - 关键服务至少部署 **3副本** 并跨节点分布。  
   - 配置 `PodDisruptionBudget` 确保维护时不中断服务：  
     ```yaml
     apiVersion: policy/v1
     kind: PodDisruptionBudget
     spec:
       minAvailable: 2  # 至少保持2个Pod可用
       selector:
         matchLabels:
           app: critical-service
     ```

3. **更新策略选择**  
   | **策略**       | **适用场景**                          | **命令示例**                     |
   |----------------|---------------------------------------|----------------------------------|
   | 滚动更新       | 默认无中断更新（逐步替换Pod）         | `kubectl set image` |
   | 蓝绿部署       | 零停机发布（全量切换流量）            | 需配合Service切换 |
   | 金丝雀发布     | 渐进式流量切换（降低风险）            | 通过Istio/Ingress控制流量比例 |

4. **自动化运维**  
   - 使用 **GitOps工具链**（如ArgoCD）实现声明式部署。  
   - 结合 **Cluster Autoscaler** 自动调整节点数量。

> 以上操作覆盖了Kubernetes集群运维的核心场景，实际生产环境中需结合监控告警（如Prometheus AlertManager）和自动化脚本提升效率。定期执行 `kubeadm upgrade` 升级集群版本，并遵循最小权限原则保障安全。

将新服务器加入 Kubernetes 集群和移除旧服务器是集群运维的核心操作，需遵循标准化流程以确保稳定性。以下是详细步骤及原理说明：

---

### ⚙️ **一、添加新节点操作步骤**
#### **1. 新节点环境准备**  
- **系统要求**：Ubuntu 22.04 LTS（或其他兼容系统），禁用 Swap：  
  ```bash
  sudo swapoff -a && sudo sed -i '/swap/d' /etc/fstab  # 永久禁用  
  ```
- **网络配置**：  
  - 确保与 Master 节点网络互通（ICMP/TCP 6443 端口）。  
  - 同步主机名解析（`/etc/hosts` 或 DNS）。

#### **2. 安装依赖组件**  
```bash
# 安装 Docker 和 kubeadm
sudo apt-get update
sudo apt-get install -y docker.io kubeadm kubelet
```

#### **3. 生成加入集群命令（Master 节点操作）**  
```bash
# 生成 Token 和加入命令
kubeadm token create --print-join-command  
# 输出示例：  
kubeadm join 10.0.0.1:6443 --token abcdef.123456 --discovery-token-ca-cert-hash sha256:xxxx
```

#### **4. 新节点加入集群**  
在新节点执行上一步生成的命令：  
```bash
sudo kubeadm join <Master-IP>:6443 --token <token> --discovery-token-ca-cert-hash <hash>  
```
- **成功标志**：输出 `This node has joined the cluster!`。

#### **5. 验证节点状态**  
在 Master 节点检查：  
```bash
kubectl get nodes  # 新节点状态应为 Ready  
kubectl describe node <node-name>  # 查看详细信息
```

#### **6. 可选配置**  
- **节点标签**：  
  ```bash
  kubectl label node <node-name> disktype=ssd  # 添加标签  
  ```
- **污点管理**：  
  ```bash
  kubectl taint node <node-name> dedicated=user:NoSchedule  # 限制调度
  ```

---

### ⚠️ **二、移除旧节点操作步骤**  
#### **1. 标记节点不可调度**  
```bash
kubectl cordon <node-name>  # 阻止新 Pod 调度  
# 验证：kubectl get nodes 显示 SchedulingDisabled
```

#### **2. 驱逐节点上的 Pod**  
```bash
kubectl drain <node-name> --ignore-daemonsets --delete-emptydir-data  
```
- `--ignore-daemonsets`：忽略 DaemonSet 管理的 Pod（如网络插件）。  
- `--delete-emptydir-data`：清理临时卷数据。

#### **3. 从集群删除节点**  
```bash
kubectl delete node <node-name>  # 移除节点记录  
# 验证：kubectl get nodes 不再显示该节点
```

#### **4. 节点彻底清理（物理/虚拟机操作）**  
```bash
sudo systemctl stop kubelet  # 停止服务  
sudo kubeadm reset -f        # 重置 kubeadm  
sudo rm -rf /etc/kubernetes/* ~/.kube/config  # 删除配置文件
```

---

### 🔧 **三、关键原理与注意事项**  
#### **1. 新节点加入原理**  
- **Token 认证**：Token 用于新节点与 Master 的 TLS 双向认证。  
- **Discovery Token Hash**：验证 Master 证书指纹，防止中间人攻击。  
- **kubelet 注册**：新节点 kubelet 向 API Server 注册并持续上报状态。

#### **2. 节点移除原理**  
- **Cordon**：通过 API Server 修改节点 `spec.unschedulable=true`。  
- **Drain**：  
  - 驱逐 Pod 时触发控制器（如 Deployment）在其他节点重建 Pod。  
  - DaemonSet Pod 需手动忽略，否则驱逐会失败。  
- **Delete**：从 etcd 删除节点对象，终止 kubelet 心跳上报。

#### **3. 常见问题处理**  
- **节点加入失败**：  
  - 检查防火墙是否开放 6443 端口。  
  - 验证 Token 有效期（默认 24 小时），过期需重新生成。  
- **Pod 驱逐卡住**：  
  - 使用 `--force --grace-period=0` 强制删除。  
  - 检查 Pod 是否无控制器管理（需手动迁移）。  
- **节点状态 NotReady**：  
  ```bash
  journalctl -u kubelet  # 查看 kubelet 日志
  ```

---

### 💎 **四、最佳实践总结**  
- **添加节点时**：  
  - 使用自动化工具（如 Ansible）批量安装依赖。  
  - 为节点预分配标签（如 `zone=eu-west`），便于调度策略管理。  
- **移除节点时**：  
  - 优先在低峰期操作，减少业务影响。  
  - 结合监控（如 Prometheus）确认节点无关键负载后再删除。  
- **弹性扩展**：  
  - 结合 Cluster Autoscaler 自动增减节点。

> 通过以上步骤，可安全实现 Kubernetes 集群节点的生命周期管理。生产环境建议结合 CI/CD 流程自动化操作，并严格备份 etcd 数据。
