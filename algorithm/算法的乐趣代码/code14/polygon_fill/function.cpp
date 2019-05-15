#include "stdafx.h"
#include "dev_adp.h"
#include "function.h"

void GetPolygonMinMax(const Polygon& py, int& ymin, int& ymax)
{
    ymin = py.pts[0].y;
    ymax = py.pts[0].y;
    for(int i = 1; i < py.GetPolyCount(); i++)
    {
        if(py.pts[i].y < ymin)
            ymin = py.pts[i].y;
        if(py.pts[i].y > ymax)
            ymax = py.pts[i].y;
    }
}


void HorizonEdgeFill(const Polygon& py, int color)
{
    for(int i = 0; i < py.GetPolyCount(); i++)
    {
        const Point& ps = py.pts[i];
        const Point& pe = py.pts[(i + 1) % py.GetPolyCount()];
        
        if(pe.y == ps.y) 
        {
            if(ps.x <= pe.x)
            {
                DrawHorizontalLine(ps.x, pe.x, ps.y, color);
            }
            else
            {
                DrawHorizontalLine(pe.x, ps.x, ps.y, color);
            }
        }
    }
}



