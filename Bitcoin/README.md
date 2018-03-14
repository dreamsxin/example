# 编译比特币核心

## 下载源码

```shell
git clone https://github.com/bitcoin/bitcoin.git
```

## 示所有发布的比特币版本

```shell
git tag
```

## 选择最高版本的版本，根据惯例，用于测试的发布候选版本具有后缀“rc”

```shell
git checkout v0.11.2
```

## 确认版本

```shell
git status
```

## 安装依赖

```shell
sudo apt-get install build-essential libtool autotools-dev autoconf pkg-config libssl-dev
sudo apt-get install libboost-all-dev
sudo apt-get install libqt5gui5 libqt5core5a libqt5dbus5 qttools5-dev qttools5-dev-tools libprotobuf-dev protobuf-compiler
sudo apt-get install libqrencode-dev autoconf openssl libssl-dev libevent-dev
sudo apt-get install libminiupnpc-dev
sudo apt-get install libdb++-dev
```

## 构建 bitcoind

See `doc/build-unix.md`

* 生成配置脚本

创建一组自动配置脚本，它会询问系统以发现正确的设置，并确保您拥有编译代码所需的所有库。 

```shell
./autogen.sh
```

* 配置

键入`./configure --help`查看各种选项：

```text
`configure' configures Bitcoin Core 0.11.2 to adapt to many kinds of systems.

Usage: ./configure [OPTION]... [VAR=VALUE]...

...
Optional Features:
  --disable-option-checking  ignore unrecognized --enable/--with options
  --disable-FEATURE       do not include FEATURE (same as --enable-FEATURE=no)
  --enable-FEATURE[=ARG]  include FEATURE [ARG=yes]

  --enable-wallet         enable wallet (default is yes)

  --with-gui[=no|qt4|qt5|auto]
