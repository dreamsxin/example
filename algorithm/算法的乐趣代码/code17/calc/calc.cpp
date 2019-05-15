// calc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BigInt.h"
#include "GcdLcm.h"
#include <cassert>

/*
class CBigInt
{

    //大数在0x100000000进制下的数字位数  
    unsigned m_nLength;  
    //用数组记录大数在0x100000000进制下每一位的值
    unsigned long m_ulValue[MAX_BI_LEN]; 

}*/

void TestAdd()
{
    CBigInt result;
    CBigInt add1 = 0x7FFFFFFFFFFFFFFFL;
    CBigInt add2 = 1;
    result.GetFromString("8000000000000000", 16);;
    CBigInt r = add1 + add2;
    assert(r == result);

    add1 = -120;
    add2 = 100;
    r = add1 + add2;
    result = -20;
    assert(r == result);

    add1 = -120;
    add2 = -100;
    r = add1 + add2;
    result = -220;
    assert(r == result);

    add1.GetFromString("313532612402", 10);
    add2.GetFromString("-2402", 10);
    r = add1 + add2;
    result.GetFromString("313532610000", 10);;
    assert(r == result);


    add1.GetFromString("352563396108773804955", 10);
    add2.GetFromString("-4955", 10);
    r = add1 + add2;
    result.GetFromString("352563396108773800000", 10);;
    assert(r == result);
}

void TestSub()
{
    CBigInt result;
    CBigInt sub1 = 0x7FFFFFFFFFFFFFFFL;
    CBigInt sub2 = 1;
    result = 0x7FFFFFFFFFFFFFFEL;
    CBigInt r = sub1 - sub2;
    assert(r == result);

    sub1 = -120;
    sub2 = 100;
    r = sub1 - sub2;
    result = -220;
    assert(r == result);

    sub1 = -120;
    sub2 = -100;
    r = sub1 - sub2;
    result = -20;
    assert(r == result);

    sub1 = 120;
    sub2 = -100;
    r = sub1 - sub2;
    result = 220;
    assert(r == result);

    sub1.GetFromString("352563396108773804955", 10);
    sub2.GetFromString("4955", 10);
    r = sub1 - sub2;
    result.GetFromString("352563396108773800000", 10);;
    assert(r == result);

    sub1.GetFromString("10000000000000000", 16);
    sub2 = 1;
    r = sub2 - sub1;
    result.GetFromString("-FFFFFFFFFFFFFFFF", 16);
    assert(r == result);

}

void TestMul()
{
    CBigInt result;
    CBigInt mul1 = 0x7FFFFFFFFFFFFFFFL;
    CBigInt mul2 = 8;
    result.GetFromString("73786976294838206456", 10);
    CBigInt r = mul1 * mul2;
    assert(r == result);

    mul1.GetFromString("56FFFFFF32", 16);
    mul2 = 1;
    result.GetFromString("56FFFFFF32", 16);
    r = mul1 * mul2;
    assert(r == result);

    mul1 = 1;
    mul2.GetFromString("56FFFFFF32", 16);
    result.GetFromString("56FFFFFF32", 16);
    r = mul1 * mul2;
    assert(r == result);

    mul1 = 120;
    mul2 = -2;
    result = -240;
    r = mul1 * mul2;
    assert(r == result);

    mul1 = -120;
    mul2 = -2;
    result = 240;
    r = mul1 * mul2;
    assert(r == result);

    mul1.GetFromString("10000005F7FFFFFFF", 16);
    mul2 = 0x0F;
    result.GetFromString("F000005987FFFFFF1", 16);
    r = mul1 * mul2;
    assert(r == result);

    mul1.GetFromString("1445628445840946744607609235783709524795667819061352", 10);
    mul2.GetFromString("18446445674407667997096551006152135", 10);
    result.GetFromString("26666706591583411822193194835830799779242590142029250059182106110878932434973010786520", 10);
    r = mul1 * mul2;
    assert(r == result);
}

void TestDiv()
{
    CBigInt result;
    CBigInt div1;
    div1.GetFromString("73786976294838206456", 10);
    CBigInt div2 = 8;
    result = 0x7FFFFFFFFFFFFFFFL;
    CBigInt r = div1 / div2;
    assert(r == result);

    div1 = 120;
    div2 = -2;
    result = -60;
    r = div1 / div2;
    assert(r == result);

    div1 = -120;
    div2 = -2;
    result = 60;
    r = div1 / div2;
    assert(r == result);

    div1 = -120;
    div2 = 2;
    result = -60;
    r = div1 / div2;
    assert(r == result);
}

