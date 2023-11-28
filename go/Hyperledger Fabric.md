# 什么是 Hyperledger Fabric
Hyperledger Fabric 网络的成员只能从可信赖的成员服务提供者（MSP） 注册，也就是说 Hyperledger Fabric 搭建的区块链是一种联盟链。
Hyperledger Fabric 是一种账本技术，其账本包括世界状态数据库和交易日志历史记录。

https://github.com/hyperledger/fabric

## 什么是联盟

联盟指参与一个基于区块链的业务协作或业务交易网络的所有组织的集合，一个联盟一般包含多个组织。

一般由联盟发起方或运营方创建 Orderer 排序节点，并负责交易排序、区块产生和达成共识。联盟发起方或运营方邀请各个组织实例加入联盟，进而创建通道。

## 什么是组织

组织代表的是参与区块链网络的企业、政府机构、团体等实体。

一个组织实例主要包含如下节点：

CA ：区块链节点类型之一，全称 Certificate Authority ，数字证书颁发机构，负责组织内部成员的 register 和 enroll 等，为该组织的区块链用户生成和颁发数字证书。
Peer ：区块链节点类型之一，负责保存和记录账本数据、对交易背书、运行智能合约等。

## 什么是节点

节点（Peers）是区块链的通信实体。它只是一个逻辑功能，只要能在“信任域”中分组并与控制它们的逻辑实体相关联，就可以将不同类型的多个节点运行在同一个物理服务器上，比如用 Docker 部署。

Orderer 排序服务节点 或 排序节点：Orderer 是一个运行实现交付担保的通信服务节点，例如原子性或总顺序广播。排序节点负责接受交易并排序（排序算法有: SOLO，KAFKA，RAFT，PBFT），最后将排序好的交易按照配置中的约定整理为区块之后提交给记账节点进行处理。
Peer 节点：Peer 是业务参与方组织在区块链网络中所拥有的参与共识和账本记录的节点。可以有多种角色。作为 Committing Peer 记账节点时，无需安装链码，只负责验证从 Orderer 发出的区块和交易的合法性、并存储账本区块信息。作为 Endorsing Peer 背书节点时，必须安装链码，在交易时需进行签名背书。
Anchor 锚节点：为了实现高可用，每个参与方组织一般包含两个或多个 Peer 节点，可以设置其中的一个为 Anchor ，与区块链网络中的其他组织进行信息同步。
客户端节点：客户端扮演了代表最终用户的实体，可以同时与 Peer 和 Orderer 通信，创建并调用交易。这里客户端可以指应用程序、SDK、命令行等。
5、什么是通道

Hyperledger Fabric 中的通道（Channel）是两个或两个以上特定网络成员之间通信的专用“子网”，用于进行私有和机密的交易。

可以理解为组织间拉了个群聊，这个群聊就是通道，在里面聊天交易，一个联盟链中可以有多个群聊（通道），一个组织可以加入多个群聊，每个群聊可以代表一项具体的业务，有自身对应的一套账本，群聊间互不干扰，互相隔离。

## 什么是链码

Hyperledger Fabric 的智能合约用链码（Chaincode）编写。在大多数情况下，链码只与账本的数据库即世界状态交互，而不与交易日志交互。

链码可以用多种编程语言实现。有 Go、Node.js 和 Java 链码等。

## 区块链基础知识
1、什么是区块

Block ，每个区块记录着上一个区块的 hash 值、本区块中的交易集合、本区块的 hash 等基础数据。由于每个区块都有上一区块的 hash 值，区块间由这个值两两串联，形成了区块链。

2、什么是区块链

Blockchain ，最早起源于比特币的底层技术，并在其后不断演进发展。

区块链本质上就是一个多方共享的分布式账本技术，用来记录网络上发生的所有交易。

而其中去中心化的概念，是因为账本信息会被复制到许多网络参与者中，每个参与者都在协作维护账本，不像传统应用的数据被中心管理着。

另外信息只能以附加的方式记录到账本上，并使用加密技术保证一旦将交易添加到账本就无法修改。这种不可修改的属性简化了信息的溯源，因为参与者可以确定信息在记录后没有改变过。所以区块链有时也被称为证明系统。

3、什么是公链、联盟链和私链

区块链分为公有链、联盟链、私有链三种基本类型。其中：

完全去中心化：公链，人人都可以参与，就像比特币（挖矿相当于在记账）。主要采取工作量证明机制(POW)、权益证明机制(POS)、股份授权证明机制(DPOS)等方式。
部分去中心化：联盟链，参与者是指定的。联盟链可以是几家公司共同拥有的链，也可能是几个国家共同承认的链。这是后续发展的趋势。
中心化：私链，写入权限仅在一个组织手里的区块链，仅对特定的团队、组织或者个人开放。
4、什么是交易

Transaction ，区块链接收的数据称之为交易。

5、什么是智能合约

Smart contract，为了支持以同样的方式更新信息，并实现一整套账本功能（交易，查询等），区块链使用智能合约来提供对账本的受控访问。

智能合约不仅是在网络中封装和简化信息的关键机制，它还可以被编写成自动执行参与者的特定交易的合约。

例如，可以编写智能合约以规定运输物品的成本，其中运费根据物品到达的速度而变化。根据双方同意并写入账本的条款，当收到物品时，相应的资金会自动转手。

通俗易懂点，智能合约就是按照大家约定好的规则编写的业务逻辑代码实现，然后只能通过这些合约来操作区块链网络这个账本。

6、什么是共识

保持账本在整个网络中同步的过程称为共识。该过程确保账本仅在交易被相应参与者批准时才会更新，并且当账本更新时，它们以相同的顺序更新相同的交易。

## 搭建区块链网络