...
```

允许使用`--enable-FEATURE`和`--disable-FEATURE`标志来启用或禁用`bitcoind`的某些功能，其中`FEATURE`由功能名称替换，如`wallet`。

一些有用的选项：

* --prefix=$HOME

这将覆盖生成的可执行文件的默认安装位置（它是`/usr/local/`）。 使用`$HOME`将所有内容放在您的主目录或不同的路径中。

* --disable-wallet

这用于禁用参考钱包的实现。

* --with-incompatible-bdb

如果您正在构建钱包，请允许使用不兼容版本的Berkeley DB库。

* --with-gui=no

不要构建图形用户界面，图形界面需要Qt库。

接下来，运行`configure`脚本来自动发现所有必需的库，并为您的系统创建一个自定义的构建脚本：

```shell
./configure --with-incompatible-bdb
```

* 构建可执行文件

```shell
make && make install
```

## 运行比特币核心节点

当你第一次运行`bitcoind`时，它会提醒你用一个安全密码给JSON-RPC接口创建一个配置文件。该密码控制对Bitcoin Core提供的应用程序编程接口（API）的访问。

```text
$ bitcoind
Error: To use the "-server" option, you must set a rpcpassword in the configuration file:
/home/ubuntu/.bitcoin/bitcoin.conf
It is recommended you use the following random password:
rpcuser=bitcoinrpc
rpcpassword=2XA4DuKNCbtZXsBQRRNDEwEY2nM6M4H9Tx5dFjoAVVbK
(you do not need to remember this password)
The username and password MUST NOT be the same.
If the file does not exist, create it with owner-readable-only file permissions.
It is also recommended to set alertnotify so you are notified of problems;
```
第一次运行`bitcoind`它会告诉你，你需要建立一个配置文件，至少有一个`rpcuser`和`rpcpassword`条目。 另外，建议您设置警报机制。

在`.bitcoin`目录（在用户的主目录下）中创建一个文件，以便它被命名为`.bitcoin/bitcoin.conf`并提供用户名和密码：
```text
rpcuser=bitcoinrpc
rpcpassword=CHANGE_THIS
```

除了`rpcuser`和`rpcpassword`选项，`Bitcoin Core`还提供了100多个配置选项，可以修改网络节点的行为，区块链的存储以及其操作的许多其他方面。

要查看这些选项的列表，请运行`bitcoind --help`。

以下是您可以在配置文件中设置的一些最重要的选项，或作为bitcoind的命令行参数：

- regtest

表示启动的是 regtest 网络；不同网络之间的区别在于创世区块不同。

- alertnotify

运行指定的命令或脚本，通常通过电子邮件将紧急警报发送给该节点的所有者。

- conf

配置文件的另一个位置。 这只是作为bitcoind的命令行参数有意义，因为它不能在它引用的配置文件内。

- datadir

选择要放入所有块链数据的目录和文件系统。 默认情况下，这是您的主目录的.bitcoin子目录。 确保这个文件系统具有几GB的可用空间。

- prune

通过删除旧的块，将磁盘空间要求降低到这个兆字节。 在资源受限的节点上不能满足完整块的节点使用这个。

- txindex

维护所有交易的索引。 这意味着可以通过ID以编程方式检索任何交易的块链的完整副本。

- maxconnections

设置接受连接的最大节点数。 从默认值减少该值将减少您的带宽消耗。 如果您的网络是按照流量计费，请使用。

- maxmempool

将交易内存池限制在几兆字节。 使用它来减少节点的内存使用。

- maxreceivebuffer/maxsendbuffer

将每连接内存缓冲区限制为1000字节的多个倍数。 在内存受限节点上使用。

- minrelaytxfee

设置您将继续的最低费用交易。 低于此值，交易被视为零费用。 在内存受限的节点上使用它来减少内存中交易池的大小。

### Regtest Mode

```shell
bitcoind -regtest -daemon
```

创建区块获取比特币

在 Bitcoin Core 0.10.1 或更早版本使用：
```shell
bitcoin-cli -regtest setgenerate true 101
```

最新版本：
```shell
bitcoin-cli -regtest generate 101
```

查看余额
```shell
bitcoin-cli -regtest getbalance
```
查看的是默认钱包 `2Mwj9ou3nV8x1QF5Y77gmEdaazqZyLFdAYz`

获取新钱包地址
```shell
bitcoin-cli -regtest getnewaddress
```
输出：
>> 2NBw7SpdGnXJH3eFGhYdRb8rgB5TrqpVt62

转 10 比特币给新的地址
```shell
bitcoin-cli -regtest sendtoaddress "2NBw7SpdGnXJH3eFGhYdRb8rgB5TrqpVt62" 10.00
```
输出：
>> 77cc99ec31024c8b8f037ee18b75e84c90ca904d1179a27b760f8b9c61971424

此时查看新钱包余额为 0
```shell
bitcoin-cli -regtest getbalance "2NBw7SpdGnXJH3eFGhYdRb8rgB5TrqpVt62"
```

查看交易信息
```shell
bitcoin-cli -regtest listunspent
```
此时交易信息没有确认，所以使用以下参数列出信息
```shell
bitcoin-cli -regtest listunspent 0
```

生成新的区块
```shell
bitcoin-cli -regtest generate 1
```

查看交易信息
```shell
bitcoin-cli -regtest listunspent
```

## 交易数据库索引和`txindex`选项

默认情况下，`Bitcoin Core`构建一个仅包含与用户钱包有关的交易的数据库。 如果您想要使用诸如`getrawtransaction`之类的命令访问任何交易，则需要配置`txindex = 1`选项来实现。如果不想一开始设置此选项，后期再想设置为完全索引，则需要使用-reindex选项重新启动bitcoind，并等待它重建索引。

下面的完整索引节点的例子配置显示了如何将上述选项与完全索引节点组合起来，作为比特币应用程序的API后端运行：
```text
alertnotify=myemailscript.sh "Alert: %s"
datadir=/lotsofspace/bitcoin
txindex=1
rpcuser=bitcoinrpc
rpcpassword=CHANGE_THIS
```

下面是小型服务器资源不足配置示例：
```text
alertnotify=myemailscript.sh "Alert: %s"
maxconnections=15
prune=5000
minrelaytxfee=0.0001
maxmempool=200
maxreceivebuffer=2500
maxsendbuffer=500
rpcuser=bitcoinrpc
rpcpassword=CHANGE_THIS
```

编辑配置文件并设置最符合您需求的选项后，可以使用此配置测试 `bitcoind`。 运行Bitcoin Core，使用选项`printtoconsole`在前台运行输出到控制台：
```shell
bitcoind -printtoconsole
```

在后台运行Bitcoin Core作为进程，使用守护程序选项启动它，如`bitcoind -daemon`。

## 基本命令

* 启动主网节点
```shell
bitcoind --daemon
```

* 关闭主网节点
```shell
bitcoin-cli stop
```

* 启动测试节点
```shell
bitcoind -testnet --daemon
```

* 关闭测试节点
```shell
bitcoin-cli -testnet --daemon
```

### 钱包的基本使用

1. 从钱包获取新地址

```shell
bitcoin-cli -testnet getnewaddress
```

输出 `3FKunsnponA5TsEt6ux6YfrRrrUDUdod2R`

2. 查询钱包的所有地址

```shell
bitcoin-cli -testnet getaddressesbyaccount ''
```
Out：
```text
[
  "mpFLSDjCcxekrrcdQd62kyUrPHiAGck8mx", 
  "mxUZ5SQfEcmCH5b8KnsDs26byEtSdwbv23", 
  "my1kprdpM17PZJnB55GwJ5AzLGjx9isHh9"
]
```

3. 查询钱包余额

```shell
bitcoin-cli -testnet getbalance
```

4. 查询钱包UTXO

```shell
bitcoin-cli -testnet listunspent
```

Out：
```text
[
  {
    "txid": "6b128ad4817d6deeb5397a4087abd9b556ffe10ccfa2e2bed31528e941b52ca1",
    "vout": 32,
    "address": "my1kprdpM17PZJnB55GwJ5AzLGjx9isHh9",
    "account": "",
    "scriptPubKey": "76a914bfeca0a0170a0baa2f77bdaf71d9878ef05c57a588ac",
    "amount": 4.75838456,
    "confirmations": 24853,
    "spendable": true,
    "solvable": true
  }, 
  {
    "txid": "be4c2f54f6dbc5fa0db1a1567d31d6b4d6619e7bcb3ea5e985c1dc792f3804ec",
    "vout": 1,
    "address": "my1kprdpM17PZJnB55GwJ5AzLGjx9isHh9",
    "account": "",
    "scriptPubKey": "76a914bfeca0a0170a0baa2f77bdaf71d9878ef05c57a588ac",
    "amount": 0.00100000,
    "confirmations": 25605,
    "spendable": true,
    "solvable": true
  }
]
```

5. 根据UTXO的txid和vout查询UTXO的详细信息

```shell
bitcoin-cli -testnet gettxout 6b128ad4817d6deeb5397a4087abd9b556ffe10ccfa2e2bed31528e941b52ca1 32
```
Out：
```text
{
  "bestblock": "00000000000001574bdd2bba2b50a81455f913789c2648758637effa6ec377fd",
  "confirmations": 24853,
  "value": 4.75838456,
  "scriptPubKey": {
    "asm": "OP_DUP OP_HASH160 bfeca0a0170a0baa2f77bdaf71d9878ef05c57a5 OP_EQUALVERIFY OP_CHECKSIG",
    "hex": "76a914bfeca0a0170a0baa2f77bdaf71d9878ef05c57a588ac",
    "reqSigs": 1,
    "type": "pubkeyhash",
    "addresses": [
      "my1kprdpM17PZJnB55GwJ5AzLGjx9isHh9"
    ]
  },
  "version": 2,
  "coinbase": false
}
```

6. 查询区块信息

```shell
bitcoin-cli -testnet getblock 00000000000001c30110204b7554c1b41006000fb15bd174f64ade661da8419f
```

## 钱包设置及加密

* 加密钱包
```shell
bitcoin-cli encryptwallet foo
```

* 解锁钱包
```shell
bitcoin-cli walletpassphrase foo 360
```

* 打印私钥

```shell
bitcoin-cli dumpprivkey "3FKunsnponA5TsEt6ux6YfrRrrUDUdod2R"
```

## 钱包备份、纯文本导出及恢复

* 创建钱包的备份文件
```shell
bitcoin-cli backupwallet wallet.backup
```
* 重新加载备份文件
```shell
bitcoin-cli importwallet wallet.backup
```
* 将钱包转储为人类可读的文本文件
```shell
bitcoin-cli dumpwallet wallet.txt
```


## 钱包地址及接收交易

* 获得其中的一个地址
```shell
bitcoin-cli getnewaddress
```
* 询问此地址已经接收到的比特币数额
```shell
bitcoin-cli getreceivedbyaddress 13Bqx1N77WKSC6ZkFxTmamLNSJDRRL9H51 0
```
* 整个钱包接收到的交易
```shell
bitcoin-cli listtransactions
```
* 列出整个钱包的所有地址
```shell
bitcoin-cli getaddressesbyaccount ""
```
* 所有经过至少minconf个确认的交易加和后的余额
```shell
bitcoin-cli getbalance
```

### 交易相关命令

1. 发起一笔交易

根据钱包的UTXO情况发起交易，然后输入转账地址

```shell
bitcoin-cli -testnet createrawtransaction '[{"txid" : "be4c2f54f6dbc5fa0db1a1567d31d6b4d6619e7bcb3ea5e985c1dc792f3804ec", "vout" : 1}]' '{"mpFLSDjCcxekrrcdQd62kyUrPHiAGck8mx": 0.0005, "mxUZ5SQfEcmCH5b8KnsDs26byEtSdwbv23": 0.00045}'
```

2. 根据上面的输出结果解码

```shell
bitcoin-cli -testnet decoderawtransaction xxx
```

>> 没有签名的交易里`scriptSig`内容为空。

3. 对交易进行签名

```shell
bitcoin-cli -testnet signrawtransaction xxx
``` 
此时再对输出结果解码：
```shell
bitcoin-cli -testnet decoderawtransaction xxx
```
`scriptSig` 里就有内容了

4. 把该交易广播到网络

```shell
bitcoin-cli -testnet sendrawtransaction xxx
```
返回txid

5. 根据txid查询交易

```shell
bitcoin-cli -testnet gettransaction xxx
```

6. 查询钱包的交易信息

```shell
bitcoin-cli -testnet listtransactions
```

7. 根据交易id获取序列化信息

```shell
bitcoin-cli -testnet getrawtransaction xxx
```

8. 根据序列化数据解码交易

```shell
bitcoin-cli -testnet decoderawtransaction xxx
```


### 其它

* 挖101个block，挖矿的奖励要在100个block以后才能使用

```text
$ bitcoin-cli -regtest generate 101
[
  "26d4fb4dd449b93ebbda9a36f390d2c7b1dd9557e662840fe61b1f6e3a09d218",
  .... #共101块
]
```

* 钱包默认账户的地址; 测试网络的地址是m或n开头的

```text
$ bitcoin-cli -regtest getaccountaddress ""
mmmSrYNiRQzeiTypt8rF9L3F742PW4ksmL
```

* 查看挖矿奖励（默认账户）

```text
$ bitcoin-cli -regtest getbalance
50.00000000
```

```text
$ bitcoin-cli -regtest getnewaddress #分配一个新的地址
mn41xHRzXuUAAVtujPLLo3Hzbe63GSP117
```
* 往这个新生成的地址转10个BTC；输出是交易的hash
```text
$ bitcoin-cli -regtest sendtoaddress mn41xHRzXuUAAVtujPLLo3Hzbe63GSP117 10
4deb2fbb98d9029a4e5808b8236d1119ad7315efdfe7406a88c923f5d4b0d5af
```
```
* 查看交易详细信息
```text
$ bitcoin-cli -regtest getrawtransaction 4deb2fbb98d9029a4e5808b8236d1119ad7315efdfe7406a88c923f5d4b0d5af true
```
输出json，包含size、vin、vout等，因为还没被打包进区块链，所以没有confirmations。
```text
$ bitcoin-cli -regtest generate 1
[
    "4deb2fbb98d9029a4e5808b8236d1119ad7315efdfe7406a88c923f5d4b0d5af"
]
```
* 查看交易详细信息
```text
$ bitcoin-cli -regtest getrawtransaction 4deb2fbb98d9029a4e5808b8236d1119ad7315efdfe7406a88c923f5d4b0d5af true
```
输出json，这次可以看到 blockhash 和 confirmations 了，说明交易已经被打包。