void TestMod()
{
    CBigInt result;
    CBigInt mod1;
    mod1.GetFromString("73786976294838206456", 10);
    CBigInt mod2 = 8;
    result = 0;
    CBigInt r = mod1 % mod2;
    assert(r == result);

    mod1 = 18;
    mod2 = -5;
    result = 3;
    r = mod1 % mod2;
    assert(r == result);

    mod1 = -18;
    mod2 = -5;
    result = -3;
    r = mod1 % mod2;
    assert(r == result);

    mod1 = 18;
    mod2 = -5;
    result = 3;
    r = mod1 % mod2;
    assert(r == result);
}

void TestCompare()
{
    CBigInt com1 = 1;
    CBigInt com2 = 0;
    assert(com1 > com2);

    com1 = 0;
    com2 = 0;
    assert(com1 == com2);
    assert(com1 <= com2);
    assert(com1 >= com2);

    com1.GetFromString("-73786976294838206456", 10);
    com2 = -1;
    assert(com1 < com2);

    com1.GetFromString("-73786976294838206456", 10);
    com2.GetFromString("-73786976294838206455", 10);
    assert(com1 < com2);

    com1.GetFromString("73786976294838206455", 10);
    com2.GetFromString("73786976294838206455", 10);
    assert(com1 == com2);
    assert(com1 <= com2);
    assert(com1 >= com2);

    com1.GetFromString("0", 10);
    com2.GetFromString("-73786976294838206455", 10);
    assert(com1 > com2);

    com1.GetFromString("-1", 10);
    com2.GetFromString("5567", 10);
    assert(com1 < com2);

    com1.GetFromString("-1", 10);
    com2 = -1;
    assert(com1 == com2);
    assert(com1 <= com2);
    assert(com1 >= com2);
}

void TestPow()
{
    CBigInt value = 0;
    CBigInt n = 3;
    CBigInt result = 0;
    CBigInt r = value ^ n;
    assert(r == result);

    value = 123;
    n = 0;
    result = 1;
    r = value ^ n;
    assert(r == result);

    value = 123;
    n = 1;
    result = 123;
    r = value ^ n;
    assert(r == result);

    value.GetFromString("56FFFFFF32", 16);
    n = 1;
    result.GetFromString("56FFFFFF32", 16);
    r = value ^ n;
    assert(r == result);

    value.GetFromString("56FFFF32", 16);
    n = 2;
    result.GetFromString("1D90FF73FC00A5C4", 16);
    r = value ^ n;
    assert(r == result);

    value.GetFromString("89", 16);
    n = 6;
    result.GetFromString("6037157D6F1", 16);
    r = value ^ n;
    assert(r == result);

    value.GetFromString("-89", 16);
    n = 6;
    result.GetFromString("6037157D6F1", 16);
    r = value ^ n;
    assert(r == result);

    value.GetFromString("-113", 16);
    n = 7;
    result.GetFromString("-1A68F8796D1357B", 16);
    r = value ^ n;
    assert(r == result);

}

void TestGcd()
{
    CBigInt gcd = EuclidGcd(15, 12);
    assert(gcd == 3);

    gcd = SteinGcd(15, 12);
    assert(gcd == 3);

    gcd = SubstractGcd(15, 12);
    assert(gcd == 3);
}

void TestLcm()
{
    CBigInt lcm = GcdLcm(15, 12);
    CBigInt result = 0x3c;
    assert(result == lcm);

    lcm = NormalLcm(15, 12);
    assert(result == lcm);
}

int main(int argc, char* argv[])
{
    TestAdd();
    TestSub();
    TestMul();
    TestDiv();
    TestMod();
    TestCompare();
    TestPow();
    TestGcd();
    TestLcm();

    CBigInt value = 9;
    CBigInt n = 81;
    CBigInt r = value ^ n;

    r /= 100000000000;
    r /= 3600;
    r /= 24;
    r /= 365;
    std::string  rrr;
    r.PutToString(rrr, 10);
    std::cout << rrr << std::endl;

    value.GetFromString("6670903752021072936960", 10);
    n.GetFromString("6000000000", 10);

    r = value / n;
    r.PutToString(rrr, 10);
    std::cout << rrr << std::endl;

	return 0;
}











