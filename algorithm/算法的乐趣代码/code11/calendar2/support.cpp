#include "stdafx.h"
#include <time.h>
#include <math.h>
#include "calendar_const.h"
#include "td_utc.h"
#include "support.h"


int daysOfMonth[MONTHES_FOR_YEAR] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


bool IsLeapYear(int year)
{
    return ( (year % 4 == 0) && (year % 100 != 0) ) || (year % 400 == 0);
}

/*
判断是否是启用格里历以后的日期
1582年，罗马教皇将1582年10月4日指定为1582年10月15日，从此废止了凯撒大帝制定的儒略历，
改用格里历
*/
bool IsGregorianDays(int year, int month, int day)
{
    if(year < 1582)
        return false;

    if(year == 1582)
    {
        if( (month < 10) || ((month == 10) && (day < 15)) )
            return false;
    }

    return true;
}

int GetDaysOfMonth(int year, int month)
{
    if((month < 1) || (month > 12))
        return 0;

    int days = daysOfMonth[month - 1];
    if((month == 2) && IsLeapYear(year))
    {
        days++;
    }

    return days;
}

/*计算一年中过去的天数，包括指定的这一天*/
int CalcYearPassedDays(int year, int month, int day)
{
    int passedDays = 0;
    
    int i;
    for(i = 0; i < month - 1; i++)
    {
        passedDays += daysOfMonth[i];
    }

    passedDays += day;
    if((month > 2) && IsLeapYear(year))
        passedDays++;

    return passedDays;
}

/*计算一年中还剩下的天数，不包括指定的这一天*/
int CalcYearRestDays(int year, int month, int day)
{
    int leftDays = daysOfMonth[month - 1] - day;
    
    int i;
    for(i = month; i < MONTHES_FOR_YEAR; i++)
    {
        leftDays += daysOfMonth[i];
    }

    if((month <= 2) && IsLeapYear(year))
        leftDays++;

    return leftDays;
}

/*
计算years年1月1日和yeare年1月1日之间的天数，
包括years年1月1日，但是不包括yeare年1月1日
*/
int CalcYearsDays(int years, int yeare)
{
    int days = 0;

    int i;
    for(i = years; i < yeare; i++)
    {
        if(IsLeapYear(i))
            days += DAYS_OF_LEAP_YEAR;
        else
            days += DAYS_OF_NORMAL_YEAR;
    }

    return days;
}

int CalculateDays(int ys, int ms, int ds, int ye, int me, int de)
{
    int days = CalcYearRestDays(ys, ms, ds);

    if(ys != ye) /*不是同一年的日期*/
    {
        if((ye - ys) >= 2) /*间隔超过一年，要计算间隔的整年时间*/
        {
            days += CalcYearsDays(ys + 1, ye);
        }
        days += CalcYearPassedDays(ye, me, de);
    }
    else
    {
        days = days - CalcYearRestDays(ye, me, de);
    }

    return days;
}

int TotalWeek(int year, int month, int day)
{
    int d = CalcYearPassedDays(year, month, day);
    int y = year - 1;
    int w = y * DAYS_OF_NORMAL_YEAR + y / 4 - y / 100 + y / 400 + d; 

    return w % 7;
}

int ZellerWeek(int year, int month, int day)
{
    int m = month;
    int d = day;
    
    if(month <= 2) /*对小于2的月份进行修正*/
    {
        year--;
        m = month + 12;
    }

    int y = year % 100;
    int c = year / 100;

    int w = (y + y / 4 + c / 4 - 2 * c + (13 * (m + 1) / 5) + d - 1) % 7;
    if(w < 0) /*修正计算结果是负数的情况*/
        w += 7;

    return w;
}

