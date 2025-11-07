## K3s

在K3s中部署一个可扩展的gRPC搜索服务和ClickHouse集群，确实是一个兼顾性能和数据处理的方案。下面我将为你梳理一套详细的部署和扩容操作流程。

### 🎯 整体架构概述

在开始之前，理解整个架构的核心组成部分和它们之间的关系非常重要。

| **组件** | **角色描述** |
| :--- | :--- |
| **K3s集群** | 轻量级的Kubernetes发行版，作为所有服务运行的底座。 |
| **gRPC Search服务** | 业务应用，提供搜索功能。它将被封装在Docker镜像中，通过K3s的Deployment和Service进行部署和暴露。 |
| **ClickHouse集群** | 高性能列式数据库，用于存储和分析搜索服务产生的数据。在K3s内通常以StatefulSet方式部署，确保有状态服务的稳定性。 |

### 🛠️ 部署准备与K3s安装

首先，你需要准备好K3s环境。

1.  **安装K3s Server (控制节点)**
    在一台作为控制节点的服务器上执行以下命令。这里使用国内镜像加速，适合中国用户。
    ```bash
    curl -sfL https://rancher-mirror.rancher.cn/k3s/k3s-install.sh | INSTALL_K3S_MIRROR=cn sh -
    ```
    安装完成后，K3s会自动运行，你可以使用 `sudo cat /etc/rancher/k3s/k3s.yaml` 获取管理集群的kubeconfig文件。

2.  **加入Agent节点 (可选)**
    如果你有多个节点，可以在工作节点上使用从Server节点获取的Token和URL加入到集群。
    ```bash
    curl -sfL https://rancher-mirror.rancher.cn/k3s/k3s-install.sh | INSTALL_K3S_MIRROR=cn K3S_URL=https://<server_ip>:6443 K3S_TOKEN=<your_token> sh -
    ```
    **提示**：`<server_ip>` 需要替换为K3s Server节点的IP地址，`<your_token>` 通常位于Server节点的 `/var/lib/rancher/k3s/server/node-token` 文件内。

### 📦 部署gRPC Search服务

我们将把gRPC服务打包并部署到K3s中。

1.  **制作服务镜像**：为你的gRPC Search服务编写Dockerfile，构建成Docker镜像，并推送到某个可访问的镜像仓库（如Docker Hub、阿里云容器镜像服务等）。

2.  **创建Kubernetes部署文件**：创建一个YAML文件（例如 `search-deployment.yaml`），定义Deployment和Service。
    ```yaml
    # search-deployment.yaml
    apiVersion: apps/v1
    kind: Deployment
    metadata:
      name: search-service
    spec:
      replicas: 2  # 初始副本数
      selector:
        matchLabels:
          app: search-service
      template:
        metadata:
          labels:
            app: search-service
        spec:
          containers:
          - name: search
            image: your-registry/your-search-image:tag  # 请替换为你的实际镜像
            ports:
            - containerPort: 50051  # 假设gRPC服务在此端口监听
            env:
            - name: CLICKHOUSE_HOST  # 通过环境变量传递ClickHouse连接信息
              value: "clickhouse-service"
    ---
    apiVersion: v1
    kind: Service
    metadata:
      name: search-service
    spec:
      selector:
        app: search-service
      ports:
      - port: 50051
        targetPort: 50051
      type: ClusterIP  # 集群内部访问，如果需要外部访问可考虑NodePort或LoadBalancer
    ```

3.  **部署到K3s**：使用 `kubectl` 应用这个配置文件。
    ```bash
    kubectl apply -f search-deployment.yaml
    ```

### 🗄️ 部署ClickHouse集群

在K3s上部署一个多节点的ClickHouse集群，需要配置分片和副本。