### 下载 fabric 二进制工具

其中几个主要的工具说明：

- cryptogen
用来生成 Hyperledger Fabric 密钥材料的工具，这个过程是静态的。cryptogen 工具通过一个包含网络拓扑的 crypto-config.yaml 文件，为所有组织和属于这些组织的组件生成一组证书和秘钥。cryptogen 适合用于测试开发环境，在生产环境建议使用动态的 CA 服务。
- configtxgen
用于创建和查看排序节点的创世区块、通道配置交易等相关的工具。configtxgen 使用 configtx.yaml 文件来定义网络配置。
- configtxlator
fabric 中 Protobuf 和 JSON 格式转换的工具，fabric 中任何的使用 Protobuf 定义的类型，都可使用该工具进行转换。
- peer
peer 命令有 5 个不同的子命令，每个命令都可以让指定的 peer 节点执行特定的一组任务。比如，可以使用子命令 peer channel 让一个 peer 节点加入通道，或者使用 peer chaincode 命令把智能合约链码部署到 peer 节点上。

将工具路径到环境变量
`export PATH=${PWD}/hyperledger-fabric-linux-amd64-1.4.12/bin:$PATH`

### 生成证书和秘钥

我们将使用 cryptogen 工具生成各种加密材料（ x509 证书和签名秘钥）。这些证书是身份的代表，在实体相互通信和交易的时候，可以对其身份进行签名和验证。

首先创建 crypto-config.yaml 文件，定义网络拓扑，为所有组织和属于这些组织的组件（也就是节点）生成一组证书和秘钥，内容如下：
```yml
# 排序节点的组织定义
OrdererOrgs:
  - Name: QQ # 名称
    Domain: qq.com # 域名
    Specs: # 节点域名：orderer.qq.com
      - Hostname: orderer # 主机名

# peer节点的组织定义
PeerOrgs:
  # Taobao-组织
  - Name: Taobao # 名称
    Domain: taobao.com # 域名
    Template: # 使用模板定义。Count 指的是该组织下组织节点的个数
      Count: 2 # 节点域名：peer0.taobao.com 和 peer1.taobao.com
    Users: # 组织的用户信息。Count 指该组织中除了 Admin 之外的用户的个数
      Count: 1 # 用户：Admin 和 User1

  # JD-组织
  - Name: JD
    Domain: jd.com
    Template:
      Count: 2 # 节点域名：peer0.jd.com 和 peer1.jd.com
    Users:
      Count: 1 # 用户：Admin 和 User
```

```shell
cryptogen generate --config=./crypto-config.yaml
tree crypto-config
```
可以看到文件已经生成。
总结：在这个环节中，我们假设 QQ 作为一个运营方，提供了 1 个 Orderer 节点 orderer.qq.com 来创建联盟链的基础设施，而 Taobao 和 JD 则是作为组织成员加入到链中，各自提供 2 个 Peer 节点 peer0.xx.com 和 peer1.xx.com参与工作，以及还各自创建了 2 个组织用户 Admin 和 User1 。
然后我们使用 crypto-config.yaml 文件和 cryptogen 工具为其定义所需要的证书文件以供后续使用。

## 创建排序通道创世区块

我们可以使用 configtx.yaml 文件和 configtxgen 工具轻松地创建通道的配置。
configtx.yaml 文件可以以易于理解和编辑的 yaml 格式来构建通道配置所需的信息。
configtxgen 工具通过读取 configtx.yaml 文件中的信息，将其转成 Fabric 可以读取的 protobuf 格式。

### 创建 configtx.yaml 文件
```yml
# 定义组织机构实体
Organizations:
  - &QQ
    Name: QQ # 组织的名称
    ID: QQMSP # 组织的 MSPID
    MSPDir: crypto-config/ordererOrganizations/qq.com/msp #组织的证书相对位置（生成的crypto-config目录)

  - &Taobao
    Name: Taobao
    ID: TaobaoMSP
    MSPDir: crypto-config/peerOrganizations/taobao.com/msp
    AnchorPeers: # 组织锚节点的配置
      - Host: peer0.taobao.com
        Port: 7051

  - &JD
    Name: JD
    ID: JDMSP
    MSPDir: crypto-config/peerOrganizations/jd.com/msp
    AnchorPeers: # 组织锚节点的配置
      - Host: peer0.jd.com
        Port: 7051

# 定义了排序服务的相关参数，这些参数将用于创建创世区块
Orderer: &OrdererDefaults
  # 排序节点类型用来指定要启用的排序节点实现，不同的实现对应不同的共识算法
  OrdererType: solo # 共识机制
  Addresses: # Orderer 的域名（用于连接）
    - orderer.qq.com:7050
  BatchTimeout: 2s # 出块时间间隔
  BatchSize: # 用于控制每个block的信息量
    MaxMessageCount: 10 #每个区块的消息个数
    AbsoluteMaxBytes: 99 MB #每个区块最大的信息大小
    PreferredMaxBytes: 512 KB #每个区块包含的一条信息最大长度
  Organizations:

# 定义Peer组织如何与应用程序通道交互的策略
# 默认策略：所有Peer组织都将能够读取数据并将数据写入账本
Application: &ApplicationDefaults
  Organizations:

# 用来定义用于 configtxgen 工具的配置入口
# 将 Profile 参数（ TwoOrgsOrdererGenesis 或 TwoOrgsChannel ）指定为 configtxgen 工具的参数
Profiles:
  #  TwoOrgsOrdererGenesis配置文件用于创建系统通道创世块
  #  该配置文件创建一个名为SampleConsortium的联盟
  #  该联盟在configtx.yaml文件中包含两个Peer组织Taobao和JD
  TwoOrgsOrdererGenesis:
    Orderer:
      <<: *OrdererDefaults
      Organizations:
        - *QQ
    Consortiums:
      SampleConsortium:
        Organizations:
          - *Taobao
          - *JD
  # 使用TwoOrgsChannel配置文件创建应用程序通道
  TwoOrgsChannel:
    Consortium: SampleConsortium
    Application:
      <<: *ApplicationDefaults
      Organizations:
        - *Taobao
        - *JD
```
执行 configtxgen 命令，并指定 Profile 为 TwoOrgsOrdererGenesis 参数（由2个Org参与的Orderer共识配置）：

