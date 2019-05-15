#include "stdafx.h"
#include "BigInt.h"


//构造大数对象并初始化为零
CBigInt::CBigInt()
{
    ZeroData(MAX_BI_LEN);
}

CBigInt::CBigInt(const CBigInt& value)
{
    SetValue(value);
}

CBigInt::CBigInt(__int64 value)
{
    SetValue(value);
}

//解构大数对象
CBigInt::~CBigInt()
{
}

int CBigInt::ToInt()
{
    int result = m_ulValue[0];
    if(m_Sign != 0)
        result = -result;

    return result;
}

bool CBigInt::IsZero() const
{
    if((m_nLength == 1) && (m_ulValue[0] == 0))
        return true;

    return false;
}

bool CBigInt::IsEven() const
{
    if(m_ulValue[0] & 0x00000001)
        return false;
    else
        return true;
}

CBigInt CBigInt::GetNegative() const
{
    CBigInt r = *this;

    r.m_Sign = (this->m_Sign == 0) ? 1 : 0;
    return r;
}

const CBigInt& CBigInt::operator=(__int64 value)
{
    SetValue(value);
    return *this;
}

const CBigInt& CBigInt::operator=(const CBigInt& value)
{
    SetValue(value);
    return *this;
}

bool CBigInt::operator<(const CBigInt& value) const
{
    if((m_Sign == 0) && (value.m_Sign == 1))
        return false;
    else if((m_Sign == 1) && (value.m_Sign == 0))
        return true;
    else if((m_Sign == 0) && (value.m_Sign == 0))
        return (CompareNoSign(value) < 0);
    else
        return (CompareNoSign(value) > 0);
}

bool CBigInt::operator<=(const CBigInt& value) const
{
    if((m_Sign == 0) && (value.m_Sign == 1))
        return false;
    else if((m_Sign == 1) && (value.m_Sign == 0))
        return true;
    else if((m_Sign == 0) && (value.m_Sign == 0))
        return (CompareNoSign(value) <= 0);
    else
        return (CompareNoSign(value) >= 0);
}

bool CBigInt::operator==(const CBigInt& value) const
{
    if(m_Sign != value.m_Sign)
        return false;
    else
        return (CompareNoSign(value) == 0);
}

bool CBigInt::operator!=(const CBigInt& value) const
{
    if((m_Sign == value.m_Sign) && (CompareNoSign(value) == 0))
        return false;
    else
        return true;
}

bool CBigInt::operator>(const CBigInt& value) const
{
    if((m_Sign == 0) && (value.m_Sign == 1))
        return true;
    else if((m_Sign == 1) && (value.m_Sign == 0))
        return false;
    else if((m_Sign == 0) && (value.m_Sign == 0))
        return (CompareNoSign(value) > 0);
    else
        return (CompareNoSign(value) < 0);
}

bool CBigInt::operator>=(const CBigInt& value) const
{
    if((m_Sign == 0) && (value.m_Sign == 1))
        return true;
    else if((m_Sign == 1) && (value.m_Sign == 0))
        return false;
    else if((m_Sign == 0) && (value.m_Sign == 0))
        return (CompareNoSign(value) >= 0);
    else
        return (CompareNoSign(value) <= 0);
}

CBigInt CBigInt::operator+(const CBigInt& value) const
{
    CBigInt r;

    if(m_Sign == value.m_Sign)
    {
        CBigInt::Add(*this, value, r);
        r.m_Sign = m_Sign;
    }
    else
    {
        if(CompareNoSign(value) >= 0)
        {
            CBigInt::Sub(*this, value, r);
            if(r.IsZero())
                r.m_Sign = 0;
            else
                r.m_Sign = m_Sign;
        }
        else
        {
            CBigInt::Sub(value, *this, r);
            r.m_Sign = value.m_Sign;
        }
    }

    return r;
}

const CBigInt& CBigInt::operator+=(const CBigInt& value)
{
    *this = this->operator +(value);

    return *this;
}

