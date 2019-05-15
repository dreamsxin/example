#ifndef __RSALIB_H__
#define __RSALIB_H__

#include "BigInt.h"

int Rsa_Pkcs15_Encrypt(CBigInt& e, CBigInt& n, int kbits, 
                       void *pSrcData, int dataSize, void *pEncData, int encBufSize);


int Rsa_Pkcs15_Decrypt(CBigInt& d, CBigInt& n, int kbits, 
                       void *pSrcData, int dataSize, void *pDecData, int decBufSize);


int Rsa_Pkcs15_Sign(CBigInt& d, CBigInt& n, int kbits, 
                       void *pSrcData, int dataSize, void *pSignBuf, int bufSize);


bool Rsa_Pkcs15_Verify(CBigInt& e, CBigInt& n, int kbits, 
                       void *pSignData, int dataSize, void *pSrcData, int srcSize);

#endif /*__RSALIB_H__*/