```shell
configtxgen -profile TwoOrgsOrdererGenesis -outputBlock ./config/genesis.block -channelID firstchannel
```
排序区块是排序服务的创世区块，通过以上命令就可以预先生成创世区块的 protobuf 格式的配置文件 ./config/genesis.block 了。这一步也是为后续做准备用的。

### 创建通道配置交易

接下来，我们需要继续使用 configtxgen 根据去创建通道的交易配置，和第 4 步不同的是，这次需要指定 Profile 为 TwoOrgsChannel 参数。

生成通道配置事务 `./config/appchannel.tx` ：
```shell
configtxgen -profile TwoOrgsChannel -outputCreateChannelTx ./config/appchannel.tx -channelID appchannel
```
为 Taobao 组织定义锚节点，生成 ./config/TaobaoAnchor.tx ：
```shell
configtxgen -profile TwoOrgsChannel -outputAnchorPeersUpdate ./config/TaobaoAnchor.tx -channelID appchannel -asOrg Taobao
```
为 JD 组织定义锚节点，生成 ./config/JDAnchor.tx ：
```shell
configtxgen -profile TwoOrgsChannel -outputAnchorPeersUpdate ./config/JDAnchor.tx -channelID appchannel -asOrg JD
```
当然，这一步也是为后续使用做准备的。不过至此，需要准备的配置都齐了。

## 创建并启动各组织的节点

我们说过：我们假设 QQ 作为一个运营方，提供了 1 个 Orderer 节点 orderer.qq.com 来创建联盟链的基础设施，而 Taobao 和 JD 则是作为组织成员加入到链中，各自提供 2 个 Peer 节点 peer0.xx.com 和 peer1.xx.com 参与工作。

现在这些组织及其节点所需要的配置已经准备好了。我们接下来就可以使用 Docker Compose 来模拟启动这些节点服务。

由于这些节点之间需要互相通信，所以我们需要将这些节点都放入到一个 Docker 网络中，以 fabric_network 为例。

docker-compose.yaml 的内容如下：
```yml
version: '2.1'

volumes:
  orderer.qq.com:
  peer0.taobao.com:
  peer1.taobao.com:
  peer0.jd.com:
  peer1.jd.com:

networks:
  fabric_network:
    name: fabric_network

services:
  # 排序服务节点
  orderer.qq.com:
    container_name: orderer.qq.com
    image: hyperledger/fabric-orderer:1.4.12
    environment:
      - GODEBUG=netdns=go
      - ORDERER_GENERAL_LISTENADDRESS=0.0.0.0
      - ORDERER_GENERAL_GENESISMETHOD=file
      - ORDERER_GENERAL_GENESISFILE=/etc/hyperledger/config/genesis.block # 注入创世区块
      - ORDERER_GENERAL_LOCALMSPID=QQMSP
      - ORDERER_GENERAL_LOCALMSPDIR=/etc/hyperledger/orderer/msp # 证书相关
    command: orderer
    ports:
      - "7050:7050"
    volumes: # 挂载由cryptogen和configtxgen生成的证书文件以及创世区块
      - ./config/genesis.block:/etc/hyperledger/config/genesis.block
      - ./crypto-config/ordererOrganizations/qq.com/orderers/orderer.qq.com/:/etc/hyperledger/orderer
      - orderer.qq.com:/var/hyperledger/production/orderer
    networks:
      - fabric_network

  #  Taobao 组织 peer0 节点
  peer0.taobao.com:
    extends:
      file: docker-compose-base.yaml
      service: peer-base
    container_name: peer0.taobao.com
    environment:
      - CORE_PEER_ID=peer0.taobao.com
      - CORE_PEER_LOCALMSPID=TaobaoMSP
      - CORE_PEER_ADDRESS=peer0.taobao.com:7051
    ports:
      - "7051:7051" # grpc服务端口
      - "7053:7053" # eventhub端口
    volumes:
      - ./crypto-config/peerOrganizations/taobao.com/peers/peer0.taobao.com:/etc/hyperledger/peer
      - peer0.taobao.com:/var/hyperledger/production
    depends_on:
      - orderer.qq.com

  #  Taobao 组织 peer1 节点
  peer1.taobao.com:
    extends:
      file: docker-compose-base.yaml
      service: peer-base
    container_name: peer1.taobao.com
    environment:
      - CORE_PEER_ID=peer1.taobao.com
      - CORE_PEER_LOCALMSPID=TaobaoMSP
      - CORE_PEER_ADDRESS=peer1.taobao.com:7051
    ports:
      - "17051:7051"
      - "17053:7053"
    volumes:
      - ./crypto-config/peerOrganizations/taobao.com/peers/peer1.taobao.com:/etc/hyperledger/peer
      - peer1.taobao.com:/var/hyperledger/production
    depends_on:
      - orderer.qq.com

  #  JD 组织 peer0 节点
  peer0.jd.com:
    extends:
      file: docker-compose-base.yaml
      service: peer-base
    container_name: peer0.jd.com
    environment:
      - CORE_PEER_ID=peer0.jd.com
      - CORE_PEER_LOCALMSPID=JDMSP
      - CORE_PEER_ADDRESS=peer0.jd.com:7051
    ports:
      - "27051:7051"
      - "27053:7053"
    volumes:
      - ./crypto-config/peerOrganizations/jd.com/peers/peer0.jd.com:/etc/hyperledger/peer
      - peer0.jd.com:/var/hyperledger/production
    depends_on:
      - orderer.qq.com

  #  JD 组织 peer1 节点
  peer1.jd.com:
    extends:
      file: docker-compose-base.yaml
      service: peer-base
    container_name: peer1.jd.com
    environment:
      - CORE_PEER_ID=peer1.jd.com
      - CORE_PEER_LOCALMSPID=JDMSP
      - CORE_PEER_ADDRESS=peer1.jd.com:7051
    ports:
      - "37051:7051"
      - "37053:7053"
    volumes:
      - ./crypto-config/peerOrganizations/jd.com/peers/peer1.jd.com:/etc/hyperledger/peer
      - peer1.jd.com:/var/hyperledger/production
    depends_on:
      - orderer.qq.com

  # 客户端节点
  cli:
    container_name: cli
    image: hyperledger/fabric-tools:1.4.12
    tty: true
    environment:
      # go 环境设置
      - GO111MODULE=auto
      - GOPROXY=https://goproxy.cn
      - CORE_PEER_ID=cli
    command: /bin/bash
    volumes:
      - ./config:/etc/hyperledger/config
      - ./crypto-config/peerOrganizations/taobao.com/:/etc/hyperledger/peer/taobao.com
      - ./crypto-config/peerOrganizations/jd.com/:/etc/hyperledger/peer/jd.com
      - ./../chaincode:/opt/gopath/src/chaincode # 链码路径注入
    networks:
      - fabric_network
    depends_on:
      - orderer.qq.com
      - peer0.taobao.com
      - peer1.taobao.com
      - peer0.jd.com
      - peer1.jd.com
```

