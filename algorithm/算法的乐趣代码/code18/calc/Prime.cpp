#include "stdafx.h"
#include "BigInt.h"
#include "Prime.h"

const int M_R_TEST_COUNT = 5;

//小素数表
const static int PrimeTable[550]=
{   3,    5,    7,    11,   13,   17,   19,   23,   29,   31,
    37,   41,   43,   47,   53,   59,   61,   67,   71,   73,
    79,   83,   89,   97,   101,  103,  107,  109,  113,  127, 
    131,  137,  139,  149,  151,  157,  163,  167,  173,  179, 
    181,  191,  193,  197,  199,  211,  223,  227,  229,  233, 
    239,  241,  251,  257,  263,  269,  271,  277,  281,  283, 
    293,  307,  311,  313,  317,  331,  337,  347,  349,  353, 
    359,  367,  373,  379,  383,  389,  397,  401,  409,  419, 
    421,  431,  433,  439,  443,  449,  457,  461,  463,  467, 
    479,  487,  491,  499,  503,  509,  521,  523,  541,  547, 
    557,  563,  569,  571,  577,  587,  593,  599,  601,  607, 
    613,  617,  619,  631,  641,  643,  647,  653,  659,  661, 
    673,  677,  683,  691,  701,  709,  719,  727,  733,  739, 
    743,  751,  757,  761,  769,  773,  787,  797,  809,  811, 
    821,  823,  827,  829,  839,  853,  857,  859,  863,  877,
    881,  883,  887,  907,  911,  919,  929,  937,  941,  947, 
    953,  967,  971,  977,  983,  991,  997,  1009, 1013, 1019, 
    1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069, 1087,
    1091, 1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151, 1153, 
    1163, 1171, 1181, 1187, 1193, 1201, 1213, 1217, 1223, 1229, 
    1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289, 1291, 1297, 
    1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373, 1381,
    1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453, 
    1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511, 1523,
    1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583, 1597, 
    1601, 1607, 1609, 1613, 1619, 1621, 1627, 1637, 1657, 1663, 
    1667, 1669, 1693, 1697, 1699, 1709, 1721, 1723, 1733, 1741, 
    1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811, 1823, 
    1831, 1847, 1861, 1867, 1871, 1873, 1877, 1879, 1889, 1901, 
    1907, 1913, 1931, 1933, 1949, 1951, 1973, 1979, 1987, 1993, 
    1997, 1999, 2003, 2011, 2017, 2027, 2029, 2039, 2053, 2063,
    2069, 2081, 2083, 2087, 2089, 2099, 2111, 2113, 2129, 2131, 
    2137, 2141, 2143, 2153, 2161, 2179, 2203, 2207, 2213, 2221, 
    2237, 2239, 2243, 2251, 2267, 2269, 2273, 2281, 2287, 2293,
    2297, 2309, 2311, 2333, 2339, 2341, 2347, 2351, 2357, 2371,
    2377, 2381, 2383, 2389, 2393, 2399, 2411, 2417, 2423, 2437, 
    2441, 2447, 2459, 2467, 2473, 2477, 2503, 2521, 2531, 2539, 
    2543, 2549, 2551, 2557, 2579, 2591, 2593, 2609, 2617, 2621, 
    2633, 2647, 2657, 2659, 2663, 2671, 2677, 2683, 2687, 2689, 
    2693, 2699, 2707, 2711, 2713, 2719, 2729, 2731, 2741, 2749, 
    2753, 2767, 2777, 2789, 2791, 2797, 2801, 2803, 2819, 2833, 
    2837, 2843, 2851, 2857, 2861, 2879, 2887, 2897, 2903, 2909,
    2917, 2927, 2939, 2953, 2957, 2963, 2969, 2971, 2999, 3001,
    3011, 3019, 3023, 3037, 3041, 3049, 3061, 3067, 3079, 3083,
    3089, 3109, 3119, 3121, 3137, 3163, 3167, 3169, 3181, 3187, 
    3191, 3203, 3209, 3217, 3221, 3229, 3251, 3253, 3257, 3259, 
    3271, 3299, 3301, 3307, 3313, 3319, 3323, 3329, 3331, 3343,
    3347, 3359, 3361, 3371, 3373, 3389, 3391, 3407, 3413, 3433, 
    3449, 3457, 3461, 3463, 3467, 3469, 3491, 3499, 3511, 3517, 
    3527, 3529, 3533, 3539, 3541, 3547, 3557, 3559, 3571, 3581,
    3583, 3593, 3607, 3613, 3617, 3623, 3631, 3637, 3643, 3659, 
    3671, 3673, 3677, 3691, 3697, 3701, 3709, 3719, 3727, 3733, 
    3739, 3761, 3767, 3769, 3779, 3793, 3797, 3803, 3821, 3823, 
    3833, 3847, 3851, 3853, 3863, 3877, 3881, 3889, 3907, 3911, 
    3917, 3919, 3923, 3929, 3931, 3943, 3947, 3967, 3989, 4001
};

