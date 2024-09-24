package main

import (
	"crypto/rand"
	"crypto/rsa"
	"crypto/x509"
	"encoding/base64"
	"encoding/pem"
	"fmt"
	"io/ioutil"
	"os"
)

func generateRSAKeyPair(bits int) (*rsa.PrivateKey, *rsa.PublicKey, error) {
	privateKey, err := rsa.GenerateKey(rand.Reader, bits)
	if err != nil {
		return nil, nil, err
	}

	return privateKey, &privateKey.PublicKey, nil
}

func savePrivateKey(privateKey *rsa.PrivateKey, filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	privateKeyBytes := x509.MarshalPKCS1PrivateKey(privateKey)
	privateKeyBlock := &pem.Block{
		Type:  "RSA PRIVATE KEY",
		Bytes: privateKeyBytes,
	}

	err = pem.Encode(file, privateKeyBlock)
	if err != nil {
		return err
	}

	return nil
}

func savePublicKey(publicKey *rsa.PublicKey, filename string) error {
	publicKeyBytes, err := x509.MarshalPKIXPublicKey(publicKey)
	if err != nil {
		return err
	}

	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	publicKeyBlock := &pem.Block{
		Type:  "RSA PUBLIC KEY",
		Bytes: publicKeyBytes,
	}

	err = pem.Encode(file, publicKeyBlock)
	if err != nil {
		return err
	}

	return nil
}

func loadPublicKey(filename string) (*rsa.PublicKey, error) {
	publicKeyFile, err := ioutil.ReadFile(filename)
	if err != nil {
		return nil, err
	}

	block, _ := pem.Decode(publicKeyFile)
	if block == nil {
		return nil, fmt.Errorf("failed to parse PEM block containing the public key")
	}

	pub, err := x509.ParsePKIXPublicKey(block.Bytes)
	if err != nil {
		return nil, err
	}

	switch pub := pub.(type) {
	case *rsa.PublicKey:
		return pub, nil
	default:
		return nil, fmt.Errorf("unknown type of public key")
	}
}

func encryptWithPublicKey(publicKey *rsa.PublicKey, data []byte) ([]byte, error) {
	return rsa.EncryptPKCS1v15(rand.Reader, publicKey, data)
}

func loadPrivateKey(filename string) (*rsa.PrivateKey, error) {
	privateKeyFile, err := os.ReadFile(filename)
	if err != nil {
		return nil, err
	}

	block, _ := pem.Decode(privateKeyFile)
	if block == nil {
		return nil, fmt.Errorf("failed to parse PEM block containing the private key")
	}

	privateKey, err := x509.ParsePKCS1PrivateKey(block.Bytes)
	if err != nil {
		return nil, err
	}

	return privateKey, nil
}

func decryptWithPrivateKey(privateKey *rsa.PrivateKey, encryptedData []byte) ([]byte, error) {
	return rsa.DecryptPKCS1v15(rand.Reader, privateKey, encryptedData)
}

func main() {
	privateKey, publicKey, err := generateRSAKeyPair(2048)
	if err != nil {
		fmt.Println("Error generating RSA key pair:", err)
		return
	}

	err = savePrivateKey(privateKey, "private_key.pem")
	if err != nil {
		fmt.Println("Error saving private key:", err)
		return
	}

	err = savePublicKey(publicKey, "public_key.pem")
	if err != nil {
		fmt.Println("Error saving public key:", err)
		return
	}

	fmt.Println("RSA key pair generated and saved successfully.")

	publicKey, err = loadPublicKey("public_key.pem")
	if err != nil {
		fmt.Println("Error loading public key:", err)
		return
	}

	data := []byte("Hello, RSA encryption!")
	encryptedData, err := encryptWithPublicKey(publicKey, data)
	if err != nil {
		fmt.Println("Error encrypting data:", err)
		return
	}

	fmt.Printf("Encrypted data: %x\n", encryptedData)

	privateKey, err = loadPrivateKey("test2.pem")
	if err != nil {
		fmt.Println("Error loading private_key key:", err)
		return
	}

	encodedString := ""
	decodedBytes, err := base64.StdEncoding.DecodeString(encodedString)
	if err != nil {
		fmt.Println("解码错误:", err)
		return
	}
	// 将解码后的字节转换成字符串

	decryptedData, err := decryptWithPrivateKey(privateKey, decodedBytes)
	if err != nil {
		fmt.Println("Error decrypt data:", err)
		return
	}
	fmt.Printf("decrypted data: %s\n", decryptedData)
}