* 查看收到的BTC
```text
$ bitcoin-cli -regtest listreceivedbyaddress
[
  {
    "address": "mn41xHRzXuUAAVtujPLLo3Hzbe63GSP117",
    "account": "",
    "amount": 10.00000000,
    "confirmations": 1,
    "label": "",
    "txids": [
      "4deb2fbb98d9029a4e5808b8236d1119ad7315efdfe7406a88c923f5d4b0d5af"
    ]
  }
]
```

## JSON-RPC API接口

查看可用的比特币RPC命令列表：
```shell
bitcoin-cli help
```

`getinfo` RPC命令显示关于比特币网络节点、钱包、区块链数据库状态的基础信息。：
```shell
bitcoin-cli getinfo
```

例，显示块链接高度为396328个块和15个活动网络连接：
```text
{
    "version" : 110200,
    "protocolversion" : 70002,
    "blocks" : 396328,
    "timeoffset" : 0,
    "connections" : 15,
    "proxy" : "",
    "difficulty" : 120033340651.23696899,
    "testnet" : false,
    "relayfee" : 0.00010000,
    "errors" : ""
}
```

```shell
./bitcoind -rpcconnect=rpc.blockchain.info -rpcport=443 -rpcssl -rpcuser=YourWalletIdentifier -rpcpassword=YourPassword getinfo
```

