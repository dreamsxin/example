// circle.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <math.h>
#include "circle.h"
#include "dev_adp.h"

#define ROUND_INT(f)     (int)((f) + 0.5)


void CirclePlot(int xc,int yc, int x, int y)
{
    SetDevicePixel(xc+x,yc+y);
    SetDevicePixel(xc-x,yc+y);
    SetDevicePixel(xc+x,yc-y);
    SetDevicePixel(xc-x,yc-y);
    SetDevicePixel(xc+y,yc+x);
    SetDevicePixel(xc-y,yc+x);
    SetDevicePixel(xc+y,yc-x);
    SetDevicePixel(xc-y,yc-x);
}



void MP_Circle(int xc , int yc , int r)
{ 
    int x, y;
    double d;

    x = 0;
    y = r; 
    d = 1.25 - r;
    CirclePlot(xc , yc , x , y);
    while(x < y)
    {
        if(d < 0)
        {
            d = d + 2 * x + 3;
        }
        else
        {
            d = d + 2 * ( x - y ) + 5;
            y--;
        }
        x++;
        CirclePlot(xc , yc , x , y);
    }
}

/*改进的中点生成算法，Bresenham算法*/
void Bresenham_Circle(int xc , int yc , int r)
{ 
    int x, y, d;

    x = 0;
    y = r; 
    d = 3 - 2 * r;
    CirclePlot(xc , yc , x , y);
    while(x < y)
    {
        if(d < 0)
        {
            d = d + 4 * x + 6;
        }
        else
        {
            d = d + 4 * ( x - y ) + 10;
            y--;
        }
        x++;
        CirclePlot(xc , yc , x , y);
    }
}

#if 0
/*正宗的Bresenham算法*/
void Bresenham_Circle(int xc , int yc , int r)
{ 
    int x, y, d;

    x = 0;
    y = r; 
    d = 3 - 2 * r;
    
    while(x < y)
    {
        CirclePlot(xc , yc , x , y);
        if(d < 0)
        {
            d = d + 4 * x + 6;
        }
        else
        {
            d = d + 4 * ( x - y ) + 10;
            y--;
        }
        x++;
    }
    if(x == y)
        CirclePlot(xc , yc , x , y);
}
#endif
/*正负法生成圆*/
void Pnar_Circle(int xc, int yc, int r)
{
    int x, y, f;

    x = 0;
    y = r; 
    f = 0;
    while(x <= y)
    {
        CirclePlot(xc, yc, x, y);
        if(f <= 0)
        {
            f = f + 2 * x + 1;
            x++;
        }
        else
        {
            f = f - 2 * y + 1;
            y--;
        }
    }
}

void Fast_Circle(int xc , int yc , int r)
{
    int x, y, d;

    x = 0;
    y = r;
    d = -r / 2;
    CirclePlot(xc , yc , x , y);
    if(r % 2 == 0)
    {
        while(x < y)
        {
            x++;
            if(d < 0)
                d += x;
            else
            {
                y--;
                d += x - y;
            }

            CirclePlot(xc , yc , x , y);
        }
    }
    else
    {
        while(x < y)
        {
            x++;
            if(d < 0)
              d += x + 1;
            else
            {
              y--;
              d += x - y + 1;
            }

            CirclePlot(xc , yc , x , y);
        }
    }
}