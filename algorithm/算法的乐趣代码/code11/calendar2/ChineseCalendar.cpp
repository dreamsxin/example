#include "StdAfx.h"
#include "calendar_const.h"
#include "calendar_func.h"
#include "ChineseCalendar.h"
#include "support.h"

CChineseCalendar::CChineseCalendar(void)
{
    memset(m_SolarTermsJD, 0, sizeof(double) * SOLAR_TERMS_CALC_COUNT);
    memset(m_NewMoonJD, 0, sizeof(double) * NEW_MOON_CALC_COUNT);
    m_bInitial = false;
}

CChineseCalendar::~CChineseCalendar(void)
{
}

bool CChineseCalendar::SetGeriYear(int year)
{
    ClearCalendar();
    m_year = year;
    CalculateProcData();
    if(!BuildAllChnMonthInfo())
        return false;
    CalcLeapChnMonth();
    m_bInitial = BuildAllMonthInfo();

    return m_bInitial;
}

void CChineseCalendar::ClearCalendar()
{
    for(std::vector<CMonthInfo>::iterator mi = m_MonthInfo.begin(); mi != m_MonthInfo.end(); ++mi)
    {
        mi->ClearInfo();
    }
    m_MonthInfo.erase(m_MonthInfo.begin(), m_MonthInfo.end());
    m_ChnMonthInfo.erase(m_ChnMonthInfo.begin(), m_ChnMonthInfo.end());
    
    memset(m_SolarTermsJD, 0, sizeof(double) * SOLAR_TERMS_CALC_COUNT);
    memset(m_NewMoonJD, 0, sizeof(double) * NEW_MOON_CALC_COUNT);
    m_bInitial = false;
}

void CChineseCalendar::GetSpringBeginsDay(int *month, int *day)
{
    if(!m_bInitial)
        return;

    if((month != NULL) && (day != NULL))
    {
        WZDAYTIME dt;
        GetDayTimeFromJulianDay(m_SolarTermsJD[SPRING_BEGINS_INDEX], &dt);
        *month = dt.month;
        *day = dt.day;
    }
}

CMonthInfo& CChineseCalendar::GetMonthInfo(int month)
{
    assert(m_MonthInfo.size() == MONTHES_FOR_YEAR);

    if((month < 1) || (month > MONTHES_FOR_YEAR))
    {
        return m_MonthInfo[0]; // 出错总是返回第一个月的数据
    }

    return m_MonthInfo[month - 1];
}

CChnMonthInfo& CChineseCalendar::GetChnMonthInfo(int mmonth)
{
    assert(m_ChnMonthInfo.size() > 0);

    if((mmonth < 0) || (mmonth >= (int)m_ChnMonthInfo.size()))
    {
        return m_ChnMonthInfo[0]; // 出错总是返回第一个月的数据
    }

    return m_ChnMonthInfo[mmonth];
}
/*
void CChineseCalendar::CalculateProcData()
{
    //计算从上一年冬至开始到今天冬至全部25个节气
    GetAllSolarTermsJD(m_year - 1, WINTER_SOLSTICE, m_SolarTermsJD);
    
    double lastDongZhi = JDLocalTimetoTD(m_SolarTermsJD[0]);
    double tmpShuo = CalculateMoonShuoJD(lastDongZhi); //求与冬至最近的一个朔日
    tmpShuo = JDTDtoLocalTime(tmpShuo);
    if(tmpShuo > m_SolarTermsJD[0])
        tmpShuo -= 29.53;

    GetNewMoonJDs(tmpShuo, m_NewMoonJD);
    
    char buf[64];
    char bufAll[128];
    for(int i = 0; i < NEW_MOON_CALC_COUNT; i++)
    {
        JDtoString(m_NewMoonJD[i], buf, 64);
        sprintf(bufAll, "%f, %s\n", m_NewMoonJD[i], buf);
        TRACE(bufAll);
    }
}
*/

void CChineseCalendar::CalculateProcData()
{
    //计算从上一年冬至开始到今天冬至全部25个节气
    GetAllSolarTermsJD(m_year - 1, WINTER_SOLSTICE, m_SolarTermsJD);
    
    double lastDongZhi = JDLocalTimetoTD(m_SolarTermsJD[0]);
    double tmpShuo = CalculateMoonShuoJD(lastDongZhi); //求与冬至最近的一个朔日
    tmpShuo = JDTDtoLocalTime(tmpShuo);
    if(tmpShuo > m_SolarTermsJD[0])/*这里原来的代码是if(tmpShuo > lastDongZhi)，有问题*/
        tmpShuo -= 29.53;

    GetNewMoonJDs(tmpShuo, m_NewMoonJD);
}
/*
计算从某个时间之后的连续13个朔日，jd是开始时间的儒略日，本地时间
NewMoon中存放jd之后的13个朔日时间，单位是儒略日，本地时间
*/
void CChineseCalendar::GetNewMoonJDs(double jd, double *NewMoon)
{
    double tdjd = JDLocalTimetoTD(jd);

    for(int i = 0; i < NEW_MOON_CALC_COUNT; i++)
    {
        double shuoJD = CalculateMoonShuoJD(tdjd);
        shuoJD = JDTDtoLocalTime(shuoJD);
        NewMoon[i] = shuoJD;

        tdjd += 29.5; /*转到下一个最接近朔日的时间*/
    }
}

/*从某一年的某个节气开始，连续计算25个节气，返回各节气的儒略日，本地时间*/
void CChineseCalendar::GetAllSolarTermsJD(int year, int start, double *SolarTerms)
{
    int i = 0;
    int st = start;
    while(i < 25)
    {
        double jd = CalculateSolarTerms(year, st * 15);
        SolarTerms[i++] = JDTDtoLocalTime(jd);
        if(st == WINTER_SOLSTICE)
        {
            year++;
        }
        st = (st + 1) % SOLAR_TERMS_COUNT;
    }
}

