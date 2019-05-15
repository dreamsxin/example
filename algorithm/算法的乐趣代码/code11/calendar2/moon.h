#ifndef __MOON_CALENDAR_H__
#define __MOON_CALENDAR_H__

/*获取月亮地心黄经，单位度*/
double GetMoonEclipticLongitudeEC(double dbJD);

double GetMoonEclipticLatitudeEC(double dbJD);
double GetMoonEarthDistance(double dbJD);

#endif //__MOON_CALENDAR_H__

