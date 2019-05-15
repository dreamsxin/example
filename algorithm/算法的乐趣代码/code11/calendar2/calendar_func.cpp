#include "stdafx.h"
#include <time.h>
#include <math.h>
#include "calendar_const.h"
#include "calendar_func.h"
#include "sun.h"
#include "moon.h"
#include "support.h"

double GetInitialEstimateSolarTerms(int year, int angle)
{
    int STMonth = int(ceil(double((angle + 90.0) / 30.0)));
    STMonth = STMonth > 12 ? STMonth - 12 : STMonth;

    /* 每月第一个节气发生日期基本都4-9日之间，第二个节气的发生日期都在16－24日之间*/
    if((angle % 15 == 0) && (angle % 30 != 0))
    {
    	return CalculateJulianDay(year, STMonth, 6, 12, 0, 0.00);
    }
    else
    {
    	return CalculateJulianDay(year, STMonth, 20, 12, 0, 0.00);
    }
}

// 计算指定年份的任意节气，angle是节气在黄道上的度数
// 返回指定节气的儒略日时间(力学时)
double CalculateSolarTerms(int year, int angle)
{
    double JD0, JD1,stDegree,stDegreep;

	JD1 = GetInitialEstimateSolarTerms(year, angle);
    do
    {
    	JD0 = JD1;
        stDegree = GetSunEclipticLongitudeEC(JD0);
        /*
            对黄经0度迭代逼近时，由于角度360度圆周性，估算黄经值可能在(345,360]和[0,15)两个区间，
            如果值落入前一个区间，需要进行修正
        */
        stDegree = ((angle == 0) && (stDegree > 345.0)) ? stDegree - 360.0 : stDegree;
		stDegreep = (GetSunEclipticLongitudeEC(JD0 + 0.000005)
                      - GetSunEclipticLongitudeEC(JD0 - 0.000005)) / 0.00001;
		JD1 = JD0 - (stDegree - angle) / stDegreep;
    }while((fabs(JD1 - JD0) > 0.0000001));

    return JD1;
}

/*
得到给定的时间后面第一个日月合朔的时间，平均误差小于3秒
输入参数是指定时间的力学时儒略日数
返回值是日月合朔的力学时儒略日数
*/
double CalculateMoonShuoJD(double tdJD)
{
    double JD0, JD1,stDegree,stDegreep;

	JD1 = tdJD;
    do
    {
    	JD0 = JD1;
		double moonLongitude = GetMoonEclipticLongitudeEC(JD0);
		double sunLongitude = GetSunEclipticLongitudeEC(JD0);
        if((moonLongitude > 330.0) && (sunLongitude < 30.0))
        {
            sunLongitude = 360.0 + sunLongitude;
        }
        if((sunLongitude > 330.0) && (moonLongitude < 30.0))
        {
            moonLongitude = 60.0 + moonLongitude;
        }

		stDegree = moonLongitude - sunLongitude;
		if(stDegree >= 360.0)
			stDegree -= 360.0;

		if(stDegree < -360.0)
			stDegree += 360.0;

        stDegreep = (GetMoonEclipticLongitudeEC(JD0 + 0.000005) - GetSunEclipticLongitudeEC(JD0 + 0.000005) - GetMoonEclipticLongitudeEC(JD0 - 0.000005) + GetSunEclipticLongitudeEC(JD0 - 0.000005)) / 0.00001;
		JD1 = JD0 - stDegree / stDegreep;
    }while((fabs(JD1 - JD0) > 0.00000001));

    return JD1;
}

void CalculateStemsBranches(int year, int *stems, int *branches)
{
    int sc = year - 2000;
    *stems = (7 + sc) % 10;
    *branches = (5 + sc) % 12;

    if(*stems < 0)
        *stems += 10;
    if(*branches < 0)
        *branches += 12;
}