1.  **准备配置文件**：ClickHouse的配置较为复杂，主要涉及 `config.xml` 和 `users.xml`。建议使用ConfigMap来管理这些配置，并挂载到容器中。以下是一个关键配置的示例，它定义了集群拓扑和ZooKeeper/ClickHouse Keeper的地址。
    ```yaml
    # clickhouse-configmap.yaml
    apiVersion: v1
    kind: ConfigMap
    metadata:
      name: clickhouse-config
    data:
      remote_servers.xml: |
        <yandex>
          <remote_servers>
            <my_cluster>  <!-- 集群名称 -->
              <shard>  <!-- 第一个分片 -->
                <internal_replication>true</internal_replication>
                <replica>
                  <host>clickhouse-0.clickhouse-service.default.svc.cluster.local</host>
                  <port>9000</port>
                </replica>
                <replica>
                  <host>clickhouse-1.clickhouse-service.default.svc.cluster.local</host>
                  <port>9000</port>
                </replica>
              </shard>
              <!-- 可以根据需要添加更多分片 -->
            </my_cluster>
          </remote_servers>
      zookeeper.xml: |
        <yandex>
          <zookeeper>
            <node index="1">
              <host>your-zookeeper-host</host>  <!-- 替换为你的ZK或Keeper地址 -->
              <port>2181</port>
            </node>
          </zookeeper>
      config.xml: |
        <yandex>
          <!-- 包含其他配置，如日志、端口等 -->
          <listen_host>0.0.0.0</listen_host>
          <http_port>8123</http_port>
          <tcp_port>9000</tcp_port>
        </yandex>
    ```
    **重要**：你需要提供一个协调服务，如ZooKeeper或ClickHouse Keeper，来管理复制表的元数据。对于生产环境，建议运行至少三个ClickHouse Keeper节点以确保高可用。

2.  **创建StatefulSet**：由于ClickHouse是有状态服务，使用StatefulSet是最佳选择。每个Pod需要有独立的存储和稳定的网络标识。
    ```yaml
    # clickhouse-statefulset.yaml
    apiVersion: apps/v1
    kind: StatefulSet
    metadata:
      name: clickhouse
    spec:
      serviceName: "clickhouse-service"
      replicas: 2  # 节点数量，应与配置中的副本数匹配
      selector:
        matchLabels:
          app: clickhouse
      template:
        metadata:
          labels:
            app: clickhouse
        spec:
          containers:
          - name: clickhouse
            image: clickhouse/clickhouse-server:latest
            ports:
            - containerPort: 9000
            - containerPort: 8123
            volumeMounts:
            - name: config
              mountPath: /etc/clickhouse-server/config.d/remote_servers.xml
              subPath: remote_servers.xml
            - name: config
              mountPath: /etc/clickhouse-server/config.d/zookeeper.xml
              subPath: zookeeper.xml
            - name: data
              mountPath: /var/lib/clickhouse
          volumes:
          - name: config
            configMap:
              name: clickhouse-config
      volumeClaimTemplates:  # 为每个Pod动态创建持久化存储
      - metadata:
          name: data
        spec:
          accessModes: [ "ReadWriteOnce" ]
          resources:
            requests:
              storage: 10Gi
    ---
    apiVersion: v1
    kind: Service
    metadata:
      name: clickhouse-service
    spec:
      clusterIP: None  # 使用Headless Service，直接通过Pod DNS解析
      selector:
        app: clickhouse
      ports:
      - name: tcp
        port: 9000
      - name: http
        port: 8123
    ```

3.  **部署ClickHouse集群**：
    ```bash
    kubectl apply -f clickhouse-configmap.yaml
    kubectl apply -f clickhouse-statefulset.yaml
    ```

### 📈 服务扩容操作

当业务增长时，你可以轻松地对各个组件进行水平扩容。

1.  **扩容gRPC Search服务**
    这是最简单的部分，直接修改Deployment的副本数量即可。这适用于无状态服务扩容。
    ```bash
    kubectl scale deployment search-service --replicas=5
    ```

2.  **扩容ClickHouse集群**
    扩容有状态的ClickHouse需要更多注意。
    - **增加节点**：修改StatefulSet的 `replicas` 字段。
      ```bash
      kubectl scale statefulset clickhouse --replicas=3
      ```
    - **更新集群配置**：**非常重要！** 新的Pod启动后，你需要更新之前创建的ConfigMap中的 `remote_servers.xml`，将新节点的信息（如 `clickhouse-2.clickhouse-service...`）添加到相应的分片或新分片中，然后重启ClickHouse Pods以使配置生效。
    - **注意事项**：水平扩缩容过程中，应**禁止进行DDL操作**（如创建、删除表）。完成后集群可能会进行高频的数据合并（Merge）操作，导致IO使用率暂时上升。

### 💎 一些关键提示

- **网络与发现**：在K3s集群内，服务间可以通过 `<service-name>.<namespace>.svc.cluster.local` 的DNS形式进行通信。这就是为什么在配置中可以使用 `clickhouse-0.clickhouse-service` 这样的主机名。
- **数据持久化**：确保你的K3s集群配置了默认的StorageClass，以便StatefulSet能成功创建持久化卷（PVC）。
- **配置管理**：对于生产环境，考虑使用更高级的配置管理工具（如Kustomize、Helm）来管理不同环境的ClickHouse和gRPC服务配置。
