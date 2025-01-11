
## dom\crypto\KeyAlgorithmProxy.cpp
- KeyAlgorithmProxy::Mechanism()

## dom\crypto\WebCryptoCommon.h
- inline CK_MECHANISM_TYPE MapAlgorithmNameToMechanism(const nsString& aName) 
## 
```c++
#include "pk11pub.h"
#include "nss.h"
#include "ScopedNSSTypes.h"
#include "mozilla/Unused.h"

#include <vector>
#include <iostream>

constexpr char kEncryptionYunLoginVersionPrefix[] = "xx";
const size_t mIVLength = 12;

static bool GenerateRandom(std::vector<uint8_t>& r) {
  if (r.empty()) {
    std::cout << "GenerateRandom r.empty()" <<std::endl;
    return false;
  }
  mozilla::UniquePK11SlotInfo slot(PK11_GetInternalSlot());
  if (!slot) {
    return false;
  }

  SECStatus srv = PK11_GenerateRandomOnSlot(slot.get(), r.data(), r.size());
  if (srv != SECSuccess) {
    r.clear();
    return false;
  }

  return true;
}

static bool BuildAesGcmKey(std::vector<uint8_t> aKeyBytes,
                                            /* out */ mozilla::UniquePK11SymKey& aKey) {
  // if (aKeyBytes.size() != mKeyByteLength) {
  //   std::cout << "BuildAesGcmKey failed aKeyBytes.size() != mKeyByteLength " << aKeyBytes.data() <<std::endl;
  //   return false;
  // }

  mozilla::UniquePK11SlotInfo slot(PK11_GetInternalSlot());
  if (!slot) {
    return false;
  }

  mozilla::UniqueSECItem key (SECITEM_AllocItem(nullptr, nullptr, aKeyBytes.size()));
  if (!key) {
    std::cout << "SECITEM_AllocItem failed" <<std::endl;
    return false;
  }
  key->type = siBuffer;
  memcpy(key->data, aKeyBytes.data(), aKeyBytes.size());
  key->len = aKeyBytes.size();

  mozilla::UniquePK11SymKey symKey(
      PK11_ImportSymKey(slot.get(), CKM_AES_GCM, PK11_OriginUnwrap,
                        CKA_DECRYPT | CKA_ENCRYPT, key.get(), nullptr));

  if (!symKey) {
    return false;
  }
  aKey.swap(symKey);

  return true;
}

void printHex(const unsigned char* data, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

static bool DoCipher(const mozilla::UniquePK11SymKey& aSymKey,
                                      const std::vector<uint8_t>& inBytes_,
                                      std::vector<uint8_t>& outBytes,
                                      bool encrypt) {
  outBytes.clear();

  std::cout << "---------------inBytes_ printHex"  <<std::endl;
  printHex(inBytes_.data(), inBytes_.size());
  std::vector<uint8_t> inBytes(inBytes_);
  size_t prefixLen = sizeof(kEncryptionYunLoginVersionPrefix) - 1;
  if (!encrypt) {
    if (prefixLen > 0) {
      // Remove the Prefix from the input.
      inBytes.erase(inBytes.begin(), inBytes.begin() + prefixLen);
    }
  }
  // Build params.
  // We need to get the IV from inBytes if we decrypt.
  if (!encrypt && (inBytes.size() < mIVLength || inBytes.empty())) {
    std::cout << "------------------DoCipher failed inBytes.size() < mIVLength" <<std::endl;
    return false;
  }

  const uint8_t* ivp = nullptr;

  // std::vector<uint8_t> ivBuf= {0xec, 0x51, 0x69, 0x16, 0xfc, 0x40, 0xac, 0x58, 0x85, 0x14, 0x7f, 0x9d};
  // ivp = ivBuf.data();
  std::vector<uint8_t> ivBuf;
  if (encrypt) {
    // Generate a new IV.
    ivBuf.resize(mIVLength);
    bool rv = GenerateRandom(ivBuf);
    if (!rv || ivBuf.size() != mIVLength) {
      std::cout << "---------------GenerateRandom failed" <<std::endl;
      return false;
    }
    ivp = ivBuf.data();
  } else {
    // An IV was passed in. Use the first mIVLength bytes from inBytes as IV.
    ivp = inBytes.data();
  }
  std::cout << "---------------ivp printHex"  <<std::endl;
    printHex(ivp, 12);


  CK_GCM_PARAMS gcm_params;
  gcm_params.pIv = const_cast<unsigned char*>(ivp);
  gcm_params.ulIvLen = mIVLength;
  gcm_params.ulIvBits = gcm_params.ulIvLen * 8;
  gcm_params.ulTagBits = 128;
  gcm_params.pAAD = nullptr;
  gcm_params.ulAADLen = 0;

  SECItem paramsItem = {siBuffer, reinterpret_cast<unsigned char*>(&gcm_params),
                        sizeof(CK_GCM_PARAMS)};

  size_t blockLength = 16;
  outBytes.resize(inBytes.size() + blockLength);
  unsigned int outLen = 0;
  SECStatus srv = SECFailure;

  if (encrypt) {
    srv = PK11_Encrypt(aSymKey.get(), CKM_AES_GCM, &paramsItem, outBytes.data(),
                       &outLen, inBytes.size() + blockLength, inBytes.data(),
                       inBytes.size());
    if (srv != SECSuccess) {
        std::cout << "---------------PK11_Encrypt failed" <<std::endl;
      outBytes.clear();
      return false;
    }

    outBytes.resize(outLen);
    // Prepend the used IV to the ciphertext.
    mozilla::Unused << outBytes.insert(outBytes.begin(), ivp, ivp + mIVLength);
    outLen += mIVLength;
    // Prepend the Prefix to the ciphertext.
    if (prefixLen > 0) {
      mozilla::Unused << outBytes.insert(
          outBytes.begin(), kEncryptionYunLoginVersionPrefix,
          kEncryptionYunLoginVersionPrefix + prefixLen);
      outLen += prefixLen;
    }
      std::cout << "---------------PK11_Encrypt printHex"  <<std::endl;
    printHex(outBytes.data(), outBytes.size());
  } else {
    std::vector<uint8_t> input(inBytes);
      std::cout << "---------------input printHex"  <<std::endl;
    printHex(input.data(), input.size());

    // Remove the IV from the input.
    input.erase(input.begin(), input.begin() + mIVLength);
      std::cout << "---------------input printHex2"  <<std::endl;
    printHex(input.data(), input.size());
    srv = PK11_Decrypt(aSymKey.get(), CKM_AES_GCM, &paramsItem, outBytes.data(),
                       &outLen, outBytes.size(), input.data(),
                       input.size());
    if (srv != SECSuccess) {
        std::cout << "---------------PK11_Decrypt failed"  <<std::endl;
      outBytes.clear();
      return false;
    }
    outBytes.resize(outLen);
    std::cout << "---------------PK11_Decrypt printHex"  <<std::endl;
    printHex(outBytes.data(), outBytes.size());
  }

  return true;
}

bool gcm_cipher(std::string key, const std::vector<uint8_t>& inBytes,
                                            std::vector<uint8_t>& outBytes, bool encrypt) {
  if (inBytes.empty()) {
    std::cout << "-------------------inBytes.empty()" <<std::endl;
    return false;
  }
  std::vector<uint8_t> buf(key.begin(), key.end());
  mozilla::UniquePK11SymKey symKey;
  bool rv = BuildAesGcmKey(buf, symKey);
  if (!rv) {
    std::cout << "---------------------BuildAesGcmKey failed" <<std::endl;
    return false;
  }
  return DoCipher(symKey, inBytes, outBytes, encrypt);
}
```
```c++
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rand.h>
#include <openssl/evp.h>

void handleErrors()
{
	printf("Some error occured\n");
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *aad,
	int aad_len, unsigned char *key, unsigned char *iv,
	unsigned char *ciphertext, unsigned char *tag)
{
	EVP_CIPHER_CTX *ctx;

	int len=0, ciphertext_len=0;

	/* Create and initialise the context */
	if(!(ctx = EVP_CIPHER_CTX_new()))
		handleErrors();

	/* Initialise the encryption operation. */
	if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
		handleErrors();

	/* Set IV length if default 12 bytes (96 bits) is not appropriate */
	if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 16, NULL))
		handleErrors();

	/* Initialise key and IV */
	if(1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv)) handleErrors();

	/* Provide any AAD data. This can be called zero or more times as
	 * required
	 */
	if(1 != EVP_EncryptUpdate(ctx, NULL, &len, aad, aad_len))
		handleErrors();

	/* Provide the message to be encrypted, and obtain the encrypted output.
	 * EVP_EncryptUpdate can be called multiple times if necessary
	 */
	/* encrypt in block lengths of 16 bytes */
	 while(ciphertext_len<=plaintext_len-16)
	 {
	 	if(1 != EVP_EncryptUpdate(ctx, ciphertext+ciphertext_len, &len, plaintext+ciphertext_len, 16))
	 		handleErrors();
	 	ciphertext_len+=len;
	 }
	 if(1 != EVP_EncryptUpdate(ctx, ciphertext+ciphertext_len, &len, plaintext+ciphertext_len, plaintext_len-ciphertext_len))
	 	handleErrors();
	 ciphertext_len+=len;

	/* Finalise the encryption. Normally ciphertext bytes may be written at
	 * this stage, but this does not occur in GCM mode
	 */
	if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + ciphertext_len, &len)) handleErrors();
	ciphertext_len += len;

	/* Get the tag */
	if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag))
		handleErrors();

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return ciphertext_len;
}


int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *aad,
	int aad_len, unsigned char *tag, unsigned char *key, unsigned char *iv,
	unsigned char *plaintext)
{
	EVP_CIPHER_CTX *ctx;
	int len=0, plaintext_len=0, ret;

	/* Create and initialise the context */
	if(!(ctx = EVP_CIPHER_CTX_new())) 
		handleErrors();

	/* Initialise the decryption operation. */
	if(!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
		handleErrors();

	/* Set IV length. Not necessary if this is 12 bytes (96 bits) */
	if(!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 16, NULL))
		handleErrors();

	/* Initialise key and IV */
	if(!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv)) handleErrors();

	/* Provide any AAD data. This can be called zero or more times as
	 * required
	 */
	if(!EVP_DecryptUpdate(ctx, NULL, &len, aad, aad_len))
		handleErrors();

	/* Provide the message to be decrypted, and obtain the plaintext output.
	 * EVP_DecryptUpdate can be called multiple times if necessary
	 */
	 while(plaintext_len<=ciphertext_len-16)
	 {
	 	if(1!=EVP_DecryptUpdate(ctx, plaintext+plaintext_len, &len, ciphertext+plaintext_len, 16))
	 		handleErrors();
	 	plaintext_len+=len;
	 }
	 if(1!=EVP_DecryptUpdate(ctx, plaintext+plaintext_len, &len, ciphertext+plaintext_len, ciphertext_len-plaintext_len))
	 		handleErrors();
	 plaintext_len+=len;

	/* Set expected tag value. Works in OpenSSL 1.0.1d and later */
	if(!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag))
		handleErrors();

	/* Finalise the decryption. A positive return value indicates success,
	 * anything else is a failure - the plaintext is not trustworthy.
	 */
	ret = EVP_DecryptFinal_ex(ctx, plaintext + plaintext_len, &len);

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	if(ret > 0)
	{
		/* Success */
		plaintext_len += len;
		return plaintext_len;
	}
	else
	{
		/* Verify failed */
		return -1;
	}
}


int main (int argc, char **argv)
{
	unsigned char plaintext[1024],key[32],ciphertext[1024+EVP_MAX_BLOCK_LENGTH],tag[100],pt[1024+EVP_MAX_BLOCK_LENGTH];
	unsigned char iv[16];
	unsigned char aad[16]="abcdefghijklmnop";	//dummy
	int k;

	printf("Enter key: ");
	scanf("%s",key);
	
	/* generate encryption key from user entered key */
	if(!PKCS5_PBKDF2_HMAC_SHA1(key, strlen(key),NULL,0,1000,32,key))
	{
		printf("Error in key generation\n");
		exit(1);
	}

	/* generate random IV */
	while(!RAND_bytes(iv,sizeof(iv)));

	/* get plaintext input */
	printf("Enter plaintext: ");
	scanf("%s",plaintext);

	/* encrypt the text and print on STDOUT */
	k = encrypt(plaintext, strlen(plaintext), aad, sizeof(aad), key, iv, ciphertext, tag);
	printf("%s\n",ciphertext);

	/* decrypt the text and print on STDOUT */
	k = decrypt(ciphertext, k, aad, sizeof(aad), tag, key, iv, pt);
	if(k>0)
	{
		pt[k]='\0';
		printf("%s\n",pt);
	}
	else
		printf("Unreliable Decryption, maybe the encrypted data was tampered\n");
	return 0;
}
```
