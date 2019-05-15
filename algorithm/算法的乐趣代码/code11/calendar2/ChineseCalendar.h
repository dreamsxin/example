#pragma once

#include "MonthInfo.h"
#include "ChnMonthInfo.h"

const int SPRING_BEGINS_INDEX = 3;
const int SOLAR_TERMS_CALC_COUNT = 25;
const int NEW_MOON_CALC_COUNT = 15;

class CChineseCalendar
{
public:
    CChineseCalendar(void);
    ~CChineseCalendar(void);
    CMonthInfo& GetMonthInfo(int month);
    CChnMonthInfo& GetChnMonthInfo(int mmonth);
    bool SetGeriYear(int year);
    void ClearCalendar();
    void GetSpringBeginsDay(int *month, int *day);
    int GetCurrentYear() { return m_year; };
    bool IsCalendarReady() { return m_bInitial; };

protected:
    void CalculateProcData();
    void GetNewMoonJDs(double jd, double *NewMoon);
    void GetAllSolarTermsJD(int year, int start, double *SolarTerms);
    SOLAR_TERMS GetSolarTermsName(double *solarTerms, int count, double today);
    bool BuildAllChnMonthInfo();
    void CalcLeapChnMonth();
    bool BuildMonthInfo(int month);
    CChnMonthInfo& FindChnMonthInfo(double todayJD);
    bool BuildMonthAllDaysInfo(CMonthInfo& mi);
    bool BuildAllMonthInfo();

protected:
    bool m_bInitial;
    int m_year;
    double m_SolarTermsJD[SOLAR_TERMS_CALC_COUNT];
    double m_NewMoonJD[NEW_MOON_CALC_COUNT];

    std::vector<CMonthInfo> m_MonthInfo;
    std::vector<CChnMonthInfo> m_ChnMonthInfo;
};


