#include "stdafx.h"
#include "MD5Hash.h"

// Padding
static const unsigned char MD5_PADDING[64] = {
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define ROL32(x,n) (((x) << (n)) | ((x) >> (32 - (n))))
#define ROR32(x,n) (((x) >> (n)) | ((x) << (32 - (n))))

// Selection, majority and parity
#define MD5_F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define MD5_G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define MD5_H(x, y, z) ((x) ^ (y) ^ (z))
#define MD5_I(x, y, z) ((y) ^ ((x) | (~z)))

// MD5_FF, MD5_GG, MD5_HH, and MD5_II transformations for rounds 1, 2, 3,
// and 4. Rotation is separate from addition to prevent recomputation.
#define MD5_FF(a, b, c, d, x, s, ac) {(a) += MD5_F((b), (c), (d)) + (x) + \
	(unsigned long)(ac); (a) = ROL32((a), (s)); (a) += (b); }
#define MD5_GG(a, b, c, d, x, s, ac) {(a) += MD5_G((b), (c), (d)) + (x) + \
	(unsigned long)(ac); (a) = ROL32((a), (s)); (a) += (b); }
#define MD5_HH(a, b, c, d, x, s, ac) {(a) += MD5_H((b), (c), (d)) + (x) + \
	(unsigned long)(ac); (a) = ROL32((a), (s)); (a) += (b); }
#define MD5_II(a, b, c, d, x, s, ac) {(a) += MD5_I((b), (c), (d)) + (x) + \
	(unsigned long)(ac); (a) = ROL32((a), (s)); (a) += (b); }

// Constants for transformation
#define MD5_S11 7
#define MD5_S12 12
#define MD5_S13 17
#define MD5_S14 22
#define MD5_S21 5
#define MD5_S22 9
#define MD5_S23 14
#define MD5_S24 20
#define MD5_S31 4
#define MD5_S32 11
#define MD5_S33 16
#define MD5_S34 23
#define MD5_S41 6
#define MD5_S42 10
#define MD5_S43 15
#define MD5_S44 21

CMD5Hash::CMD5Hash()
{
}

CMD5Hash::~CMD5Hash()
{
}

void CMD5Hash::Init()
{	
	m_i[0] = m_i[1] = 0;

	// Load magic constants
	m_buf[0] = (unsigned long)0x67452301;
	m_buf[1] = (unsigned long)0xefcdab89;
	m_buf[2] = (unsigned long)0x98badcfe;
	m_buf[3] = (unsigned long)0x10325476;
}

void CMD5Hash::Update(const unsigned char *pbuf,int nLen)
{
	unsigned long pIn[16];
	int mdi;
	int i, ii;

	// Compute number of bytes mod 64
	mdi = (int)((m_i[0] >> 3) & 0x3F);

	// Update number of bits
	if((m_i[0] + ((unsigned long)nLen << 3)) < m_i[0]) 
		m_i[1]++; // Overflow
	m_i[0] += ((unsigned long)nLen << 3);
	m_i[1] += ((unsigned long)nLen >> 29);

	while(nLen--)
	{
		// Add new character to buffer, increment mdi
		m_in[mdi++] = *pbuf++;

		// Transform if necessary
		if(mdi == 0x40)
		{
			for(i = 0, ii = 0; i < 16; i++, ii += 4)
				pIn[i] = (((unsigned long)m_in[ii+3]) << 24) |
					(((unsigned long)m_in[ii+2]) << 16) |
					(((unsigned long)m_in[ii+1]) << 8) |
					((unsigned long)m_in[ii]);

			_Transform(m_buf, pIn);
			mdi = 0; // Reset mdi
		}
	}
}

