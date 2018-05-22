## bitcoinj 实现 HD Wallets

- org.bitcoinj.crypto.DeterministicHierarchy

Returns a key for the given path, optionally creating it.

`public DeterministicKey get(List<ChildNumber> path, boolean relativePath, boolean create)`

针对每个币种生成一个账户秘钥。

## 币的类型

- com.coinomi.core.coins.families.Families
```java
public enum Families {
    NXT("nxt"),
    FIAT("fiat"),
    // same as in org.bitcoinj.params.Networks
    BITCOIN("bitcoin"),
    NUBITS("nubits"),
    PEERCOIN("peercoin"),
    REDDCOIN("reddcoin"),
    VPNCOIN("vpncoin"),
    CLAMS("clams"),
    ;
```

- com.coinomi.core.coins.families.BitFamily extends CoinType
-     com.coinomi.core.coins.families.ClamsFamily extends BitFamily
-     com.coinomi.core.coins.families.NuFamily extends BitFamily
-     com.coinomi.core.coins.families.PeerFamily extends BitFamily
-     com.coinomi.core.coins.families.ReddFamily extends BitFamily
-     com.coinomi.core.coins.families.VpncoinFamily extends BitFamily
- com.coinomi.core.coins.families.NxtFamily extends CoinType

## 应用启动

- com.coinomi.wallet.WalletApplication
- com.coinomi.wallet.ui.WalletActivity

## 钱包数据加载

- com.coinomi.wallet.WalletApplication::onCreate

```java
@Override
public void onCreate() {
    walletFile = getFileStreamPath(Constants.WALLET_FILENAME_PROTOBUF);
    loadWallet();
}
```

## 钱包数据保存

- com.coinomi.wallet.service.CoinServiceImpl::onDestroy

```java
@Override
public void onDestroy() {
    application.saveWalletNow();
}
```

## 钱包账户创建

- com.coinomi.core.wallet.Wallet::createAndAddAccount

Generate and add a new BIP44 account for a specific coin type

`public List<WalletAccount> createAndAddAccount(List<CoinType> coins, boolean generateAllKeys, @Nullable KeyParameter key)`

```java
if (coinType instanceof BitFamily) {
    newPocket = new WalletPocketHD(rootKey, coinType, getKeyCrypter(), key);
} else if (coinType instanceof NxtFamily) {
    newPocket = new NxtFamilyWallet(rootKey, coinType, getKeyCrypter(), key);
} else {
    throw new UnsupportedCoinTypeException(coinType);
}
```
- com.coinomi.core.wallet.Wallet::saveToFile
- com.coinomi.core.wallet.Wallet::saveToFileStream
- com.coinomi.core.wallet.Wallet::loadFromFile
- com.coinomi.core.wallet.Wallet::loadFromFileStream
- com.coinomi.core.wallet.WalletPocketHD
- com.coinomi.core.wallet.WalletAccount
- com.coinomi.core.coins.CoinType::getBip44Path

## 交易

- com.coinomi.core.wallet.families.bitcoin.BitSendRequest
- com.coinomi.core.wallet.TransactionCreator::completeTx
- com.coinomi.core.wallet.TransactionCreator::signTransaction