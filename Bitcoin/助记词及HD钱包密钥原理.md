# 助记词 及 HD 钱包密钥原理

## 概念

BIP 全名是 Bitcoin Improvement Proposals，是提出 Bitcoin 的新功能或改进措施的文件。可由任何人提出，经过审核后公布在 bitcoin/bips 上。BIP 和 Bitcoin 的关系，就像是 RFC 之于 Internet。

BIP32, BIP39, BIP44 共同定义了目前被广泛使用的 HD Wallet，包含其设计动机和理念、实作方式、实例等。

* BIP32
定义 Hierarchical Deterministic wallet (简称 "HD Wallet")，是一个系统可以从单一个 seed 产生一树状结构储存多组 keypairs（私钥和公钥）。好处是可以方便的备份、转移到其他相容装置（因为都只需要 seed），以及分层的权限控制等。

![https://github.com/dreamsxin/example/blob/master/Bitcoin/img/bip32.png](https://github.com/dreamsxin/example/blob/master/Bitcoin/img/bip32.png?raw=true)


* BIP39

将 seed 用方便记忆和书写的单字表示。一般由 12 个单字组成，称为 mnemonic code(phrase)，中文称为助记词或助记码。例如下面的12个单词：

>> rose rocket invest real refuse margin festival danger anger border idle brown

BIP 39 生成助记词的过程非常重要，大家肯定关心 BIP39 的安全性，如果一个 HD 钱包助记词是 12 个单词，一共有 2048 个单词可能性，如何算出随机的生成的助记词可能性是一个排列问题。
根据公式：`n!/( n - r )! `，既 `2048!/(2048-12)! = 5.2715379713014884760003093175282e+39`。
我们可以举个类比，地球上的沙子数量在大约是1 后面 18 个零。如果你可以每秒生成一百万个助记词，那么一年可以生成 `1000000*60*60*24*365=3.1536e+13`，大约需要 `1.6715937e+26` 年遍历所有助记词，这是多少年呢？

* BIP44
基于 BIP32 的系统，赋予树状结构中的各层特殊的意义。让同一个 seed 可以支援多币种、多帐户等。各层定义如下：

`m / purpose / coin_type / account / change / address_index`

其中的 `purporse` 固定是 `44`，代表使用 BIP44。而 `coin_type` 用来表示不同币种，例如 `Bitcoin` 就是 `0`，`Ethereum` 是 `60`。

## 分类

区块链相关的话题持续发酵之时，应该不少人知道加密货币钱包，钱包是普通用户与加密货币系统交互的入口，各种形态的钱百花齐放，手机钱包、桌面钱包、硬件钱包、网页钱包和纸质钱包等。
通过钱包可以无国界无限制地转移你的数字资产。从开发者的角度看，钱包的作用是管理用户的私钥、通过私钥签名交易管理用户在区块链上的数字货币。

* Bitcoin Address + Private key = Bitcoin Wallet

根据密钥之间是否有关联可把钱包分为两类：

1. Non-Deterministic wallet
密钥对之间没有关联

2. Deterministic wallet
密钥对由一个原始的种子主密钥推导而来。最常见的推导方式是树状层级推导 (hierarchical deterministic) 简称 HD


比特币钱包 (Bitcoin Core) 生成密钥对之间没有任何关联，属于 nondeterministic wallet ，这种类型的钱包如果想备份导入是比较麻烦的，用户必须逐个操作钱包中的私钥和对应地址。
Deterministic wallet 基于 BIP32 (Bitcoin Improvement Proposal 32) 标准实现，通过一个共同的种子维护 n 多私钥，种子推导私钥采用不可逆哈希算法，在需要备份钱包私钥时，只备份这个种子即可（大多数情况下的种子是通过 BIP44 生成了助记词，方便抄写），在支持 BIP32、BIP44 标准的钱包只需导入助记词即可导入全部的私钥。

* principle of avoiding address reuse
提倡避免地址重复使用，当数字货币地址已经发生过一次转账就存在私钥泄漏的可能性。

## 符合 BIP-32/BIP-44 标准的 HD 钱包

上文提到 Deterministic wallets 能够通过一个种子推导出很多密钥，它基于 BIP32 标准实现，种子能够推导出主密钥 (master key), 主密钥推导出子密钥 (children keys)，子密钥推导出孙密钥 (grandchildren keys), 以此递推。

>> 有树状特征的 HD 钱包非常适用于有组织结构的公司使用，HD 钱包能够在不需知道私钥的前提下生成大量的公钥，非常适用于只负责收款的服务

BIP32 标准的种子是一个随机 16 字节的 16 进制的字符串。如果能够适用英文单词作为助记词无疑会降低种子备份及恢复钱包难度，BIP39 标准就是为了解决助记词的需求，通过随机生成 12 ~ 24 个容易记住的单词，单词序列通过 PBKDF2 与 HMAC-SHA512 函数创建出随机种子作为 BIP32 的种子。

## BIP32/BIP39 标准详解

BIP39 标准定义了钱包助记词和种子生成规则。

通过九个步骤即可生成钱包助记词和种子：

1. 步骤 1~6 生成助记词
2. 步骤 7~9 把前六步生成的助记词转化为 BIP32 种子

* 生成助记词

1. 规定熵的位数必须是 32 的整数倍，所以熵的长度取值位 128 到 256 之间取 32 的整数倍的值，分别为 128, 160, 192, 224, 256；
2. 校验和的长度为熵的长度/32 位, 所以校验和长度可为 4，5，6，7，8 位；

助记词库有 2048 个词，用 11 位可全部定位词库中所有的词，作为词的索引，故一个词用 11 位表示，助记词的个数可为 (熵+校验和)/11，值为 12，15，18，21，24

```text
熵(bits) 	校验和(bits) 	熵 + 校验和 (bits) 	助记词长度
128 		4 		132 			12
160 		5 		165 			15
192 		6 		198 			18
224 		7 		231 			21
256 		8 		264 			24
```
1. 生成一个长度为 128~256 位 (bits) 的随机序列(熵)
2. 取熵哈希后的前 n 位作为校验和 (n= 熵长度/32)
3. 随机序列 + 校验和
4. 把步骤三得到的结果每 11 位切割
5. 步骤四得到的每 11 位字节匹配词库的一个词

步骤五得到的结果就是助记词串。

![https://github.com/dreamsxin/example/blob/master/Bitcoin/img/bip39_mnemonic_word.png](https://github.com/dreamsxin/example/blob/master/Bitcoin/img/bip39_mnemonic_word.png?raw=true)

## 通过助记词生成种子

助记词由长度为 128 到 256 位的随机序列(熵)匹配词库而来，随后采用 PBKDF2 function 推导出更长的种子(seed)。生成的种子被用来生成构建 deterministic Wallet 和推导钱包密钥。

在密码学中，Key stretching 技术被用来增强弱密钥的安全性，增加了暴力破解 (Brute-force attack) 对每个可能密钥尝试攻破的时间，增强了攻击难度。各种编程语言原生库都提供了 key stretching 的实现。PBKDF2 (Password-Based Key Derivation Function 2) 是常用的 key stretching 算法中的一种。基本原理是通过一个为随机函数(例如 HMAC 函数)，把明文和盐值作为输入参数，然后重复进行运算最终产生密钥。

为了从助记词中生成二进制种子，BIP39 采用 PBKDF2 函数推算种子，其参数如下：

* PRF:
助记词句子作为密码

* Password:
"mnemonic" + passphrase 作为盐

* Salt
2048 作为重复计算的次数

* c
HMAC-SHA512 作为随机算法

*dkLen
512 位(64 字节)是期望得到的密钥长度

`DK = PBKDF2(PRF, Password, Salt, c, dkLen)`

![https://github.com/dreamsxin/example/blob/master/Bitcoin/img/mnemonic2seed.png](https://github.com/dreamsxin/example/blob/master/Bitcoin/img/mnemonic2seed.png?raw=true)

BIP32 标准定义了 HD 钱包的生成规则。HD 钱包中的所有层级密钥都是由根种子推导而来，通常根种子由上述步骤 BIP39 生成。所以只需通过助记词就能备份和恢复钱包，这也是 HD 钱包的缺陷，如果你的根种子泄漏，那么全部密钥随之都泄漏。

## 主私钥和主链码

首先是从根种子生成主密钥 (master key) 和主链码 (master chain code)

![https://github.com/dreamsxin/example/blob/master/Bitcoin/img/master_key_chain_code.png](https://github.com/dreamsxin/example/blob/master/Bitcoin/img/master_key_chain_code.png?raw=true)

上图中根种子通过不可逆 HMAC-SHA512 算法推算出 512 位的哈希串，左 256 位是 Master Private key(m), 右 256 位是 master chain code, 通过 m 结合推导公钥的椭圆曲线算法能推导出与之对应的 264 位 master public Key (M)。chain code 作为推导下级密钥的熵。

## 子私钥推导

HD 钱包使用 CKD(child key derivation) 函数从父密钥(parent keys)推导子密钥(child keys)，CKD 由下列三个要素做单向散列哈希(one way hash function) 。

* 父密钥 (没有压缩过的椭圆曲线推导的私钥或公钥 ECDSA uncompressed key)
* 链码作为熵 (chain code 256 bits)
* 子代索引序号 (index 32 bits)

![https://github.com/dreamsxin/example/blob/master/Bitcoin/img/mchild_key_derivation.png](https://github.com/dreamsxin/example/blob/master/Bitcoin/img/child_key_derivation.png?raw=true)

索引号个数为 2 的 32 次方，每个父级密钥能推导出该数目一半的子密钥 (
索引号从 0x00 到 0x7fffffff (0 to 2 的 21 次方减 1) 会生成正常的密钥；索引号从 0x80000000 到 0xffffffff 会生成增强密钥）。CKD 采用不可逆的 HMAC-SHA512 不可逆加密算法，子密钥不能向上推导出父密钥、同时也不能水平推导出同一级的密钥。

## 扩展密钥

CKD 推导子密钥的三个元素中，其中父密钥和链码结合统称为扩展密钥 (Extended keys)。256 位的密钥和 256 位的链码串联起来的 512 位就是扩展密钥。

* 包含私钥的扩展密钥用以推导子私钥，从子私钥又可推导对应的公钥和比特币地址
* 包含公钥的扩展密钥用以推导子公钥

扩展密钥使用 Base58Check 算法加上特定的前缀编码，编码得到的包含私钥的前缀为 xprv, 包含公钥的扩展密钥前缀为 xpub，相比比特币的公私钥，扩展密钥编码之后得到的长度为 512 或 513 位。

## 子公钥推导

上述方法中通过推导出的私钥可推导出对应公钥，但 HD 钱包非常好用的特征之一就是在隐藏私钥的前提下通过公钥推导出子公钥，极大加强安全性。在只需要生成地址接受比特币而无权消费的场景下非常有用，通过公钥扩展密钥能生成无穷尽的公钥和比特币地址。

HD 钱包通过公钥推导子公钥的使用场景：在接受数字货币支付的电商系统中，Web 服务中集成了比特币扩展公钥服务，系统为客户的每笔订单生成一个接受比特币支付的地址，不涉及到私钥的 web 服务极大地减低了被盗币的可能性。如果不采用 HD 钱包，通常都是在物理隔绝的服务器中批量生成比特币地址，然后再导入到电商系统，这种方式需要定时生成并导入地址维护防止 web 服务系统把预先导入的地址用完。

子私钥推导流程和子公钥流程基本一样，差异之处有两点：

* 把子私钥推导过程中私钥替换为公钥。
* 子公钥推导出对应出与之的子链码

![https://github.com/dreamsxin/example/blob/master/Bitcoin/img/extendingaparentpublickeytocreateachildpublickey.png](https://github.com/dreamsxin/example/blob/master/Bitcoin/img/extendingaparentpublickeytocreateachildpublickey.png?raw=true)

## 增强扩展密钥推导

密钥需加强保管以免泄漏，泄漏私钥意味着对应的地址上的币可被转走、泄漏公钥意味着 HD 钱包的隐私被泄漏。
增强密钥推导 (Hardened child key derivation) 解决下述两个问题：

* 虽然泄漏公钥并不会导致丢币，但含有公钥的扩展密钥泄漏会导致以此为根节点推导出来的扩展公钥全部泄漏，一定程度上破坏了隐私性。
* 泄漏扩展公钥加上该公钥推导出的后任一代扩展公钥对应的私钥有被推导出该扩展公钥的所有后代私钥的可能性

于此，BIP32 协议把 CKD 函数改为 HKD (hardened key derivation formula) 生成增强密钥推导函数。

CKD 函数以推导扩展密钥的序列号 ( 0x00 到 0x7fffffff)、父链码和父公钥生或父私钥成子链码和子公钥，子私钥从父私钥推导；而 HKD 通过父私钥、父链码和推导增强扩展密钥的序列号 (0x80000000 到 0xffffffff) 增强子私钥和增强子链码。

![https://github.com/dreamsxin/example/blob/master/Bitcoin/img/hardened_child_derivation.png](https://github.com/dreamsxin/example/blob/master/Bitcoin/img/hardened_child_derivation.png?raw=true)

HD 钱包密钥路径表示

```text
HD 路径 	密钥描述
m/0 		从主私钥(m) 推导出的第一代的第一个子私钥
m/0/0 		从第一代子密钥m(0)推导出的第二代第一个孙私钥
m/0'/0 		从第一代增强密钥 (m/0')推导出得第二代第一个孙密钥
m/1/0 		从第一代的第二个子密钥推导出的第二代第一个孙密钥
```
