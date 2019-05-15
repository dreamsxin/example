#include "stdafx.h"
#include "dev_adp.h"
#include "edge_mark_fill.h"

void EdgeMarkFill(int xmin, int xmax, int ymin, int ymax, 
                  int boundarycolor, int color)
{
    int flag = -1;
    for(int y = ymin; y <= ymax; y++)
    {
        flag = -1;
        for(int x = xmin; x <= xmax; x++)
        {
            if(GetPixelColor(x, y) == boundarycolor)
            {
                flag = -flag;
            }
            if(flag == 1)
            {
                SetPixelColor(x, y, color);
            }
        }
    }
}

