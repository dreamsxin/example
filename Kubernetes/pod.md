## 直接创建 Pod
```shell
kubectl run nginx --image=nginx:1.16.1 
kubectl create deployment nginx --image=nginx:1.14 --port=80 --replicas=3
```

## 利用 Pod 控制器创建 Pod
```shell
kubectl apply -f nginx-deploy.yaml
```
```yml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: nginx-deploy
spec:
  replicas: 2
  selector:
    matchLabels:
      app: nginx-deploy
  template:
    metadata:
      labels:
        app: nginx-deploy
    spec:
      containers:
      - name: nginx
        image: nginx:1.16.1
```
