## K8s

在Kubernetes (K8s) 上部署服务，核心步骤包括准备K8s集群、将应用容器化、编写Kubernetes配置文件，然后部署到集群并管理。

### 🗺️ Kubernetes部署路线图

下表概述了在K8s中部署服务的主要阶段和核心任务：

| 阶段 | 核心任务 | 关键组件/概念 |
| :--- | :--- | :--- |
| **1. 环境准备** | 搭建K8s集群，准备容器镜像仓库 | K8s集群 (Master/Worker节点)、Docker、kubeadm/kubectl |
| **2. 应用容器化** | 将应用及其依赖打包成标准镜像 | Dockerfile、Docker镜像 |
| **3. 定义部署** | 使用YAML文件描述应用如何在K8s中运行 | Deployment、Service、ConfigMap/Secret、PersistentVolume (PV)/PersistentVolumeClaim (PVC) |
| **4. 部署与验证** | 将应用部署到集群并验证其运行状态 | `kubectl apply`、`kubectl get`、`kubectl describe`、`kubectl logs` |
| **5. 运维与拓展** | 监控、扩缩容、更新应用 | `kubectl scale`、Horizontal Pod Autoscaler (HPA)、`kubectl rollout`、Helm、CI/CD (如GitLab CI, Jenkins) |

### ⚙️ 详细部署步骤

1.  **准备K8s集群**
    *   **搭建集群**：你可以使用kubeadm等工具从头搭建集群，过程包括初始化Master节点、将Worker节点加入集群以及安装网络插件（如Flannel、Calico）。对于开发和测试，也可以使用Minikube或Kind快速创建单节点集群。
    *   **配置kubectl**：在Master节点上，将admin配置文件复制到用户目录下的`.kube`文件夹，并设置正确的权限。这样你就可以使用`kubectl`命令来管理集群了。
    *   **验证集群**：使用 `kubectl get nodes` 命令检查所有节点状态是否为 `Ready`。

2.  **将应用容器化**
    *   **编写Dockerfile**：创建一个文本文件，命名为`Dockerfile`，定义应用的基础镜像、工作目录、复制文件、安装依赖、暴露端口和启动命令。
    *   **构建和推送镜像**：使用 `docker build` 命令构建Docker镜像，然后使用 `docker push` 将镜像推送到镜像仓库（如Docker Hub、阿里云容器镜像服务等）。

3.  **编写K8s配置文件**
    K8s通常使用YAML文件来定义各种资源。以下是几个核心配置文件的概览：
    <table>
        <thead>
            <tr>
                <th>配置文件</th>
                <th>主要作用</th>
                <th>关键字段示例</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td><b>Deployment (deployment.yaml)</b></td>
                <td>定义无状态应用的部署，如副本数量、更新策略、使用的镜像等。</td>
                <td><code>replicas</code>, <code>selector</code>, <code>template</code> (定义Pod), <code>containers.image</code></td>
            </tr>
            <tr>
                <td><b>Service (service.yaml)</b></td>
                <td>为一组Pod提供稳定的网络访问入口，实现负载均衡。</td>
                <td><code>selector</code> (匹配Pod标签), <code>ports</code> (端口映射), <code>type</code> (如ClusterIP, LoadBalancer)</td>
            </tr>
            <tr>
                <td><b>ConfigMap & Secret</b></td>
                <td>ConfigMap用于存储应用配置，Secret用于存储敏感信息（如密码、密钥）。</td>
                <td><code>data</code> (键值对配置信息)</td>
            </tr>
        </tbody>
    </table>

    *   一个基本的Deployment配置示例：
        ```yaml
        apiVersion: apps/v1
        kind: Deployment
        metadata:
          name: my-app-deployment
        spec:
          replicas: 3
          selector:
            matchLabels:
              app: my-app
          template:
            metadata:
              labels:
                app: my-app
            spec:
              containers:
              - name: my-app
                image: myrepository/my-app:latest # 请替换为你的镜像地址
                ports:
                - containerPort: 3000
        ```
    *   一个基本的Service配置示例：
        ```yaml
        apiVersion: v1
        kind: Service
        metadata:
          name: my-app-service
        spec:
          selector:
            app: my-app
          ports:
            - protocol: TCP
              port: 80
              targetPort: 3000
          type: LoadBalancer
        ```

4.  **部署与验证**
    *   **部署应用**：使用 `kubectl apply -f <yaml文件>` 命令来部署Deployment和Service。
    *   **检查状态**：
        *   查看Pod：`kubectl get pods`
        *   查看Service：`kubectl get svc`
        *   查看Deployment：`kubectl get deployment`
    *   **排查问题**：
        *   查看Pod详细描述：`kubectl describe pod <pod-name>`
        *   查看Pod日志：`kubectl logs <pod-name>`

5.  **运维与拓展**
    *   **扩缩容**：
        *   **手动**：使用 `kubectl scale deployment <deployment-name> --replicas=<number>` 命令。
        *   **自动**：配置Horizontal Pod Autoscaler (HPA)，根据CPU使用率等指标自动调整Pod数量。
    *   **应用更新**：
        *   **滚动更新**：修改Deployment的镜像版本后，使用 `kubectl apply` 即可触发，K8s会逐步替换旧Pod。
        *   **回滚**：如果更新出现问题，使用 `kubectl rollout undo deployment/<deployment-name>` 快速回滚到上一个版本。
    *   **使用Helm**：Helm是K8s的包管理工具，可以帮你简化复杂应用的部署和管理。
    *   **CI/CD**：结合Jenkins、GitLab CI等工具，可以实现从代码提交到自动构建镜像并部署到K8s的流水线。

### 💡 部署有状态服务（如ClickHouse）

*   **使用StatefulSet**：StatefulSet更适合有状态服务，它能提供稳定的网络标识（如`clickhouse-0`，`clickhouse-1`）和有序的部署、扩缩容。
*   **数据持久化**：必须配置**持久化存储（PersistentVolume, PV 和 PersistentVolumeClaim, PVC）**，确保Pod重启或迁移后数据不丢失。
*   **使用Operator**：生产环境强烈建议使用**ClickHouse Operator**。Operator通过自定义资源（CRD）和控制器，能极大地简化ClickHouse集群的创建、配置、备份、扩缩容等运维操作的复杂度。安装Operator通常可以通过Helm完成。