//返回值：X=N^A MOD B
CBigInt RsaTrans(const CBigInt& N, const CBigInt& A, const CBigInt& B)
{
	CBigInt X = N;
	int i,j,k;
	unsigned n;
	unsigned long num;
	k = A.m_nLength * 32 - 32;
	num = A.m_ulValue[A.m_nLength - 1];
	while(num)
    {
        num = num >> 1;
        k++;
    }
	for(i = k - 2; i >= 0; i--)
	{
		CBigInt Y = X * X.m_ulValue[X.m_nLength - 1];
        Y = Y % B;
        for(n = 1; n < X.m_nLength; n++)
		{          
			for(j = Y.m_nLength; j > 0; j--)
                Y.m_ulValue[j] = Y.m_ulValue[j - 1];
			Y.m_ulValue[0]=0;
			Y.m_nLength++;
            Y = Y + X * X.m_ulValue[X.m_nLength - n - 1];
            Y = Y % B;
		}
		X = Y;
		if((A.m_ulValue[i >> 5] >> (i & 31)) & 1)
		{
            Y = N * X.m_ulValue[X.m_nLength - 1];
		    Y = Y % B;
            for(n = 1; n < X.m_nLength; n++)
			{          
			    for(j = Y.m_nLength; j > 0; j--)
                    Y.m_ulValue[j] = Y.m_ulValue[j - 1];
			    Y.m_ulValue[0] = 0;
			    Y.m_nLength++;
                Y = Y + N * X.m_ulValue[X.m_nLength - n - 1];
			    Y = Y % B;
			}
		    X = Y;
		}
	}

    return X;
}

/****************************************************************************************
拉宾米勒算法测试素数
调用方式：N.Rab()
返回值：若N为素数，返回1，否则返回0
****************************************************************************************/
int RabPrimeTest(const CBigInt& prime)
{
    unsigned int i,j,pass;
    for(i = 0; i < 550; i++)
    {
        if((prime % PrimeTable[i]) == 0)
            return 0;
    }
    CBigInt K = prime;
	K.m_ulValue[0]--;
    for(i = 0; i < 5; i++)
    {
        CBigInt I;
        pass = 0;
        CBigInt A = rand() * rand();
		CBigInt S = K;
        while((S.m_ulValue[0] & 1) == 0)
		{
            //S右移一位
            for(j = 0; j < S.m_nLength; j++)
			{
			    S.m_ulValue[j] = S.m_ulValue[j] >> 1;
			    if(S.m_ulValue[j + 1] & 1)
                    S.m_ulValue[j] = S.m_ulValue[j] | 0x80000000;
			}
		    if(S.m_ulValue[S.m_nLength - 1] == 0)
                S.m_nLength--;

			I = RsaTrans(A, S, prime);
			if(I == K)
            {
                pass = 1;
                break;
            }
		}
		if((I.m_nLength == 1) && (I.m_ulValue[0] == 1))
            pass = 1;
		if(pass == 0) //prime是合数
            return 0;
	}
    return 1;
}



