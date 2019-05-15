#ifndef __CALENDAR_CONST_H__
#define __CALENDAR_CONST_H__

const int MONTHES_FOR_YEAR = 12;
const int DAYS_FOR_WEEK = 7;

const int DAYS_OF_LEAP_YEAR = 366;
const int DAYS_OF_NORMAL_YEAR = 365;

const int SOLAR_TERMS_COUNT = 24;

const int HEAVENLY_STEMS = 10;
const int EARTHLY_BRANCHES = 12;
const int CHINESE_SHENGXIAO = 12;
const int MAX_GREGORIAN_MONTH_DAYS = 31;

const int MAX_CHINESE_MONTH_DAYS = 30;
const int CHINESE_L_MONTH_DAYS = 30;
const int CHINESE_S_MONTH_DAYS = 29;
const int MAX_CHINESE_MONTH_FOR_YEAR = 13;

const double JD2000 = 2451545.0;
// 圆周率
const double PI = 3.1415926535897932384626433832795;

//每弧度的角秒数
const double ARC_SEC_PER_RADIAN = 180.0 * 3600.0 / PI;
// 一度代表的弧度
const double RADIAN_PER_ANGLE = PI / 180.0;

const double SUN_EL_V = 360.0 / 365.2422;
//const double MOON_EL_V = 360.0 / 29.53058886;
const double MOON_EL_V = 360.0 / 27.32;

typedef int SOLAR_TERMS;

// 节气定义
const int VERNAL_EQUINOX      = 0;    // 春分
const int CLEAR_AND_BRIGHT    = 1;    // 清明
const int GRAIN_RAIN          = 2;    // 谷雨
const int SUMMER_BEGINS       = 3;    // 立夏
const int GRAIN_BUDS          = 4;    // 小满
const int GRAIN_IN_EAR        = 5;    // 芒种
const int SUMMER_SOLSTICE     = 6;    // 夏至
const int SLIGHT_HEAT         = 7;    // 小暑
const int GREAT_HEAT          = 8;    // 大暑
const int AUTUMN_BEGINS       = 9;    // 立秋
const int STOPPING_THE_HEAT   = 10;   // 处暑
const int WHITE_DEWS          = 11;   // 白露
const int AUTUMN_EQUINOX      = 12;   // 秋分
const int COLD_DEWS           = 13;   // 寒露
const int HOAR_FROST_FALLS    = 14;   // 霜降
const int WINTER_BEGINS       = 15;   // 立冬
const int LIGHT_SNOW          = 16;   // 小雪
const int HEAVY_SNOW          = 17;   // 大雪
const int WINTER_SOLSTICE     = 18;   // 冬至
const int SLIGHT_COLD         = 19;   // 小寒
const int GREAT_COLD          = 20;   // 大寒
const int SPRING_BEGINS       = 21;   // 立春
const int THE_RAINS           = 22;   // 雨水
const int INSECTS_AWAKEN      = 23;   // 惊蛰


#define COUNT_ITEM(x)   (sizeof(x) / sizeof(x[0]))

#endif //__CALENDAR_CONST_H__

