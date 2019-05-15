#include "StdAfx.h"
#include "calendar_const.h"
#include "MonthInfo.h"

CMonthInfo::CMonthInfo(const MONTH_INFO *info)
{
    SetMonthInfo(info);
}

CMonthInfo::~CMonthInfo(void)
{
}

void CMonthInfo::SetMonthInfo(const MONTH_INFO *info)
{
    if(info != NULL)
    {
        memmove(&m_Info, info, sizeof(MONTH_INFO));
    }
}

void CMonthInfo::GetMonthInfo(MONTH_INFO *info)
{
    if(info != NULL)
    {
        memmove(info, &m_Info, sizeof(MONTH_INFO));
    }
}

const CDayInfo& CMonthInfo::GetDayInfo(int day) const
{
    assert((m_DayInfo.size() > 0) && (m_DayInfo.size() <= MAX_GREGORIAN_MONTH_DAYS));

    if((day < 1) || (day > (int)m_DayInfo.size()))
    {
        return m_DayInfo[0]; // 出错总是返回第一天的数据
    }

    return m_DayInfo[day - 1];
}

void CMonthInfo::AddSingleDay(CDayInfo& info)
{
    m_DayInfo.push_back(info);
}

bool CMonthInfo::CheckValidDayCount()
{
    return (m_Info.days == m_DayInfo.size());
}

void CMonthInfo::ClearInfo()
{
    m_DayInfo.erase(m_DayInfo.begin(), m_DayInfo.end());
}