/****************************************************************************************
求解方程：ax + by = 1
****************************************************************************************/
CBigInt ExtEuclid(const CBigInt& a, const CBigInt& b, CBigInt& x, CBigInt& y)
{
    if(b == 0)
    {
        x = 1;
        y = 0;
        return a;
    }
    CBigInt xp,yp;
    CBigInt c = ExtEuclid(b, a%b, xp, yp);
    x = yp;
    y = xp - (a / b) * yp;

    return c;
}

__int64 ExtEuclid64(__int64 a, __int64 b, __int64& x, __int64& y)
{
    if (b == 0)
    {
        x = 1;
        y = 0;
        return a;
    }

    __int64 xp,yp;
    __int64 c = ExtEuclid64(b, a%b, xp, yp);
    x = yp;
    y = xp - (a / b) * yp;

    return c;
}

/****************************************************************************************
返回值：x,满足：ax mod n = 1
****************************************************************************************/
/*这个算法会出现解为负数的情况，因此需要+n修正*/
CBigInt CongruenceEquation2(const CBigInt& a, const CBigInt& n)
{
    CBigInt x,y;

    //CBigInt b = CBigInt(0) - n;
    CBigInt r = ExtEuclid(a, n, x, y);
    if(r > 0)
    {
        if(x < 0)
        {
            x += n;
        }
        return x;
    }

    return 0;
}

CBigInt CongruenceEquation(const CBigInt& a, const CBigInt& n)
{
	CBigInt I,J;
    int x,y;
	CBigInt m = n;
	CBigInt e = a;
	CBigInt X = 0;
	CBigInt Y = 1;
	x = y = 1;
	while(e != 0)
	{
		I = m / e;
		J = m % e;
		m = e;
		e = J;
		J = Y;
		Y = Y * I;
		if(x == y)
		{
		    if(X >= Y)
                Y = X - Y;
			else
            {
                Y = Y - X;
                y = 0;
            }
		}
		else
        {
            Y = X + Y;
            x = 1 - x;
            y = 1 - y;
        }
		X = J;
	}

	if(x == 0)
        X = n - X;

	return X;
}
/*这个算法会出现解为负数的情况，因此需要+n修正*/
__int64 CongruenceEquation64(__int64 a, __int64 n)
{
    __int64 x,y;

    ExtEuclid64(a, n, x, y);
    if(x < 0)
    {
        x += n;
    }
    return x;
}

CBigInt GetBigIntPrime(int bits)
{
    CBigInt r;
    r.m_nLength = bits / 32;
begin:
    unsigned int i;
	for(i = 0; i < r.m_nLength; i++)
        r.m_ulValue[i] = rand()*0x10000 + rand();

    //偶数肯定不是素数，因此给最低位置1，保证其不是偶数
    r.m_ulValue[0] = r.m_ulValue[0] | 1;
    //r.m_ulValue[0] = r.m_ulValue[0] | 0x10000001;
	for(i = r.m_nLength - 1; i > 0; i--)
	{
		r.m_ulValue[i] = r.m_ulValue[i] << 1;
		if(r.m_ulValue[i - 1] & 0x80000000)
            r.m_ulValue[i]++;
	}
	r.m_ulValue[0] = r.m_ulValue[0] << 1;
	r.m_ulValue[0]++;
    //用小素数表进行测试
    for(i = 0; i < 550; i++)
    {
        if((r % PrimeTable[i]) == 0)
            goto begin;
    }
    CBigInt K = r;
	K.m_ulValue[0]--;
    for(i = 0; i < 5; i++)
	{
        CBigInt A = rand() * rand();
	    CBigInt S = K / 2;
	    CBigInt I = RsaTrans(A, S, r);
	    if(((I.m_nLength != 1) || (I.m_ulValue[0] != 1)) && (I != K))
            goto begin;
	}

    return r;
}

CBigInt ModularPower(const CBigInt& M, const CBigInt& E, const CBigInt& N)
{
    CBigInt k = 1;
    CBigInt n = M % N;

    for(__int64 i = 0; i < E.GetTotalBits(); i++)
    {
        if(E.TestBit(i))
        {
            k = (k * n) % N;
        }
        n = (n * n) % N;
    }

    return k;
}

