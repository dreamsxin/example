//
//  lunardate.h
//  ChineseCalendar
//
//  Created by Tony Li on 6/5/12.
//  Copyright (c) 2012 Tony Li. All rights reserved.
//

#ifndef ChineseCalendar_lunardate_h
#define ChineseCalendar_lunardate_h

typedef struct LunarDate {
    int year;
    int month;
    int day;
    int isLeap;
} LunarDate;

LunarDate lunardate_from_solar(int year, int month, int day);

const char *lunardate_day(int lunarday);
const char *lunardate_month(int lunarmonth);
const char *lunardate_zodiac(int lunaryear);

const char *lunardate_tiangan(int lunaryear);
const char *lunardate_dizhi(int lunaryear);

#endif
