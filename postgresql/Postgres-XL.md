# Postgres-XL


##组件简介

* Global Transaction Monitor (GTM)
全局事务管理器，确保群集范围内的事务一致性。 GTM负责发放事务ID和快照作为其多版本并发控制的一部分。
集群可选地配置一个备用GTM，以改进可用性。此外，可以在协调器间配置代理GTM， 可用于改善可扩展性，减少GTM的通信量。

* GTM Standby
GTM的备节点，在pgxc,pgxl中，GTM控制所有的全局事务分配，如果出现问题，就会导致整个集群不可用，为了增加可用性，增加该备用节点。当GTM出现问题时，GTM Standby可以升级为GTM，保证集群正常工作。

* GTM-Proxy
GTM需要与所有的Coordinators通信，为了降低压力，可以在每个Coordinator机器上部署一个GTM-Proxy。

Coordinator
协调员管理用户会话，并与GTM和数据节点进行交互。协调员解析，并计划查询，并给语句中的每一个组件发送下一个序列化的全局性计划。
为节省机器，通常此服务和数据节点部署在一起。

* Data Node
数据节点是数据实际存储的地方。数据的分布可以由DBA来配置。为了提高可用性，可以配置数据节点的热备以便进行故障转移准备。

总结：gtm是负责ACID的，保证分布式数据库全局事务一致性。得益于此，就算数据节点是分布的，但是你在主节点操作增删改查事务时，就如同只操作一个数据库一样简单。Coordinator是调度的，将操作指令发送到各个数据节点。datanodes是数据节点，分布式存储数据。


## 集群规划

准备三台Centos7服务器（或者虚拟机），集群规划如下：
```text
主机名 	IP 	角色 	端口 	nodename 	数据目录
gtm
		192.168.0.125 	GTM 	6666 	gtm 	/nodes/gtm
		GTM Slave 	20001 	gtmSlave 	/nodes/gtmSlave

datanode1
		192.168.0.127 	Coordinator 	5432 	coord1 	/nodes/coord
		Datanode 	5433 	datanode1 	/nodes/dn_master
		Datanode 	15433 	datanode2 	/nodes/dn_slave
		GTM Proxy 	6666 	gtm_pxy1 	/nodes/gtm_pxy

datanode2
		192.168.0.128 	Coordinator 	5432 	coord2 	/nodes/coord
		Datanode 	5433 	datanode2 	nodes/dn_master
		Datanode 	15433 	datanode1 	/nodes/dn_slave
		GTM Proxy 	6666 	gtm_pxy2 	/nodes/gtm_pxy
```

在每台机器的 /etc/hosts中加入以下内容：

```text
192.168.0.125 gtm
192.168.0.126 datanode1
192.168.0.127 datanode2
```

gtm上部署gtm，gtm_sandby测试环境暂未部署。
Coordinator与Datanode节点一般部署在同一台机器上。
实际上，GTM-proxy,Coordinator与Datanode节点一般都在同一个机器上，使用时避免端口号与连接池端口号重叠！
规划datanode1，datanode2作为协调节点与数据节点。

## 系统环境设置

关闭防火墙：

```shell
systemctl stop firewalld.service
systemctl disable firewalld.service
```
selinux设置:

`/etc/selinux/config` 设置`SELINUX=disabled`，保存退出。

## 安装依赖包

```shell
yum install -y flex bison readline-devel zlib-devel openjade docbook-style-dsssl
```

## 新建用户

每个节点都建立用户postgres，并且建立.ssh目录，并配置相应的权限：
```shell
useradd postgres
passwd postgres
su - postgres
mkdir ~/.ssh
chmod 700 ~/.ssh
```

## ssh免密码登录

