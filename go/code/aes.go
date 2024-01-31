package main

/**
## NewCipher
`func NewCipher(key []byte) (cipher.Block, error)`
创建一个cipher.Block接口。参数key为密钥，长度只能是16、24、32字节，用以选择AES-128、AES-192、AES-256。

## NewCTR
`func NewCTR(block Block, iv []byte) Stream`
返回一个计数器模式的、底层采用block生成key流的Stream接口，初始向量iv的长度必须等于block的块尺寸。

## stream 接口的方法
`func XORKeyStream(dst, src []byte)`
从加密器的key流和src中依次取出字节二者xor后写入dst，src和dst可指向同一内存地址

## 加密/解密
使用aes.NewCipher获取块
使用cipher.CTR转为CTR模式流
使用stearm.XORKeyStream进行加解密
*/
import (
	"bytes"
	"crypto/aes"
	"crypto/cipher"
	"crypto/md5"
	"encoding/hex"
	"fmt"
	"log"
	"runtime"
)

func main() {

	plaintext := []byte("test")

	md5 := md5.New()
	md5.Write(plaintext)
	md5str := hex.EncodeToString(md5.Sum(nil))
	log.Println("md5", md5str)

	key := []byte("xxxxxxxxxxxxxxxx")
	iv := []byte("xxxxxxxxxxxxxxxx")

	ret, err := AesEncryptCTR([]byte(md5str), key, iv)
	log.Println("ret", hex.EncodeToString(ret), err)
	ret, err = AesEncrypt([]byte(md5str), key, iv)
	log.Println("ret", hex.EncodeToString(ret), err)
}

// plainText：明文
// iv： 初始化向量
// key：密钥
// 返回密文/明文，以及错误
func AesEncryptCTR(plainText, key, iv []byte) ([]byte, error) {
	block, err := aes.NewCipher(key)
	if err != nil {
		_, file, line, _ := runtime.Caller(0)
		return nil, fmt.Errorf("file:%s,line:%d, err %s", file, line+1, err.Error())
	}
	if len(iv) != block.BlockSize() {
		_, file, line, _ := runtime.Caller(0)
		return nil, fmt.Errorf("file:%s,line:%d, err %s", file, line+1, err.Error())
	}
	// create a CTR interface
	stream := cipher.NewCTR(block, iv)
	cipherText := make([]byte, len(plainText))
	// encrypt or decrypt
	stream.XORKeyStream(cipherText, plainText)
	return cipherText, nil
}

// AesEncrypt 加密函数
func AesEncrypt(text []byte, key, iv []byte) ([]byte, error) {
	block, err := aes.NewCipher(key)
	if err != nil {
		fmt.Println(err)
	}
	// 填充
	blockSize := block.BlockSize()
	paddText := ZeroPadding(text)
	blockMode := cipher.NewCBCEncrypter(block, iv[:blockSize])

	// 加密
	result := make([]byte, len(paddText))
	blockMode.CryptBlocks(result, paddText)
	// 返回密文
	return result, nil
}

// AesDecrypt 解密函数
func AesDecrypt(encrypter []byte, key, iv []byte) ([]byte, error) {
	block, err := aes.NewCipher(key)
	if err != nil {
		fmt.Println(err)
	}
	blockSize := block.BlockSize()
	blockMode := cipher.NewCBCDecrypter(block, iv[:blockSize])
	result := make([]byte, len(encrypter))
	blockMode.CryptBlocks(result, encrypter)
	// 去除填充
	result = NullUnPadding(result)
	return result, nil
}

func ZeroPadding(in []byte) []byte {
	length := len(in)
	if length%16 == 0 {
		return in
	} else {
		blockCount := length / 16
		out := make([]byte, (blockCount+1)*16)
		var i int
		for i = 0; i < length; i++ {
			out[i] = in[i]
		}
		return out
	}
}

func NullUnPadding(in []byte) []byte {
	return bytes.TrimRight(in, string([]byte{0}))
}
