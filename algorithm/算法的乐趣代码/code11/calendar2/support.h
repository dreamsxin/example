#ifndef __CALENDAR_SUPPORT_H__
#define __CALENDAR_SUPPORT_H__

#define COUNT_OF(element)  (sizeof(element) / sizeof(element[0]))

typedef struct tagWzDayTime
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    double second;
}WZDAYTIME;

bool IsLeapYear(int year);
bool IsGregorianDays(int year, int month, int day);
int GetDaysOfMonth(int year, int month);
int CalcYearRestDays(int year, int month, int day);
int CalcYearPassedDays(int year, int month, int day);
int CalcYearsDays(int years, int yeare);

int CalculateDays(int ys, int ms, int ds, int ye, int me, int de);
int ZellerWeek(int year, int month, int day);
int TotalWeek(int year, int month, int day);

double CalculateJulianDay(int year, int month, int day, int hour, int minute, double second);
double CalculateJulianDay2(WZDAYTIME *pDT);
void GetDayTimeFromJulianDay(double jd, WZDAYTIME *pDT);

void UTCToLocalTime(int &year, int &month, int &day, int &hour, int &minute, double &second);
void LocalTimetoUTC(int &year, int &month, int &day, int &hour, int &minute, double &second);

double JDUTCToLocalTime(double utcJD);
double JDLocalTimetoUTC(double localJD);

double JDUTCtoTD(double utcJD);
double JDTDtoUTC(double tdJD);

double JDLocalTimetoTD(double localJD);
double JDTDtoLocalTime(double tdJD);

double RadianToDegree(double radian);
double DegreeToRadian(double degree);

// 调整角度到 0-360 之间
double Mod360Degree(double degrees);

int JDtoString(double jd, char *strBuf, int bufSize);

#endif //__CALENDAR_SUPPORT_H__



