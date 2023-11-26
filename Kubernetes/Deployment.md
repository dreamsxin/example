# Deployment
Deployment 是 Kubernetes 的控制器之一，用于声明性地管理 Pod 副本集。它允许定义 Pod 模板、副本数和更新策略，使得应用程序的部署和更新变得简单可控。
可以使用 Deployment 的副本数字段来进行水平扩展。通过增加副本数，Kubernetes 会创建更多的 Pod 副本以应对负载增加。
通过更新 Deployment 的 Pod 模板来进行滚动更新。Kubernetes 会逐步替换现有的 Pod 副本，确保在整个更新过程中应用程序的可用性。

## Minikube
Minikube 是一个易于在本地运行 Kubernetes 的工具，可在你的笔记本电脑上的虚拟机内轻松创建单机版 Kubernetes 集群。
```shell
minikube start --driver=docker
```
## 创建 nginx-deployment
```yml
# vim nginx-deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: nginx-deployment
  labels:
    app: nginx
spec:
  replicas: 3
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
        image: nginx:1.14.2
        ports:
        - containerPort: 80
```
- `metadata.name` 
创建名为 `nginx-deployment` 的 `Deployment` 对象。该名称将作为 `ReplicaSet`、`Pod` 的前缀自动闯将这两种类型的对象该名称将作为 `ReplicaSet`、Pod的前缀自动闯将这两种类型的对象。
- `spec.replicas`
`Deployment 根据自动创建 `ReplicaSet` 对象，并创建 3 个副本 Pod。
- `spec.selector`
定义 `ReplicaSet` 如何匹配 `Pod`，需要跟 `template.labels.app` 一致。
- template.metadata.labels.app - 字段标记Pod
- template.spec 字段指示 Pod 运行的容器(名称为nginx) ，以及容器镜像
- template.port 容器端口

## 创建 Deployment
```shell
kubectl create -f nginx-deploy.yaml
```
## 查看 Deployment 创建状态
```shell
kubectl rollout status deployment/nginx-deployment
```
## 查看 Deployment 运行状态
```shell
# 创建成功后 使用以下命令查看deployment运行状态
kubectl get deployments | grep nginx-deployment
```
## 查看 Deployment 日志
```shell
kubectl describe deploy nginx
```
## 查看 Deployment 自动创建的 ReplicSet 信息
```shell
kubectl get rs | grep nginx
```
## 查看 Deployment 底层的 Pod 信息
```shell
kubectl get pods --show-labels | grep nginx-deployment
```
Pod 后面携带的 Hash 值跟 Deployment 对应 ReplicaSet 对象的 Hash 值一致，他们之间由此产生关联。

## 更新 Deployment
仅当 Deployment 配置文件中的 Pod 模板（`spec.template`）更改时，才会触发 Deployment 更新操作。例如 Pod 模板中的镜像文件更改，触发 Deployment 更新。
### 更新策略
默认情况下，K8S会保留10个旧ReplicaSet版本，以方便回滚。超过10个以上的ReplicaSet将会被系统回收。
- spec.strategy.type 参数设置会影响Deployment的更新机制
  Recreate - 先删除旧的Pod，再创建新的Pod
  RollingUpdate - 滚动更新，默认值，可以通过指定 maxUnavailable 、maxSurge来控制更新过程
- spec.strategy.rollingUpdate.maxUnavailable 指定回滚更新时，最大不可用的Pod数量，默认25%，简单理解为一部分更新
- spec.strategy.rollingUpdate.maxSurge 可以超过期望值的最大Pod数，可选字段，默认为25%
- spec.minReadySeconds 可选参数，指定新建的Pod在没有任何容器崩溃情况下视为就绪- Ready状态的最小时间，单位为秒，默认为0，即一旦创建Pod，则被视为可用

### 更新镜像版本
将之前nginx的镜像版本从 nginx:1.14.2 升级到 nginx:1.16.1
```shell
kubectl set image deployment/nginx-deployment nginx=nginx:1.16.1
# 可以指定 apiVersion
kubectl set image deployment.v1.apps/nginx-deployment nginx=nginx:1.16.1
```
直接修改 `nginx-deployment.yaml`，然后执行
```shell
kubectl apply -f nginx-deployment.yaml
```
查看执行日志
```shell
kubectl describe deploy nginx-deployment
```

## 回滚 Deployment
```shell
kubectl set image deployment/nginx-deployment nginx=nginx:1.14.2 
```
验证更新状态
```shell
kubectl rollout status deployment/nginx-deployment
```
检查回滚版本
```shell
kubectl rollout history deployment/nginx-deployment
```
查看回滚版本详细信息
```shell
kubectl rollout history deployment/nginx-deployment --revision=2
```
回滚到上一个版本
```shell
kubectl rollout undo deployment/nginx-deployment
```
回滚到指定版本
```shell
kubectl rollout undo deployment/nginx-deployment --to-revision=1
```
## 扩容 Deployment
```shell
kubectl scale deployment/nginx-deployment --replicas=10
# 当cpu超过80%的时候，进行扩容，副本数量最低10个，最高不超过15个
kubectl autoscale deployment/nginx-deployment --min=10 --max=15 --cpu-percent=80
```