为了方便，这里我定义一个 docker-compose-base.yaml 作为 Peer 节点的公共模板，内容如下：
```yml
version: '2.1'

services:
  peer-base: # peer的公共服务
    image: hyperledger/fabric-peer:1.4.12
    environment:
      - GODEBUG=netdns=go
      - CORE_VM_ENDPOINT=unix:///host/var/run/docker.sock
      - CORE_LOGGING_PEER=info
      - CORE_CHAINCODE_LOGGING_LEVEL=INFO
      - CORE_PEER_MSPCONFIGPATH=/etc/hyperledger/peer/msp # msp证书（节点证书）
      - CORE_LEDGER_STATE_STATEDATABASE=goleveldb # 状态数据库的存储引擎（or CouchDB）
      - CORE_VM_DOCKER_HOSTCONFIG_NETWORKMODE=fabric_network # docker 网络
    volumes:
      - /var/run/docker.sock:/host/var/run/docker.sock
    working_dir: /opt/gopath/src/github.com/hyperledger/fabric
    command: peer node start
    networks:
      - fabric_network
```
注意观察，在 volumes 配置项中，我们将 config 和 crypto-config 内的配置文件都挂载到相对应的节点中了。并且在 peer 的公共服务中，我们还挂载了 /var/run/docker.sock 文件，有了该文件，在容器内就可以向其发送 http 请求和 Docker Daemon 通信，通俗理解，就是有了它，就可以在容器内操作宿主机的 Docker 了，比如在容器内控制 Docker 再启动一个容器出来。而这，就是为了后面可以部署智能合约（节点部署链码其实就是启动一个链码容器）。

现在继续将这些节点服务启动起来：
```shell
docker-compose up -d
```

### 为 cli 服务配置环境

接下来我们要使用 cli 服务来执行 peer 命令，所以要为其先配置一下环境变量，使用四个不同的变量 TaobaoPeer0Cli、TaobaoPeer1Cli、JDPeer0Cli、JDPeer1Cli ，代表 cli 服务代表着不同的节点：
```shell
$ TaobaoPeer0Cli="CORE_PEER_ADDRESS=peer0.taobao.com:7051 CORE_PEER_LOCALMSPID=TaobaoMSP CORE_PEER_MSPCONFIGPATH=/etc/hyperledger/peer/taobao.com/users/Admin@taobao.com/msp"
$ TaobaoPeer1Cli="CORE_PEER_ADDRESS=peer1.taobao.com:7051 CORE_PEER_LOCALMSPID=TaobaoMSP CORE_PEER_MSPCONFIGPATH=/etc/hyperledger/peer/taobao.com/users/Admin@taobao.com/msp"
$ JDPeer0Cli="CORE_PEER_ADDRESS=peer0.jd.com:7051 CORE_PEER_LOCALMSPID=JDMSP CORE_PEER_MSPCONFIGPATH=/etc/hyperledger/peer/jd.com/users/Admin@jd.com/msp"
$ JDPeer1Cli="CORE_PEER_ADDRESS=peer1.jd.com:7051 CORE_PEER_LOCALMSPID=JDMSP CORE_PEER_MSPCONFIGPATH=/etc/hyperledger/peer/jd.com/users/Admin@jd.com/msp"
```

### 开始创建通道

我们现在有请 Taobao 组织的 peer0 节点来创建一个通道 appchannel ：

```shell
docker exec cli bash -c "$TaobaoPeer0Cli peer channel create -o orderer.qq.com:7050 -c appchannel -f /etc/hyperledger/config/appchannel.tx"
```
通道就相当于“群聊”， Taobao 组织的 peer0 节点创建了一个名称为 appchannel 的“群聊”。

### 将所有节点加入通道

