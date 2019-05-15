#include "StdAfx.h"
#include "DayInfo.h"

CDayInfo::CDayInfo(const DAY_INFO *info)
{
    SetDayInfo(info);
}

CDayInfo::~CDayInfo(void)
{
}

void CDayInfo::SetDayInfo(const DAY_INFO *info)
{
    if(info != NULL)
    {
        memmove(&m_Info, info, sizeof(DAY_INFO));
    }
}

void CDayInfo::GetDayInfo(DAY_INFO *info)
{
    if(info != NULL)
    {
        memmove(info, &m_Info, sizeof(DAY_INFO));
    }
}

