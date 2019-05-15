// ellipse.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <math.h>
#include "ellipse.h"
#include "dev_adp.h"

#define ROUND_INT(f)     (int)((f) + 0.5)


void EllipsePlot(int xc,int yc, int x, int y)
{
    SetDevicePixel(xc+x,yc+y);
    SetDevicePixel(xc-x,yc+y);
    SetDevicePixel(xc+x,yc-y);
    SetDevicePixel(xc-x,yc-y);
}

void MP_Ellipse(int xc , int yc , int a, int b)
{  
    double sqa = a * a;
    double sqb = b * b;
    
    double d = sqb + sqa * (-b + 0.25);
    int x = 0; 
    int y = b;
    EllipsePlot(xc, yc, x, y);
    while( sqb * (x + 1) < sqa * (y - 0.5))
    {     
        if (d < 0)
        {
            d += sqb * (2 * x + 3); 
        }
        else  
        {  
            d += (sqb * (2 * x + 3) + sqa * (-2 * y + 2));
            y--;	
        } 
        x++;  
        EllipsePlot(xc, yc, x, y);
    }
    d = (b * (x + 0.5)) * 2 + (a * (y - 1)) * 2 - (a * b) * 2;
    while(y > 0)
    {  
        if (d < 0) 
        { 
            d += sqb * (2 * x + 2) + sqa * (-2 * y + 3);
            x++;  
        }
        else  
        {
            d += sqa * (-2 * y + 3);  
        }
        y--; 
        EllipsePlot(xc, yc, x, y);
    }
}

/*Bresenham算法*/
void Bresenham_Ellipse(int xc , int yc , int a, int b)
{ 
    int sqa = a * a;
    int sqb = b * b;
    
    int x = 0;
    int y = b;
    int d = 2 * sqb - 2 * b * sqa + sqa;
    EllipsePlot(xc, yc, x, y);
    int P_x = ROUND_INT( (double)sqa/sqrt((double)(sqa+sqb)) );
    while(x <= P_x)
    {
        if(d < 0)
        {
            d += 2 * sqb * (2 * x + 3);
        }
        else
        {
            d += 2 * sqb * (2 * x + 3) - 4 * sqa * (y - 1);
            y--;
        }
        x++;
        EllipsePlot(xc, yc, x, y);
    }

    d = sqb * (x * x + x) + sqa * (y * y - y) - sqa * sqb;
    while(y >= 0)
    { 
        EllipsePlot(xc, yc, x, y);
        y--;
        if(d < 0)
        { 
            x++; 
            d = d - 2 * sqa * y - sqa + 2 * sqb * x + 2 * sqb; 
        }
        else
        { 
            d = d - 2 * sqa * y - sqa; 
        } 
    } 
}

void Bresenham_Ellipse2(int xc , int yc , int a, int b)
{ 
    int sqa = a * a;
    int sqb = b * b;
    
    int x = 0;
    int y = b;
    int d = 4 * (sqb - b * sqa) + sqa;
    EllipsePlot(xc, yc, x, y);
    int P_x = ROUND_INT( (double)sqa/sqrt((double)(sqa+sqb)) );
    while(x <= P_x)
    {
        if(d <= 0)
        {
            d += 4 * sqb * (2 * x + 3);
        }
        else
        {
            d += 4 * sqb * (2 * x + 3) - 8 * sqa * (y - 1);
            y--;
        }
        x++;
        EllipsePlot(xc, yc, x, y);
    }

    d = sqb * (x * x + x) + sqa * (y * y - y) - sqa * sqb;
    while(y >= 0)
    { 
        EllipsePlot(xc, yc, x, y);
        y--;
        if(d <= 0)
        { 
            x++; 
            d = d - 2 * sqa * y - sqa + 2 * sqb * x + 2 * sqb; 
        }
        else
        { 
            d = d - 2 * sqa * y - sqa; 
        } 
    } 
}

#if 0
void Bresenham_Ellipse(int xc , int yc , int a, int b)
{ 
    int sqa = a * a;
    int sqb = b * b;
    
    int P_x = ROUND_INT( (double)sqa/sqrt((double)(sqa+sqb)) );
    int P_y = ROUND_INT( (double)sqb/sqrt((double)(sqa+sqb)) );

    /* 生成第一象限内的上半部分椭圆弧 */
    int x = 0;
    int y = b;
    int d = 4 * (sqb - b * sqa) + sqa;
    EllipsePlot(xc, yc, x, y);
    while(x <= P_x)
    {
        if(d <= 0)
        {
            d += 4 * sqb * (2 * x + 3);
        }
        else
        {
            d += 4 * sqb * (2 * x + 3) - 8 * sqa * (y - 1);
            y--;
        }
        x++;
        EllipsePlot(xc, yc, x, y);
    }
    /* 生成第一象限内的下半部分椭圆弧 */
    x = a;
    y = 0;
    d = 4 * (sqa - a * sqb) + sqb;
    EllipsePlot(xc, yc, x, y);
    while(y < P_y)
    {
        if(d <= 0)
        {
            d += 4 * sqa * (2 * y + 3);
        }
        else
        {
            d += 4 * sqa * (2 * y + 3) - 8 * sqb * (x - 1);
            x--;
        }
        y++;
        EllipsePlot(xc, yc, x, y);
    }
}
#endif