## 探索和解码交易

命令：`getrawtransaction`，`decodeawtransaction`

在买咖啡的故事中，Alice从Bob咖啡厅买了一杯咖啡。 她的交易记录在交易ID（txid）`0627052b6f28912f2703066a912ea577f2ce4da4caa5a5fbd8a57286c345c2f2`的封锁上。 我们使用API通过传递交易ID作为参数来检索和检查该交易：
```shell
bitcoin-cli getrawtransaction 0627052b6f28912f2703066a912ea577f2ce4da4caa5a5fbd8a57286c345c2f2
```

命令`getrawtransaction`以十六进制返回顺序交易：
```text
0100000001186f9f998a5aa6f048e51dd8419a14d8a0f1a8a2836dd734d2804fe65fa35779000000008b483045022100884d142d86652a3f47ba4746ec719bbfbd040a570b1deccbb6498c75c4ae24cb02204b9f039ff08df09cbe9f6addac960298cad530a863ea8f53982c09db8f6e381301410484ecc0d46f1918b30928fa0e4ed99f16a0fb4fde0735e7ade8416ab9fe423cc5412336376789d172787ec3457eee41c04f4938de5cc17b4a10fa336a8d752adfffffffff0260e31600000000001976a914ab68025513c3dbd2f7b92a94e0581f5d50f654e788acd0ef8000000000001976a9147f9b1a7fb68d60c536c2fd8aeaa53a8f3cc025a888ac00000000
```

