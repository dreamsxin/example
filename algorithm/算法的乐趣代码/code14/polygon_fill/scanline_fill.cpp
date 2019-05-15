#include "stdafx.h"
#include <list>
#include <vector>
#include <iostream>
#include <cassert>
#include "dev_adp.h"
#include "common.h"
#include "edge_def.h"
#include "function.h"
#include "scanline_fill.h"


void InitScanLineNewEdgeTable(std::vector< std::list<EDGE> >& slNet, 
                              const Polygon& py, int ymin, int ymax)
{
    EDGE e;
    for(int i = 0; i < py.GetPolyCount(); i++)
    {
        const Point& ps = py.pts[i];
        const Point& pe = py.pts[(i + 1) % py.GetPolyCount()];
        const Point& pss = py.pts[(i - 1 + py.GetPolyCount()) % py.GetPolyCount()];
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
                e.xi = ps.x;
                if(pee.y >= pe.y)
                    e.ymax = pe.y - 1;
                else
                    e.ymax = pe.y;

                slNet[ps.y - ymin].push_front(e);
            }
            else
            {
                e.xi = pe.x;
                if(pss.y >= ps.y)
                    e.ymax = ps.y - 1;
                else
                    e.ymax = ps.y;
                slNet[pe.y - ymin].push_front(e);
            }
        }
    }
}

void PrintEdgeInfo(EDGE& e)
{
    std::cout << "\t";
#ifdef _DEBUG
    std::cout << "ps = (" << e.ps.x << ", " << e.ps.y << ") pe = (";
    std::cout << e.pe.x << ", " << e.pe.y << ") ";
#endif
    std::cout << "x1 = " << e.xi << " dx = " << e.dx << " ymax = " << e.ymax << std::endl;
}

void PrintNewEdgeTable(std::vector< std::list<EDGE> >& slNet)
{
    int lCount = static_cast<int>(slNet.size());
    for(int i = 0; i < lCount; i++)
    {
        std::list<EDGE>& eList = slNet[i];
        if(eList.empty())
        {
            std::cout << "Scan-Line " << i << " has empty NET !" << std::endl;
        }
        else
        {
            std::cout << "Scan-Line " << i << " has "<< eList.size() << " NET(s) !" << std::endl;
            for_each(eList.begin(), eList.end(), PrintEdgeInfo);
        }
    }
}

//aet是有序表，按照EDGE.x1从小到大排序，插入操作必须保证aet有序
void InsertEdgeToAet(EDGE& e, std::list<EDGE>& aet)
{
    if(aet.empty())
    {
        aet.push_front(e);
    }
    else
    {
        bool isInsert = false;
        std::list<EDGE>::iterator it;
        for(it = aet.begin(); it != aet.end(); ++it)
        {
            if(e.xi <= it->xi)
            {
                aet.insert(it, e);
                isInsert = true;
                break;
            }
        }
        if(!isInsert)
        {
            aet.push_back(e);
        }
    }
}

//将新边表中的边逐一插入到活动边表中
void InsertNetListToAet(std::list<EDGE>& net, std::list<EDGE>& aet)
{
    std::list<EDGE>::iterator it;
    for(it = net.begin(); it != net.end(); ++it)
    {
        InsertEdgeToAet(*it, aet);
    }
}

void FillAetScanLine(std::list<EDGE>& aet, int y, int color)
{
    assert((aet.size() % 2) == 0);

    std::list<EDGE>::iterator it = aet.begin();
    while(it != aet.end())
    {
        int x1 = ROUND_INT(it->xi);
        ++it;
        int x2 = ROUND_INT(it->xi);
        ++it;
        
        DrawHorizontalLine(x1, x2, y, color);
    }
}

bool IsEdgeOutOfActive(EDGE e, int y)
{
    return (e.ymax == y);
}

void RemoveNonActiveEdgeFromAet(std::list<EDGE>& aet, int y)
{
    aet.remove_if(std::bind2nd(std::ptr_fun(IsEdgeOutOfActive), y));
}

void UpdateAetEdgeInfo(EDGE& e)
{
    e.xi += e.dx;
}

bool EdgeXiComparator(EDGE& e1, EDGE& e2)
{
    return (e1.xi <= e2.xi);
}

void UpdateAndResortAet(std::list<EDGE>& aet)
{
    //更新xi
    for_each(aet.begin(), aet.end(), UpdateAetEdgeInfo);
    //根据xi从小到大重新排序
    aet.sort(EdgeXiComparator);
}

void ProcessScanLineFill(std::vector< std::list<EDGE> >& slNet, 
                         int ymin, int ymax, int color)
{
    std::list<EDGE> aet;

    for(int y = ymin; y <= ymax; y++)
    {
        InsertNetListToAet(slNet[y - ymin], aet);
        FillAetScanLine(aet, y, color);
        //删除非活动边
        RemoveNonActiveEdgeFromAet(aet, y);
        //更新活动边表中每项的xi值，并根据xi重新排序
        UpdateAndResortAet(aet);
    }
}


void ScanLinePolygonFill(const Polygon& py, int color)
{
    assert(py.IsValid());

    int ymin = 0;
    int ymax = 0;
    GetPolygonMinMax(py, ymin, ymax);
    std::vector< std::list<EDGE> > slNet(ymax - ymin + 1);
    InitScanLineNewEdgeTable(slNet, py, ymin, ymax);
    //PrintNewEdgeTable(slNet);
    HorizonEdgeFill(py, color); //水平边直接画线填充
    ProcessScanLineFill(slNet, ymin, ymax, color);
}