CBigInt CBigInt::operator-(const CBigInt& value) const
{
    CBigInt r;

    if(m_Sign != value.m_Sign)
    {
        CBigInt::Add(*this, value, r);
        r.m_Sign = m_Sign;
    }
    else
    {
        if(CompareNoSign(value) >= 0)
        {
            CBigInt::Sub(*this, value, r);
            if(r.IsZero())
                r.m_Sign = 0;
            else
                r.m_Sign = m_Sign;
        }
        else
        {
            CBigInt::Sub(value, *this, r);
            r.m_Sign = (m_Sign == 0) ? 1 : 0; //需要变号
        }
    }

    return r;
}

const CBigInt& CBigInt::operator-=(const CBigInt& value)
{
    *this = this->operator -(value);

    return *this;
}

CBigInt CBigInt::operator*(const CBigInt& value) const
{
    CBigInt r;

    if(m_Sign != value.m_Sign)
    {
        CBigInt::Mul(*this, value, r);
        r.m_Sign = 1;
    }
    else
    {
        CBigInt::Mul(*this, value, r);
        r.m_Sign = 0;
    }

    return r;
}

const CBigInt& CBigInt::operator*=(const CBigInt& value)
{
    *this = this->operator *(value);

    return *this;
}

CBigInt CBigInt::operator/(const CBigInt& value) const
{
    CBigInt r,m;

    if(m_Sign != value.m_Sign)
    {
        CBigInt::Div(*this, value, r, m);
        r.m_Sign = 1;
    }
    else
    {
        CBigInt::Div(*this, value, r, m);
        r.m_Sign = 0;
    }

    return r;
}

const CBigInt& CBigInt::operator/=(const CBigInt& value)
{
    *this = this->operator /(value);

    return *this;
}

CBigInt CBigInt::operator%(const CBigInt& value) const
{
    CBigInt r,m;

    CBigInt::Div(*this, value, r, m);
    m.m_Sign = m_Sign;

    return m;
}

const CBigInt& CBigInt::operator%=(const CBigInt& value)
{
    *this = this->operator %(value);

    return *this;
}

CBigInt CBigInt::operator^(const CBigInt& n) const
{
    CBigInt r;

    CBigInt::Power(*this, n, r);
    if(n.IsEven())
    {
        r.m_Sign = 0;
    }
    else
    {
        r.m_Sign = this->m_Sign;
    }

    return r;
}

const CBigInt& CBigInt::operator^=(const CBigInt& n)
{
    *this = this->operator ^(n);

    return *this;
}

CBigInt CBigInt::operator << (__int64 bits) const
{
    CBigInt r;

    CBigInt::ShiftLeft(*this, bits, r);
    r.m_Sign = this->m_Sign;

    return r;
}

const CBigInt& CBigInt::operator <<=(__int64 bits)
{
    CBigInt::ShiftLeft(*this, bits, *this);

    return *this;
}

CBigInt CBigInt::operator >> (__int64 bits) const
{
    CBigInt r;

    CBigInt::ShiftRight(*this, bits, r);
    r.m_Sign = this->m_Sign;

    return r;
}

const CBigInt& CBigInt::operator >>=(__int64 bits)
{
    CBigInt::ShiftRight(*this, bits, *this);

    return *this;
}

void CBigInt::GetRightBigN(unsigned int N, CBigInt& rightN)
{
    rightN = 0;
    unsigned int rN = std::min(m_nLength, N);
    for(unsigned int i = 0; i < rN; i++)
    {
        rightN.m_ulValue[i] = m_ulValue[i];
    }
    rightN.m_nLength = rN;
    rightN.m_Sign = m_Sign;
}

bool CBigInt::ShiftLeftBigN(unsigned int N)
{
    if((m_nLength + N) > MAX_BI_LEN)
        return false;

    int i;
    for(i = m_nLength - 1; i >= 0; i--)
    {
        m_ulValue[N + i] = m_ulValue[i];
    }
    for(i = 0; i < N; i++)
    {
        m_ulValue[i] = 0;
    }
    m_nLength += N;
    return true;
}