double CalculateJulianDay(int year, int month, int day, int hour, int minute, double second)
{
    if(month <= 2)
    {
        month += 12;
        year -= 1;
    }
    int B = -2;
    if(IsGregorianDays(year, month, day))
    {
        B = year / 400 - year / 100;
    }

    double a = 365.25 * year;
    double b = 30.6001 * (month + 1);
    return int(a) + int(b) + B + 1720996.5 + day + hour / 24.0 + minute / 1440.0 + second / 86400.0;
}

double CalculateJulianDay2(WZDAYTIME *pDT)
{
    return CalculateJulianDay(pDT->year, pDT->month, pDT->day, pDT->hour, pDT->minute, pDT->second);
}

void GetDayTimeFromJulianDay(double jd, WZDAYTIME *pDT)
{
    int cna, cnd;
    double cnf;

    double jdf = jd + 0.5;
    cna = int(jdf);
    cnf = jdf - cna;
    if(cna > 2299161)
    {
        cnd = int((cna - 1867216.25) / 36524.25);
        cna = cna + 1 + cnd - int(cnd / 4);
    }
    cna = cna + 1524;
    int year = int((cna - 122.1) / 365.25);
    cnd = cna - int(365.25 * year);
    int month = int(cnd / 30.6001);
    int day = cnd - int(month * 30.6001);
    year = year - 4716;
    month = month - 1;
    if(month  > 12)
        month = month - 12;
    if(month <= 2)
        year = year + 1;
    if(year < 1)
        year = year - 1;
    cnf = cnf * 24.0;
    pDT->hour = int(cnf);
    cnf = cnf - pDT->hour;
    cnf = cnf * 60.0;
    pDT->minute = int(cnf);
    cnf = cnf - pDT->minute;
    //cnf = cnf * 60.0;
    pDT->second = cnf * 60.0;
    pDT->year = year;
    pDT->month = month;
    pDT->day = day;
}

// ------------------------------------------------------------------------
//
// 格林威治时间转本地时间（以格里历表示本地时间）
//
// ------------------------------------------------------------------------
void UTCToLocalTime(int &year, int &month, int &day, int &hour, int &minute, double &second)
{
    _tzset ();

    long _tz = 0;
     _get_timezone(&_tz);
    // 计算本地时间和标准时间的时差（单位：秒）
    int nDifference_hour = static_cast<int>(_tz / 3600);
    int nDifference_minute = static_cast<int>((_tz - nDifference_hour * 3600) / 60);
    int nDifference_second = static_cast<int>((_tz - nDifference_hour * 3600) - nDifference_minute * 60);

    // 格林威治时间 + 时差 = 本地时间

    // 秒
    second = second - nDifference_second;
    if(second >= 60 || second < 0)
    {
        minute = second > 0 ? minute + 1 : minute - 1 ;
        second = abs(abs(second) - 60);
    }

    // 分
    minute = minute - nDifference_minute;
    if(minute >= 60 || minute < 0)
    {
        hour = minute > 0 ? hour + 1 : hour - 1;
        minute = abs(abs(minute) - 60);
    }

    // 时
    hour = hour - nDifference_hour;
    if(hour >= 24 || hour < 0)
    {
        day = (hour >= 24 || hour == 0) ? day + 1 : day - 1;
        hour = abs(abs(hour) - 24);
    }

    // 日
    int nDaysOfMonth = GetDaysOfMonth(year, month);
    if(day > nDaysOfMonth || day <= 0)
    {
        if(day > nDaysOfMonth)
           month++;
        if(day < nDaysOfMonth || day <= 0)
           month--;

        day = abs(abs(day) - nDaysOfMonth);
    }
    // 月
    if(month > 12 || month <= 0)
    {
        year = month > 0 ? year + 1 : year - 1;
        month = month > 0 ? abs(month - 12) : abs(12 + month);
    }
}