将所有的节点都加入到通道 appchannel 中（正常是按需加入）：
```shell
$ docker exec cli bash -c "$TaobaoPeer0Cli peer channel join -b appchannel.block"
$ docker exec cli bash -c "$TaobaoPeer1Cli peer channel join -b appchannel.block"
$ docker exec cli bash -c "$JDPeer0Cli peer channel join -b appchannel.block"
$ docker exec cli bash -c "$JDPeer1Cli peer channel join -b appchannel.block"
```
这时相当于大家都加入到了 appchannel “群聊”中，之后大家都可以在里面“聊天”了。

### 更新锚节点

锚节点是必需的。普通节点只能发现本组织下的其它节点，而锚节点可以跨组织服务发现到其它组织下的节点，建议每个组织都选择至少一个锚节点。

利用之前准备好的配置文件，向通道更新锚节点：
```shell
$ docker exec cli bash -c "$TaobaoPeer0Cli peer channel update -o orderer.qq.com:7050 -c appchannel -f /etc/hyperledger/config/TaobaoAnchor.tx"
$ docker exec cli bash -c "$JDPeer0Cli peer channel update -o orderer.qq.com:7050 -c appchannel -f /etc/hyperledger/config/JDAnchor.tx"
```
这样，Taobao 和 JD 组织间的节点就都可以互相发现了。

### 编写智能合约
fabric 的智能合约称为链码，编写智能合约也就是编写链码。

链码其实很简单，可以由 Go 、 node.js 、或者 Java 编写，其实只是实现一些预定义的接口。

以 Go 为例，创建一个 main.go 文件：
```go
package main

import (
 "fmt"

 "github.com/hyperledger/fabric/core/chaincode/shim"
 pb "github.com/hyperledger/fabric/protos/peer"
)

type MyChaincode struct {
}

// Init 初始化时会执行该方法
func (c *MyChaincode) Init(stub shim.ChaincodeStubInterface) pb.Response {
 fmt.Println("链码初始化")
 return shim.Success(nil)
}

// Invoke 智能合约的功能函数定义
func (c *MyChaincode) Invoke(stub shim.ChaincodeStubInterface) pb.Response {
 funcName, args := stub.GetFunctionAndParameters()
 switch funcName {

 default:
  return shim.Error(fmt.Sprintf("没有该功能: %s", funcName))
 }
}

func main() {
 err := shim.Start(new(MyChaincode))
 if err != nil {
  panic(err)
 }
}
```
我们定义的 MyChaincode 结构体实现了 shim.Chaincode 接口：
```go
type Chaincode interface {

 Init(stub ChaincodeStubInterface) pb.Response
 Invoke(stub ChaincodeStubInterface) pb.Response
}
```
### 链码的功能定义在 Invoke 方法中。

```go
package main

import (
 "encoding/json"
 "errors"
 "fmt"
 "strconv"

 "github.com/hyperledger/fabric/core/chaincode/shim"
 pb "github.com/hyperledger/fabric/protos/peer"
)

type MyChaincode struct {
}

// Init 初始化时会执行该方法
func (c *MyChaincode) Init(stub shim.ChaincodeStubInterface) pb.Response {
 fmt.Println("链码初始化")
 // 假设A有1000元，以复合主键 userA 的形式写入账本
 err := WriteLedger(stub, map[string]interface{}{"name": "A", "balance": 1000}, "user", []string{"A"})
 if err != nil {
  return shim.Error(err.Error())
 }
 // 假设B有1000元，以复合主键 userB 的形式写入账本
 err = WriteLedger(stub, map[string]interface{}{"name": "B", "balance": 1000}, "user", []string{"B"})
 if err != nil {
  return shim.Error(err.Error())
 }
 return shim.Success(nil)
}

// Invoke 智能合约的功能函数定义
func (c *MyChaincode) Invoke(stub shim.ChaincodeStubInterface) pb.Response {
 funcName, args := stub.GetFunctionAndParameters()
 switch funcName {
 case "query":
  return query(stub, args)
 case "transfer":
  return transfer(stub, args)
 default:
  return shim.Error(fmt.Sprintf("没有该功能: %s", funcName))
 }
}

func query(stub shim.ChaincodeStubInterface, args []string) pb.Response {
 // 如果 args 为空，则表示查询所有 user
 results, err := ReadLedger(stub, "user", args)
 if err != nil {
  return shim.Error(err.Error())
 }
 var users []map[string]interface{}
 for _, result := range results {
  var user map[string]interface{}
  if err = json.Unmarshal(result, &user); err != nil {
   return shim.Error(err.Error())
  }
  users = append(users, user)
 }
 usersByte, err := json.Marshal(&users)
 if err != nil {
  return shim.Error(err.Error())
 }
 return shim.Success(usersByte)
}

func transfer(stub shim.ChaincodeStubInterface, args []string) pb.Response {
 // 验证参数
 if len(args) != 3 {
  return shim.Error("参数个数不满足")
 }
 from := args[0]
 to := args[1]
 money, err := strconv.ParseFloat(args[2], 64)
 if err != nil {
  return shim.Error(err.Error())
 }
 // 从账本查询 from 用户
 fromResults, err := ReadLedger(stub, "user", []string{from})
 if err != nil {
  return shim.Error(err.Error())
 }
 if len(fromResults) != 1 {
  return shim.Error("没有该用户 " + from)
 }
 var fromUser map[string]interface{}
 if err = json.Unmarshal(fromResults[0], &fromUser); err != nil {
  return shim.Error(err.Error())
 }
 // 从账本查询 to 用户
 toResults, err := ReadLedger(stub, "user", []string{to})
 if err != nil {
  return shim.Error(err.Error())
 }
 if len(toResults) != 1 {
  return shim.Error("没有该用户 " + to)
 }
 var toUser map[string]interface{}
 if err = json.Unmarshal(toResults[0], &toUser); err != nil {
  return shim.Error(err.Error())
 }
 // from 用户扣除余额
 if money > fromUser["balance"].(float64) {
  return shim.Error("余额不足")
 }
 fromUser["balance"] = fromUser["balance"].(float64) - money
 // to 用户增加余额
 toUser["balance"] = toUser["balance"].(float64) + money
 // 写回账本
 err = WriteLedger(stub, fromUser, "user", []string{from})
 if err != nil {
  return shim.Error(err.Error())
 }
 err = WriteLedger(stub, toUser, "user", []string{to})
 if err != nil {
  return shim.Error(err.Error())
 }
 return shim.Success([]byte("ok"))
}

func main() {
 err := shim.Start(new(MyChaincode))
 if err != nil {
  panic(err)
 }
}

// WriteLedger 写入账本
// obj 为要写入的数据
// objectType和keys 共同组成复合主键
func WriteLedger(stub shim.ChaincodeStubInterface, obj interface{}, objectType string, keys []string) error {
 //创建复合主键
 var key string
 if val, err := stub.CreateCompositeKey(objectType, keys); err != nil {
  return errors.New(fmt.Sprintf("%s-创建复合主键出错 %s", objectType, err.Error()))
 } else {
  key = val
 }
 bytes, err := json.Marshal(obj)
 if err != nil {
  return err
 }
 //写入区块链账本
 if err := stub.PutState(key, bytes); err != nil {
  return errors.New(fmt.Sprintf("%s-写入区块链账本出错: %s", objectType, err.Error()))
 }
 return nil
}

// ReadLedger 根据复合主键查询账本数据(适合获取全部或指定的数据)
// objectType和keys 共同组成复合主键
func ReadLedger(stub shim.ChaincodeStubInterface, objectType string, keys []string) (results [][]byte, err error) {
 // 通过主键从区块链查找相关的数据，相当于对主键的模糊查询
 resultIterator, err := stub.GetStateByPartialCompositeKey(objectType, keys)
 if err != nil {
  return nil, errors.New(fmt.Sprintf("%s-获取全部数据出错: %s", objectType, err))
 }
 defer resultIterator.Close()

 //检查返回的数据是否为空，不为空则遍历数据，否则返回空数组
 for resultIterator.HasNext() {
  val, err := resultIterator.Next()
  if err != nil {
   return nil, errors.New(fmt.Sprintf("%s-返回的数据出错: %s", objectType, err))
  }

  results = append(results, val.GetValue())
 }
 return results, nil
}
```