void CBigInt::ShiftRightBigN(unsigned int N)
{
    if(N >= m_nLength)
    {
        ZeroData(m_nLength);
        return;
    }
    for(unsigned int i = 0; i < m_nLength; i++)
    {
        if(i < (m_nLength - N))
            m_ulValue[i] = m_ulValue[N + i];
        else
            m_ulValue[i] = 0;
    }

    m_nLength -= N;
}

void CBigInt::RemRightBigN(unsigned int N)
{
    if(m_nLength > N)
    {
        for(unsigned int i = N; i < m_nLength; i++)
        {
            m_ulValue[i] = 0;
        }
        m_nLength = N;

        while((m_ulValue[m_nLength - 1] == 0) && (m_nLength > 1))
            m_nLength--;
    }
}

void CBigInt::TrimRightBigN(unsigned int N)
{
    if(m_nLength <= N)
    {
        ZeroData(m_nLength);
        return;
    }
    unsigned int left = m_nLength - N;
    for(unsigned int i = 0; i < m_nLength; i++)
    {
        if(i < left)
        {
            m_ulValue[i] = m_ulValue[i + N];
        }
        else
        {
            m_ulValue[i] = 0;
        }
    }
    m_nLength = left;
}

bool CBigInt::TestBit(__int64 bits) const
{
    __int64 wp = bits / 32;
    __int64 bp = bits % 32;

    if(wp >= m_nLength)
        return false;

    __int64 bitMask = 0x1 << bp;
    
    return (m_ulValue[wp] & bitMask) != 0;
}

__int64 CBigInt::GetTotalBits() const
{
    __int64 c = (m_nLength - 1) * 32;

    unsigned long mv = m_ulValue[m_nLength - 1];
    while(mv)
    { 
        mv = mv >> 1;
        c++;
    }

    return c;
}

void CBigInt::ZeroData(unsigned int length)
{
    if(length > MAX_BI_LEN)
    {
        length = MAX_BI_LEN;
    }
    m_Sign = 0;
    m_nLength = 1;
    for(unsigned int i = 0;i < length; i++)
        m_ulValue[i] = 0;
}

void CBigInt::SetValue(const CBigInt& value)
{
    m_Sign = value.m_Sign;
    m_nLength = value.m_nLength;
    for(int i = 0; i < MAX_BI_LEN; i++)
        m_ulValue[i] = value.m_ulValue[i];
}

void CBigInt::SetValue(__int64 value)
{
    unsigned __int64 v;
    if(value >= 0)
    {
        m_Sign = 0;
        v = value;
    }
    else
    {
        m_Sign = 1;
        v = -value;
    }
    if(v > 0xffffffff)
    {
        m_nLength = 2;
        m_ulValue[1] = (unsigned long)(v >> 32);
        m_ulValue[0] = (unsigned long)v;
    }
    else
    {
        m_nLength = 1;
        m_ulValue[0] = (unsigned long)v;
    }
    for(int i = m_nLength; i < MAX_BI_LEN; i++)
        m_ulValue[i] = 0;
}

int CBigInt::CompareNoSign(const CBigInt& value) const
{
    if(m_nLength > value.m_nLength)
        return 1;
    if(m_nLength < value.m_nLength)
        return -1;
    for(int i = m_nLength - 1; i >= 0; i--)
    {
        if(m_ulValue[i] > value.m_ulValue[i])
            return 1;
        if(m_ulValue[i] < value.m_ulValue[i])
            return -1;
    }
    return 0;
}

void CBigInt::Add(const CBigInt& value1, const CBigInt& value2, CBigInt& result)
{
    CBigInt r = value1;

    unsigned carry = 0;
    /*先调整位数对齐*/
    if(r.m_nLength < value2.m_nLength)
        r.m_nLength = value2.m_nLength;
    for(unsigned int i = 0; i < r.m_nLength; i++)
    {
        unsigned __int64 sum = value2.m_ulValue[i];
		sum = sum + r.m_ulValue[i] + carry;
        r.m_ulValue[i] = (unsigned long)sum;
        carry = (unsigned)(sum >> 32);
    }
    //处理最高位，如果当前最高位进位carry!=0，则需要增加大数的位数
    r.m_ulValue[r.m_nLength] = carry;
    r.m_nLength += carry;
    
    result = r;
}

