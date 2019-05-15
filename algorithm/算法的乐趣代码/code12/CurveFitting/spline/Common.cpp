#include "stdafx.h"
#include "Common.h"


bool IsPrecisionZero(double value)
{
    if(std::fabs(value) < PRECISION)
    {
        return true;
    }

    return false;
}


