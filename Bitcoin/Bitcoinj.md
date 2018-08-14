# Bitcoinj

## 使用

* 设置日志系统

```java
BriefLogFormatter.init();
 if (args.length < 2) {
    System.err.println("Usage: address-to-send-back-to [regtest|testnet]");
    return;
}
```

* 选择网络

- main 或者 “production” 网络用与人们买卖交易
- public test network（testnet）用于测试实验，可以研究新特性
- 回归测试模式（regtest）不是一个共有网络，需要自己启动 daemon

```java
// Figure out which network we should connect to. Each one gets its own set of files.
NetworkParameters params;
String filePrefix;
if (args[1].equals("testnet")) {
    params = TestNet3Params.get();
    filePrefix = "forwarding-service-testnet";
} else if (args[1].equals("regtest")) {
    params = RegTestParams.get();
    filePrefix = "forwarding-service-regtest";
} else {
    params = MainNetParams.get();
    filePrefix = "forwarding-service";
}
```

每一个网络都拥有起始的区块、独立的端口号和地址前缀。这些都封装在 NetworkParameters 单例对象中，使用时调用`call()`方法。值得一提的是，在`testnet`中可以免费从`TestNet Faucet`获取大量的币。在`regtest`模式中不存在公有的设施，我们可以使用`bitcoind -regtest setgenerate true`产生新的区块。

* NetworkParameters

NetworkParameters包含bitcoin链实例工作需要的数据。这是一个抽象类，具体实例可在params包中被发现。有4个实现类：一个是主网络MainNetParams，一个是为了测试网，另外两个是为了单元测试和本地app开发。虽然这些类包含一些别号，我们鼓励去调用get的静态方法。

这个类所有的实现类都在org.bitcoinj.params这个包下面。

* MainNetParams

MainNetParams 继承 AbstractBitcoinNetParams，这里指的是主链上的配置信息，包括 dns seeds。

三种seed

- dnsSeeds
- httpSeeds
- addrSeeds

* 得到节点列表

https://github.com/sipa/bitcoin-seeder

* 密钥和地址

比特币交易通常是将钱发送到一个由椭圆曲线生成的公钥。发送者生成一个交易，交易中包含有接受者的地址。接受者的地址是由公钥经过hash得到的。关于私钥、公钥、地址的关系我总结如下：

- k私钥（CSPRNG随机产生,256bit二进制数，64位16进制表示）
- K公钥（私钥 通过椭圆曲线相乘产生，20字节160bit）
- 公钥哈希（公钥通过HSA256和RIPEMD160处理得到，20字节160bit）
- 比特币地址（公钥哈希通过Base58check编码得到）

>> 椭圆曲线算法可以从私钥计算得到公钥，这是不可逆的过程。K=k*G，其中k是私钥，G是被称之为生成点的常数点。K是所得公钥，不可以从公钥计算得到私钥，此运算被称为“寻找离散对数”问题，难以解决。

密码学公式：