void CBigInt::Sub(const CBigInt& value1, const CBigInt& value2, CBigInt& result)
{
    CBigInt r = value1;

    unsigned int borrow = 0;
    for(unsigned int i = 0; i < r.m_nLength; i++)
    {
        if((r.m_ulValue[i] > value2.m_ulValue[i])||((r.m_ulValue[i] == value2.m_ulValue[i])&&(borrow == 0)))
        {
            r.m_ulValue[i] = r.m_ulValue[i] - borrow - value2.m_ulValue[i];
            borrow = 0;
        }
        else
        {
            unsigned __int64 num = 0x100000000 + r.m_ulValue[i];
            r.m_ulValue[i] = (unsigned long)(num - borrow - value2.m_ulValue[i]);
            borrow = 1;
        }
    }
    while((r.m_ulValue[r.m_nLength - 1] == 0) && (r.m_nLength > 1))
        r.m_nLength--;

    result = r;
}

void CBigInt::Mul(const CBigInt& value1, const CBigInt& value2, CBigInt& result)
{
    if(value1.IsZero() || value2.IsZero())
    {
        result = 0;
        return;
    }
	unsigned __int64 carry = 0;
    CBigInt r;
	r.m_nLength = value1.m_nLength + value2.m_nLength - 1; //初步估算结果的位数
    for(unsigned int i = 0; i < r.m_nLength; i++)
	{
		unsigned __int64 sum = carry;
		carry = 0;
		for(unsigned int j = 0; j < value2.m_nLength; j++)
		{
            if(((i - j) >= 0)&&( (i - j) < value1.m_nLength))
			{
				unsigned __int64 mul = value1.m_ulValue[i - j];
				mul *= value2.m_ulValue[j];
			    carry += mul >> 32;
				mul = mul & 0xffffffff;
				sum += mul;
			}
        }
		carry += sum >> 32;
		r.m_ulValue[i] = (unsigned long)sum;
	}
	if(carry != 0) //最后仍有进位，则大数位数需要扩大
    {
        r.m_nLength++;
        r.m_ulValue[r.m_nLength - 1] = (unsigned long)carry;
    }
    result = r;
}

void CBigInt::Div(const CBigInt& value1, const CBigInt& value2, CBigInt& quotient, CBigInt& remainder)
{
    CBigInt r = 0;
    CBigInt a = value1;
    while(a.CompareNoSign(value2) >= 0)
    {       
		unsigned __int64 div = a.m_ulValue[a.m_nLength - 1];
		unsigned __int64 num = value2.m_ulValue[value2.m_nLength - 1];
		unsigned int len = a.m_nLength - value2.m_nLength;
		if((div == num) && (len == 0))
        {
            CBigInt::Add(r, CBigInt(1), r);
            CBigInt::Sub(a, value2, a);
            break;
        }
		if((div <= num) && (len > 0))
        {
            len--;
            div = (div << 32) + a.m_ulValue[a.m_nLength - 2];
        }
		div = div / (num + 1);
		CBigInt multi = div; //试商的结果
		if(len > 0)
		{
			multi.m_nLength += len;
            unsigned int i;
			for(i = multi.m_nLength - 1; i >= len; i--)
                multi.m_ulValue[i] = multi.m_ulValue[i - len];
			for(i = 0; i < len; i++)
                multi.m_ulValue[i] = 0;
		}
        CBigInt tmp;
        CBigInt::Add(r, multi, r);
        CBigInt::Mul(value2, multi, tmp);
        CBigInt::Sub(a, tmp, a);
    }
    quotient = r;
    remainder = a;
}

