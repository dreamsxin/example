#ifndef __NUTATION_PARAM_H__
#define __NUTATION_PARAM_H__


/*计算某时刻的黄经章动干扰量，dt是儒略千年数，返回值单位是度*/
double CalcEarthLongitudeNutation(double dt);

/*计算某时刻的黄赤交角章动干扰量，dt是儒略千年数，返回值单位是度*/
double CalcEarthObliquityNutation(double dt);


#endif //__NUTATION_PARAM_H__



