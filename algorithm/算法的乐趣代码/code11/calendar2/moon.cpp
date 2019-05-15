#include "stdafx.h"
#include <math.h>
#include "calendar_const.h"
#include "td_utc.h"
#include "elp2000_data.h"
#include "vsop87_data.h"
#include "moon.h"
#include "support.h"
#include "nutation.h"

void GetMoonEclipticParameter(double dt, double *Lp, double *D, double *M, double *Mp, double *F, double *E)
{
    double T = dt;/*T是从J2000起算的儒略世纪数*/
    double T2 = T * T;
    double T3 = T2 * T;
    double T4 = T3 * T;

    /*月球平黄经*/
    *Lp = 218.3164591 + 481267.88134236 * T - 0.0013268 * T2 + T3/538841.0 - T4 / 65194000.0;
    *Lp = Mod360Degree(*Lp);

    /*月日距角*/
    *D = 297.8502042 + 445267.1115168 * T - 0.0016300 * T2 + T3 / 545868.0 - T4 / 113065000.0;
    *D = Mod360Degree(*D);

    /*太阳平近点角*/
    *M = 357.5291092 + 35999.0502909 * T - 0.0001536 * T2 + T3 / 24490000.0;
    *M = Mod360Degree(*M);

    /*月亮平近点角*/
    *Mp = 134.9634114 + 477198.8676313 * T + 0.0089970 * T2 + T3 / 69699.0 - T4 / 14712000.0;
    *Mp = Mod360Degree(*Mp);

    /*月球经度参数(到升交点的平角距离)*/
    *F = 93.2720993 + 483202.0175273 * T - 0.0034029 * T2 - T3 / 3526000.0 + T4 / 863310000.0;
    *F = Mod360Degree(*F);

    *E = 1 - 0.002516 * T - 0.0000074 * T2;
}

/*计算月球地心黄经周期项的和*/
double CalcMoonECLongitudePeriodic(double D, double M, double Mp, double F, double E)
{
    double EI = 0.0 ;

    for(int i = 0; i < COUNT_OF(Moon_longitude); i++)
    {
        double sita = Moon_longitude[i].D * D + Moon_longitude[i].M * M + Moon_longitude[i].Mp * Mp + Moon_longitude[i].F * F;
        sita = DegreeToRadian(sita);
        EI += (Moon_longitude[i].eiA * sin(sita) * pow(E, fabs(Moon_longitude[i].M)));
    }

    return EI;
}

/*计算月球地心黄纬周期项的和*/
double CalcMoonECLatitudePeriodicTbl(double D, double M, double Mp, double F, double E)
{
    double EB = 0.0 ;

    for(int i = 0; i < COUNT_OF(moon_Latitude); i++)
    {
        double sita = moon_Latitude[i].D * D + moon_Latitude[i].M * M + moon_Latitude[i].Mp * Mp + moon_Latitude[i].F * F;
        sita = DegreeToRadian(sita);
        EB += (moon_Latitude[i].eiA * sin(sita) * pow(E, fabs(Moon_longitude[i].M)));
    }

    return EB;
}

/*计算月球地心距离周期项的和*/
double CalcMoonECDistancePeriodicTbl(double D, double M, double Mp, double F, double E)
{
    double ER = 0.0 ;

    for(int i = 0; i < COUNT_OF(Moon_longitude); i++)
    {
        double sita = Moon_longitude[i].D * D + Moon_longitude[i].M * M + Moon_longitude[i].Mp * Mp + Moon_longitude[i].F * F;
        sita = DegreeToRadian(sita);
        ER += (Moon_longitude[i].erA * cos(sita) * pow(E, fabs(Moon_longitude[i].M)));
    }

    return ER;
}

/*计算金星摄动,木星摄动以及地球扁率摄动对月球地心黄经的影响,dt 是儒略世纪数，Lp和F单位是度*/
double CalcMoonLongitudePerturbation(double dt, double Lp, double F)
{
    double T = dt; /*T是从J2000起算的儒略世纪数*/
    double A1 = 119.75 + 131.849 * T;
    double A2 = 53.09 + 479264.290 * T;

    A1 = Mod360Degree(A1);
    A2 = Mod360Degree(A2);

    double result = 3958.0 * sin(DegreeToRadian(A1));
    result += (1962.0 * sin(DegreeToRadian(Lp - F)));
    result += (318.0 * sin(DegreeToRadian(A2)));

    return result;
}

/*计算金星摄动,木星摄动以及地球扁率摄动对月球地心黄纬的影响,dt 是儒略世纪数，Lp、Mp和F单位是度*/
double CalcMoonLatitudePerturbation(double dt, double Lp, double F, double Mp)
{
    double T = dt; /*T是从J2000起算的儒略世纪数*/
    double A1 = 119.75 + 131.849 * T;
    double A3 = 313.45 + 481266.484 * T;

    A1 = Mod360Degree(A1);
    A3 = Mod360Degree(A3);

    double result = -2235.0 * sin(DegreeToRadian(Lp));
    result += (382.0 * sin(DegreeToRadian(A3)));
    result += (175.0 * sin(DegreeToRadian(A1 - F)));
    result += (175.0 * sin(DegreeToRadian(A1 + F)));
    result += (127.0 * sin(DegreeToRadian(Lp - Mp)));
    result += (115.0 * sin(DegreeToRadian(Lp + Mp)));

    return result;
}

double GetMoonEclipticLongitudeEC(double dbJD)
{
    double Lp,D,M,Mp,F,E;
    double dt = (dbJD - JD2000) / 36525.0; /*儒略世纪数*/
    
    GetMoonEclipticParameter(dt, &Lp, &D, &M, &Mp, &F, &E);

    /*计算月球地心黄经周期项*/
    double EI = CalcMoonECLongitudePeriodic(D, M, Mp, F, E);
    
    /*修正金星,木星以及地球扁率摄动*/
    EI += CalcMoonLongitudePerturbation(dt, Lp, F);

    /*计算月球地心黄经*/
    double longitude = Lp + EI / 1000000.0;

	/*计算天体章动干扰*/
    longitude += CalcEarthLongitudeNutation(dt / 10.0);

    longitude = Mod360Degree(longitude); /*映射到0-360范围内*/
    return longitude;
}

double GetMoonEclipticLatitudeEC(double dbJD)
{
    double dt = (dbJD - JD2000) / 36525.0; /*儒略世纪数*/
    
    double Lp,D,M,Mp,F,E;
    GetMoonEclipticParameter(dt, &Lp, &D, &M, &Mp, &F, &E);

    /*计算月球地心黄纬周期项*/
    double EB = CalcMoonECLatitudePeriodicTbl(D, M, Mp, F, E);
    
    /*修正金星,木星以及地球扁率摄动*/
    EB += CalcMoonLatitudePerturbation(dt, Lp, F, Mp);

    /*计算月球地心黄纬*/
    double latitude = EB / 1000000.0;

    return latitude;
}

double GetMoonEarthDistance(double dbJD)
{
    double dt = (dbJD - JD2000) / 36525.0; /*儒略世纪数*/
    
    double Lp,D,M,Mp,F,E;
    GetMoonEclipticParameter(dt, &Lp, &D, &M, &Mp, &F, &E);

    /*计算月球地心距离周期项*/
    double ER = CalcMoonECDistancePeriodicTbl(D, M, Mp, F, E);

    /*计算月球地心黄纬*/
    double distance = 385000.56 + ER / 1000.0;

    return distance;
}