void CMD5Hash::Final(unsigned char *pFinalDigest)
{
	unsigned long pIn[16];
	int mdi;
	int i, ii, padLen;

	// Save number of bits
	pIn[14] = m_i[0];
	pIn[15] = m_i[1];

	// Compute number of bytes mod 64
	mdi = (int)((m_i[0] >> 3) & 0x3F);

	// Pad out to 56 mod 64
	padLen = (mdi < 56) ? (56 - mdi) : (120 - mdi);
	Update(MD5_PADDING, padLen);

	// Append length in bits and transform
	for(i = 0, ii = 0; i < 14; i++, ii += 4)
		pIn[i] = (((unsigned long)m_in[ii+3]) << 24) | (((unsigned long)m_in[ii+2]) << 16) |
			(((unsigned long)m_in[ii+1]) <<  8) | ((unsigned long)m_in[ii]);
	_Transform(m_buf, pIn);

	// Store buffer in digest
	for(i = 0, ii = 0; i < 4; i++, ii += 4)
	{
		pFinalDigest[ii]   = (unsigned char)( m_buf[i]        & 0xFF);
		pFinalDigest[ii+1] = (unsigned char)((m_buf[i] >>  8) & 0xFF);
		pFinalDigest[ii+2] = (unsigned char)((m_buf[i] >> 16) & 0xFF);
		pFinalDigest[ii+3] = (unsigned char)((m_buf[i] >> 24) & 0xFF);
	}
}
	
