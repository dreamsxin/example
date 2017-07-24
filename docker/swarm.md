Swarm 是 Docker 社区提供的唯一一个原生支持 Docker 集群管理的工具。它可以把多个 Docker 主机组成的系统转换为单一的虚拟 Docker 主机，使得容器可以组成跨主机的子网网络。

## Docker Machine

为了方便演示跨主机网络，我们需要用到一个工具——Docker Machine，这个工具与 Docker Compose、Docker Swarm 并称 Docker 三剑客，下面我们来看看如何安装 Docker Machine：

```shell
curl -L https://github.com/docker/machine/releases/download/v0.9.0-rc2/docker-machine-`uname -s`-`uname -m` >/tmp/docker-machine &&
chmod +x /tmp/docker-machine &&
sudo cp /tmp/docker-machine /usr/local/bin/docker-machine
```

安装过程和 Docker Compose 非常类似。现在 Docker 三剑客已经全部到齐了。
在开始之前，我们需要了解一些基本概念，有关集群的 Docker 命令如下：

- docker swarm：集群管理，子命令有 init, join,join-token, leave, update
- docker node：节点管理，子命令有 demote, inspect,ls, promote, rm, ps, update
- docker service：服务管理，子命令有 create, inspect, ps, ls ,rm , scale, update
- docker stack/deploy：试验特性，用于多应用部署，等正式版加进来再说。

## 创建集群

首先使用 Docker Machine 创建一个虚拟机作为 manger 节点。
```shell
docker-machine create --driver virtualbox manager1
```

查看虚拟机的环境变量等信息，包括虚拟机的 IP 地址：

```shell
docker-machine env manager1
```

然后再创建一个节点作为 work 节点：

```shell
docker-machine create --driver virtualbox worker1
```

现在我们有了两个虚拟主机，使用 Machine 的命令可以查看：

```shell
docker-machine ls
```
```text
NAME     ACTIVE   DRIVER       STATE    URL                        SWARM  DOCKER   ERRORS
manager1   -      virtualbox   Running  tcp://192.168.99.100:2376         v1.12.3   
worker1    -      virtualbox   Running  tcp://192.168.99.101:2376         v1.12.3
````

因为我们使用的是 Docker Machine 创建的虚拟机，因此可以使用 docker-machine ssh 命令来操作虚拟机，在实际生产环境中，并不需要像下面那样操作，只需要执行 docker swarm 即可。

把 manager1 加入集群：

```shell
docker-machine ssh manager1 docker swarm init --listen-addr 192.168.99.100:2377 --advertise-addr 192.168.99.100
```
`--advertise-addr` 指定播发地址
`--listen-addr` 指定监听的地址与端口，实际的 Swarm 命令格式如下，本例使用 Docker Machine 来连接虚拟机而已：
```shell
docker swarm init --listen-addr <MANAGER-IP>:<PORT>
```

> 注意：如果你在新建集群时遇到双网卡情况，可以指定使用哪个 IP，例如上面的例子会有可能遇到下面的错误。

```text
$ docker-machine ssh manager1 docker swarm init --listen-addr $MANAGER1_IP:2377
Error response from daemon: could not choose an IP address to advertise since this system has multiple addresses on different interfaces (10.0.2.15 on eth0 and 192.168.99.100 on eth1) - specify one with --advertise-addr
exit status 1
```

发生错误的原因是因为有两个 IP 地址，而 Swarm 不知道用户想使用哪个，因此要指定 IP。

接下来，再把 work1 加入集群中：

```shell
docker-machine ssh worker1 docker swarm join --token \
    SWMTKN-1-3z5rzoey0u6onkvvm58f7vgkser5d7z8sfshlu7s4oz2gztlvj-c036gwrakjejql06klrfc585r \
    192.168.99.100:2377