为了解码，我们使用`decodeawtransaction`命令，将十六进制数据作为参数传递：
```shell
bitcoin-cli decoderawtransaction 0100000001186f9f998a5aa6f048e51dd8419a14d8a0f1a8a2836dd734d2804fe65fa35779000000008b483045022100884d142d86652a3f47ba4746ec719bbfbd040a570b1deccbb6498c75c4ae24cb02204b9f039ff08df09cbe9f6addac960298cad530a863ea8f53982c09db8f6e381301410484ecc0d46f1918b30928fa0e4ed99f16a0fb4fde0735e7ade8416ab9fe423cc5412336376789d172787ec3457eee41c04f4938de5cc17b4a10fa336a8d752adfffffffff0260e31600000000001976a914ab68025513c3dbd2f7b92a94e0581f5d50f654e788acd0ef8000000000001976a9147f9b1a7fb68d60c536c2fd8aeaa53a8f3cc025a888ac00000000
```
返回：
```json
{
  "txid": "0627052b6f28912f2703066a912ea577f2ce4da4caa5a5fbd8a57286c345c2f2",
  "size": 258,
  "version": 1,
  "locktime": 0,
  "vin": [
    {
      "txid": "7957a35fe64f80d234d76d83a2...8149a41d81de548f0a65a8a999f6f18",
      "vout": 0,
      "scriptSig": {
        "asm":"3045022100884d142d86652a3f47ba4746ec719bbfbd040a570b1decc...",
        "hex":"483045022100884d142d86652a3f47ba4746ec719bbfbd040a570b1de..."
      },
      "sequence": 4294967295
    }
  ],
  "vout": [
    {
      "value": 0.01500000,
      "n": 0,
      "scriptPubKey": {
        "asm": "OP_DUP OP_HASH160 ab68...5f654e7 OP_EQUALVERIFY OP_CHECKSIG",
        "hex": "76a914ab68025513c3dbd2f7b92a94e0581f5d50f654e788ac",
        "reqSigs": 1,
        "type": "pubkeyhash",
        "addresses": [
          "1GdK9UzpHBzqzX2A9JFP3Di4weBwqgmoQA"
        ]
      }
    },
    {
      "value": 0.08450000,
      "n": 1,
      "scriptPubKey": {
        "asm": "OP_DUP OP_HASH160 7f9b1a...025a8 OP_EQUALVERIFY OP_CHECKSIG",
        "hex": "76a9147f9b1a7fb68d60c536c2fd8aeaa53a8f3cc025a888ac",
        "reqSigs": 1,
        "type": "pubkeyhash",
        "addresses": [
          "1Cdid9KFAaatwczBwBttQcwXYCpvK8h7FK"
        ]
      }
    }
  ]
}
```