void CMD5Hash::_Transform(unsigned long *pBuf, unsigned long *pIn)
{
	unsigned long a = pBuf[0], b = pBuf[1], c = pBuf[2], d = pBuf[3];

	MD5_FF( a, b, c, d, pIn[ 0], MD5_S11, (unsigned long) 3614090360u);
	MD5_FF( d, a, b, c, pIn[ 1], MD5_S12, (unsigned long) 3905402710u);
	MD5_FF( c, d, a, b, pIn[ 2], MD5_S13, (unsigned long)  606105819u);
	MD5_FF( b, c, d, a, pIn[ 3], MD5_S14, (unsigned long) 3250441966u);
	MD5_FF( a, b, c, d, pIn[ 4], MD5_S11, (unsigned long) 4118548399u);
	MD5_FF( d, a, b, c, pIn[ 5], MD5_S12, (unsigned long) 1200080426u);
	MD5_FF( c, d, a, b, pIn[ 6], MD5_S13, (unsigned long) 2821735955u);
	MD5_FF( b, c, d, a, pIn[ 7], MD5_S14, (unsigned long) 4249261313u);
	MD5_FF( a, b, c, d, pIn[ 8], MD5_S11, (unsigned long) 1770035416u);
	MD5_FF( d, a, b, c, pIn[ 9], MD5_S12, (unsigned long) 2336552879u);
	MD5_FF( c, d, a, b, pIn[10], MD5_S13, (unsigned long) 4294925233u);
	MD5_FF( b, c, d, a, pIn[11], MD5_S14, (unsigned long) 2304563134u);
	MD5_FF( a, b, c, d, pIn[12], MD5_S11, (unsigned long) 1804603682u);
	MD5_FF( d, a, b, c, pIn[13], MD5_S12, (unsigned long) 4254626195u);
	MD5_FF( c, d, a, b, pIn[14], MD5_S13, (unsigned long) 2792965006u);
	MD5_FF( b, c, d, a, pIn[15], MD5_S14, (unsigned long) 1236535329u);

	MD5_GG( a, b, c, d, pIn[ 1], MD5_S21, (unsigned long) 4129170786u);
	MD5_GG( d, a, b, c, pIn[ 6], MD5_S22, (unsigned long) 3225465664u);
	MD5_GG( c, d, a, b, pIn[11], MD5_S23, (unsigned long)  643717713u);
	MD5_GG( b, c, d, a, pIn[ 0], MD5_S24, (unsigned long) 3921069994u);
	MD5_GG( a, b, c, d, pIn[ 5], MD5_S21, (unsigned long) 3593408605u);
	MD5_GG( d, a, b, c, pIn[10], MD5_S22, (unsigned long)   38016083u);
	MD5_GG( c, d, a, b, pIn[15], MD5_S23, (unsigned long) 3634488961u);
	MD5_GG( b, c, d, a, pIn[ 4], MD5_S24, (unsigned long) 3889429448u);
	MD5_GG( a, b, c, d, pIn[ 9], MD5_S21, (unsigned long)  568446438u);
	MD5_GG( d, a, b, c, pIn[14], MD5_S22, (unsigned long) 3275163606u);
	MD5_GG( c, d, a, b, pIn[ 3], MD5_S23, (unsigned long) 4107603335u);
	MD5_GG( b, c, d, a, pIn[ 8], MD5_S24, (unsigned long) 1163531501u);
	MD5_GG( a, b, c, d, pIn[13], MD5_S21, (unsigned long) 2850285829u);
	MD5_GG( d, a, b, c, pIn[ 2], MD5_S22, (unsigned long) 4243563512u);
	MD5_GG( c, d, a, b, pIn[ 7], MD5_S23, (unsigned long) 1735328473u);
	MD5_GG( b, c, d, a, pIn[12], MD5_S24, (unsigned long) 2368359562u);

	MD5_HH( a, b, c, d, pIn[ 5], MD5_S31, (unsigned long) 4294588738u);
	MD5_HH( d, a, b, c, pIn[ 8], MD5_S32, (unsigned long) 2272392833u);
	MD5_HH( c, d, a, b, pIn[11], MD5_S33, (unsigned long) 1839030562u);
	MD5_HH( b, c, d, a, pIn[14], MD5_S34, (unsigned long) 4259657740u);
	MD5_HH( a, b, c, d, pIn[ 1], MD5_S31, (unsigned long) 2763975236u);
	MD5_HH( d, a, b, c, pIn[ 4], MD5_S32, (unsigned long) 1272893353u);
	MD5_HH( c, d, a, b, pIn[ 7], MD5_S33, (unsigned long) 4139469664u);
	MD5_HH( b, c, d, a, pIn[10], MD5_S34, (unsigned long) 3200236656u);
	MD5_HH( a, b, c, d, pIn[13], MD5_S31, (unsigned long)  681279174u);
	MD5_HH( d, a, b, c, pIn[ 0], MD5_S32, (unsigned long) 3936430074u);
	MD5_HH( c, d, a, b, pIn[ 3], MD5_S33, (unsigned long) 3572445317u);
	MD5_HH( b, c, d, a, pIn[ 6], MD5_S34, (unsigned long)   76029189u);
	MD5_HH( a, b, c, d, pIn[ 9], MD5_S31, (unsigned long) 3654602809u);
	MD5_HH( d, a, b, c, pIn[12], MD5_S32, (unsigned long) 3873151461u);
	MD5_HH( c, d, a, b, pIn[15], MD5_S33, (unsigned long)  530742520u);
	MD5_HH( b, c, d, a, pIn[ 2], MD5_S34, (unsigned long) 3299628645u);

	MD5_II( a, b, c, d, pIn[ 0], MD5_S41, (unsigned long) 4096336452u);
	MD5_II( d, a, b, c, pIn[ 7], MD5_S42, (unsigned long) 1126891415u);
	MD5_II( c, d, a, b, pIn[14], MD5_S43, (unsigned long) 2878612391u);
	MD5_II( b, c, d, a, pIn[ 5], MD5_S44, (unsigned long) 4237533241u);
	MD5_II( a, b, c, d, pIn[12], MD5_S41, (unsigned long) 1700485571u);
	MD5_II( d, a, b, c, pIn[ 3], MD5_S42, (unsigned long) 2399980690u);
	MD5_II( c, d, a, b, pIn[10], MD5_S43, (unsigned long) 4293915773u);
	MD5_II( b, c, d, a, pIn[ 1], MD5_S44, (unsigned long) 2240044497u);
	MD5_II( a, b, c, d, pIn[ 8], MD5_S41, (unsigned long) 1873313359u);
	MD5_II( d, a, b, c, pIn[15], MD5_S42, (unsigned long) 4264355552u);
	MD5_II( c, d, a, b, pIn[ 6], MD5_S43, (unsigned long) 2734768916u);
	MD5_II( b, c, d, a, pIn[13], MD5_S44, (unsigned long) 1309151649u);
	MD5_II( a, b, c, d, pIn[ 4], MD5_S41, (unsigned long) 4149444226u);
	MD5_II( d, a, b, c, pIn[11], MD5_S42, (unsigned long) 3174756917u);
	MD5_II( c, d, a, b, pIn[ 2], MD5_S43, (unsigned long)  718787259u);
	MD5_II( b, c, d, a, pIn[ 9], MD5_S44, (unsigned long) 3951481745u);

	pBuf[0] += a; pBuf[1] += b; pBuf[2] += c; pBuf[3] += d;
}


void CalcMD5Hash(void *data, int dataSize, unsigned char *digestBuf)
{
    CMD5Hash hash;

    hash.Init();
    hash.Update((unsigned char *)data, dataSize);
    hash.Final(digestBuf);
}

