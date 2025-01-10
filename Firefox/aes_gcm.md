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
