## 什么是控制器？
Kubernetes内拥有许多的控制器类型，用来控制pod的状态、行为、副本数量等等，控制器通过Pod的标签来控制Pod ，从而实现对应用的运维，如伸缩、升级等。

### 常用的控制器类型如下

- ReplicationController 、ReplicaSet、Deployment：无状态服务，保证在任意时间运行Pod指定的副本数量，能够保证Pod总是可用的，支持滚动更新、回滚。典型用法：web服务。
- DaemonSet：确保集群内全部（或部分）node节点上都分配一个pod，如果新加node节点，也会自动再分配对应的pod。典型用法：filebeat日志收集、prometheus资源监控。
- StatefulSet：有状态服务，如各种数据存储系统。StatefullSet内的服务有着稳定的持久化存储和网络标识，有序部署，有序伸缩。
- Job：只运行一次的作业。
- CronJob：周期性运行的作业。典型用法：数据库定时备份。

#### ReplicationController(RC)
RC 只支持基于等式的 selector。
简单来说，RC可以保证在任意时间运行Pod的副本数量，能够保证Pod总是可用的。如果实际Pod数量比指定的多，那就结束掉多余的，如果实际数量比指定的少，就新启动一些Pod。当Pod失败、被删除或者挂掉后，RC都会去自动创建新的Pod来保证副本数量，所以即使只有一个Pod，我们也应该使用RC来管理我们的Pod。

简而言之，RC用于确保其管控的Pod对象副本数满足期望的数值，它能实现以下功能：

- 确保Pod的资源数量精确反应期望值
- 确保Pod健康运行
- 弹性伸缩

#### ReplicaSet(RS)
随着Kubernetes的高速发展，官方已经推荐我们使用RS和Deployment来代替RC了，实际上RS和RC的功能基本一致， 但RS还支持基于集合的selector（如：version in (v1.0, v2.0)），这对复杂的运维管理就更方便了。不必在selector属性中直接列出pod需要的标签， 而是在 selector.matchLabels 下指定它们。

#### Deployment
不过我们也很少会去单独使用RS，它主要被Deployment这个更加高层的资源对象使用，除非用户需要自定义升级功能或根本不需要升级Pod， Deployment 为 Pod 和 ReplicaSet 提供了一个申明式的定义方法。
Deployment基于ReplicaSet之上，可为Pod和ReplicaSet资源提供声明式更新，它具有以下特性：

- 事件和状态查看：可以查看Deployment对象升级的详细进度和状态
- 回滚：当升级操作完成后发现问题时，支持将应用返回到指定的历史版本中
- 版本记录：对Deployment 对象的每一次操作都予以保存
- 暂停和启动：每一次升级，都可以随时暂停和启动
- 多种自动更新方案：Recreate-重建更新、RollingUpdate-滚动更新。

#### StatefulSet
StatefulSet 本质上是Deployment的一种变体，它为了解决有状态服务的问题，它所管理的Pod拥有固定的Pod名称，启停顺序，在 StatefulSet 中，Pod 名字称为网络标识(hostname)，还必须要用到共享存储。
在Deployment中，与之对应的服务是Service，而在StatefulSet中与之对应的Headless Service，Headless Service，即无头服务，与service的区别就是它没有Cluster IP，解析它的名称时将返回该Headless Service对应的全部Pod的Endpoint列表。 
StatefulSet的核心功能就是通过某种方式，记录这些状态，然后在Pod被重新创建时，能够为新Pod恢复这些状态。

#### DaemonSet
DaemonSet控制器确保集群中的每一个Node只运行一个特定的Pod副本，实现系统级的后台任务，也具有标签选择器。
也可以指定部分满足条件的Node运行一个Pod副本，比如监控具有ssd存储的Node节点。
常用来部署一些集群的日志、监控或者其他系统管理应用。

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