## 使用比特币核心的编程接口

`Bitcoin Core`的API是一个JSON-RPC接口。 RPC代表远程过程调用，这意味着我们通过网络协议调用远程（位于比特币核心节点）的过程（函数）。 在这种情况下，网络协议是HTTP或HTTPS（用于加密连接）。

当我们使用bitcoin-cli命令获取命令的帮助时，它给了我们一个例子，它使用curl，通用的命令行HTTP客户端来构造这些JSON-RPC调用之一：
```shell
curl --user myusername --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "getinfo", "params": [] }' -H 'content-type: text/plain;' http://127.0.0.1:8332/
```

如果您在自己的程序中实现JSON-RPC调用，则可以使用通用的HTTP库构建调用，类似于前面的curl示例所示。

## 挖矿

### CPU挖矿原理

CPU挖矿，即利用RPC接口 setgenerate 控制挖矿。
控制台输入 `setgenerate true 2`，即开始挖矿，后边的数字表示代表的挖矿线程数，当然前提先完成同步数据。
由于单CPU运算SHA256D算力约为2 MH/S，因此nNonce提供的4字节搜索空间完全够用，即支持4G种取值。

### GPU挖矿原理

GPU运算SHA256D算力约为200M-1G，nNonce提供4G搜索空间，如果仅调整nNonce取值，可以支持4秒左右。因此可以调整nTime，每调整一次nTime，可以继续挖矿4秒。

GPU挖矿使用GETWORK协议，即挖矿程序和节点分离，也即挖矿部件与区块链数据分离。GPU挖矿时代，使用GETWORK协议，使得挖矿程序与节点交互。

核心思路为：节点构造区块，将区块头数据交给挖矿程序，挖矿程序遍历nNonce进行挖矿。验证合格交付给节点，节点提取nNonce和nTime验证区块，如果符合要求即向全网广播。遍历结束将调用GETWORK，节点构造新区块，然后重复上述过程。

GPU经典挖矿驱动为cgminer，源码为 https://github.com/ckolivas/cgminer。

GPU挖矿缺陷：GETWORK协议给挖矿程序提供的搜索空间为4G，结束后需再次调用GETWORK RPC接口。矿机出现后，矿机算力已达10 TH/S，继续使用GETWORK协议将频繁调用RPC接口，显然不太合适。因此需转向更高效的getblocktemplate协议。

