#ifndef __SUN_CALENDAR_H__
#define __SUN_CALENDAR_H__


/*计算太阳的地心黄经(度)，dt是儒略千年数*/
double CalcSunEclipticLongitudeEC(double dt);

/*计算太阳的地心黄纬(度)，dt是儒略千年数*/
double CalcSunEclipticLatitudeEC(double dt);

/*修正太阳的地心黄经，longitude, latitude 是修正前的日心黄经和日心黄纬(度)，dt是儒略千年数，返回值单位度*/
double AdjustSunEclipticLongitudeEC(double dt, double longitude, double latitude);

/*修正太阳的地心黄纬，longitude是修正前的太阳地心黄经(度)，dt是儒略千年数，返回值单位度*/
double AdjustSunEclipticLatitudeEC(double dt, double longitude);

/*计算*/
double CalcSunEarthRadius(double dt);

/*得到某个时间的太阳地心黄经(视黄经)，单位度，dt是儒略世纪数*/
double GetSunEclipticLongitudeEC2(double dt);

/*得到某个儒略日的太阳地心黄经(视黄经)，单位度*/
double GetSunEclipticLongitudeEC(double jde);

/*得到某个儒略日的太阳地心黄纬(视黄纬)，单位度*/
double GetSunEclipticLatitudeEC(double jde);

#endif //__SUN_CALENDAR_H__

