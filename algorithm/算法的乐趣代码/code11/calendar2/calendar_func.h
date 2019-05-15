#ifndef __CALENDAR_FUNC_H__
#define __CALENDAR_FUNC_H__


double CalculateSolarTerms(int year, int angle);
double CalculateMoonShuoJD(double tdJD);
void CalculateStemsBranches(int year, int *stems, int *branches);


#endif //__CALENDAR_FUNC_H__
