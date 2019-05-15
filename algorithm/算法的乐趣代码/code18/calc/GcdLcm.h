#ifndef __GCD_LCM_H__
#define __GCD_LCM_H__

#include "BigInt.h"

CBigInt EuclidGcd(const CBigInt& a, const CBigInt& b);
CBigInt SteinGcd(const CBigInt& a, const CBigInt& b);
CBigInt SubstractGcd(const CBigInt& a, const CBigInt& b);

CBigInt GcdLcm(const CBigInt& a, const CBigInt& b);
CBigInt NormalLcm(const CBigInt& a, const CBigInt& b);


#endif //__GCD_LCM_H__