```

上面 join 命令中可以添加 `--listen-addr $WORKER1_IP:2377` 作为监听准备，因为有时候可能会遇到把一个 work 节点提升为 manger 节点的可能，当然本例子没有这个打算就不添加这个参数了。

现在我们新建了一个有两个节点的“集群”，现在进入其中一个管理节点使用 docker node 命令来查看节点信息：
```shell
docker-machine ssh manager1 docker node ls
```
```text
ID                          HOSTNAME STATUS AVAILABILITY MANAGER STATUS
23lkbq7uovqsg550qfzup59t6 * manager1 Ready  Active       Leader
dqb3fim8zvcob8sycri3hy98a   worker1  Ready  Active
```

现在每个节点都归属于 Swarm，并都处在了待机状态。Manager1 是领导者，work1 是工人。

现在，我们继续新建虚拟机 manger2、worker2、worker3，现在已经有五个虚拟机了，使用 docker-machine ls 来查看虚拟机：
```text
NAME ACTIVE DRIVER STATE URL SWARM DOCKER ERRORS
manager1 - virtualbox Running tcp://192.168.99.100:2376 v1.12.3
manager2 - virtualbox Running tcp://192.168.99.105:2376 v1.12.3
worker1  - virtualbox Running tcp://192.168.99.102:2376 v1.12.3
worker2  - virtualbox Running tcp://192.168.99.103:2376 v1.12.3
worker3  - virtualbox Running tcp://192.168.99.104:2376 v1.12.3
```

添加 worker2 到集群中：

```shell
docker-machine ssh worker2 docker swarm join \
  --token SWMTKN-1-3z5rzoey0u6onkvvm58f7vgkser5d7z8sfshlu7s4oz2gztlvj-c036gwrakjejql06klrfc585r \
  192.168.99.100:2377
```

添加 worker3 到集群中：

```shell
docker-machine ssh worker3 docker swarm join \
  --token SWMTKN-1-3z5rzoey0u6onkvvm58f7vgkser5d7z8sfshlu7s4oz2gztlvj-c036gwrakjejql06klrfc585r \
  192.168.99.100:2377
```

先从 manager1 中获取 manager 的 token：

```shell
docker-machine ssh manager1 docker swarm join-token manager
```
```text
To add a manager to this swarm, run the following command:

  docker swarm join \
  --token SWMTKN-1-3z5rzoey0u6onkvvm58f7vgkser5d7z8sfshlu7s4oz2gztlvj-8tn855hkjdb6usrblo9iu700o \
192.168.99.100:2377
```

然后添加 manager2 到集群中：

```shell
docker-machine ssh manager2 docker swarm join \
  --token SWMTKN-1-3z5rzoey0u6onkvvm58f7vgkser5d7z8sfshlu7s4oz2gztlvj-8tn855hkjdb6usrblo9iu700o \
  192.168.99.100:2377
```

现在再来查看集群信息：

```shell
docker-machine ssh manager2 docker node ls
```text
ID                            HOSTNAME   STATUS   AVAILABILITY   MANAGER STATUS
16w80jnqy2k30yez4wbbaz1l8     worker1     Ready     Active        
2gkwhzakejj72n5xoxruet71z     worker2     Ready     Active        
35kutfyn1ratch55fn7j3fs4x     worker3     Ready     Active        
a9r21g5iq1u6h31myprfwl8ln *   manager2    Ready     Active        Reachable
dpo7snxbz2a0dxvx6mf19p35z     manager1    Ready     Active        Leader
```

建立跨主机网络

为了演示更清晰，下面我们把宿主机当作manager也加入到集群之中，这样我们使用 Docker 命令操作会清晰很多。
直接在本地执行加入集群命令（少了docker-machine命令）：

```shell
docker swarm join \           
    --token SWMTKN-1-3z5rzoey0u6onkvvm58f7vgkser5d7z8sfshlu7s4oz2gztlvj-8tn855hkjdb6usrblo9iu700o \
    192.168.99.100:2377
```

现在我们有三台 manager，三台 worker。其中一台是宿主机，五台虚拟机。

```shell
docker node ls
```
```text
ID                          HOSTNAME    STATUS    AVAILABILITY  MANAGER STATUS
6z2rpk1t4xucffzlr2rpqb8u3    worker3     Ready     Active        
7qbr0xd747qena4awx8bx101s *  user-pc     Ready     Active         Reachable
9v93sav79jqrg0c7051rcxxev    manager2    Ready     Active         Reachable
a1ner3zxj3ubsiw4l3p28wrkj    worker1     Ready     Active        
a5w7h8j83i11qqi4vlu948mad    worker2     Ready     Active        
d4h7vuekklpd6189fcudpfy18    manager1    Ready     Active          Leader
```

查看网络状态：

```shell
docker network ls
```
```text
NETWORK ID         NAME            DRIVER          SCOPE
764ff31881e5        bridge          bridge          local                  
fbd9a977aa03        host            host            local               
6p6xlousvsy2        ingress         overlay         swarm            
e81af24d643d        none            null            local
```
可以看到在 swarm 上默认已有一个名为 ingress 的 overlay 网络, 默认在 swarm 里使用，本例子中会创建一个新的 overlay 网络。

```shell
docker network create --driver overlay swarm_test
```

在跨主机网络上部署应用

首先我们上面创建的节点都是没有镜像的，因此我们要逐一 pull 镜像到节点中。

分别在五个虚拟机节点拉取 `nginx:xxx` 镜像。接下来我们要在五个节点部署一组 Nginx 服务。

