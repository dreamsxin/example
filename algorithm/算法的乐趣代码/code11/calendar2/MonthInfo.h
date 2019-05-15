#pragma once

#include "DayInfo.h"

typedef struct tagMonthInfo
{
    int month;           //公历月序号
    int days;            //公历月天数
    int first_week;      //本月1号的星期
    double jieqiJD;        //本月节气的JD，本地时间
    SOLAR_TERMS jieqi;     //本月节气序号
    double zhongqiJD;      //本月中气的JD，本地时间
    SOLAR_TERMS zhongqi;   //本月中气的序号
}MONTH_INFO;

class CChineseCalendar;

class CMonthInfo
{
    friend class CChineseCalendar;
protected:
    CMonthInfo(const MONTH_INFO *info);
public:
    ~CMonthInfo(void);
    void SetMonthInfo(const MONTH_INFO *info);
    void GetMonthInfo(MONTH_INFO *info);
    const CDayInfo& GetDayInfo(int day) const;
    void AddSingleDay(CDayInfo& info);
    int GetMonthIndex() const { return m_Info.month; };
    int GetDaysCount() const { return m_Info.days; };
    int GetFirstDayWeek() const { return m_Info.first_week; };
    bool CheckValidDayCount();
    void ClearInfo();
protected:
    MONTH_INFO m_Info;

    std::vector<CDayInfo> m_DayInfo;
};
