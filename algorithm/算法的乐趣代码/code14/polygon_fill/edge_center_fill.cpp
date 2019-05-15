#include "stdafx.h"
//#include <algorithm>
#include <functional>

#include "dev_adp.h"
#include "common.h"
#include "function.h"
#include "edge_def.h"
#include "edge_center_fill.h"

const int MAX_X_CORD = MAX_MAP_WIDTH - 1;
const int A = 9;

void InitScanLineEdgesTable(std::vector<EDGE3>& et, const Polygon& py)
{
    EDGE3 e;
    for(int i = 0; i < py.GetPolyCount(); i++)
    {
        const Point& ps = py.pts[i];
        const Point& pe = py.pts[(i + 1) % py.GetPolyCount()];
        const Point& pee = py.pts[(i + 2) % py.GetPolyCount()];

#ifdef _DEBUG
        e.ps.x = ps.x;
        e.ps.y = ps.y;
        e.pe.x = pe.x;
        e.pe.y = pe.y;
#endif
        if(pe.y != ps.y) //不处理水平线
        {
            e.dx = double(pe.x - ps.x) / double(pe.y - ps.y);
            if(pe.y > ps.y)
            {
                if(pe.y < pee.y) //左顶点
                {
                    e.xi = pe.x - e.dx;
                    e.ymax = pe.y - 1;
                }
                else
                {
                    e.xi = pe.x;
                    e.ymax = pe.y;
                }
                e.ymin = ps.y;
            }
            else //(pe.y < ps.y)
            {
                if(pe.y > pee.y) //右顶点
                {
                    e.xi = ps.x;
                    e.ymax = ps.y;
                }
                else
                {
                    e.xi = ps.x;
                    e.ymax = ps.y;
                }
                e.ymin = pe.y;
            }

            et.push_back(e);
        }
    }
}

void ComplementScanLineColor(int x1, int x2, int y)
{
    y = (MAX_MAP_HEIGHT - 1) - y;
    for(int x = x1; x <= x2; x++)
    {
        int color = GetPixelColor(x, y);
        SetPixelColor(x, y, A - color);
    }
}

void EdgeScanMarkColor(EDGE3& e)
{
    for(int y = e.ymax; y >= e.ymin; y--)
    {
        int x = ROUND_INT(e.xi);
        ComplementScanLineColor(x, MAX_X_CORD, y);

        e.xi -= e.dx;
    }
}

void EdgeCenterMarkFill(const Polygon& py, int color)
{
    std::vector<EDGE3> et;
    
    InitScanLineEdgesTable(et, py);//初始化边表
    
    FillBackground(A - color); //对整个填充区域背景颜色取补
    for_each(et.begin(), et.end(), EdgeScanMarkColor);//依次处理每一条边
}

void FenceScanMarkColor(EDGE3 e, int fence)
{
    for(int y = e.ymax; y >= e.ymin; y--)
    {
        int x = ROUND_INT(e.xi);
        if(x > fence)
        {
            ComplementScanLineColor(fence, x, y);
        }
        else
        {
            ComplementScanLineColor(x, fence - 1, y);
        }

        e.xi -= e.dx;
    }
}

void EdgeFenceMarkFill(const Polygon& py, int fence, int color)
{
    std::vector<EDGE3> et;
    
    InitScanLineEdgesTable(et, py);//初始化边表
    
    FillBackground(A - color); //对整个填充区域背景颜色取补
    for_each(et.begin(), et.end(), 
        std::bind2nd(std::ptr_fun(FenceScanMarkColor), fence));//依次处理每一条边
}