### 部署链码
我们将刚刚编写的智能合约也就是链码安装到区块链网络中，同样是借助 cli 服务，我们在 Taobao 组织的 peer0节点和 JD 组织的 peer0 节点上都安装上链码：
```shell
$ docker exec cli bash -c "$TaobaoPeer0Cli peer chaincode install -n fabric-realty -v 1.0.0 -l golang -p chaincode"
$ docker exec cli bash -c "$JDPeer0Cli peer chaincode install -n fabric-realty -v 1.0.0
```

其中 `-n` 参数是链码名称，可以自己随便设置，-v 是链码版本号，`-p` 是链码的目录（我们已经将链码挂载到 cli容器中了，在 /opt/gopath/src/ 目录下）

链码安装后，还需要实例化后才可以使用，只需要在任意一个节点实例化就可以了，以 Taobao 组织的 peer0 节点为例：
```shell
$ docker exec cli bash -c "$TaobaoPeer0Cli peer chaincode instantiate -o orderer.qq.com:7050 -C appchannel -n fabric-realty -l golang -v 1.0.0 -c '{\"Args\":[\"init\"]}' -P \"AND ('TaobaoMSP.member','JDMSP.member')\""
```
实例化链码主要就是传入 {"Args":["init"]} 参数，此时会调用我们编写的 func (c *MyChaincode) Init 方法，进行链码的初始化。
其中 -P 参数用于指定链码的背书策略，AND ('TaobaoMSP.member','JDMSP.member') 代表链码的写入操作需要同时得到 Taobao和 JD 组织成员的背书才允许通过。AND 也可以替换成 OR，代表任意一组织成员背书即可，更多具体用法，可以去看官方文档。

查看启动的链码容器：
```shell
$ docker ps -a | awk '($2 ~ /dev-peer.*fabric-realty.*/) {print $2}'
```
`dev-peer0.taobao.com-fabric-realty-1.0.0-4f127a0415dd835529133a69b480ce24581dd5ddcaf18426ecc1d3dfb02b4670`
因为我们使用 Taobao 组织的 peer0 节点实例化链码，所以此时还只有这个节点的链码容器启动起来了。

链码实例化成功之后就会启动链码容器，而启动的方法，就是我们之前提过的 peer 节点服务挂载了 /var/run/docker.sock 文件。

我们可以试着使用 cli 服务去调用链码：
```shell
$ docker exec cli bash -c "$TaobaoPeer0Cli peer chaincode invoke -C appchannel -n fabric-realty -c '{\"Args\":[\"query\"]}'"
```
```txt
2022-03-22 21:13:40.152 UTC [chaincodeCmd] InitCmdFactory -> INFO 001 Retrieved channel (appchannel) orderer endpoint: orderer.qq.com:7050
2022-03-22 21:13:40.157 UTC [chaincodeCmd] chaincodeInvokeOrQuery -> INFO 002 Chaincode invoke successful. result: status:200 payload:"[{\
"balance\":1000,\"name\":\"A\"},{\"balance\":1000,\"name\":\"B\"}]"
```
使用JD组织的节点去查询：
```shell
$ docker exec cli bash -c "$JDPeer0Cli peer chaincode invoke -C appchannel -n fabric-realty -c '{\"Args\":[\"query\"]}'"
```
```txt
2022-03-22 21:14:45.397 UTC [chaincodeCmd] InitCmdFactory -> INFO 001 Retrieved channel (appchannel) orderer endpoint: orderer.qq.com:7050
2022-03-22 21:14:45.402 UTC [chaincodeCmd] chaincodeInvokeOrQuery -> INFO 002 Chaincode invoke successful. result: status:200 payload:"[{\
"balance\":1000,\"name\":\"A\"},{\"balance\":1000,\"name\":\"B\"}]"
```
此时，因为我们查询了 JD 组织的 peer0 节点上的链码，所以对应的链码容器也会启动起来了。

