// calendar.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

const int MONTHES_FOR_YEAR = 12;
const int DAYS_FOR_WEEK = 7;

const int DAYS_OF_LEAP_YEAR = 366;
const int DAYS_OF_NORMAL_YEAR = 365;

int daysOfMonth[MONTHES_FOR_YEAR] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
char *nameOfMonth[MONTHES_FOR_YEAR] = { "January", "February", "March", "April", "May", 
                                        "June", "July", "August", "September", "October", 
                                        "November", "December" };
char *nameOfWeek[DAYS_FOR_WEEK] = { "Sunday", "Monday", "Tuesday", "Wednesday", 
                                    "Thursday", "Friday", "Saturday" };

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


//维基百科的公式
double CalculateJulianDay(int year, int month, int day, int hour, int minute, double second)
{
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;

    double jdn = day + (153 * m + 2) / 5 + 365 * y + y / 4;
    if(IsGregorianDays(year, month, day))
    {
        jdn = jdn - y / 100 + y / 400 - 32045.5;
    }
    else
    {
        jdn -= 32083.5;
    }

    return jdn + hour / 24.0 + minute / 1440.0 + second / 86400.0;
}

int CalculateDays2(int ys, int ms, int ds, int ye, int me, int de)
{
    double jd1 = CalculateJulianDay(ys, ms, ds, 12, 0, 0);
    double jd2 = CalculateJulianDay(ye, me, de, 12, 0, 0);

    return int(jd2 - jd1);
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

void PrintYearBanner(int year)
{
    printf("\n********************************************************************************");
    printf("\n                              Calendar of %d\n", year);
    printf("\n********************************************************************************");
}

void PrintMonthBanner(char *monthName)
{
    printf("\n----------%s----------\n\n", monthName);
}

void PrintWeekBanner()
{
    int i;
    for(i = 0; i < DAYS_FOR_WEEK; i++)
    {
        printf("%-10s", nameOfWeek[i]);
    }
    printf("\n");
}

void InsertRowSpace(int firstWeek)
{
    int count = firstWeek;

    int i;
    for(i = 0; i < count; i++)
    {
        printf("          ");
    }
}

void SetNextRowStart()
{
    printf("\n");
    InsertRowSpace(0);
}

void PrintMonthCalendar(int year, int month)
{
    int days = GetDaysOfMonth(year, month); /*确定这个月的天数*/
    if(days <= 0)
        return;

    PrintMonthBanner(nameOfMonth[month - 1]);
    PrintWeekBanner();
    int firstDayWeek = ZellerWeek(year, month, 1);
    InsertRowSpace(firstDayWeek);
    int week = firstDayWeek;
    int i = 1;
    while(i <= days)
    {
        printf("%-10d", i);
        if(week == 6) /*到一周结束，切换到下一行输出*/
        {
            SetNextRowStart();
        }
        i++;
        week = (week + 1) % 7;
    }
}

void PrintYearCalendar(int year)
{
    PrintYearBanner(year);

    int i;
    for(i = 0; i < MONTHES_FOR_YEAR; i++)
    {
        PrintMonthCalendar(year, i + 1);
        printf("\n");
    }

    printf("\n\n");
}

int main(int argc, char* argv[])
{
    int days1 = CalculateDays(1977, 3, 27, 2005, 5, 31);
    int days2 = CalculateDays2(1977, 3, 27, 2005, 5, 31);

    double jd31 = CalculateJulianDay(1986, 12, 25, 12, 0, 1); //2446790.0000115740
    double jd32 = CalculateJulianDay(1996, 1, 1, 12, 0, 0); //2450084.0
    double jd33 = CalculateJulianDay(1858, 11, 17, 0, 0, 0); //2400000.5
    double jd34 = CalculateJulianDay(1979, 10, 1, 0, 0, 0); //2444174.5
    double jd35 = CalculateJulianDay(-4712, 1, 1, 12, 0, 0); //0.0

    PrintYearCalendar(2012);
	return 0;
}

void testZellerWeek()
{
    int week = ZellerWeek(2049, 10, 1);
    assert(week == 5);

    week = ZellerWeek(1977, 3, 27);
    assert(week == 0);

    week = ZellerWeek(1980, 2, 21);
    assert(week == 4);

    week = ZellerWeek(1999, 12, 31);
    assert(week == 5);

    week = ZellerWeek(2000, 1, 1);
    assert(week == 6);

    week = ZellerWeek(2009, 10, 1);
    assert(week == 4);
}

void testTotalWeek()
{
    int week = TotalWeek(2049, 10, 1);
    assert(week == 5);

    week = TotalWeek(1977, 3, 27);
    assert(week == 0);

    week = TotalWeek(1980, 2, 21);
    assert(week == 4);

    week = TotalWeek(1999, 12, 31);
    assert(week == 5);

    week = TotalWeek(2000, 1, 1);
    assert(week == 6);

    week = TotalWeek(2009, 10, 1);
    assert(week == 4);
}