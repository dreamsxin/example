## 安装运行 Etcd
从Github上下载指定版本的Etcd发布包：
```shell
$ wget https://github.com/coreos/etcd/releases/download/v2.2.0/etcd-v2.2.0-linux-amd64.tar.gz 
$ tar xzvf etcd-v2.2.0-linux-amd64.tar.gz 
$ cd etcd-v2.2.0-linux-amd64 
$ cp etcd /usr/bin/etcd 
$ cp etcdctl /usr/bin/etcdctl
```
运行Etcd：
```shell
$ etcd -name etcd -data-dir /var/lib/etcd \ 
-listen-client-urls http://0.0.0.0:2379，http://0.0.0.0:4001 \ 
-advertise-client-urls http://0.0.0.0:2379，http://0.0.0.0:4001 \ 
>> /var/log/etcd.log 2>&1 & 
```
Etcd运行后可以查询其健康状态：
```shell
$ etcdctl -C http://etcd:4001 cluster-health 
member ce2a822cea30bfca is healthy: got healthy result from http://etcd:4001 
cluster is healthy
```

## 获取Kubernetes发布包
```shell
$ wget https://github.com/kubernetes/kubernetes/releases/download/v1.1.1/kubernetes.tar.gz
$tar zxvf kubernetes.tar.gz
```
Kubernetes发布包中包含如下内容。

+ cluster：Kubernetes自动化部署脚本。
+ contrib：Kubernetes非必需程序。
+ examples：Kubernetes示例配置文件。
+ docs：Kubernetes文档。
+ platforms：Kubernetes的命令行工具kubectl。
+ server：Kubernetes组件。
+ third_party：第三方程序。

Kubernetes 发布包中的 `server/kubernetes-server-linux-amd64.tar.gz` 包含各个组件的可执行程序，将这些可执行程序复制到Linux系统目录/use/bin/下：
```shell
$ cd kubernetes/server
$ tar zxvf kubernetes-server-linux-amd64.tar.gz
$ cd kubernetes/server/bin/
$ find ./ -perm 755 | xargs -i cp {} /usr/bin/
```

## 运行Kubernetes Master组件
在Kubernetes Master上需要运行以下组件：

+ Kubernetes API Server
+ Kubernetes Controller Manager
+ Kubernetes Scheduler
+ Kubernetes Proxy（可选）

运行Kubernetes API Server：
```shell
$ kube-apiserver \
--logtostderr=true --v=0 \
--etcd_servers=http://etcd:4001 \
--insecure-bind-address=0.0.0.0 --insecure-port=8080 \
--service-cluster-ip-range=10.254.0.0/16 \
>> /var/log/kube-apiserver.log 2>&1 &
```

运行Kubernetes Controller Manager：
```shell
$ kube-controller-manager \
--logtostderr=true --v=0 \
--master=http://kube-master:8080 \
>> /var/log/kube-controller-manager.log 2>&1 &
```

运行Kubernetes Scheduler：
```shell
$ kube-scheduler \
--logtostderr=true --v=0 \
--master=http://kube-master:8080 \
>> /var/log/kube-scheduler.log 2>&1 &
```

运行Kubernetes Proxy：
```shell
$ kube-proxy \
--logtostderr=true --v=0 \
--api_servers=http://kube-master:8080 \
>> /var/log/kube-proxy.log 2>&1 &
```

## 运行Kubernetes Node组件
需要运行以下组件：

+ Docker
+ Kubelet
+ Kubernetes Proxy

安装Docker
Docker官方社区提供各个平台的安装方法（可参考http://docs.docker.com/installation/），很多Linux发行版都陆续对Docker进行了支持，可以使用以下方式快速安装Docker：
```shell
$ curl -sSL https://get.docker.com/ | sh
```
启动Docker：
```shell
$ docker -d \
-H unix:///var/run/docker.sock -H 0.0.0.0:2375 \
>> /var/log/docker.log 2>&1 &
```

运行Kubelet：
```shell
$ kubelet \
--logtostderr=true --v=0 \
--config=/etc/kubernetes/manifests \
--address=0.0.0.0 \
--api-servers=http://kube-master:8080 \
>> /var/log/kubelet.log 2>&1 &
```

运行Kubelet Proxy：
```shell
$ kube-proxy \
--logtostderr=true --v=0 \
--api_servers=http://kube-master:8080 \
>> /var/log/kube-proxy.log 2>&1 &
```

## 查询Kubernetes的健康状态

在部署运行各个组件以后，可以通过Kubernetes命令行kubectl查询Kubernetes Master各组件的健康状态：
```shell
$ kubectl -s http://kube-master:8080 get componentstatus
NAME                  STATUS    MESSAGE                ERROR 
controller-manager    Healthy   ok                     nil 
scheduler             Healthy   ok                     nil 
etcd-0                Healthy   {"health": "true"}     nil
```
以及Kubernetes Node的健康状态：
```shell
$ kubectl -s http://kube-master:8080 get node
NAME          LABELS                               STATUS     AGE 
kube-node-1   kubernetes.io/hostname=kube-node-1   Ready      19m 
kube-node-2   kubernetes.io/hostname=kube-node-2   Ready      18m 
kube-node-3   kubernetes.io/hostname=kube-node-3   Ready      18m 
```

## 创建Kubernetes覆盖网络
Kubernetes 的网络模型要求每一个 `Pod` 都拥有一个扁平化共享网络命名空间的IP，称为`PodIP`，Pod能够直接通过PodIP跨网络与其他物理机和`Pod`进行通信。要实现Kubernetes的网络模型，需要在 Kubernetes 集群中创建一个覆盖网络（Overlay Network），联通各个节点，目前可以通过第三方网络插件来创建覆盖网络，比如`Flannel`和`Open vSwitch`。