// ------------------------------------------------------------------------
//
// 本地时间转格林威治时间（以格里历表示）
//
// ------------------------------------------------------------------------
void LocalTimetoUTC(int &year, int &month, int &day, int &hour, int &minute, double &second)
{
    _tzset () ;

    long _tz = 0;
     _get_timezone(&_tz);
    // 计算本地时间和标准时间的时差（单位：秒）
    int nDifference_hour = static_cast<int>(_tz / 3600);
    int nDifference_minute = static_cast<int>((_tz - nDifference_hour * 3600) / 60);
    int nDifference_second = static_cast<int>((_tz - nDifference_hour * 3600) - nDifference_minute * 60);

    // 本地时间 - 时差 = 格林威治时间

    // 秒
    second = second + nDifference_second;
    if(second >= 60 || second < 0)
    {
        minute = second > 0 ? minute + 1 : minute - 1 ;
        second = abs(abs(second) - 60);
    }

    // 分
    minute = minute + nDifference_minute;
    if(minute >= 60 || minute < 0)
    {
        hour = minute > 0 ? hour + 1 : hour - 1;
        minute = abs(abs(minute) - 60);
     }

    // 时
    hour = hour + nDifference_hour;
    if(hour >= 24 || hour < 0)
    {
        day = (hour >= 24 || hour == 0) ? day + 1 : day - 1;
        hour = abs(abs(hour) - 24);
    }

    // 日
    int nDaysOfMonth = GetDaysOfMonth(year, month);
    if(day > nDaysOfMonth || day <= 0)
    {
        if(day > nDaysOfMonth)
             month++;
        if(day < nDaysOfMonth || day <= 0)
           month--;

        day = abs(abs(day) - nDaysOfMonth);
    }
    // 月
    if(month > 12 || month <= 0)
    {
        year = month > 0 ? year + 1 : year - 1;
        month = month > 0 ? abs(month - 12) : abs(12 + month);
    }
}

double JDUTCToLocalTime(double utcJD)
{
    _tzset () ;

    long seconds = 0;
     _get_timezone(&seconds);

     return utcJD - (double)seconds / 86400.0;
}

double JDLocalTimetoUTC(double localJD)
{
    _tzset () ;

    long seconds = 0;
     _get_timezone(&seconds);

     return localJD + (double)seconds / 86400.0;
}

/*
double JDUTCToLocalTime(double utcJD)
{
    WZDAYTIME dt;
    GetDayTimeFromJulianDay(utcJD, &dt);

    UTCToLocalTime(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);

    return CalculateJulianDay(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
}

double JDLocalTimetoUTC(double localJD)
{
    WZDAYTIME dt;
    GetDayTimeFromJulianDay(localJD, &dt);
    LocalTimetoUTC(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);

    return CalculateJulianDay(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
}
*/
double JDUTCtoTD(double utcJD)
{
	double jd2K = utcJD - JD2000;
	double tian = TdUtcDeltatT2(jd2K);
	utcJD += tian;

	return utcJD;
}

double JDTDtoUTC(double tdJD)
{
	double jd2K = tdJD - JD2000;
	double tian = TdUtcDeltatT2(jd2K);
	tdJD -= tian;

    return tdJD;
}

double JDLocalTimetoTD(double localJD)
{
    double tmp = JDLocalTimetoUTC(localJD);

    return JDUTCtoTD(tmp);
}

double JDTDtoLocalTime(double tdJD)
{
    double tmp = JDTDtoUTC(tdJD);

    return JDUTCToLocalTime(tmp);
}

double RadianToDegree(double radian)
{
	return radian * 180.0 / PI;
}

double DegreeToRadian(double degree)
{
	return degree * PI / 180.0;
}

double Mod360Degree(double degrees)
{
   double dbValue = degrees;

   while(dbValue < 0.0)
     dbValue += 360.0;

   while(dbValue > 360.0)
     dbValue -= 360.0;

   return dbValue;
}

int JDtoString(double jd, char *strBuf, int bufSize)
{
    WZDAYTIME dt;

    GetDayTimeFromJulianDay(jd, &dt);

    return sprintf_s(strBuf, bufSize, "%04d-%02d-%02d, %02d:%02d:%4.2f",
                     dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
}

