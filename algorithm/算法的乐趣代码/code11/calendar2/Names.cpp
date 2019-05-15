#include "stdafx.h"
#include "Names.h"

TCHAR *nameOfMonth[MONTHES_FOR_YEAR] = { _T("January"), _T("February"), _T("March"), _T("April"), _T("May"), 
                                        _T("June"), _T("July"), _T("August"), _T("September"), _T("October"), 
                                        _T("November"), _T("December") };
TCHAR *nameOfWeek[DAYS_FOR_WEEK] = { _T("星期日"),_T("星期一"),_T("星期二"),_T("星期三"),_T("星期四"),_T("星期五"),_T("星期六") };

TCHAR *nameOfStems[HEAVENLY_STEMS] = { _T("甲"),_T("乙"),_T("丙"),_T("丁"),_T("戊"),_T("己"),_T("庚"),_T("辛"),_T("壬"),_T("癸") };
TCHAR *nameOfBranches[EARTHLY_BRANCHES] = { _T("子"),_T("丑"),_T("寅"),_T("卯"),_T("辰"),_T("巳"),_T("午"),_T("未"),_T("申"),_T("酉"),_T("戌"),_T("亥") };
TCHAR *nameOfShengXiao[CHINESE_SHENGXIAO] = { _T("鼠"),_T("牛"),_T("虎"),_T("兔"),_T("龙"),_T("蛇"),_T("马"),_T("羊"),_T("猴"),_T("鸡"),_T("狗"),_T("猪") };
TCHAR *nameOfChnDay[MAX_CHINESE_MONTH_DAYS] = { _T("初一"),_T("初二"),_T("初三"),_T("初四"),_T("初五"),_T("初六"),_T("初七"),_T("初八"),_T("初九"),_T("初十"),
                                              _T("十一"),_T("十二"),_T("十三"),_T("十四"),_T("十五"),_T("十六"),_T("十七"),_T("十八"),_T("十九"),_T("二十"),
                                              _T("廿一"),_T("廿二"),_T("廿三"),_T("廿四"),_T("廿五"),_T("廿六"),_T("廿七"),_T("廿八"),_T("廿九"),_T("三十")
                                            };
TCHAR *nameOfChnMonth[MONTHES_FOR_YEAR] = { _T("正"),_T("二"),_T("三"),_T("四"),_T("五"),_T("六"),_T("七"),_T("八"),_T("九"),_T("十"),_T("十一"),_T("腊") };
TCHAR *nameOfJieQi[SOLAR_TERMS_COUNT] = { _T("春分"), _T("清明"), _T("谷雨"), _T("立夏"), _T("小满"), _T("芒种"), _T("夏至"), _T("小暑"), 
                                        _T("大暑"), _T("立秋"), _T("处暑"), _T("白露"), _T("秋分"), _T("寒露"), _T("霜降"), _T("立冬"), 
                                        _T("小雪"), _T("大雪"), _T("冬至"), _T("小寒"), _T("大寒"), _T("立春"), _T("雨水"), _T("惊蛰") };