### 编写应用程序
在部署链码之后，我们是使用 cli 服务去调用的，但这种方式一般只是作为验证使用，更多情况下，应该是我们自己编写应用程序集成 fabric 提供的 SDK 去调用。

Go 语言可以使用官方的 `github.com/hyperledger/fabric-sdk-go` 库。

这个 SDK 使用起来也很简单。

第一步调用其 New 方法创建一个 FabricSDK 实例，后续使用这个实例就可以调用操作合约的方法了。
```go
// New 根据提供的一组选项初始化 SDK
// ConfigOptions 提供应用程序配置
func New(configProvider core.ConfigProvider, opts ...Option) (*FabricSDK, error) {
 pkgSuite := defPkgSuite{}
 return fromPkgSuite(configProvider, &pkgSuite, opts...)
}
```
其中 configProvider 可以从 Reader(实现了io.Reader接口的实例) 、 File(文件) 或 Raw([]byte) 获取。我们选择最简单的文件方式。

创建一个 config.yaml ，配置如下：
```yml
version: 1.0.0

# GO SDK 客户端配置
client:
  # 客户端所属的组织，必须是organizations定义的组织
  organization: JD
  # 日志级别
  logging:
    level: info
  # MSP证书的根路径
  cryptoconfig:
    path: /network/crypto-config

# 通道定义
channels:
  appchannel:
    orderers:
      - orderer.qq.com
    peers:
      peer0.jd.com:
        endorsingPeer: true
        chaincodeQuery: true
        ledgerQuery: true
        eventSource: true
      peer1.jd.com:
        endorsingPeer: true
        chaincodeQuery: true
        ledgerQuery: true
        eventSource: true

# 组织配置
organizations:
  JD:
    mspid: "JDMSP"
    cryptoPath: peerOrganizations/jd.com/users/{username}@jd.com/msp
    peers:
      - peer0.jd.com
      - peer1.jd.com

# orderer节点列表
orderers:
  orderer.qq.com:
    url: orderer.qq.com:7050
    # 传递给gRPC客户端构造函数
    grpcOptions:
      ssl-target-name-override: orderer.qq.com
      keep-alive-time: 0s
      keep-alive-timeout: 20s
      keep-alive-permit: false
      fail-fast: false
      allow-insecure: true

# peers节点列表
peers:
  # peer节点定义，可以定义多个
  peer0.jd.com:
    # URL用于发送背书和查询请求
    url: peer0.jd.com:7051
    # 传递给gRPC客户端构造函数
    grpcOptions:
      ssl-target-name-override: peer0.jd.com
      keep-alive-time: 0s
      keep-alive-timeout: 20s
      keep-alive-permit: false
      fail-fast: false
      allow-insecure: true
  peer1.jd.com:
    url: peer1.jd.com:7051
    grpcOptions:
      ssl-target-name-override: peer1.jd.com
      keep-alive-time: 0s
      keep-alive-timeout: 20s
      keep-alive-permit: false
      fail-fast: false
      allow-insecure: true
  peer0.taobao.com:
    url: peer0.taobao.com:7051
    grpcOptions:
      ssl-target-name-override: peer0.taobao.com
      keep-alive-time: 0s
      keep-alive-timeout: 20s
      keep-alive-permit: false
      fail-fast: false
      allow-insecure: true
  peer1.taobao.com:
    url: peer1.taobao.com:7051
    grpcOptions:
      ssl-target-name-override: peer1.taobao.com
      keep-alive-time: 0s
      keep-alive-timeout: 20s
      keep-alive-permit: false
      fail-fast: false
      allow-insecure: true
```
我们假定是 JD 组织来编写这个应用程序，该配置主要就是用于验证 JD 组织及其节点的身份。

其中组织配置中 {username} 为动态传递， MSP 证书的根路径我们后续会挂载进去。