![https://github.com/dreamsxin/example/blob/master/Bitcoin/img/密码学公式.png](https://github.com/dreamsxin/example/blob/master/Bitcoin/img/密码学公式.png?raw=true)

## Wallet App 工具

bitcionj 有很多不同的层组成，每层的级别都低于后一层。在一个典型的操作场景中，一方想发送金额给另一方时至少需要使用到 BlockChain、BlockStore、PeerGroup 和 Wallet。

bitcoinj 提供了一个高级别的封装类名为 WalletAppKit。它设置为SPV模式（与之相对的是全校验模式），可以提供一些简单的属性和默认配置的修改：
```java
// Start up a basic app using a class that automates some boilerplate. Ensure we always have at least one key.
kit = new WalletAppKit(params, new File("."), filePrefix) {
    @Override
    protected void onSetupCompleted() {
        // This is called in a background thread after startAndWait is called, as setting up various objects
        // can do disk and network IO that may cause UI jank/stuttering in wallet apps if it were to be done
        // on the main thread.
        if (wallet().getKeychainSize() < 1)
            wallet().importKey(new ECKey());
    }
};

if (params == RegTestParams.get()) {
    // Regression test mode is designed for testing and development only, so there's no public network for it.
    // If you pick this mode, you're expected to be running a local "bitcoind -regtest" instance.
    kit.connectToLocalHost();
}

// Download the block chain and wait until it's done.
kit.startAsync();
kit.awaitRunning();
```

这个工具需要传入三个参数

- NetworkParameters 几乎所有的API都会需要
- 文件存储目录
- 文件存储前缀（选填）

我们可以重写`onSetupCompleted`方法，加入自己的代码。bitcoinj 会为他起一个线程后台运行。接着检查钱包是否至少拥有一个 key，如果没有就刷新一个新的 key。下一步检查是否使用了 regtest 模式，如果是那么链接 localhost。最后调用`kit.startAsync()`。

## 处理事件

bitcoinj中的事件监听和我们平时大多数使用的监听一样，对象需要实现一个如下几个接口：

- WalletEventListener 钱包中发生的事件
- BlockChainListener 区块链相关的事件
- PeerEventListener 关于网络中节点的事件
- TransactionConfidence.Listener 交易安全回滚级别相关的事件

对于大部分的应用来说不需要使用全部这些接口，因为每个接口中都定义了很多相关的事件。使用的时候我们只需实现抽象接口即可。
Bitcoinj中后台的user thread专门负责运行事件监听：
```java
kit.wallet().addEventListener(new AbstractWalletEventListener() {
    @Override
    public void onCoinsReceived(Wallet w, Transaction tx, Coin prevBalance, Coin newBalance) {
        // Runs in the dedicated "user thread".
    }
});
```

## 接收币

设置监听，实现onCoinReceived方法并传入四个参数，示例中打印出收到的金额，设置 Future 回调：
```java
kit.wallet().addEventListener(new AbstractWalletEventListener() {
    @Override
    public void onCoinsReceived(Wallet w, Transaction tx, Coin prevBalance, Coin newBalance) {
        // Runs in the dedicated "user thread".
        //
        // The transaction "tx" can either be pending, or included into a block (we didn't see the broadcast).
        Coin value = tx.getValueSentToMe(w);
        System.out.println("Received tx for " + value.toFriendlyString() + ": " + tx);
        System.out.println("Transaction will be forwarded after it confirms.");
        // Wait until it's made it into the block chain (may run immediately if it's already there).
        //
        // For this dummy app of course, we could just forward the unconfirmed transaction. If it were
        // to be double spent, no harm done. Wallet.allowSpendingUnconfirmedTransactions() would have to
        // be called in onSetupCompleted() above. But we don't do that here to demonstrate the more common
        // case of waiting for a block.
        Futures.addCallback(tx.getConfidence().getDepthFuture(1), new FutureCallback<Transaction>() {
            @Override
            public void onSuccess(Transaction result) {
                // "result" here is the same as "tx" above, but we use it anyway for clarity.
                forwardCoins(result);
            }

            @Override
            public void onFailure(Throwable t) {}
        });
    }
});
```

因为比特币是一个需要全球交易顺序达成一致的全球共识系统，所以每个交易都需要有一个置信对象（confidence object）。置信对象需要处理成功和失败两种情况，因为很可能我们自己认为的已经收到了金额，但是稍后发现全球其他人并不认同。我们可以使用置信对象包括的数据做出风险决策、计算我们实际收到钱的可能性、了解共识变化。

Futures 是并发编程的一个重要概念，在bitcoinj中大量使用。bitcoinj使用`Guava`扩展了标准的JavaFuture类，生成ListenableFuture。ListenableFuture 可以获取一些future的计算和状态，可以等待执行结束也可以注册回调。Futures失败的时候会收到一个异常。

当交易被确认后调用`forwardCoins`。

## 发送币

```java
Coin value = tx.getValueSentToMe(kit.wallet());
System.out.println("Forwarding " + value.toFriendlyString() + " BTC");
// Now send the coins back! Send with a small fee attached to ensure rapid confirmation.
final Coin amountToSend = value.subtract(Transaction.REFERENCE_DEFAULT_MIN_TX_FEE);
final Wallet.SendResult sendResult = kit.wallet().sendCoins(kit.peerGroup(), forwardingAddress, amountToSend);
System.out.println("Sending ...");
// Register a callback that is invoked when the transaction has propagated across the network.
// This shows a second style of registering ListenableFuture callbacks, it works when you don't
// need access to the object the future returns.
sendResult.broadcastComplete.addListener(new Runnable() {
    @Override
    public void run() {
         // The wallet has changed now, it'll get auto saved shortly or when the app shuts down.
         System.out.println("Sent coins onwards! Transaction hash is " + sendResult.tx.getHashAsString());
    }
});
```

首先我们查询收到了多少金额（同样也可以通过`onCoinsReceived`中的`newBalance`获得），接着确定要发送的金额等同于收到的金额，设置交易手续费为最低，这样可能会延长确认时间。

最后调用`sendCoins`，该方法会返回一个SendResult对象其中包括创建的交易和`ListenableFuture`。

交易手续费一方面可以用来防止拒绝服务另一方面可以用来激励矿工。用户可以自定义每笔交易的手续费：

```java
SendRequest req = SendRequest.to(address, value);
//kb kilobyte 每千字节
req.feePerKb = Coin.parseCoin("0.0005");
Wallet.SendResult result = wallet.sendCoins(peerGroup, req);
Transaction createdTx = result.tx;
```

## 基本结构

bitcoinj主要使用的对象

- NetworkParameters 选择生产或测试环境网络
- Wallet 用于存储ECKey以及其他数据
- PeerGroup管理网络链接
- BLockChain管理共享的、全局的数据结构
- BlockStore保存区块链上的数据结构，如光盘一样
- WalletEventListener 接收钱包事件
- WalletAppKit可以方便的创建和使用上面几个对象

## 核心类

- Monetary.java

这是一个接口类，实现此接口代表是一个货币，例如比特币。接口中包含`int smallestUnitExponet()`(最小分割单元，通常是8)、`long getValue()`、`int sigum()`。

-  Coin.java

这是一个实现了 Monetary 接口的final类。 定义了不同大小的比特币：ZERO、CENT、MILLICOIN、MICROCOIN、SATOSHI、FIFTY_COIN、NEGATIVE_SATOSHI。定义了价值的加减乘除运算方法。

-  Base58.java

这个类主要用于编码比特币地址或者任意数据,使其变为方便阅读的有大小写字母组成的字符串。bitcoin中的base58与Flickr的略有不同。校验部分使用sha256哈希两次。主要的方法有：
```java
// 58字符 不包括 0IlO
public static final char[] ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz".toCharArray();
private static final char ENCODED_ZERO = ALPHABET[0];
private static final int[] INDEXES = new int[128];
static {
	Arrays.fill(INDEXES, -1);
	for (int i = 0; i < ALPHABET.length; i++) {
	    INDEXES[ALPHABET[i]] = i;
	}
}

/**
* 将输入的byte编码成base58 字符串，无校验和.
*
* @param input the bytes to encode
* @return base58-encoded string
*/
public static String encode(byte[] input){...}   

/**
* 解码base58字符串 为原数据的 bytes.
*
* @param input the base58-encoded string to decode
* @return the decoded data bytes
* @throws AddressFormatException 如果base58不是一个合法的字符串 继承于IllegalArgumentException
*/
public static byte[] decode(String input) throws AddressFormatException {...}
```

- VersionedChecksummedBytes.java

比特币key的格式化类。格式化一般包括[one version byte]、[data types]、[4 checksum bytes]。在base58的基础上添加了比特币地址的前后缀校验。


- BaseEncoding

bitcoinj 在单元测试中使用 BaseEncoding 类，这个类来自 `com.google.common.io`

```java
public abstract class BaseEncoding {
	private static final BaseEncoding BASE64 = new BaseEncoding.StandardBaseEncoding("base64()", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", Character.valueOf('='));
	private static final BaseEncoding BASE64_URL = new BaseEncoding.StandardBaseEncoding("base64Url()", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_", Character.valueOf('='));
	private static final BaseEncoding BASE32 = new BaseEncoding.StandardBaseEncoding("base32()", "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567", Character.valueOf('='));
	private static final BaseEncoding BASE32_HEX = new BaseEncoding.StandardBaseEncoding("base32Hex()", "0123456789ABCDEFGHIJKLMNOPQRSTUV", Character.valueOf('='));
	private static final BaseEncoding BASE16 = new BaseEncoding.StandardBaseEncoding("base16()", "0123456789ABCDEF", (Character)null);
}
```

从源码中可以看出其支持 base16、base32、base32Hex、base64、base64Url。

```text
Encoding 	Alphabet 	char:byte ratio 	Default padding 	Comments
base16() 	0-9 A-F 	2.00 			N/A 			传统16进制，默认大写字母
base32() 	A-Z 2-7 	1.60 			= 			方便阅读，不会有0/O, 1/l的混淆，默认大写
base32Hex() 	0-9 A-V 	1.60 			= 	
base64() 	0-9 A-Z a-z + / 1.33 			= 	
base64Url() 	0-9 A-Z a-z - _ 1.33 			= 			可安全的用于文件名、URLs, 不怕泄露
```

- Address.java

比特币种的地址类似“1MsScoe2fTJoq4ZPdQgqyhgWeoNamYPevy”是从椭圆曲线产生的公钥处生成的。因为是使用RIPEMD160对公钥进行hash，所以长度是160bits即20bytes。比特币采用支付给脚本的哈希值（pay-to-script-hash，P2SH）。


>> 比特币的工作机制要求币的发送者必须在交易时时明确指定脚本。这种机制有的时候不太适用：假如在遇到支付场景时，卖家使用了多重签名地址，需要用户支付给一个脚本地址而不是一个简单的地址，这样对于用户而言会变得很复杂。
>> 比特币用了一种很聪明的办法来解决这个问题，不仅可以实现多重签名地址支付，而且还可以实现复杂的资金监管规则。比特币使用的办法是：收款方告诉付款方“请把比特币支付给某个脚本地址，脚本的哈希值是xxx，在取款的时候，我会提上述哈希值对应的脚本，同时，提供数据通过脚本的验证。”而不是“请把比特币支付给某个公钥，公钥的哈希值是XXX”。

类中重要的方法有：
```java
public class Address extends VersionedChecksummedBytes {
    /**
     * 20字节
     */
    public static final int LENGTH = 20;
    private transient NetworkParameters params;

    /**
     *
     * eg. new Address(MainNetParams.get(), NetworkParameters.getAddressHeader(), Hex.decode("4a22c3c4cbb31e4d03b15550636762bda0baf85a"));</pre>
     */
    public Address(NetworkParameters params, int version, byte[] hash160) throws WrongNetworkException {
       
    }
    
    /**
     * 返回代表P2SH脚本hash的地址
     */
    public static Address fromP2SHHash(NetworkParameters params, byte[] hash160) {}

    /** 
     *  返回一个地址，根据给定的脚本公钥计算出的hash
     */
    public static Address fromP2SHScript(NetworkParameters params, Script scriptPubKey) {}
   
}
```

- StoredBlock.java

类中包含了 blockheader、链工作总量等额外信息，提高了查询和计算效率不再需要每次从创世区块开始遍历。
```java
public static final int CHAIN_WORK_BYTES = 12;
   public static final byte[] EMPTY_BYTES = new byte[CHAIN_WORK_BYTES];
   public static final int COMPACT_SERIALIZED_SIZE = Block.HEADER_SIZE + CHAIN_WORK_BYTES + 4;  // for height

   private Block header;
   private BigInteger chainWork;
   private int height;

   public StoredBlock(Block header, BigInteger chainWork, int height) {
       this.header = header;
       this.chainWork = chainWork;
       this.height = height;
   }
```

- AbstractBlockChain.java

弄懂这个抽象类实现的功能对源码的理解非常重要，因为他是 bitcoinj 的7大对象之一。`AbstractBlockChain`的实现需要`BlockStore`存储所有数据、`SPVBlockStore`存储在SPV模式下的轻量数据、`Block`是组成链的必要数据结构、连接Wallet帮助其接收交易数据和通知。
`AbstractBlockChain`有两个子类：BlockChain 是一个非常简单的子类，实现了简易的交易验证，帮助 SPV 模式下的节点高效率的校验 header 信息而不校验完成的信息，这一点对推广移动应用非常有价值。`FullPrunedBlockChain`联合`FullPrunedBlockStore`实现了对block的完整校验。

```java
public abstract class AbstractBlockChain{
    static final Logger log = LoggerFactory.getLogger(AbstractBlockChain.class);
    protected final ReentrantLock lock = Threading.lock("blockchain");

    /** Keeps a map of block hashes to StoredBlocks. */
    private final BlockStore blockStore;
    
}
```

这段代码中值得注意的是 ReentrantLock。多线程和并发不是什么新内容，一般使用核心类库Thread构造、启动和操作线程。Java语言中包括了跨线程传达并发性约束的构造`Synchronized`和`volatile`。
`volatile`主要用在多个线程感知实例变量被更改了场合，从而使得各个线程获得最新的值。它强制线程每次从主内存中讲到变量，而不是从线程的私有内存中读取变量，从而保证了数据的可见性。
`ReentrantLock` 相比于`volatile`, `synchronized`更重量级一点不仅可以修饰变量还可以修饰方法。并且在保证了可见性的同事也保证了原子性。

`java.util.concurrent.lock` 中的 Lock 框架是锁定的一个抽象，它允许把锁定的实现作为 Java 类，而不是作为语言的特性来实现。这就为 Lock 的多种实现留下了空间，各种实现可能有不同的调度算法、性能特性或者锁定语义。
`ReentrantLock` 类实现了 Lock ，它拥有与 synchronized 相同的并发性和内存语义，但是添加了类似锁投票、定时锁等候和可中断锁等候的一些特性。此外，它还提供了在激烈争用情况下更佳的性能。（换句话说，当许多线程都想访问共享资源时，JVM 可以花更少的时候来调度线程，把更多时间用在执行线程上。）

```java
   /**
    * Tracks the top of the best known chain.<p>
    *
    * Following this one down to the genesis block produces the story of the economy from the creation of Bitcoin
    * until the present day. The chain head can change if a new set of blocks is received that results in a chain of
    * greater work than the one obtained by following this one down. In that case a reorganize is triggered,
    * potentially invalidating transactions in our wallet.
    */
   protected StoredBlock chainHead;

   // TODO: Scrap this and use a proper read/write for all of the block chain objects.
   // The chainHead field is read/written synchronized with this object rather than BlockChain. However writing is
   // also guaranteed to happen whilst BlockChain is synchronized (see setChainHead). The goal of this is to let
   // clients quickly access the chain head even whilst the block chain is downloading and thus the BlockChain is
   // locked most of the time.
   private final Object chainHeadLock = new Object();

   protected final NetworkParameters params;
   private final CopyOnWriteArrayList<ListenerRegistration<NewBestBlockListener>> newBestBlockListeners;
   private final CopyOnWriteArrayList<ListenerRegistration<ReorganizeListener>> reorganizeListeners;
   private final CopyOnWriteArrayList<ListenerRegistration<TransactionReceivedInBlockListener>> transactionReceivedListeners;
   
  class OrphanBlock {
       final Block block;
       final List<Sha256Hash> filteredTxHashes;
       final Map<Sha256Hash, Transaction> filteredTxn;
       OrphanBlock(Block block, @Nullable List<Sha256Hash> filteredTxHashes, @Nullable Map<Sha256Hash, Transaction> filteredTxn) {
           final boolean filtered = filteredTxHashes != null && filteredTxn != null;
           Preconditions.checkArgument((block.transactions == null && filtered)
                                       || (block.transactions != null && !filtered));
           this.block = block;
           this.filteredTxHashes = filteredTxHashes;
           this.filteredTxn = filteredTxn;
       }
   }
   // Holds blocks that we have received but can't plug into the chain yet, eg because they were created whilst we
   // were downloading the block chain.
   private final LinkedHashMap<Sha256Hash, OrphanBlock> orphanBlocks = new LinkedHashMap<Sha256Hash, OrphanBlock>();
```

孤立块代表接收到但是没有被记入主链的区块，可能由于网络传播的延迟导致同一时间段内发现的两个区块，其中一个更快的传播到了更多的节点处。孤立块的变化情况可以用来反映区块链网络中自私挖掘的情况，因为自私挖掘（Selfish Mining）策略的执行会导致被抛弃的区块数量增加。

`removeListener()`和`addListener()`批量注销和注册三个监听分别是`add|removeReorganizeListener`、`add|removeNewBestBlockListener`、`add|removeTransactionReceivedListener`。

- ECKey.java

此类代表一个椭圆曲线公共密钥和（可选）私钥，可用于数字签名但不可用于加密。
用空的构造函数创建一个新的ECKey将生成一个新的随机密钥对。