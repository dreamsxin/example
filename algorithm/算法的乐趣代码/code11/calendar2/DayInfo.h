#pragma once

#include "calendar_const.h"

typedef struct tagDayInfo
{
    int dayNo;         //本月内的日序号
    int week;          //本日的星期
    int mdayNo;        //本日对应的农历月内日序号
    int mmonth;        //本日所在的农历月序号
    SOLAR_TERMS st;    //本日对应的节气，-1 表示不是节气
}DAY_INFO;

class CDayInfo
{
public:
    CDayInfo(const DAY_INFO *info);
    ~CDayInfo(void);

    void SetDayInfo(const DAY_INFO *info);
    void GetDayInfo(DAY_INFO *info);
    int GetDayDate() const { return m_Info.dayNo; };
    int GetDayChnDate() const { return m_Info.mdayNo; };
    int GetDayChnMonth() const { return m_Info.mmonth; };
    SOLAR_TERMS GetDaySolarTerm() const { return m_Info.st; };
    int GetDayWeek() const { return m_Info.week; };
protected:
    DAY_INFO m_Info;
};