现在开始编写代码，我们先来实例化 SDK ，创建 sdk.go：
```go
package main

import (
 "github.com/hyperledger/fabric-sdk-go/pkg/client/channel"
 "github.com/hyperledger/fabric-sdk-go/pkg/core/config"
 "github.com/hyperledger/fabric-sdk-go/pkg/fabsdk"
)

// 配置信息
var (
 sdk           *fabsdk.FabricSDK                              // Fabric SDK
 channelName   = "appchannel"                                 // 通道名称
 username      = "Admin"                                      // 用户
 chainCodeName = "fabric-realty"                              // 链码名称
 endpoints     = []string{"peer0.jd.com", "peer0.taobao.com"} // 要发送交易的节点
)

// init 初始化
func init() {
 var err error
 // 通过配置文件初始化SDK
 sdk, err = fabsdk.New(config.FromFile("config.yaml"))
 if err != nil {
  panic(err)
 }
}

// ChannelExecute 区块链交互
func ChannelExecute(fcn string, args [][]byte) (channel.Response, error) {
 // 创建客户端，表明在通道的身份
 ctx := sdk.ChannelContext(channelName, fabsdk.WithUser(username))
 cli, err := channel.New(ctx)
 if err != nil {
  return channel.Response{}, err
 }
 // 对区块链账本的写操作（调用了链码的invoke）
 resp, err := cli.Execute(channel.Request{
  ChaincodeID: chainCodeName,
  Fcn:         fcn,
  Args:        args,
 }, channel.WithTargetEndpoints(endpoints...))
 if err != nil {
  return channel.Response{}, err
 }
 //返回链码执行后的结果
 return resp, nil
}

// ChannelQuery 区块链查询
func ChannelQuery(fcn string, args [][]byte) (channel.Response, error) {
 // 创建客户端，表明在通道的身份
 ctx := sdk.ChannelContext(channelName, fabsdk.WithUser(username))
 cli, err := channel.New(ctx)
 if err != nil {
  return channel.Response{}, err
 }
 // 对区块链账本查询的操作（调用了链码的invoke），只返回结果
 resp, err := cli.Query(channel.Request{
  ChaincodeID: chainCodeName,
  Fcn:         fcn,
  Args:        args,
 }, channel.WithTargetEndpoints(endpoints...))
 if err != nil {
  return channel.Response{}, err
 }
 //返回链码执行后的结果
 return resp, nil
}
```
在这段代码中，我们将使用 Admin 的身份去调用合约，并将每次的交易同时发送给 peer0.jd.com 和 peer0.taobao.com 节点进行背书，这是因为我们在实例化链码的时候指定了背书策略为 AND ('TaobaoMSP.member','JDMSP.member') ，代表交易需要同时得到 Taobao和 JD 组织成员的背书才允许通过。每次写入账本时，会验证这两个节点的数据一致性，只有当这两个节点的数据一致时，交易才算最终成功。

继续编写 main.go ，我们使用 gin 来创建一个 http 服务：
```go
package main

import (
 "bytes"
 "encoding/json"

 "github.com/gin-gonic/gin"
)

func main() {
 g := gin.Default()
 g.GET("/query", func(c *gin.Context) {
  args := make([][]byte, 0)
  user := c.Query("user")
  if user != "" {
   args = append(args, []byte(user))
  }
  // 调用链码的query函数
  resp, err := ChannelQuery("query", args)
  if err != nil {
   c.AbortWithStatusJSON(500, gin.H{"err": err.Error()})
   return
  }
  var data []map[string]interface{}
  if err = json.Unmarshal(bytes.NewBuffer(resp.Payload).Bytes(), &data); err != nil {
   c.AbortWithStatusJSON(500, gin.H{"err": err.Error()})
   return
  }
  c.JSON(200, data)
 })
 g.POST("/transfer", func(c *gin.Context) {
  from := c.Query("from")
  to := c.Query("to")
  money := c.Query("money")
  if from == "" || to == "" || money == "" {
   c.AbortWithStatusJSON(400, gin.H{"err": "参数不能为空"})
   return
  }
  args := make([][]byte, 0)
  args = append(args, []byte(from), []byte(to), []byte(money))
  // 调用链码的transfer函数
  resp, err := ChannelExecute("transfer", args)
  if err != nil {
   c.AbortWithStatusJSON(500, gin.H{"err": err.Error()})
   return
  }
  c.JSON(200, gin.H{"msg": string(resp.Payload)})
 })
 g.Run("0.0.0.0:8000")
}
```
在 main 函数中，我们创建了两个接口 GET /query 和 POST /transfer ，其中 /query 接口调用链码的 query 函数功能实现查询用户余额，/transfer 接口调用链码的 transfer 函数功能实现转账功能。

我们将继续使用 Docker 部署该应用程序，这样的好处是可以和区块链网络处于同一网络下，方便调用节点，当然你也可以更改 config.yaml 文件去调用暴露在宿主机的节点端口也是可以的，首先编写 Dockerfile 文件：

```yml
FROM golang:1.14 AS app
ENV GO111MODULE=on
ENV GOPROXY https://goproxy.cn,direct
WORKDIR /root/togettoyou
COPY . .
RUN CGO_ENABLED=0 go build -v -o "app" .

FROM scratch
WORKDIR /root/togettoyou/
COPY --from=app /root/togettoyou/app ./
COPY --from=app /root/togettoyou/config.yaml ./
ENTRYPOINT ["./app"]
docker-compose.yml 文件：

version: '2.1'

networks:
  fabric_network:
    external:
      name: fabric_network

services:
  app:
    build: .
    image: app:latest
    ports:
      - "8000:8000"
    volumes:
      - ./../network/crypto-config:/network/crypto-config # 挂载搭建区块链网络时生成的crypto-config文件夹
    networks:
      - fabric_network
```

其中挂载的 crypto-config 文件夹就是之前搭建区块链网络时生成的。

编译部署应用程序：

```shell
$ docker-compose build
$ docker-compose up
```
调用应用程序的接口：

```shell
$ curl "http://localhost:8000/query"
[{"balance":1000,"name":"A"},{"balance":1000,"name":"B"}]
```
```shell
$ curl "http://localhost:8000/query?user=A"
[{"balance":1000,"name":"A"}]
```
```shell
$ curl "http://localhost:8000/query?user=B"
[{"balance":1000,"name":"B"}]
```
```shell
$ curl -X POST "http://localhost:8000/transfer?from=A&to=B&money=500"
{"msg":"ok"}
```
```shell
$ curl "http://localhost:8000/query"
[{"balance":500,"name":"A"},{"balance":1500,"name":"B"}]
```
到这里，我们就已经完整地实现了一个区块链应用了。你也可以继续为这个区块链应用实现前端页面。流程呢，和传统前后端分离架构也没什么区别。

