#include "StdAfx.h"
#include "calendar_const.h"
#include "ChnMonthInfo.h"

CChnMonthInfo::CChnMonthInfo(const CHN_MONTH_INFO *info)
{
    SetChnmonthInfo(info);
}

CChnMonthInfo::~CChnMonthInfo(void)
{
}

void CChnMonthInfo::SetChnmonthInfo(const CHN_MONTH_INFO *info)
{
    if(info != NULL)
    {
        memmove(&m_Info, info, sizeof(CHN_MONTH_INFO));
    }
}

void CChnMonthInfo::GetChnmonthInfo(CHN_MONTH_INFO *info)
{
    if(info != NULL)
    {
        memmove(info, &m_Info, sizeof(CHN_MONTH_INFO));
    }
}