### 矿池挖矿原理

矿工通过getblocktemplate协议与节点交互，或矿池采用stratum协议与矿工交互，即为矿池的两种典型搭建模式。

与getwork相比，getblocktemplate协议让矿工自行构造区块，因此使得节点与挖矿完全分离。矿工拿到一系列数据后，开始挖矿：
1、构建coinbase交易。
2、coinbase交易放在交易列表之前，构建hashMerkleRoot。因coinbase、以及交易次序均可调整，因此hashMerkleRoot空间可以认为无限大。因此getblocktemplate协议也使矿工获得了巨大的搜索空间。
3、构建区块头。
4、挖矿，即矿工可以在nNonce、nTime、hashMerkleRoot提供的搜索空间中涉及任意的挖矿策略。
5、上交数据，如果挖矿成功即提交给节点，由节点验证并广播。

getblocktemplate协议的问题：
1、矿工通过HTTP方式调用RPC接口向节点申请挖矿数据，因此网络中最新区块变动无法告知矿工，造成算力浪费。
2、每次调用getblocktemplate，节点都会返回1.5M左右数据，因频繁交互将因此增加大量成本。
Stratum协议将解决上述问题。

### Stratum协议

Stratum协议，采用主动分配任务的方式，也即矿池任何时候都可以给矿工分派任务。对于矿工，如收到新任务，将无条件转向新任务。另外矿工也可以向矿池申请新任务。

最核心问题为，如何使得矿工获得更大的搜索空间。如果仅矿工仅可改变nNonce和nTime，交互数据少但搜索空间不足。如果允许矿工构造coinbase，搜索空间大但代价是需要将所有交易交给矿工，因此对矿池带宽要求较高。

Stratum协议巧妙解决了这个问题。即：基于Merkler树的原理，无需将全部交易发给矿工，只需将构造hashMerkleroot所需的少数几个节点交给矿工即可。同时将构造coinbase所需信息交给矿工，矿工可基于少数信息构造hashMerkleroot。照此方式，如果包含N笔交易，仅需将log2(N)个hash值交给矿工。因此可大大降低交互的数据量。

矿池的核心即给矿工分派任务，统计工作量并分发收益。矿池可以将区块难度分成更小的任务发给矿工，矿工完成任务提交矿池。如果全网区块难度要求前70位为0，那么矿池可以给矿工分派难度为前30位0的任务，矿池再判断是否碰巧前70位都为0。

几个开源矿池：
PHP-MPOS：https://github.com/MPOS/php-mpos
node-open-mining-portal：https://github.com/zone117x/node-open-mining-portal
Powerpool：https://github.com/sigwo/powerpool

### 混合挖矿

混合挖矿，即某种币的挖矿挂靠在另一种币的链条上。辅链需要做针对性设计（如域名币和狗狗币）。混合挖矿，使用AuxPOW协议实现。AuxPOW的实现得益于比特币Coinbase的输入字段。

经典的PoW区块，规定符合要求才算合格的区块。AuxPOW协议附加两个要求：
1、辅链区块的hash值必须内置于父链区块的Coinbase里。
2、父链区块的难度比较符合辅链的难度要求。

一般来说，父链的算力比较辅链大，满足父链难度要求的区块一定满足辅链的难度要求。因此过去很多达不到父链难度要求的区块，可以达到辅链难度，可以在辅链获得收益。

### ASIC 矿机

FPGA，Field-Programmable Gate Array，译为现场可编程门阵列。是在PAL、GAL、CPLD等可编程器件的基础上进一步发展的产物。是作为专用集成电路(ASIC)领域中的一种半定制电路而出现的，既解决了定制电路的不足，又克服了原有可编程器件门电路数有限的缺点。能用FPGA实现各种AISC、DSP和单片机。FPGA作为挖矿硬件，对于ASIC来说属于必然的过度技术。

ASIC，Application Specific Integrated Circuits，即专用集成电路。是指应特定用户要求和特定电子系统的需要而设计、制造的集成电路。