void CBigInt::Mod(const CBigInt& value1, const CBigInt& value2, CBigInt& result)
{
    CBigInt a = value1;
    while(a.CompareNoSign(value2) >= 0)
    {
		unsigned __int64 div = a.m_ulValue[a.m_nLength - 1];
		unsigned __int64 num = value2.m_ulValue[value2.m_nLength - 1];
		unsigned int len = a.m_nLength - value2.m_nLength;
		if((div == num) && (len == 0))
        {
            CBigInt::Sub(a, value2, a);
            break;
        }
		if((div <= num) && (len > 0))
        {
            len--;
            div = (div << 32) + a.m_ulValue[a.m_nLength - 2];
        }
		div = div / (num + 1);
		CBigInt multi = div;
        CBigInt::Mul(value2, multi, multi);
		if(len > 0)
		{
			multi.m_nLength += len;
	        unsigned int i;
			for(i = multi.m_nLength - 1; i >= len; i--)
                multi.m_ulValue[i] = multi.m_ulValue[i - len];
			for(i = 0; i < len; i++)
                multi.m_ulValue[i] = 0;
		}
        CBigInt::Sub(a, multi, a);
    }
    result = a;
}

void CBigInt::Power(const CBigInt& value, const CBigInt& n, CBigInt& result)
{
    result = 1;
    CBigInt t = value;

    for(__int64 i = 0; i < n.GetTotalBits(); i++)
    {
        if(n.TestBit(i))
        {
            CBigInt::Mul(result, t, result);
        }
        CBigInt::Mul(t, t, t);
    }
}

void CBigInt::ShiftLeft(const CBigInt& value, __int64 bits, CBigInt& result)
{
    __int64 wpos = bits / 32;
    __int64 woff = bits % 32;

    result = value;
    if(woff != 0)
    {
        unsigned long maskRight = 0xFFFFFFFF >> woff;
        unsigned long maskLeft = ~maskRight;
        result.m_ulValue[result.m_nLength] = 0;
        result.m_nLength++;
        for(int i = result.m_nLength - 1; i >=0; i--)
        {
            unsigned long orp = result.m_ulValue[i] & maskRight;
            if(i > 0)
            {
                unsigned long nlp = result.m_ulValue[i - 1] & maskLeft;
                result.m_ulValue[i] = (orp << woff) | (nlp >> (32 - woff));
            }
            else
            {
                result.m_ulValue[i] = orp << woff;
            }
        }
        if(result.m_ulValue[result.m_nLength - 1] == 0)
            result.m_nLength--;
    }
    if(wpos != 0)
    {
        for(int j = result.m_nLength - 1; j >= 0; j--)
        {
            result.m_ulValue[j + wpos] = result.m_ulValue[j];
        }
        for(int k = 0; k < wpos; k++)
        {
            result.m_ulValue[k] = 0;
        }
        result.m_nLength += (unsigned int)wpos;
    }
}

void CBigInt::ShiftRight(const CBigInt& value, __int64 bits, CBigInt& result)
{
    if(bits >= value.GetTotalBits())
    {
        result = 0;
        return;
    }

    __int64 wpos = bits / 32;
    __int64 woff = bits % 32;

    result = value;
    if(woff != 0)
    {
        unsigned long maskLeft = 0xFFFFFFFF << woff;
        unsigned long maskRight = ~maskLeft;
        for(unsigned int i = 0; i < result.m_nLength; i++)
        {
            unsigned long olp = result.m_ulValue[i] & maskLeft;
            if(i < (result.m_nLength - 1))
            {
                unsigned long nrp = result.m_ulValue[i + 1] & maskRight;
                result.m_ulValue[i] = (olp >> woff) | (nrp << (32 - woff));
            }
            else
            {
                result.m_ulValue[i] = olp >> woff;
            }
        }
        if(result.m_ulValue[result.m_nLength - 1] == 0)
            result.m_nLength--;
    }
    if(wpos != 0)
    {
        if(result.m_nLength <= wpos)
        {
            result = 0;
        }
        else
        {
            unsigned int j;
            for(j = 0; j < (result.m_nLength - wpos); j++)
            {
                result.m_ulValue[j] = result.m_ulValue[j + wpos];
            }
            while(j < result.m_nLength)
            {
                result.m_ulValue[j] = 0;
                j++;
            }

            result.m_nLength -= (unsigned int)wpos;
        }
    }
}

