#

Kubernetes 支持多个虚拟集群，它们底层依赖于同一个物理集群。 这些虚拟集群被称为命名空间。
命名空间namespace是k8s集群级别的资源，可以给不同的用户、租户、环境或项目创建对应的命名空间，例如，可以为 test、devlopment、production 环境分别创建各自的命名空间。

## 查看命令空间
```shell
kubectl get namespace
kubectl get ns
```

## 创建

```shell
kubectl create ns xxx
kubectl get ns
kubectl delete ns xxx
kubectl get ns	
```

## namespace 资源限额

```shell
kubectl explain quota
```
- APIVersion 定义了对象的版本
- kind 是表示此对象所代表的 REST 资源的字符串值。服务器可以根据客户端提交请求的端点来推断它。不能更新。采用驼峰命名法
- metadata 对象的元数据
- spec 定义所需的配额。它指定了命名空间中资源使用的限制

`nginx_quota.yaml`
```yml
apiVersion: v1
kind: ResourceQuota
metadata:
  name: cpu-mem-quota
  namespace: test
spec:
  hard:
    requests.cpu: 2 #所有容器的CPU请求总额不得超过2 CPU
    requests.memory: 2Gi #所有容器的内存请求总额不得超过2GiB
    limits.cpu: 4 #所有容器的CPU限额总额不得超过4CPU
    limits.memory: 4Gi  #所有容器的内存限额总额不得超过4 GiB

#创建的ResourceQuota对象将在test名字空间中添加以下限制：
#每个容器必须设置内存请求（memory request），内存限额（memory limit），cpu请求（cpu request）和cpu限额（cpu limit）。
#ResouceQuota 对象是在我们的名称空间中创建的，并准备好控制该名称空间中的所有容器的总请求和限制.
```
### 应用 namespace
```shell
kubectl apply -f nginx_quota.yaml #--namespace=test
```
### 查看 namespace
```shell
kubectl get resourcequota cpu-mem-quota --namespace=test --output=yaml
```
创建一个nginx pod来验证限制
```yml
apiVersion: apps/v1
kind: Deployment #定义了资源对象类型为 Deployment
metadata:
  labels:
    app: nginx-test
  name: nginx
  namespace: test
spec:
  selector: #指定了用于选择Pod的标签（matchLabels）
    matchLabels:
      app: nginx
  template:#定义了创建Pod的模板，包括了Pod的标签（labels）
    metadata:
      labels:
        app: nginx #跟matchLabels指定的app: nginx必须一样
    spec:
      containers: #定义了要在 Pod 中运行的容器
      - name: nginx
        image: nginx:1.17.2
        imagePullPolicy: IfNotPresent #指定了镜像拉取策略为仅在本地不存在时才拉取
        ports:
        - containerPort: 80
        resources:
          requests:
            memory: 100Mi
            cpu: 4 #请求cpu超过了2Gi限额
          limits:
            memory: 4Gi
            cpu: 2
```
创建pod

```shell
kubectl apply -f pod_deployment.yaml 
```
`The Deployment "nginx" is invalid: spec.template.spec.containers[0].resources.requests: Invalid value: "4": must be less than or equal to cpu limit`
创建pod报错 提示cpu必须小于或等于限额,说明命名空间限制成功