#if 1
#if 0
static CBigInt MontgomeryReduction(const CBigInt& X, const CBigInt& Y, const CBigInt& Np, const CBigInt& N, const CBigInt& R)
{
    CBigInt S = X * Y;
    CBigInt m = (S * Np) % R;
    S = (S + m * N) / R;
    if(S >= N)
        return S - N;
    else
        return S;
}
#endif
#if 1
static CBigInt MontgomeryReduction(const CBigInt& X, const CBigInt& Y, const CBigInt& Np, const CBigInt& N, const CBigInt& R)
{
    CBigInt D = X * Y;
    CBigInt q = (D * Np);
    q.RemRight(R.m_nLength - 1);
    D = (D + q * N);
    D.TrimRight(R.m_nLength - 1);
    if(D >= N)
        return D - N;
    else
        return D;
}
#endif
CBigInt MontgomeryModularPower(const CBigInt& M, const CBigInt& E, const CBigInt& N)
{
    CBigInt R = 1;
    //R <<= (N.GetTotalBits() + 1);
    R <<= N.m_nLength * 32;

    CBigInt Np = CongruenceEquation(R - N, R);
    /*转换到R的剩余系*/
    CBigInt Mp = (M * R) % N;
    CBigInt D = R % N;

    for(__int64 i = 0; i < E.GetTotalBits(); i++)
    {
        if(E.TestBit(i))
        {
            D = MontgomeryReduction(D, Mp, Np, N, R);
        }
        Mp = MontgomeryReduction(Mp, Mp, Np, N, R);
    }
    //转出R的剩余系
    D = MontgomeryReduction(D, 1, Np, N, R);
    
    return D;
}
#endif
#if 0
static CBigInt MontgomeryReduction(const CBigInt& X, const CBigInt& Y, const __int64 k, const CBigInt& N)
{
    CBigInt D = 0;
    
    for(unsigned int i = 0; i < X.m_nLength; i++)
    {
#if 0
        //CBigInt q = X.m_ulValue[i] * Y.m_ulValue[0] + D.m_ulValue[0];
        CBigInt q = X.m_ulValue[i];
        q = q * Y.m_ulValue[0] + D.m_ulValue[0];
        q = q * k;
        __int64 qq = q.m_ulValue[0];
#endif
        unsigned __int64 q = X.m_ulValue[i] * Y.m_ulValue[0];
        q &= 0xFFFFFFFF;
        q += D.m_ulValue[0];
        q &= 0xFFFFFFFF;
        q *= k;
        q &= 0xFFFFFFFF;


        D = D + Y * X.m_ulValue[i];
        D = D + N * q;
        D >>= 32;
    }
    if(D >= N)
        return D - N;
    else
        return D;
}

CBigInt MontgomeryModularPower(const CBigInt& M, const CBigInt& E, const CBigInt& N)
{
    CBigInt R = 1;
    //R <<= (N.GetTotalBits() + 1);
    R <<= N.m_nLength * 32;

    __int64 k = CongruenceEquation64(0x100000000 - N.m_ulValue[0], 0x100000000);

    CBigInt Mp = (M * R) % N;
    CBigInt D = R % N;

    for(__int64 i = 0; i < E.GetTotalBits(); i++)
    {
        if(E.TestBit(i))
        {
            D = MontgomeryReduction(D, Mp, k, N);
        }
        Mp = MontgomeryReduction(Mp, Mp, k, N);
    }

    D = MontgomeryReduction(D, 1, k, N);
    
    return D;
}
#endif

static bool MillerRabinHelper(const CBigInt& a, const CBigInt& m, int k, const CBigInt& n)
{
    //CBigInt b = ModularPower(a, m, n);
    CBigInt b = MontgomeryModularPower(a, m, n);

    if(b == 1)
        return true;

    for(int r = 0; r < k; r++)
    {
        if(b == (n - 1)) //b != 1 && b != n-1，满足合数条件
        {
            return true;
        }
        //b = ModularPower(b, 2, n);
        b = MontgomeryModularPower(b, 2, n);
    }

    return false;
}

