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

