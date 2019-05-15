#ifndef __ELP2000_DATA_H__
#define __ELP2000_DATA_H__



typedef struct
{
    double D;
    double M;
    double Mp;
    double F;
    double eiA;
    double erA;
}MOON_ECLIPTIC_LONGITUDE_COEFF;

typedef struct
{
    double D;
    double M;
    double Mp;
    double F;
    double eiA;
}MOON_ECLIPTIC_LATITUDE_COEFF;

extern const MOON_ECLIPTIC_LONGITUDE_COEFF Moon_longitude[60];
extern const MOON_ECLIPTIC_LATITUDE_COEFF moon_Latitude[60];



#endif //__ELP2000_DATA_H__

