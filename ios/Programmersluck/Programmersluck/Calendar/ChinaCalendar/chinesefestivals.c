//
//  chinesefestivals.c
//  Programmersluck
//
//  Created by cuibo on 3/15/13.
//  Copyright (c) 2013 cuibo. All rights reserved.
//

#include <stdio.h>

//根据月日声称索引
#define FestivalIndex(x, y) ((x)*100 + (y))

//中国节日
const char *festival(int month, int day)
{
    const char *nume = NULL;
    switch(FestivalIndex(month, day))
    {
        case FestivalIndex( 1,  1): nume = "元旦";    break;
        case FestivalIndex( 2, 14): nume = "情人节";   break;
        case FestivalIndex( 4,  1): nume = "愚人节";   break;
        case FestivalIndex( 5,  1): nume = "劳动节";   break;
        case FestivalIndex( 5,  4): nume = "青年节";   break;
        case FestivalIndex( 6,  1): nume = "儿童节";   break;
        case FestivalIndex( 8,  1): nume = "建军节";   break;
        case FestivalIndex( 9, 10): nume = "教师节";   break;
        case FestivalIndex(10,  1): nume = "国庆";    break;
        case FestivalIndex(12, 24): nume = "平安夜";   break;
        case FestivalIndex(12, 25): nume = "圣诞节";   break;
        default:                    nume = NULL;     break;
    }
    
    return nume;
}

//阴历节日
const char *lunarFestival(int lmonth, int lday)
{
    const char *nume = NULL;
    switch(FestivalIndex(lmonth, lday))
    {
        case FestivalIndex( 1,  1): nume = "春节";    break;
        case FestivalIndex( 1, 15): nume = "元宵";   break;
        case FestivalIndex( 5,  5): nume = "端午";   break;
        case FestivalIndex( 7,  7): nume = "七夕";   break;
        case FestivalIndex( 7, 15): nume = "中元";   break;
        case FestivalIndex( 8, 15): nume = "中秋";   break;
        case FestivalIndex( 9,  9): nume = "重阳";   break;
        case FestivalIndex(12,  8): nume = "腊八";   break;
        case FestivalIndex(12, 23): nume = "小年";    break;
        case FestivalIndex(12, 30): nume = "除夕";   break;
        default:                    nume = NULL;     break;
    }
    
    return nume;
}


