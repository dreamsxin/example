# Storm

## 依赖

- Zookeeper 集群（http://zookeeper.apache.org/doc/r3.3.3/zookeeperAdmin.html）
- Java6.0
- Python2.6.6
- Unzip命令    

**NOTE**: Nimbus 和管理进程将要依赖 Java、Python 和 unzip 命令

## 安装 ZeroMQ

```
wget http://download.zeromq.org/historic/zeromq-2.1.7.tar.gz
tar -xzf zeromq-2.1.7.tar.gz
cd zeromq-2.1.7
./configure
make
sudo make install
```  

## 安装 JZMQ

```
     git clone https://github.com/nathanmarz/jzmq.git
     cd jzmq
     ./autogen.sh
     ./configure
     make
     sudo make install
```  

## 下载 Storm 稳定版解压

https://github.com/apache/incubator-storm/releases

## 编辑配置文件

可以从 Storm 仓库的 defaults.yaml 看到所有的默认配置。  
编辑 Storm 目录下的 `conf/storm.yaml`，添加以下参数，增加集群配置：

```yaml
storm.zookeeper.servers:  
– "zookeeper addres 1"  
– "zookeeper addres 2"  
– "zookeeper addres N"  
storm.local.dir: "a local directory"  
nimbus.host: "Nimbus host addres"  
supervisor.slots.ports:  
– supervisor slot port 1  
– supervisor slot port 2  
– supervisor slot port N  
```

参数解释：  

- storm.zookeeper.servers
你的 zookeeper 服务器地址。
  
- storm.local.dir：
Storm 进程保存内部数据的本地目录。（务必保证运行 Storm 进程的用户拥有这个目录的写权限。）  
   
- nimbus.host
Nimbus运行的机器的地址  

- supervisor.slots.ports
接收消息的工人进程监听的端口号（通常从6700开始）；管理进程为这个属性指定的每个端口号运行一个工人进程。

当你完成了这些配置，就可以运行所有的 Storm 进程了。如果你想运行一个本地进程测试一下，就把 nimbus.host 配置成 localhost。

启动一个 Storm 进程，在 Storm 目录下执行`./bin/storm`进程名。

**NOTE**：Storm 提供了一个出色的叫做 Storm UI 的工具，用来辅助监控拓扑。

## 安装 Storm 客户端  

Storm 客户端能让我们使用命令管理集群中的拓扑。

把 Storm 目录加入 PATH 环境变量，这样就不用每次都输入全路径执行 Storm 了。
执行 `export PATH=$PATH:/usr/local/bin/storm`。  

创建 Storm 本地配置文件：~/.storm/storm.yaml，在配置文件中按如下格式加入nimbus 主机：  

- nimbus.host:"我们的nimbus主机"

现在，你可以管理你的 Storm 集群中的拓扑了。

**NOTE**：Storm 客户端包含运行一个 Storm 集群所需的所有 Storm 命令，但是要运行它你需要安装一些其它的工具并做一些配置。详见附录B。

有许多简单且有用的命令可以用来管理拓扑，它们可以提交、杀死、禁用、再平衡拓扑。

**jar** 命令负责把拓扑提交到集群，并执行它，通过 **StormSubmitter** 执行主类。
  
```shell
storm jar path-to-topology-jar class-with-the-main arg1 arg2 argN   
```   

path-to-topology-jar 是拓扑 jar 文件的全路径，它包含拓扑代码和依赖的库。 class-with-the-main 是包含 main 方法的类，这个类将由 StormSubmitter 执行，其余的参数作为 main 方法的参数。

我们能够挂起或停用运行中的拓扑。当停用拓扑时，所有已分发的元组都会得到处理，但是spouts 的 **nextTuple** 方法不会被调用。

停用拓扑：
  
```shell
storm deactivte topology-name  
```  

启动一个停用的拓扑：
  
```shell
storm activate topology-name  
```  

销毁一个拓扑，可以使用 kill 命令。它会以一种安全的方式销毁一个拓扑，首先停用拓扑，在等待拓扑消息的时间段内允许拓扑完成当前的数据流。
杀死一个拓扑：
  
```shell
storm kill topology-name  
```  

**NOTE**:执行 kill 命令时可以通过 -w [等待秒数]指定拓扑停用以后的等待时间。

再平衡使你重分配集群任务。这是个很强大的命令。比如，你向一个运行中的集群增加了节点。再平衡命令将会停用拓扑，然后在相应超时时间之后重分配工人，并重启拓扑。
再平衡拓扑：
  
```shell
storm rebalance topology-name  
```  

**NOTE**:执行不带参数的 Storm 客户端可以列出所有的 Storm 命令。