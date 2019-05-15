#include "stdafx.h"
#include "dev_adp.h"
#include "common.h"
#include "recursion_seed_fill.h"

typedef struct tagDIRECTION
{
    int x_offset;
    int y_offset;
}DIRECTION;



DIRECTION direction_8[] = { {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1} };
DIRECTION direction_4[] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };




void FloodSeedFill(int x, int y, int old_color, int new_color)
{
    if(GetPixelColor(x, y) == old_color)
    {
        SetPixelColor(x, y, new_color);
        for(int i = 0; i < COUNT_OF(direction_8); i++)
        {
            FloodSeedFill(x + direction_8[i].x_offset, 
                          y + direction_8[i].y_offset, old_color, new_color);
        }
    }
}

void BoundarySeedFill(int x, int y, int new_color, int boundary_color)
{
    int curColor = GetPixelColor(x, y);
    if( (curColor != boundary_color) 
        && (curColor != new_color) )
    {
        SetPixelColor(x, y, new_color);
        for(int i = 0; i < COUNT_OF(direction_8); i++)
        {
            BoundarySeedFill(x + direction_8[i].x_offset, 
                             y + direction_8[i].y_offset, new_color, boundary_color);
        }
    }
}