仅仅在gtm节点配置如下操作：
```shell
su - postgres
ssh-keygen -t rsa
cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys
chmod 600 ~/.ssh/authorized_keys
```
将刚生成的认证文件拷贝到datanode1到datanode2中，使得gtm节点可以免密码登录`datanode1~datanode2`的任意一个节点：
```shell
scp ~/.ssh/authorized_keys postgres@datanode1:~/.ssh/
scp ~/.ssh/authorized_keys postgres@datanode2:~/.ssh/
```
对所有提示都不要输入，直接enter下一步。直到最后，因为第一次要求输入目标机器的用户密码，输入即可。

## Postgres-XL安装

pg1-pg3 每个节点都需安装配置。切换回root用户下，执行如下步骤安装
```shell
cd /opt
git clone git://git.postgresql.org/git/postgres-xl.git
cd postgres-xl
./configure --prefix=/home/postgres/pgxl/
make
make install
cd contrib/  
make
make install
```
cortrib中有很多postgres很牛的工具，一般要装上。如`ltree,uuid,postgres_fdw`等等。

## 配置环境变量

进入postgres用户，修改其环境变量，开始编辑
```shell
su - postgres
```
在打开的`.bashrc`文件末尾，新增如下变量配置：
```shell
export PGHOME=/home/postgres/pgxl
export LD_LIBRARY_PATH=$PGHOME/lib:$LD_LIBRARY_PATH
export PATH=$PGHOME/bin:$PATH
```
输入以下命令对更改重启生效。

```shell
source .bashrc
#输入以下语句，如果输出变量结果，代表生效
echo $PGHOME
#应该输出/home/postgres/pgxl代表生效
```

如上操作，除特别强调以外，是datanode1-datanode2节点都要配置安装的。

## 集群配置

通过pgxc_ctl这个工具配置管理集群：使用pgxc_ctl配置集群之前需要在当前节点和集群各个节点之间做ssh免密码认证，执行pgxc_ctl的节点可以是集群内的任意一个节点也可以是集群外的节点。这个工具的原理大概是通过ssh执行各种bash命令完成集群各节点的配置与启动，非常方便，可以完全控制整个集群与各个节点。
http://files.postgres-xl.org/documentation/pgxc-ctl.html

### 生成pgxc_ctl配置文件

```shell
pgxc_ctl

PGXC prepare ---执行该命令将会生成一份配置文件模板
PGXC		 ---按ctrl c退出。
```

### 配置pgxc_ctl.conf

在pgxc_ctl文件夹中存在一个pgxc_ctl.conf文件，编辑如下：
```conf
pgxcInstallDir=$PGHOME
pgxlDATA=$PGHOME/data 

pgxcOwner=postgres

#---- GTM Master -----------------------------------------
gtmName=gtm
gtmMasterServer=gtm
gtmMasterPort=6666
gtmMasterDir=$pgxlDATA/nodes/gtm

gtmSlave=y                    # Specify y if you configure GTM Slave.   Otherwise, GTM slave will not be configured and
                            # all the following variables will be reset.
gtmSlaveName=gtmSlave
gtmSlaveServer=gtm        # value none means GTM slave is not available.  Give none if you don't configure GTM Slave.
gtmSlavePort=20001            # Not used if you don't configure GTM slave.
gtmSlaveDir=$pgxlDATA/nodes/gtmSlave    # Not used if you don't configure GTM slave.

#---- GTM-Proxy Master -------
gtmProxyDir=$pgxlDATA/nodes/gtm_proxy
gtmProxy=y                                
gtmProxyNames=(gtm_pxy1 gtm_pxy2)    
gtmProxyServers=(datanode1 datanode2)            
gtmProxyPorts=(6666 6666)                
gtmProxyDirs=($gtmProxyDir $gtmProxyDir)            

#---- Coordinators ---------
coordMasterDir=$pgxlDATA/nodes/coord
coordNames=(coord1 coord2)        
coordPorts=(5432 5432)            
poolerPorts=(6667 6667)            
coordPgHbaEntries=(0.0.0.0/0)

coordMasterServers=(datanode1 datanode2)    
coordMasterDirs=($coordMasterDir $coordMasterDir)
coordMaxWALsernder=0    #没设置备份节点，设置为0
coordMaxWALSenders=($coordMaxWALsernder $coordMaxWALsernder) #数量保持和coordMasterServers一致

coordSlave=n

#---- Datanodes ----------
datanodeMasterDir=$pgxlDATA/nodes/dn_master
primaryDatanode=datanode1                # 主数据节点
datanodeNames=(datanode1 datanode2)
datanodePorts=(5433 5433)    
datanodePoolerPorts=(6668 6668)    
datanodePgHbaEntries=(0.0.0.0/0)

datanodeMasterServers=(datanode1 datanode2)
datanodeMasterDirs=($datanodeMasterDir $datanodeMasterDir)
datanodeMaxWalSender=4
datanodeMaxWALSenders=($datanodeMaxWalSender $datanodeMaxWalSender)

datanodeSlave=n
#将datanode1节点的slave做到了datanode2服务器上，交叉做了备份
datanodeSlaveServers=(datanode2 datanode1)    # value none means this slave is not available
datanodeSlavePorts=(15433 15433)    # value none means this slave is not available
datanodeSlavePoolerPorts=(20012 20012)    # value none means this slave is not available
datanodeSlaveSync=y        # If datanode slave is connected in synchronized mode
datanodeSlaveDirs=($datanodeSlaveDir $datanodeSlaveDir)
datanodeArchLogDirs=( $datanodeArchLogDir $datanodeArchLogDir)
```
如上配置，都没有配置slave，具体生产环境，请阅读配置文件，自行配置。

