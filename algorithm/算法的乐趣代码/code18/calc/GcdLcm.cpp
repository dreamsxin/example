#include "stdafx.h"
#include "GcdLcm.h"
    
    
CBigInt EuclidGcd(const CBigInt& a, const CBigInt& b)
{
    CBigInt biger = (a > b) ? a : b;
    CBigInt smaller = (a > b) ? b : a;

    biger = biger % smaller;
    while(biger != 0)
    {
        CBigInt tmp = biger;
        biger = smaller;
        smaller = tmp;
        biger = biger % smaller;
    }

    return smaller;
}

CBigInt SteinGcd(const CBigInt& a, const CBigInt& b)
{
    CBigInt biger = (a > b) ? a : b;
    CBigInt smaller = (a > b) ? b : a;

    if(smaller == 0)
        return biger;
    if((biger % 2 == 0) && (smaller % 2 == 0))
        return SteinGcd(biger / 2, smaller / 2) * 2;
    if(biger % 2 == 0)
        return SteinGcd(biger / 2, smaller);
    if(smaller % 2 == 0)
        return SteinGcd(biger, smaller / 2);

    return SteinGcd((biger + smaller) / 2, (biger - smaller) / 2);
}

CBigInt SubstractGcd(const CBigInt& a, const CBigInt& b)
{
    CBigInt aa = a;
    CBigInt bb = b;

    while(aa != bb)   
    {   
        if(aa > bb)   
        {   
            aa = aa - bb;   
        }   
        else  
        {   
            bb = bb - aa;   
        }   
    } 

    return aa;   
}  

CBigInt GcdLcm(const CBigInt& a, const CBigInt& b)
{
    CBigInt r = (a * b) / EuclidGcd(a, b);

    return r;
}  

CBigInt NormalLcm(const CBigInt& a, const CBigInt& b)
{
    CBigInt r = a;

    while(r % b != 0)
    {
        r += a;
    }

    return r;
}  

