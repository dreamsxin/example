// line.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <math.h>
#include "line.h"
#include "dev_adp.h"


#define ROUND_INT(f)     (int)((f) + 0.5)

void DDA_Line(int x1, int y1, int x2, int y2)
{
    double k,dx,dy,x,y,xend,yend;

    dx = x2 - x1;
    dy = y2 - y1;
    if(fabs(dx) >= fabs(dy)) 
    {
        k = dy / dx;
        if(dx > 0)
        {
            x = x1;
            y = y1;
            xend = x2;
        }
        else
        {
            x = x2;
            y = y2;
            xend = x1;
        }
        while(x <= xend) 
        {     
            SetDevicePixel((int)x, ROUND_INT(y)); 
            y = y + k; 
            x = x + 1; 
        }

    }
    else
    {
        k = dx / dy; 
        if(dy > 0)
        {
            x = x1;
            y = y1;
            yend = y2;
        }
        else
        {
            x = x2;
            y = y2;
            yend = y1;
        }
        while(y <= yend) 
        {     
            SetDevicePixel(ROUND_INT(x), (int)y); 
            x = x + k; 
            y = y + 1; 
        } 
    }
}

void Bresenham_Line(int x1, int y1, int x2, int y2)
{
    int   x;   
    int   y;   
    int   p;   
    int   const1;   
    int   const2;   
    int   inc;   
    int   tmp;   

    int dx = x2 - x1;   
    int dy = y2 - y1;   
    
    if   (dx*dy >= 0)   /*准备x或y的单位递变值。*/   
        inc = 1;   
    else   
        inc = -1;   
    
    if(abs(dx)>abs(dy))
    {   
        if(dx<0)
        {   
            tmp=x1;   /*将2a,   3a象限方向*/   
            x1=x2;   /*的直线变换到1a,   4a*/   
            x2=tmp;   
            tmp=y1;   /*象限方向去*/   
            y1=y2;
            y2 = tmp;
            dx=-dx;   
            dy=-dy;   
        }   
        if(inc == -1)
            dy = -dy;
    
        p = 2 * dy - dx;   
        const1 = 2 * dy;   /*注意此时误差的*/   
        const2 = 2 * (dy - dx);   /*变化参数取值.   */   
        x=x1;   
        y=y1;   
        SetDevicePixel(x, y);   
    
        while(x < x2)
        {   
            x++;   
            if(p<0)   
                p += const1;   
            else
            {   
                y += inc;   
                p += const2;   
            }   

            SetDevicePixel(x, y);
        }   
    }   
    else
    {   
        if(dy < 0)
        {   
            tmp=x1;   /*将2a,   3a象限方向*/   
            x1=x2;   /*的直线变换到1a,   4a*/   
            x2=tmp;   
            tmp=y1;   
            y1=y2; 
            y2 = tmp;
            dx=-dx;   
            dy=-dy;   
        }
        if(inc == -1)
            dx = -dx;
        p=2*dx-dy;   /*注意此时误差的*/   
        const1=2*dx;   /*变化参数取值.   */   
        const2=2*(dx-dy);   
        x=x1;   
        y=y1;   
        SetDevicePixel(x, y);
    
        while(y < y2)
        {   
            y++;   
            if(p<0)   
                p+=const1;   
            else
            {   
                x+=inc;   
                p+=const2; 
            }
            SetDevicePixel(x, y);
        }   
    }   
}

void SwapInt(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void Bresenham_Line2(int x1, int y1, int x2, int y2)
{
    int dx,dy,p,const1,const2,x,y,inc;

    int steep = (abs(y2 - y1) > abs(x2 - x1)) ? 1 : 0;
    if(steep == 1)
    {
        SwapInt(&x1, &y1);
        SwapInt(&x2, &y2);
    }
    if(x1 > x2)
    {
        SwapInt(&x1, &x2);
        SwapInt(&y1, &y2);
    }
    dx = abs(x2 - x1);   
    dy = abs(y2 - y1);   
    p = 2 * dy - dx;   
    const1 = 2 * dy;  
    const2 = 2 * (dy - dx); 
    x = x1;   
    y = y1;

    inc = (y1 < y2) ? 1 : -1;
    while(x <= x2)
    {   
        if(steep == 1)
            SetDevicePixel(y, x);   
        else
            SetDevicePixel(x, y);   
        x++;   
        if(p<0)   
            p += const1;   
        else
        {   
            p += const2;   
            y += inc;   
        }   
    }   
}

void Sym_Bresenham_Line(int x1, int y1, int x2, int y2)
{
    int dx,dy,p,const1,const2,xs,ys,xe,ye,half,inc;

    int steep = (abs(y2 - y1) > abs(x2 - x1)) ? 1 : 0;
    if(steep == 1)
    {
        SwapInt(&x1, &y1);
        SwapInt(&x2, &y2);
    }
    if(x1 > x2)
    {
        SwapInt(&x1, &x2);
        SwapInt(&y1, &y2);
    }
    dx = x2 - x1;   
    dy = abs(y2 - y1);   
    p = 2 * dy - dx;   
    const1 = 2 * dy;  
    const2 = 2 * (dy - dx); 
    xs = x1;   
    ys = y1;
    xe = x2;
    ye = y2;
    half = (dx + 1) / 2;
    inc = (y1 < y2) ? 1 : -1;
    while(xs <= half)
    {   
        if(steep == 1)
        {
            SetDevicePixel(ys, xs);
            SetDevicePixel(ye, xe);
        }
        else
        {
            SetDevicePixel(xs, ys);
            SetDevicePixel(xe, ye);
        }
        xs++;  
        xe--;
        if(p<0)   
            p += const1;   
        else
        {   
            p += const2;   
            ys += inc;   
            ye -= inc;   
        }   
    }   
}


void Double_Step_Line(int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int e = dy * 4 - dx;
    int x = x1; 
    int y = y1;

    SetDevicePixel(x, y);

    while(x < x2)
    {   
        if (e > dx)
        {
            if (e > ( 2 * dx))
            {
                e += 4 * (dy - dx);
                x++;
                y++;
                SetDevicePixel(x, y);
                x++;
                y++;
                SetDevicePixel(x, y);
            }
            else 
            {
                e += (4 *dy - 2 * dx);
                x++;
                y++;
                SetDevicePixel(x, y);
                x++;
                SetDevicePixel(x, y);
            }
        }
        else 
        {
            if (e > 0)
            {
                e += (4 * dy - 2 * dx);
                x++;
                SetDevicePixel(x, y);
                x++;
                y++;
                SetDevicePixel(x, y);
            }
            else
            {
                x++;
                SetDevicePixel(x, y);
                x++;
                SetDevicePixel(x, y);
                e += 4 * dy;     
            }
        }
    }
}






void Bresenham_Line_half(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1);   
    int dy = abs(y2 - y1);   
    int p = 2 * dy - dx;   
    int x = x1;   
    int y = y1;

    while(x <= x2)
    {   
        SetDevicePixel(x, y);   
        x++;   
        if(p<0)   
            p += 2 * dy;   
        else
        {   
            p += 2 * (dy - dx);   
            y += 1;   
        }   
    }   
}