bool MillerRabin(const CBigInt& n)
{
    CBigInt m = n - 1;
    int k = 0;
    
    //根据 n-1 = m*2^k，计算m和k
    while(!m.TestBit(0))
    {
        m >>= 1; //m = m / 2;
        k++;
    }

    CBigInt a,b;
    for(int i = 0; i< M_R_TEST_COUNT; i++)
    {
        __int64 nbits = n.GetTotalBits();
        a = CBigInt::GenRandomInteger((nbits > 32) ? 32 : nbits - 1) + 1; // 1 <= a <= n - 1
        if(!MillerRabinHelper(a, m, k, n))
        {
            return false;//测试失败，明确是合数
        }
    }
    return true;
}

bool IsPrimeNumber(const CBigInt& n)
{
    //先用小素数表进行测试
    for(int i = 0; i < 550; i++)
    {
        if((n % PrimeTable[i]) == 0)
            return false;
    }

    return MillerRabin(n);
}

CBigInt GeneratePrime(int bits)
{
    CBigInt r;
begin:
    r = CBigInt::GenRandomInteger(bits);

    //偶数肯定不是素数，因此给最低位置1，保证其不是偶数
    r.m_ulValue[0] = r.m_ulValue[0] | 1;
    //r.m_ulValue[0] = r.m_ulValue[0] | 0x10000001;
	for(int i = r.m_nLength - 1; i > 0; i--)
	{
		r.m_ulValue[i] = r.m_ulValue[i] << 1;
		if(r.m_ulValue[i - 1] & 0x80000000)
            r.m_ulValue[i]++;
	}
	r.m_ulValue[0] = r.m_ulValue[0] << 1;
	r.m_ulValue[0]++;
    if(!IsPrimeNumber(r))
        goto begin;

    return r;
}

#if 0

/*
这个算法的解不会出现负数的情况
*/
CBigInt CongruenceEquation(const CBigInt& a, const CBigInt& n)
{
	CBigInt I,J;
    int x,y;
	CBigInt m = n;
	CBigInt e = a;
	CBigInt X = 0;
	CBigInt Y = 1;
	x = y = 1;
	while(e != 0)
	{
		I = m / e;
		J = m % e;
		m = e;
		e = J;
		J = Y;
		Y = Y * I;
		if(x == y)
		{
		    if(X >= Y)
                Y = X - Y;
			else
            {
                Y = Y - X;
                y = 0;
            }
		}
		else
        {
            Y = X + Y;
            x = 1 - x;
            y = 1 - y;
        }
		X = J;
	}

	if(x == 0)
        X = n - X;

	return X;
}

//这个算法会出现负数的情况
CBigInt CongruenceEquation(const CBigInt& a, const CBigInt& n)
{
    CBigInt i = a;
    CBigInt j = n;
    CBigInt x0 = 1;
    CBigInt x1 = 0;
    CBigInt x;
    CBigInt r = i % j;
    CBigInt q = (i - r) / j;
    while(r > 0)
    {
        x = x0 - q * x1;
        x0 = x1;
        x1 = x;
        i = j;
        j = r;
        r = i % j;
        q = (i - r) / j;
    }

    return x;
}

//这个算法会出现负数的情况
__int64 CongruenceEquation64(__int64 a, __int64 n)
{
    __int64 x0 = 1;
    __int64 x1 = 0;
    __int64 x;
    __int64 r = a % n;
    __int64 q = (a - r) / n;
    while(r > 0)
    {
        x = x0 - q * x1;
        x0 = x1;
        x1 = x;
        a = n;
        n = r;
        r = a % n;
        q = (a - r) / n;
    }

    return x;
}

/*一个简化方案*/
void ExtEuclid64(__int64 a, __int64 b, __int64& x, __int64& y)
{
    if (b)
    {
//      gcdEx(b,a % b,x,y);
//      int t = x;
//      x = y;
//      y = t - (int)(a / b) * y;
        // 上面四句可化简为下面两句
        ExtEuclid64(b,a % b,y,x);
        y -= (int)(a / b) * x;
    } else {
        x = 1;
        y = 0;
    }
}
#endif