SOLAR_TERMS CChineseCalendar::GetSolarTermsName(double *solarTerms, int count, double today)
{
    int i = 0;
    while(i < 25)
    {
        if(int(solarTerms[i] + 0.5) == int(today + 0.5))
        {
            return (i + 18) % SOLAR_TERMS_COUNT;
        }
        i++;
    }

    return -1;
}

bool CChineseCalendar::BuildAllChnMonthInfo()
{
    CHN_MONTH_INFO info; //一年最多可13个农历月
    int i;
    int yuejian = 11;   //采用夏历建寅，冬至所在月份为农历11月
    for(i = 0; i < (NEW_MOON_CALC_COUNT - 1); i++)
    {
        info.mmonth = i;
        info.mname = (yuejian <= 12) ? yuejian : yuejian - 12;
        info.shuoJD = m_NewMoonJD[i];
        info.nextJD = m_NewMoonJD[i + 1];
        info.mdays = int(info.nextJD + 0.5) - int(info.shuoJD + 0.5);
        info.leap = 0;

        CChnMonthInfo cm(&info);
        m_ChnMonthInfo.push_back(cm);

        yuejian++;
    }

    return (m_ChnMonthInfo.size() == (NEW_MOON_CALC_COUNT - 1));
}

/*根据节气计算是否有闰月，如果有闰月，根据农历月命名规则，调整月名称*/
void CChineseCalendar::CalcLeapChnMonth()
{
    assert(m_ChnMonthInfo.size() > 0); /*阴历月的初始化必须在这个之前*/

    int i;
    /*这里原来的代码是if(int(m_NewMoonJD[13] + 0.5) < int(m_SolarTermsJD[24] + 0.5))，有问题*/
    if(int(m_NewMoonJD[13] + 0.5) <= int(m_SolarTermsJD[24] + 0.5)) //第13月的月末没有超过冬至，说明今年需要闰一个月
    {
        //找到第一个没有中气的月
        i = 1;
        while(i < (NEW_MOON_CALC_COUNT - 1))
        {
            /*这块原来的代码是if(m_NewMoonJD[i + 1] < m_SolarTermsJD[2 * i])，有问题*/
            /*m_NewMoonJD[i + 1]是第i个农历月的下一个月的月首，本该属于第i个月的中气如果比下一个月
              的月首还晚，或者与下个月的月首是同一天（民间历法），则说明第i个月没有中气*/
            if(int(m_NewMoonJD[i + 1] + 0.5) <= int(m_SolarTermsJD[2 * i] + 0.5))
                break;
            i++;
        }
        if(i < (NEW_MOON_CALC_COUNT - 1)) /*找到闰月，对后面的农历月调整月名*/
        {
            m_ChnMonthInfo[i].SetLeapMonth(true);
            while(i < (NEW_MOON_CALC_COUNT - 1))
            {
                m_ChnMonthInfo[i++].ReIndexMonthName();
            }
        }
    }
}

/*找到today所在的阴历月*/
CChnMonthInfo& CChineseCalendar::FindChnMonthInfo(double todayJD)
{
    assert(m_ChnMonthInfo.size() > 0);

    int this_day = int(todayJD + 0.5);

    int k = 0; 
    while(k < (int)m_ChnMonthInfo.size())
    {
        int last_day = int(m_ChnMonthInfo[k].GetNewMoonDayJD() + 0.5);
        int next_day = int(m_ChnMonthInfo[k].GetNextNewMoonDayJD() + 0.5);
        
        if((this_day >= last_day) && (this_day < next_day))
        {
            return m_ChnMonthInfo[k];
        }
        k++;
    }

    return m_ChnMonthInfo[0]; /*异常出现时，总是返回第一个月信息*/
}

bool CChineseCalendar::BuildMonthAllDaysInfo(CMonthInfo& mi)
{
    DAY_INFO info;
    int j = 0;
    for(j = 0; j < mi.GetDaysCount(); j++)
    {
        double today = CalculateJulianDay(m_year, mi.GetMonthIndex(), j + 1, 0, 0, 1);
        info.dayNo = j + 1;
        info.week = (mi.GetFirstDayWeek() + j) % DAYS_FOR_WEEK;
        CChnMonthInfo& cm = FindChnMonthInfo(today);
        info.mmonth = cm.GetMonthIndex();
        info.mdayNo = int(today + 0.5) - int(cm.GetNewMoonDayJD() + 0.5);
        info.st = GetSolarTermsName(m_SolarTermsJD, 25, today);

        CDayInfo di(&info);
        mi.AddSingleDay(di);
    }

    return mi.CheckValidDayCount();
}

bool CChineseCalendar::BuildMonthInfo(int month)
{
    MONTH_INFO info;
    info.month = month;
    info.first_week = ZellerWeek(m_year, info.month, 1);
    info.days = GetDaysOfMonth(m_year, info.month);
    CMonthInfo mi(&info);
    
    if(BuildMonthAllDaysInfo(mi))
    {
        m_MonthInfo.push_back(mi);

        return true;
    }

    return false;
}

bool CChineseCalendar::BuildAllMonthInfo()
{
    assert(m_ChnMonthInfo.size() > 0); /*阴历月的初始化必须在这个之前*/

    for(int i = 0; i < MONTHES_FOR_YEAR; i++)
    {
        if(!BuildMonthInfo(i + 1))
        {
            return false;
        }
    }

    return true;
}

