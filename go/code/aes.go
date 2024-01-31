package main

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
