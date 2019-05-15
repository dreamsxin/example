#ifndef __MD5HASH_H__
#define __MD5HASH_H__

#define MD5_DIGEST_SIZE 16

class CMD5Hash
{
public:
	CMD5Hash();
	~CMD5Hash();
	void Init();
	void Update(const unsigned char *pbuf,int nLen);
	void Final(unsigned char *pFinalDigest);

private:
	void _Transform(unsigned long *pBuf, unsigned long *pIn);
	unsigned long m_i[2];
	unsigned long m_buf[4];
	unsigned char m_in[64];
};

void CalcMD5Hash(void *data, int dataSize, unsigned char *digestBuf);

#endif //__MD5HASH_H__