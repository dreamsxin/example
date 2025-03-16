package main

import (
	"bytes"
	"crypto/aes"
	"encoding/base64"
	"errors"
	"fmt"
	"io"
	"log"
	"os"
)

func main() {

	// 检查参数数量
	if len(os.Args) < 2 {
		fmt.Println("请使用以下命令格式：")
		fmt.Println("  加密: ./program encrypt [输入文件] [输出文件]")
		fmt.Println("  解密: ./program decrypt [输入文件] [输出文件]")
		os.Exit(1)
	}

	// 固定密钥（示例用，生产环境应从安全途径获取）
	key := []byte("(H+MbQeThWmZq4t7")

	switch os.Args[1] {
	case "encrypt":
		if len(os.Args) != 4 {
			fmt.Println("加密命令格式：encrypt [明文文件] [加密输出文件]")
			os.Exit(1)
		}
		err := AESECBEncryptFile(os.Args[2], os.Args[3], key)
		if err != nil {
			fmt.Printf("加密失败: %v\n", err)
			os.Exit(1)
		}
		fmt.Println("文件加密成功")

	case "decrypt":
		if len(os.Args) != 4 {
			fmt.Println("解密命令格式：decrypt [加密文件] [解密输出文件]")
			os.Exit(1)
		}
		err := AESECBDecryptFile(os.Args[2], os.Args[3], key)
		if err != nil {
			fmt.Printf("解密失败: %v\n", err)
			os.Exit(1)
		}
		fmt.Println("文件解密成功")

	default:
		fmt.Println("无效命令，可用命令：encrypt | decrypt")
		os.Exit(1)
	}
}

// AESECBEncryptFile 文件加密函数（保持不变）
func AESECBEncryptFile(srcPath, dstPath string, key []byte) error {
	plainText, err := os.ReadFile(srcPath)
	if err != nil {
		return fmt.Errorf("读取文件失败: %w", err)
	}

	encrypted, err := AESECBEncrypt(string(plainText), key)
	if err != nil {
		return err
	}

	return os.WriteFile(dstPath, []byte(encrypted), 0644)
}
func ReadFileWithoutBOM(path string) ([]byte, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}

	// 去除 UTF-8 BOM 头
	if bytes.HasPrefix(data, []byte{0xEF, 0xBB, 0xBF}) {
		data = data[3:]
	}

	return data, nil
}

// AESECBDecryptFile 文件解密函数（保持不变）
func AESECBDecryptFile(srcPath, dstPath string, key []byte) error {
	cipherText, err := ReadFileWithoutBOM(srcPath)
	if err != nil {
		return fmt.Errorf("读取文件失败: %w", err)
	}
	decrypted, err := AESECBDecrypt(string(cipherText), key)
	if err != nil {
		return err
	}

	return os.WriteFile(dstPath, []byte(decrypted), 0644)
}

// 流式解密示例（需调整主逻辑）
func streamDecrypt(inFile, outFile string, key []byte) error {
	src, _ := os.Open(inFile)
	defer src.Close()

	dest, _ := os.Create(outFile)
	defer dest.Close()

	block, _ := aes.NewCipher(key)
	blockSize := block.BlockSize()

	buf := make([]byte, blockSize*1024) // 8KB缓冲区
	for {
		n, err := src.Read(buf)
		if err != nil && err != io.EOF {
			return err
		}
		if n == 0 {
			break
		}

		// 处理最后一个块
		if err == io.EOF {
			buf = buf[:n]
		}

		// 解密处理
		for i := 0; i < n; i += blockSize {
			block.Decrypt(buf[i:i+blockSize], buf[i:i+blockSize])
		}

		dest.Write(buf[:n])
	}
	return nil
}

// AESECBEncrypt AES-ECB加密
func AESECBEncrypt(plainText string, key []byte) (string, error) {
	// 创建AES密码块
	block, err := aes.NewCipher(key)
	if err != nil {
		return "", err
	}
	blockSize := block.BlockSize()

	// 明文填充
	plainBytes := pkcs7Pad([]byte(plainText), blockSize)

	// 分块加密
	cipherText := make([]byte, len(plainBytes))
	for i := 0; i < len(plainBytes); i += blockSize {
		block.Encrypt(cipherText[i:i+blockSize], plainBytes[i:i+blockSize])
	}

	// Base64编码
	return base64.StdEncoding.EncodeToString(cipherText), nil
}

// AESECBDecrypt AES-ECB解密
func AESECBDecrypt(cryptedText string, key []byte) (string, error) {
	// Base64解码
	cipherText, err := base64.StdEncoding.DecodeString(cryptedText)
	if err != nil {
		log.Println("Base64解码失败:", len(cryptedText), err)
		return "", err
	}

	// 创建AES密码块
	block, err := aes.NewCipher(key)
	if err != nil {
		return "", err
	}
	blockSize := block.BlockSize()

	// 检查密文长度
	if len(cipherText)%blockSize != 0 {
		return "", errors.New("ciphertext is not a multiple of the block size")
	}

	// 分块解密
	plainBytes := make([]byte, len(cipherText))
	for i := 0; i < len(cipherText); i += blockSize {
		block.Decrypt(plainBytes[i:i+blockSize], cipherText[i:i+blockSize])
	}

	// 去除填充
	plainBytes, err = pkcs7Unpad(plainBytes)
	if err != nil {
		return "", err
	}

	return string(plainBytes), nil
}

// pkcs7Pad PKCS7填充
func pkcs7Pad(data []byte, blockSize int) []byte {
	padding := blockSize - (len(data) % blockSize)
	if padding == 0 {
		padding = blockSize
	}
	padText := bytes.Repeat([]byte{byte(padding)}, padding)
	return append(data, padText...)
}

// pkcs7Unpad PKCS7去填充
func pkcs7Unpad(data []byte) ([]byte, error) {
	length := len(data)
	if length == 0 {
		return nil, errors.New("pkcs7: data is empty")
	}

	padding := int(data[length-1])
	if padding < 1 || padding > aes.BlockSize {
		return nil, errors.New("pkcs7: invalid padding")
	}

	// 检查填充内容是否一致
	for i := length - padding; i < length; i++ {
		if int(data[i]) != padding {
			return nil, errors.New("pkcs7: invalid padding")
		}
	}

	return data[:length-padding], nil
}