部署的服务使用 swarm_test 跨主机网络。
```shell
docker service create --replicas 2 --name helloworld --network=swarm_test nginx:xxx
```

查看服务状态：

```shell
docker service ls
```
```text
ID            NAME        REPLICAS  IMAGE         COMMAND
5gz0h2s5agh2  helloworld  0/2       nginx:xxx
```
查看 helloworld 服务详情（为了方便阅读，已调整输出内容）：

```shell
docker service ps helloworld
```
```text
ID          NAME          IMAGE         NODE      DESIRED STATE   CURRENT STATE              ERROR
ay081uome3   helloworld.1  nginx:xxx    manager1  Running         Preparing 2 seconds ago  
16cvore0c96  helloworld.2  nginx:xxx    worker2   Running         Preparing 2 seconds ago
```

可以看到两个实例分别运行在两个节点上。

进入两个节点，查看服务状态（为了方便阅读，已调整输出内容）：
```shell
docker-machine ssh manager1 docker ps -a
docker-machine ssh worker2 docker ps -a
```

记住上面这两个实例的名称。现在我们来看这两个跨主机的容器是否能互通：
首先使用 Machine 进入 manager1 节点，然后使用 docker exec -i 命令进入 helloworld.1 容器中 ping 运行在 worker2 节点的 helloworld.2 容器。
```shell
docker-machine ssh manager1 docker exec -i helloworld.1.ay081uome3eejeg4mspa8pdlx \
    ping helloworld.2.16cvore0c96rby1vp0sny3mvt
```

然后使用 Machine 进入 worker2 节点，然后使用 docker exec -i 命令进入 helloworld.2 容器中 ping 运行在 manager1 节点的 helloworld.1 容器。

可以看到这两个跨主机的服务集群里面各个容器是可以互相连接的。

为了体现 Swarm 集群的优势，我们可以使用虚拟机的 ping 命令来测试对方虚拟机内的容器。
```shell
docker-machine ssh worker2 ping helloworld.1.ay081uome3eejeg4mspa8pdlx
```

Swarm 集群负载

现在我们已经学会了 Swarm 集群的部署方法，现在来搭建一个可访问的 Nginx 集群吧。体验最新版的 Swarm 所提供的自动服务发现与集群负载功能。
首先删掉上一节我们启动的 helloworld 服务：
```shell
docker service rm helloworld
```

然后在新建一个服务，提供端口映射参数，使得外界可以访问这些 Nginx 服务：

```shell
docker service create --replicas 2 --name helloworld -p 7080:80 --network=swarm_test nginx:alpine
```

查看服务运行状态：

```shell
docker service ls
```
```text
ID           NAME         REPLICAS     IMAGE           COMMAND
9gfziifbii7a  helloworld     2/2       nginx:xxx
```
查看服务详细状态：
```shell
docker service ps helloworld
```
```text
ID           NAME         IMAGE     NODE     DESIRED STATE CURRENT STATE ERROR
9ikr3agyi... helloworld.1 nginx:xxx user-pc  Running       Running             13 seconds ago
7acmhj0u...  helloworld.2 nginx:xxx worker2  Running       Running             6 seconds ago
```

虽然我们使用 `--replicas` 参数的值都是一样的，但是上一节中获取服务状态时，REPLICAS 返回的是 0/2，现在的 REPLICAS 返回的是 2/2。
可以看到实例的 CURRENT STATE 中是 Running 状态的，而上一节中的 CURRENT STATE 中全部是处于 Preparing 状态。

对于一个容器来说如果没有外部通信但又是运行中的状态会被服务发现工具认为是 Preparing 状态，本小节例子中因为映射了端口，因此有了 Running 状态。
现在我们来看 Swarm 另一个有趣的功能，当我们杀死其中一个节点时，会发生什么。
首先 kill 掉 worker2 的实例：

```shell
docker-machine ssh worker2 docker kill helloworld.2.7acmhj0udzusv1d7lu2tbuhu4
```

稍等几秒，再来看服务状态：

可以看到即使我们 kill 掉其中一个实例，Swarm 也会迅速把停止的容器撤下来，同时在节点中启动一个新的实例顶上来。这样服务依旧还是两个实例在运行。
此时如果你想添加更多实例可以使用 scale 命令：

```shell
docker service scale helloworld=3
```

现在如果想减少实例数量，一样可以使用 scale 命令。
至此，Swarm的主要用法都已经介绍完了，主要讲述了 Swarm 集群网络的创建与部署。介绍了 Swarm 的常规应用，包括 Swarm 的服务发现、负载均衡等，然后使用 Swarm 来配置跨主机容器网络，并在上面部署应用。