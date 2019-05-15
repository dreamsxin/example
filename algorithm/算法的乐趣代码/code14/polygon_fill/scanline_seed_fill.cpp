#include "stdafx.h"
#include <stack>
#include "dev_adp.h"
#include "common.h"
#include "point.h"
#include "scanline_seed_fill.h"

int FillLineRight(int x, int y, int new_color, int boundary_color)
{
    int xt = x;
    while(GetPixelColor(xt, y) != boundary_color)
    {
        SetPixelColor(xt, y, new_color);
        xt++;
    }

    return xt - x;
}

int FillLineLeft(int x, int y, int new_color, int boundary_color)
{
    int xt = x;
    while(GetPixelColor(xt, y) != boundary_color)
    {
        SetPixelColor(xt, y, new_color);
        xt--;
    }

    return x - xt;
}

/*点不是边界，而且没有填充过*/
bool IsPixelValid(int x, int y, int new_color, int boundary_color)
{
    return ( (GetPixelColor(x, y) != boundary_color) 
              && (GetPixelColor(x, y) != new_color) );
}

int SkipInvalidInLine(int x, int y, int xRight, int new_color, int boundary_color)
{
    int xt = x;
    while(!IsPixelValid(xt, y, new_color, boundary_color) && (xt < xRight))
    {
        xt++;
    }

    return xt - x;
}

void SearchLineNewSeed(std::stack<Point>& stk, int xLeft, int xRight, 
                       int y, int new_color, int boundary_color)
{
    int xt = xLeft;
    bool findNewSeed = false;

    while(xt <= xRight)
    {
        findNewSeed = false;
        while(IsPixelValid(xt, y, new_color, boundary_color) && (xt < xRight))
        {
            findNewSeed = true;
            xt++;
        }
        if(findNewSeed)
        {
            if(IsPixelValid(xt, y, new_color, boundary_color) && (xt == xRight))
                stk.push(Point(xt, y));
            else
                stk.push(Point(xt - 1, y));
        }

        /*向右跳过内部的无效点（处理区间右端有障碍点的情况）*/
        int xspan = SkipInvalidInLine(xt, y, xRight, new_color, boundary_color);
        xt += (xspan == 0) ? 1 : xspan;
        /*处理特殊情况,以退出while(x<=xright)循环*/
    }
}

void ScanLineSeedFill(int x, int y, int new_color, int boundary_color)
{
    std::stack<Point> stk;

    stk.push(Point(x, y)); //第1步，种子点入站
    while(!stk.empty()) 
    {
        Point seed = stk.top(); //第2步，取当前种子点
        stk.pop();

        //第3步，向左右填充
        int count = FillLineRight(seed.x, seed.y, new_color, boundary_color);//向右填充
        int xRight = seed.x + count - 1;
        count = FillLineLeft(seed.x - 1, seed.y, new_color, boundary_color);//向左填充
        int xLeft = seed.x - count;

        //第4步，处理相邻两条扫描线
        SearchLineNewSeed(stk, xLeft, xRight, seed.y - 1, new_color, boundary_color); 
        SearchLineNewSeed(stk, xLeft, xRight, seed.y + 1, new_color, boundary_color); 
    }
}