CBigInt CBigInt::GenRandomInteger(__int64 bits)
{
    CBigInt r;

    r.m_Sign = 0;
    r.m_nLength = (unsigned int)(bits / 32);

    for(unsigned int i = 0; i < r.m_nLength; i++)
        r.m_ulValue[i] = rand()*0x10000 + rand();

    __int64 woff = bits % 32;
    if(woff != 0)
    {
        r.m_nLength++;
        r.m_ulValue[r.m_nLength - 1] = rand()*0x10000 + rand();
        r.m_ulValue[r.m_nLength - 1] = r.m_ulValue[r.m_nLength - 1] & (0xFFFFFFFF >> (32 - woff));
    }

    return r;
}

void CBigInt::GetFromString(const char *str, unsigned int system)
{
    ZeroData(MAX_BI_LEN);
    int i = 0;
    int len = (int)strlen(str),k;
    if((len == 0) || ((len == 1) && (str[0] == '-')))
    {
        return;
    }
    if(str[0] == '-')
        i++;

    while(i < len)
    {
       CBigInt::Mul(*this, system, *this);
       if((str[i]>='0')&&(str[i]<='9'))
           k = str[i] - 48;
       else if((str[i]>='A')&&(str[i]<='F'))
           k = str[i] - 55;
       else if((str[i]>='a')&&(str[i]<='f'))
           k = str[i] - 87;
       else 
           k = 0;
       CBigInt::Add(*this, k, *this);
       i++;
    }
    if(str[0] == '-')
        m_Sign = 1;
}

void CBigInt::PutToString(std::string& str, unsigned int system)
{
    if((m_nLength == 1) && (m_ulValue[0] == 0))
    {
        str="0";
        return;
    }
	str="";
    const char *t="0123456789ABCDEF";
    CBigInt value = *this;
    CBigInt sys = system;
    while(value.m_ulValue[value.m_nLength - 1] > 0)
    {
        CBigInt a;
        CBigInt::Div(value, sys, value, a);
        str.insert(str.begin(), t[abs(a.ToInt())]);
    }
    if(m_Sign == 1)
    {
        str.insert(str.begin(), '-');
    }
}

void CBigInt::GetLong(unsigned long *values, unsigned int count)
{
    ZeroData(MAX_BI_LEN);
    m_nLength = count;
    for(unsigned int i = 0; i < count; i++)
        m_ulValue[i] = values[i];
}

int CBigInt::PutLong(unsigned long *values, unsigned int bufLen)
{
    if(m_nLength > bufLen)
        return 0;

    for(unsigned int i = 0; i < m_nLength; i++)
        values[i] = m_ulValue[i];

    return m_nLength;
}

int  CBigInt::GetFromData(const char *pData, int nBytes)
{
	int i,j;

    ZeroData(MAX_BI_LEN);
	int n = nBytes / sizeof(unsigned long);
	int r = nBytes % sizeof(unsigned long);
	if(n >= MAX_BI_LEN)
	{
		return -1;
	}	
    
    unsigned long uword = 0;
    unsigned long tmv;
	m_nLength = n;
	if(r!=0)
	{
		m_nLength++; 
		for(j = 0; j < r; j++) 
		{
			uword <<= 8;
            tmv = *pData++;
			uword |= (tmv & 0x000000FF); 
		}
		m_ulValue[m_nLength - 1] = uword;
	}
	for(i = n - 1; i >= 0; i--)
	{
        uword = 0;
		for(j = 0; j < sizeof(unsigned long); j++) 
		{ 
			uword <<= 8; 
            tmv = *pData++;
			uword |= (tmv & 0x000000FF); 
		}
		m_ulValue[i] = uword;
	}

    while((m_ulValue[m_nLength - 1] == 0) && (m_nLength > 1))
        m_nLength--;

    return nBytes;
}

