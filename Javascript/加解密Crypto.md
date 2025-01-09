# 加解密 SubtleCrypto

https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto/encrypt

window.crypto.subtle 支持AES-128对称加密算法。AES（高级加密标准）是一种广泛使用的对称加密算法，它有三种密钥长度：128位、192位和256位。在Web Crypto API中，你可以选择不同的密钥长度来生成AES密钥。

## 示例

```js
async function generateKey() {
    return await window.crypto.subtle.generateKey(
        {
            name: "AES-CBC",
            length: 128, // 使用128位密钥
        },
        true, // 是否可导出
        ["encrypt", "decrypt"] // 使用密钥的操作
    );
}

function generateIV() {
    return window.crypto.getRandomValues(new Uint8Array(16)); // 128位IV
}

async function encryptData(key, data, iv) {
    const encodedData = new TextEncoder().encode(data);
    return await window.crypto.subtle.encrypt(
        {
            name: "AES-CBC",
            iv: iv,
        },
        key,
        encodedData
    );
}

async function decryptData(key, encryptedData, iv) {
    const decryptedData = await window.crypto.subtle.decrypt(
        {
            name: "AES-CBC",
            iv: iv,
        },
        key,
        encryptedData
    );
    return new TextDecoder().decode(decryptedData);
}

// 使用示例
(async () => {
    const key = await generateKey();
    const iv = generateIV();
    const data = "Hello, World!";
    const encryptedData = await encryptData(key, data, iv);
    const decryptedData = await decryptData(key, encryptedData, iv);
    console.log("Decrypted Data:", decryptedData);
})();
```

在这个示例中，generateKey 函数生成了一个 128 位的 AES 密钥。然后，encryptData 和 decryptData 函数分别用于加密和解密数据。需要注意的是，AES-CBC模式需要使用一个初始化向量（IV），并且每个消息应该使用唯一的IV。

## 注意事项

- IV 唯一性：对于AES-CBC模式，每个消息应该使用唯一的IV，以避免重放攻击。
- 填充：AES是块加密算法，因此需要处理不足一个块大小的数据。AES-CBC模式通常使用PKCS7填充。
- 性能：AES-CBC模式不支持并行处理，因此在处理大量数据时可能比AES-GCM模式慢。
- 通过 window.crypto.subtle，你可以在前端应用中使用AES-128对称加密算法来保护数据的安全性。不过，通常情况下，前端加密操作应该与后端配合使用，确保整体系统的安全性。前端主要负责传输加密数据，而敏感数据的加密和解密操作最好在服务器端完成。
