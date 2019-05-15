// equation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <math.h>

const double PRECISION = 0.000000001;

typedef double (*FunctionPtr)(double);

double binFunc(double x)
{
    return (2.0*x*x + 3.2*x - 1.8);
}

double DichotomyEquation(double a, double b, FunctionPtr f)
{
    double mid = (a + b) / 2.0;
    while((b - a) > PRECISION)
    {
        if(f(a) * f(mid) < 0.0)
        {
            b = mid;
        }
        else
        {
            a = mid;
        }
        mid = (a + b) / 2.0;
    }

    return mid;
}

double CalcDerivative(FunctionPtr f, double x)
{
    return (f(x + 0.000005) - f(x - 0.000005)) / 0.00001;
}

double NewtonRaphson(FunctionPtr f, double x0) 
{
    double x1 = x0 - f(x0) / CalcDerivative(f, x0);
    while(fabs(x1 - x0) > PRECISION)
    {
        x0 = x1;
        x1 = x0 - f(x0) / CalcDerivative(f, x0);
    }

    return x1;
}

int main(int argc, char* argv[])
{
    double s = binFunc(-0.8);
    double t = binFunc(8);
    double k = binFunc(0.44096736423671234);
    double m = binFunc(-2.040967365);
                      

    double x = DichotomyEquation(-0.8, 12.0, binFunc);
    double y = NewtonRaphson(binFunc, 8.0);
    double z = NewtonRaphson(binFunc, -8.0);
	return 0;
}

