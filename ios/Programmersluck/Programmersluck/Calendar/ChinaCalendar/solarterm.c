//
//  solarterm.c
//  ChineseCalendar
//
//  Created by Tony Li on 6/4/12.
//  Copyright (c) 2012 Tony Li. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include "solarterm.h"

int IfGregorian(int y, int m, int d, int opt);
int DayDifference(int y, int m, int d);
double AntiDayDifference(int y, double x);
double EquivalentStandardDay(int y, int m, int d);
double Term(int y, int n, int pd);

//#define compress_date(year, month, day) ((year) * 10000 + (month) * 100 + (day))
#define compress_date(year, month, day) (((year) << 7) | (month << 5) | day)
#define solarterm_range_contains(range, day) (day >= (range & 0x1F) && day <= (range >> 5))

struct SolarTerm {
    int solarIndex;
    int solarDate;
};

int solarterm_index(int year, int month, int day) {
    // 节气的日期范围 0-4个字节表示最小日期
    const static int ranges[] = {
        0xa3, 0x292, 0xe4, 0x2d3, 0xc4, 0x2b3, 0xe4, 0x2d4, 0xe4, 0x2d4, 0x106, 0x316,
        0x126, 0x316, 0x126, 0x316, 0x127, 0x316, 0x106, 0x2f5, 0x106, 0x2f5, 0xe4, 0x2b3
    };
    
    int index = -1;
    int guess = month == 1 ? 22 : (month - 2) * 2;
    if (solarterm_range_contains(ranges[guess], day) || solarterm_range_contains(ranges[guess + 1], day)) {
        struct SolarTerm solarTerms[2];
        int n = month * 2 - 1;
        for (; n <= month * 2; n++)
        {
            double Termdays = Term(year, n, 1);
            double mdays = AntiDayDifference(year, floor(Termdays));
            int tMonth = (int)ceil((double)n / 2);
            int tday = (int)mdays % 100;
            
            if (n >= 3)
                solarTerms[n - month * 2 + 1].solarIndex = n - 3;
            else
                solarTerms[n - month * 2 + 1].solarIndex = n + 21;
            
            solarTerms[n - month * 2 + 1].solarDate = compress_date(year, tMonth, tday);
        }
        
        int i = 0;
        for (; i<2; i++)
        {
            if (solarTerms[i].solarDate == compress_date(year, month, day)) {
                index = solarTerms[i].solarIndex;
                break;
            }
        }
    }

    return index;
}

const char *solarterm_name(const int index) {
    if (index >= 0 && index < 24) {
        const static char* SolarTerms[] = {"立春", "雨水", "惊蛰", "春分", "清明", "谷雨", "立夏", "小满", "芒种", "夏至", "小暑", "大暑", "立秋", "处暑", "白露", "秋分", "寒露", "霜降", "立冬", "小雪", "大雪", "冬至", "小寒", "大寒" };
        return SolarTerms[index];
    } else {
        return NULL;
    }
}

int IfGregorian(int y, int m, int d, int opt)
{
    if (opt == 1)
    {
        if (y > 1582 || (y == 1582 && m > 10) || (y == 1582 && m == 10 && d > 14))
            return (1);     //Gregorian
        else
            if (y == 1582 && m == 10 && d >= 5 && d <= 14)
                return (-1);  //空
            else
                return (0);  //Julian
    }
    
    if (opt == 2)
        return (1);     //Gregorian
    if (opt == 3)
        return (0);     //Julian
    return (-1);
}

int DayDifference(int y, int m, int d)
{
    int ifG = IfGregorian(y, m, d, 1);
    //NSArray *monL = [NSArray arrayWithObjects:, nil];
    int monL[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (ifG == 1) {
        if ((y % 100 != 0 && y % 4 == 0) || (y % 400 == 0)) {
            monL[2] += 1;
        } else {
            if (y % 4 == 0) {
                monL[2] += 1;
            }
        }
    }
    
    int v = 0, i = 0;
    for (; i <= m - 1; i++)
    {
        v += monL[i];
    }
    v += d;
    if (y == 1582)
    {
        if (ifG == 1)
            v -= 10;
        if (ifG == -1)
            v = 0;  //infinity 
    }
    return v;
}

double EquivalentStandardDay(int y, int m, int d)
{
    //Julian的等效标准天数
    double v = (y - 1) * 365 + floor((double)((y - 1) / 4)) + DayDifference(y, m, d) - 2;
    
    if (y > 1582)
    {//Gregorian的等效标准天数
        v += -floor((double)((y - 1) / 100)) + floor((double)((y - 1) / 400)) + 2; 
    } 
    return v;
}

double Term(int y, int n, int pd)
{
    //儒略日
    double juD = y * (365.2423112 - 6.4e-14 * (y - 100) * (y - 100) - 3.047e-8 * (y - 100)) + 15.218427 * n + 1721050.71301;
    
    //角度
    double tht = 3e-4 * y - 0.372781384 - 0.2617913325 * n;
    
    //年差实均数
    double yrD = (1.945 * sin(tht) - 0.01206 * sin(2 * tht)) * (1.048994 - 2.583e-5 * y);
    
    //朔差实均数
    double shuoD = -18e-4 * sin(2.313908653 * y - 0.439822951 - 3.0443 * n);
    
    return (pd) ? (juD + yrD + shuoD - EquivalentStandardDay(y, 1, 0) - 1721425) : (juD - EquivalentStandardDay(y, 1, 0) - 1721425);
}


double AntiDayDifference(int y, double x)
{
    int m = 1, j = 1;
    for (; j <= 12; j++)
    {
        int mL = DayDifference(y, (j+1), 1) - DayDifference(y, j, 1);
        if (x <= mL || j == 12)
        {
            m = j;
            break;
        }
        else
            x -= mL;
    }
    return 100 * m + x;
}
