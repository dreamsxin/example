[citation:X]。

以下是使用 StatefulSet 结合 Headless Service 实现稳定 Pod 网络标识的完整步骤，从集群构建到验证：

---

### **一、Kubernetes 集群构建**
1. **环境准备**  
   - **操作系统**：Ubuntu 22.04/CentOS 8（禁用交换分区 `swapoff -a`）  
   - **节点要求**：  
     - Master：2核 CPU，4GB 内存  
     - Worker：2核 CPU，2GB 内存（至少 2 个 Worker 节点）  
   - **网络配置**：节点间互通，开放端口（6443、10250 等）  

2. **安装依赖**  
   ```bash
   # 所有节点安装 Docker、kubeadm、kubelet
   sudo apt-get update
   sudo apt-get install -y docker.io kubeadm kubelet kubectl
   sudo systemctl enable docker kubelet
   ```

3. **初始化 Master 节点**  
   ```bash
   sudo kubeadm init --pod-network-cidr=10.244.0.0/16
   mkdir -p $HOME/.kube
   sudo cp /etc/kubernetes/admin.conf $HOME/.kube/config
   sudo chown $(id -u):$(id -g) $HOME/.kube/config
   ```

4. **部署网络插件（Flannel）**  
   ```bash
   kubectl apply -f https://raw.githubusercontent.com/coreos/flannel/master/Documentation/kube-flannel.yml
   ```

5. **加入 Worker 节点**  
   ```bash
   # 在 Worker 节点执行 Master 初始化后输出的 join 命令，例如：
   sudo kubeadm join <Master-IP>:6443 --token <token> --discovery-token-ca-cert-hash sha256:<hash>
   ```

6. **验证集群**  
   ```bash
   kubectl get nodes  # 所有节点状态应为 Ready
   kubectl get pods -A  # 检查系统 Pod 是否运行正常
   ```

---

### **二、配置 Headless Service**
**作用**：提供稳定的 DNS 记录，直接暴露 Pod IP（无负载均衡）。  
**YAML 配置** (`headless-service.yaml`)：  
```yaml
apiVersion: v1
kind: Service
metadata:
  name: stateful-app-service
spec:
  clusterIP: None  # 关键：定义为 Headless Service
  selector:
    app: stateful-app  # 匹配 StatefulSet 的 Pod 标签
  ports:
  - port: 80
    name: web
  type: ClusterIP
```
**部署命令**：  
```bash
kubectl apply -f headless-service.yaml
```

---

### **三、部署 StatefulSet**
**作用**：为每个 Pod 分配固定名称、顺序部署、独立存储。  
**YAML 配置** (`statefulset.yaml`)：  
```yaml
apiVersion: apps/v1
kind: StatefulSet
metadata:
  name: stateful-app
spec:
  serviceName: "stateful-app-service"  # 绑定 Headless Service
  replicas: 3
  selector:
    matchLabels:
      app: stateful-app
  template:
    metadata:
      labels:
        app: stateful-app
    spec:
      containers:
      - name: nginx
        image: nginx:1.20
        ports:
        - containerPort: 80
          name: web
        volumeMounts:
        - name: data
          mountPath: /usr/share/nginx/html
  volumeClaimTemplates:  # 每个 Pod 独立存储
  - metadata:
      name: data
    spec:
      accessModes: ["ReadWriteOnce"]
      storageClassName: "standard"
      resources:
        requests:
          storage: 1Gi
```
**部署命令**：  
```bash
kubectl apply -f statefulset.yaml
```

---

### **四、验证稳定网络标识**
1. **Pod 名称与启动顺序**  
   ```bash
   kubectl get pods -l app=stateful-app
   ```
   - 输出示例（按顺序创建）：  
     ```
     stateful-app-0    Running
     stateful-app-1    Running
     stateful-app-2    Running
     ```

2. **DNS 解析验证**  
   ```bash
   # 启动临时工具 Pod
   kubectl run dns-test --image=busybox:1.28 --rm -it --restart=Never -- /bin/sh
   ```
   - **查询 Headless Service 的 DNS 记录**：  
     ```bash
     nslookup stateful-app-service
     ```
     返回所有 Pod IP（如 `10.244.1.10`, `10.244.2.11`）。  
   - **查询单个 Pod 的 DNS 记录**：  
     ```bash
     nslookup stateful-app-0.stateful-app-service.default.svc.cluster.local
     ```
     返回固定 IP（格式：`<pod-name>.<service>.<namespace>.svc.cluster.local`）。

3. **数据持久性测试**  
   - 在 `stateful-app-0` 写入数据：  
     ```bash
     kubectl exec stateful-app-0 -- sh -c "echo 'Pod0 Data' > /usr/share/nginx/html/index.html"
     ```
   - 删除 Pod 并验证数据保留：  
     ```bash
     kubectl delete pod stateful-app-0
     kubectl exec stateful-app-0 -- cat /usr/share/nginx/html/index.html  # 应输出 "Pod0 Data"
     ```

---

### **五、关键机制解析**
| **特性**               | **实现方式**                                                                 |
|------------------------|-----------------------------------------------------------------------------|
| **稳定 Pod 名称**       | StatefulSet 按索引命名（如 `web-0`, `web-1`），重建后不变。 |
| **固定 DNS 记录**       | Headless Service 为每个 Pod 生成 A 记录（`pod-name.service-name`）。 |
| **有序部署/扩缩容**     | Pod 按索引顺序创建（0→N-1）、逆序删除（N-1→0）。         |
| **独立存储**           | `volumeClaimTemplates` 为每个 Pod 生成专用 PVC（如 `data-stateful-app-0`）。 |

---

### **六、最佳实践**
1. **探针配置**：  
   - 使用 `startupProbe` 处理慢启动容器（如数据库初始化）。  
   ```yaml
   startupProbe:
     httpGet:
       path: /health
       port: 80
     failureThreshold: 30
     periodSeconds: 10
   ```

2. **更新策略**：  
   - **滚动更新（RollingUpdate）**：默认策略，按逆序更新 Pod。  
   - **分区更新**：通过 `partition` 参数分段更新（如先更新索引 > N 的 Pod）。  

3. **存储优化**：  
   - 使用 SSD 存储类提升 I/O 性能。  
   - 定期备份 PV 数据（如 Velero 工具）。  

4. **网络策略**：  
   - 结合 `NetworkPolicy` 限制 Pod 间通信，增强安全性。  

> 通过以上步骤，StatefulSet + Headless Service 可为有状态应用（如 MySQL、ZooKeeper）提供稳定的网络标识和存储，确保集群高可用。