## 集群初始化，启动，停止

第一次启动集群，需要初始化，初始化如下：

```shell
pgxc_ctl -c /home/postgres/pgxc_ctl/pgxc_ctl.conf init all
```

初始化后会直接启动集群。

以后启动，直接执行如下命令：

```shell
pgxc_ctl -c /home/postgres/pgxc_ctl/pgxc_ctl.conf start all
```

停止集群如下：
```shell
pgxc_ctl -c /home/postgres/pgxc_ctl/pgxc_ctl.conf stop all
```

## 新增datanode节点

在gtm集群管理节点上执行pgxc_ctl命令

```shell
pgxc_ctl
```
```shell
# 在服务器datanode1上，新增一个master角色的datanode节点，名称是dn3
# 端口号暂定5430，pool master暂定6669 ，指定好数据目录位置，从两个节点升级到3个节点，之后要写3个none
# none应该是datanodeSpecificExtraConfig或者datanodeSpecificExtraPgHba配置
PGXC add datanode master dn3 datanode1 5430 6669 /home/postgres/pgxl9.5/data/nodes/dn_master3 none none none
```

## 建表说明

* REPLICATION 表
各个datanode节点中，表的数据完全相同，也就是说，插入数据时，会分别在每个datanode节点插入相同数据。读数据时，只需要读任意一个datanode节点上的数据。

```sql
CREATE TABLE repltab (col1 int, col2 int) DISTRIBUTE BY REPLICATION;
```
* DISTRIBUTE 表
会将插入的数据，按照拆分规则，分配到不同的datanode节点中存储，也就是sharding技术。每个datanode节点只保存了部分数据，通过coordinate节点可以查询完整的数据视图。

```sql
CREATE TABLE disttab(col1 int, col2 int, col3 text) DISTRIBUTE BY HASH(col1);
```

将sharding表数据重新分配到三个节点上，将repl表复制到新节点：
```sql
# 重分布sharding表
ALTER TABLE disttab ADD NODE (dn3);
# 复制数据到新节点
postgres=#  ALTER TABLE repltab ADD NODE (dn3);
```

## 从datanode节点中回收数据
```sql
ALTER TABLE disttab DELETE NODE (dn3);
postgres=# ALTER TABLE repltab DELETE NODE (dn3);
```

