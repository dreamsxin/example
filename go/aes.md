- https://github.com/dreamsxin/cphalcon7/blob/master/ext/crypt.c
- https://github.com/yanue/aes-cbc-pkcs7/tree/main


常见的对称加密算法有 AES、SM4、ChaCha20、3DES、Salsa20、DES、Blowfish、IDEA、RC5、RC6、Camellia。

对称加密算法可分为两大类型:

**分组加密:** 
先将明文切分成一个个固定大小的块，再对每个块进行加密，这种方式被称为分组加密或块加密，有的资料称呼为"分组密码"或"块密码"。

**流加密:**
将密钥扩展到与密文等长后，用扩展后的密钥与明文按比特位做异或运算。
相比分组加密，流加密具有速度快，消耗少的优点，在网络通信的某些特定场景比较有优势。然而流加密的发展落后于分组加密，其安全性、可扩展性、使用灵活性上，目前认为还是比不上分组加密的，同时某些分组加密算法可以兼具流加密的部分特点。因此对称加密的主流仍然是分组加密。

常见的流加密算法如RC4、ChaCha20等等，它们的安全强度主要取决于扩展后密钥的随机性。

## 填充算法
使用分组加密算法对明文进行分组时，有时需要事先对明文字节数组进行填充，使其长度变为块长度的整数倍。这就是分组加密的填充算法，或者说填充规则。

但并不是所有的分组加密算法都需要做明文填充，这取决于分组模式，事实上只有ECB、CBC、PCBC等分组模式需要填充明文。

目前主流的分组加密填充规则是 PKCS7Padding。

### 填充方式的区别
ZeroPadding，数据长度不对齐时使用0填充，否则不填充。使用0填充有个缺点，当元数据尾部也存在0时，在unpadding时可能会存在问题。

我们这里主要讨论PKCS7Padding与PKCS5Padding。

（1）PKCS7Padding，

  假设每个区块大小为blockSize

  <1>已对齐，填充一个长度为blockSize且每个字节均为blockSize的数据。

  <2>未对齐，需要补充的字节个数为n，则填充一个长度为n且每个字节均为n的数据。

（2）PKCS5Padding，PKCS7Padding的子集，只是块大小固定为8字节。

两者的区别在于PKCS5Padding是限制块大小的PKCS7Padding

## 分组模式
分组之后，接下来分组加密算法要解决的问题就是: 如何将分组后的各个块组织起来，协同实现明文整体的加解密。这个问题就是分组密码工作模式要解决的问题。常见的分组密码工作模式(简称分组模式或加密模式)有:

- ECB : 电子密码本(Electronic codebook)
- CBC : 密码块链接(Cipher-block chaining)
- PCBC : 填充密码块链接(Propagating cipher-block chaining), 也被称为 明文密码块链接(Plaintext cipher-block chaining)
- CFB : 密文反馈(Cipher feedback)
- OFB : 输出反馈(Output feedback)
- CTR : 计数器模式(Counter mode), 也被称为 ICM整数计数模式(Integer Counter Mode), 或 SIC模式(Segmented Integer Counter)
- GCM : 伽罗瓦/计数器模式(Galois/Counter Mode)

其中CBC、CTR、GCM是较为常用的分组模式，他们都需要一个随机初始化向量IV。

## AES加密相关概念

分组：用来执行加密程序的内存空间有限。

密钥长度：128 bits、192 bits或256 bits。密钥的长度不同，推荐加密轮数也不同。

初始向量（IV)：防止同样的明文块，始终加密成同样的密文块

填充方式：密钥只能对确定长度的数据块进行处理，所以需要对最后一块做额外处理，常用的模式有PKCS5, PKCS7, NOPADDING

## GCM
GCM ( Galois/Counter Mode) 指的是该对称加密采用Counter模式，并带有GMAC消息认证码。二者分别保证了加密算法的保密性、完整性。
GCM中的 G 就是指 GMAC，C就是指CTR。 所以 GCM 可以提供对消息的加密和完整性校验。

```go
package main

import (
    "fmt"
    "crypto/aes"
    "crypto/cipher"
    "encoding/hex"
)

func main() {
    key, _ := hex.DecodeString("2575590594a9cd809e5bfacf397f8c1ac730dbc38a3e137ecd1ab66591c8c3c9")

    ciphertext, _ := hex.DecodeString("afe56cf6228c6ea8c79da49186e718e92a579824596ae1d0d4d20d7793dca797bd4034ccf467bfae5c79a3981e7a2968c41949237e2b2db678c1c3d39c9ae564c5cafd52f2b77a3dc77bf1bae063114d0283b97417487207735da31ddc1531d5645a9c3e602c195a0ebf69c272aa5fda3a2d781cb47e117310164715a54c7a5a032740584e2789a7b4e596034c16425139a77e507c492b629c848573c714a03a2e7d25b9459b95842332b460f3682d19c35dbc7d53e3a51e0497ff6a6cbb367e760debc4194ae097498108df7b95eac2fa9bac4320077b510be3b7b823248bfe02ae501d9fe4ba179c7de6733c92bf89d523df9e31238ef497b9db719484cbab7531dbf6c5ea5a8087f95d59f5e4f89050e0f1dc03e464168ad76a64cca64b79")  //加密生成的结果

    nonce, _ := hex.DecodeString("b7203c6a6fb633d16e9cf5c1") //加密用的nonce

    block, err := aes.NewCipher(key)
    if err != nil {
        panic(err.Error())
    }

    aesgcm, err := cipher.NewGCM(block)
    if err != nil {
        panic(err.Error())
    }

    plaintext, err := aesgcm.Open(nil, nonce, ciphertext, nil)
    if err != nil {
        panic(err.Error())
    }
    fmt.Println(string(plaintext))
}
```
