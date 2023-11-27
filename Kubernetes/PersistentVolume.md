# PersistentVolume

PersistentVolume（PV），其是K8s中对实际物理存储系统的抽象。作为K8s中集群层面的资源，与节点资源类似。
PersistentVolumeClaim（PVC），其是K8s中用户对存储请求的抽象。
类似地，Pod 会消耗节点资源，而 PVC 则会消耗PV资源。

## 创建持久卷 PV
```yml
apiVersion: v1
kind: PersistentVolume
metadata:
  # PV卷名称
  name: mongodb-pv
spec:
  # 容量
  capacity: 
    # 存储大小: 100MB
    storage: 100Mi
  # 该卷支持的访问模式
  accessModes:
    - ReadWriteOnce # RWO, 该卷可以被一个节点以读写方式挂载
    - ReadOnlyMany  # ROX, 该卷可以被多个节点以只读方式挂载
  # 回收策略: 保留
  persistentVolumeReclaimPolicy: Retain
  # 该持久卷的实际存储类型: 此处使用HostPath类型卷
  hostPath:
    path: /tmp/MongodbData
```
```yml
# host-10m-pv.yml
apiVersion: v1
kind: PersistentVolume
metadata:
  name: host-10m-pv

spec:
  storageClassName: host-test
  accessModes:
  - ReadWriteOnce
  capacity:
    storage: 10Mi
  hostPath:
    path: /tmp/host-10m-pv/
```
`accessModes` 定义了存储设备的访问模式，简单来说就是虚拟盘的读写权限，和 Linux 的文件访问模式差不多，目前 Kubernetes 里有 3 种：

- ReadWriteOnce：存储卷可读可写，但只能被一个节点上的 Pod 挂载。
- ReadOnlyMany：存储卷只读不可写，可以被任意节点上的 Pod 多次挂载。
- ReadWriteMany：存储卷可读可写，也可以被任意节点上的 Pod 多次挂载。
显然，本地目录只能是在本机使用，所以这个 PV 使用了 ReadWriteOnce。
```shell
kubectl apply -f host-10m-pv.yml
```
## 创建持久卷 PVC
PVC 的内容与 PV 很像，但它不表示实际的存储，而是一个“申请”或者“声明”，spec 里的字段描述的是对存储的“期望状态”。

所以 PVC 里的 storageClassName、accessModes 和 PV 是一样的，但不会有字段 capacity，而是要用 resources.request 表示希望要有多大的容量。

```yml
# host-5m-pvc.yml
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: host-5m-pvc

spec:
  storageClassName: host-test
  accessModes:
    - ReadWriteOnce
  resources:
    requests:
      storage: 5Mi
```
```shell
kubectl apply -f host-5m-pvc.yml
kubectl get pvc
```
## 删除 PVC
```shell
kubectl delete -f host-5m-pvc.yml
```
## 删除 pv 配置中绑定的内容
```shell
kubectl edit pv host-10m-pv
```
## 为 Pod 挂载 PersistentVolume
```shell
# host-path-pod.yml
apiVersion: v1
kind: Pod
metadata:
  name: host-pvc-pod

spec:
  volumes:
  - name: host-pvc-vol
    persistentVolumeClaim:
      claimName: host-5m-pvc

  containers:
    - name: ngx-pvc-pod
      image: nginx:alpine
      ports:
      - containerPort: 80
      volumeMounts:
      - name: host-pvc-vol
        mountPath: /tmp
```
```shell
kubectl apply -f host-path-pod.yml
kubectl get pod -o wide # -o 选项指定输出格式为"wide"视图
```
