
## js
```js
async function generateKey() {
    // 生成AES-GCM-256密钥
    return window.crypto.subtle.generateKey(
      {
        name: "AES-GCM",
        length: 256, // 密钥长度
      },
      true, // 是否允许导出密钥
      ["encrypt", "decrypt"] // 密钥用途
    );
  }
  
  async function encryptData(plainText, key) {
    // 将字符串转换为ArrayBuffer
    const encoder = new TextEncoder();
    const encodedText = encoder.encode(plainText);
  
    // 生成随机的IV
    const iv = window.crypto.getRandomValues(new Uint8Array(12)); // GCM模式通常使用12字节的IV
  
    // 加密数据
    const encrypted = await window.crypto.subtle.encrypt(
      {
        name: "AES-GCM",
        iv: iv,
      },
      key,
      encodedText
    );
  
    // 返回加密后的数据和IV，以便之后解密
    return { encrypted, iv };
  }
  
  async function decryptData(encryptedData, iv, key) {
    // 解密数据
    const decrypted = await window.crypto.subtle.decrypt(
      {
        name: "AES-GCM",
        iv: iv,
      },
      key,
      encryptedData
    );
  
    // 将ArrayBuffer转换回字符串
    const decoder = new TextDecoder();
    return decoder.decode(decrypted);
  }

  async function importKey(keyString) {
    // 将十六进制字符串转换为ArrayBuffer
    const keyBuffer = hexToBuffer(keyString);

    // 导入密钥
    return await window.crypto.subtle.importKey(
        "raw",
        keyBuffer,
        { name: "AES-GCM" },
        false, // 是否可导出
        ["encrypt", "decrypt"] // 密钥用途
    );
}

// 辅助函数：将十六进制字符串转换为ArrayBuffer
function hexToBuffer(hexString) {
    const bytes = new Uint8Array(hexString.length / 2);
    for (let i = 0; i < bytes.length; i++) {
        bytes[i] = parseInt(hexString.substr(i * 2, 2), 16);
    }
    return bytes.buffer;
}

  // 使用示例
  (async () => {
    try {
      const keyString = '49505152535455565748495051525354';
      const key = await importKey(keyString); //await generateKey();
      const plainText = "123456";
      
      // 加密
      const { encrypted, iv } = await encryptData(plainText, key);
      
      // 解密
      const decryptedText = await decryptData(encrypted, iv, key);
      
      console.log("Decrypted text:", decryptedText);
    } catch (error) {
      console.error("An error occurred:", error);
    }
  })();
```
