## go
```go
package main

import (
	"crypto/aes"
	"crypto/cipher"
	"crypto/rand"
	"log"

	"io"
)

func encryptData(plaintext []byte, key []byte) ([]byte, []byte, error) {
	log.Println("key", len(key))
	c, err := aes.NewCipher(key)
	if err != nil {
		return nil, nil, err
	}

	gcm, err := cipher.NewGCM(c)
	if err != nil {
		return nil, nil, err
	}

	nonce := make([]byte, gcm.NonceSize())
	if _, err = io.ReadFull(rand.Reader, nonce); err != nil {
		return nil, nil, err
	}

	return gcm.Seal(nil, nonce, plaintext, nil), nonce, nil
}

func decryptData(crypted, key, nounce []byte) ([]byte, error) {
	log.Println("key", len(key))
	c, err := aes.NewCipher(key)
	if err != nil {
		return nil, err
	}
	blockMode, err := cipher.NewGCM(c)
	if err != nil {
		return nil, err
	}
	origData, err := blockMode.Open(nil, nounce, crypted, nil)
	if err != nil {
		return nil, err
	}
	return origData, nil
}

func main() {
	// 16 24 32
	plaintext := []byte("123456")
	key := []byte("1234567890123456")
	log.Println("key", key)
	log.Printf("hexkey %x\n", key)
	encrypttext, nounce, err := encryptData(plaintext, key)
	log.Println("aesGCMencrypt", encrypttext, nounce, err)
	log.Printf("encrypt %x nounce %x\n", encrypttext, nounce)
	str, err := decryptData(encrypttext, key, nounce)
	log.Println("decryptData", string(str), err)
}
```
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
    true, // 是否可导出
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
function hexToUint8Array(hexString) {
    const bytes = new Uint8Array(hexString.length / 2);
    for (let i = 0; i < bytes.length; i++) {
      bytes[i] = parseInt(hexString.substr(i * 2, 2), 16);
    }
    return bytes;
}
// 使用示例
(async () => {
  try {
    const keyString = "31323334353637383930313233343536";
    const key = await importKey(keyString); //await generateKey();
    const plainText = "123456";

    // 加密
    let { encrypted, iv } = await encryptData(plainText, key);

    console.log("Encrypted data:", encrypted);
    console.log("iv:", iv);
    // 解密
    let decryptedText = await decryptData(encrypted, iv, key);

    console.log("Decrypted text:", decryptedText);

    // 解密
    encrypted = hexToBuffer("d35326423e6b27fe632c7fd44bf2c566de50b5412d55");
    iv = hexToUint8Array("58b3a85e36d6f6edfe3cb4aa");
    console.log("encryptedText:", encrypted);
    console.log("iv:", iv);
    decryptedText = await decryptData(encrypted, iv, key);

    console.log("Decrypted text:", decryptedText);
  } catch (error) {
    console.error("An error occurred:", error);
  }
})();
```
