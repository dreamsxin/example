#pragma once

typedef struct tagChnMonthInfo
{
    int mmonth;           //农历月序号
    int mname;            //农历月名称
    int mdays;            //本月天数
    double shuoJD;        //本月朔日的JD，本地时间
    double nextJD;        //下月朔日的JD，本地时间
    int leap;             //0 是正常月，1 是闰月
}CHN_MONTH_INFO;

class CChnMonthInfo
{
public:
    CChnMonthInfo(const CHN_MONTH_INFO *info);
    ~CChnMonthInfo(void);
    void SetChnmonthInfo(const CHN_MONTH_INFO *info);
    void GetChnmonthInfo(CHN_MONTH_INFO *info);
    void SetLeapMonth(bool leap) { m_Info.leap = leap ? 1 : 0; };
    void ReIndexMonthName() { m_Info.mname--; };
    double GetNewMoonDayJD() { return m_Info.shuoJD; };
    double GetNextNewMoonDayJD() { return m_Info.nextJD; };
    int GetMonthIndex() { return m_Info.mmonth; };
    bool IsLeapMonth() { return (m_Info.leap == 1); };
    int GetMonthDays() { return m_Info.mdays; };
    int GetMonthName() { return m_Info.mname; };
protected:
    CHN_MONTH_INFO m_Info;
};