int CBigInt::PutToData(char *pBuffer, int nBufferBytes)
{
	int i,j;

    int len = (m_nLength - 1) * sizeof(unsigned long);
    unsigned long uword = m_ulValue[m_nLength - 1]; /* most significant */
    int r = 0;
    while(uword != 0) 
    { 
	    r++; 
	    uword >>= 8; 
	    len++;
    }
    r %= sizeof(unsigned long);

    if(len > nBufferBytes)
    {
        return 0; 
    }

    int start = nBufferBytes - len;
    for (i = 0; i < start; i++) 
        pBuffer[i]=0; 

    int n = m_nLength - 1;
    if(r != 0)
    {
        uword = m_ulValue[n--];
        for(i = r - 1; i >= 0; i--)
        {
            pBuffer[start + i] = (char)(uword & 0x000000FF);
            uword >>= 8;
        }  
    }

    for(i = r; i < len; i += sizeof(unsigned long))
    {
        uword = m_ulValue[n--];
        for(j = sizeof(unsigned long) - 1; j >= 0; j--)
        {
            pBuffer[start + i + j] = (char)(uword & 0x000000FF);
            uword>>=8;
        }
    }

	return nBufferBytes;
}

#if 0
int  CBigInt::GetFromData(const char *pData, int nBytes)
{
	int i,j;
#if 0
/* remove leading zeros.. */
  while(*pData == 0) 
  {
    pData++; 
		nBytes--;
    if(nBytes == 0) 
    {
      return -1;
    } 
  }
#endif
    ZeroData(MAX_BI_LEN);
	int m = sizeof(unsigned long);//32 / 8;
	int n = nBytes / m;
	int r = nBytes % m;
	unsigned long wrd = 0;
	unsigned long cMask = 0x000000FF;
	unsigned long uTmp = 0;
	if(r!=0)
	{
		n++; 
		for(j = 0; j < r; j++) 
		{
			wrd <<= 8;
			uTmp = *pData++;
			uTmp &= cMask;
			wrd |= uTmp; 
		}
	}
	m_nLength = n;
	if(n > MAX_BI_LEN)
	{
		return -1;
	}

	if(r != 0) 
	{
		n--;
		m_ulValue[n]=wrd;
	}
	for(i = n - 1; i >= 0; i--)
	{
		for(j = 0; j < m;j ++) 
		{ 
			wrd <<= 8; 
			uTmp = *pData++;
			uTmp &= cMask;
			wrd |= uTmp; 
		}
		m_ulValue[i] = wrd;
	}

  /*  strip leading zeros from big number  */
  unsigned long s;
  int zm;
  s = m_nLength & MSBIT;
  zm = (int)(m_nLength & OBITS);
  while(zm > 0 && m_ulValue[zm - 1]==0)
    zm--;
  m_nLength = zm;
  if(zm > 0) 
		m_nLength |= s;

	return nBytes;
}

int CBigInt::PutToData(char *pBuffer, int nBufferBytes, bool bAdjust)
{
	int i,j;
  /*  strip leading zeros from big number  */
  unsigned long s;
  int zm;
  s = m_nLength & MSBIT;
  zm = (int)(m_nLength & OBITS);
  while(zm > 0 && m_ulValue[zm - 1]==0)
    zm--;
  m_nLength = zm;
  if(zm > 0) 
		m_nLength |= s;

	int m = sizeof(unsigned long);//32 / 8;
	unsigned long wrd = 0;
	unsigned long cMask = 0x000000FF;
	unsigned long uTmp = 0;
	int start;

  int n = (int)(m_nLength & OBITS);
  n--;
  int len = n * m;
  wrd = m_ulValue[n]; /* most significant */
  int r = 0;
  while(wrd != 0) 
	{ 
		r++; 
		wrd >>= 8; 
		len++;
	}
  r %= m;

  if(len > nBufferBytes)
  {
    return 0; 
  }

  if(bAdjust)
  {
    start = nBufferBytes - len;
    for (i = 0; i < start; i++) 
			pBuffer[i]=0; 
  }
  else 
		start = 0;
  
  if(r != 0)
  {
    wrd = m_ulValue[n--];
    for(i = r - 1; i >= 0; i--)
    {
      pBuffer[start + i] = (char)(wrd & cMask);
      wrd >>= 8;
    }  
  }

  for(i = r; i < len; i += m)
  {
    wrd = m_ulValue[n--];
    for(j = m - 1; j >= 0; j--)
    {
      pBuffer[start + i + j] = (char)(wrd & cMask);
      wrd>>=8;
    }
  }

	if(bAdjust)
		return nBufferBytes;
	else
		return len;
}
#endif