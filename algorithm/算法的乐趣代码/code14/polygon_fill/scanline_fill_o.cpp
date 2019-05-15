#include "stdafx.h"
#include <vector>
#include <cassert>
#include "dev_adp.h"
#include "common.h"
#include "edge_def.h"
#include "function.h"
#include "scanline_fill_o.h"


typedef struct tagSP_EDGES_TABLE
{
    std::vector<EDGE2> slEdges;
    int first;
    int last;
}SP_EDGES_TABLE; 


//slEdges是按照maxy从大到小有序的表，插入操作要满足这个条件
void InsertEdgeToEdgesTable(EDGE2& e, std::vector<EDGE2>& slEdges)
{
    if(slEdges.empty())
    {
        slEdges.push_back(e);
    }
    else
    {
        bool isInsert = false;
        std::vector<EDGE2>::iterator it;
        for(it = slEdges.begin(); it != slEdges.end(); ++it)
        {
            if(e.ymax >= it->ymax)
            {
                slEdges.insert(it, e);
                isInsert = true;
                break;
            }
        }
        if(!isInsert)
        {
            slEdges.push_back(e);
        }
    }
}

void InitScanLineEdgesTable(SP_EDGES_TABLE& spET, const Polygon& py)
{
    EDGE2 e;
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
                    e.dy = e.ymax - ps.y + 1;
                }
                else
                {
                    e.xi = pe.x;
                    e.ymax = pe.y;
                    e.dy = pe.y - ps.y + 1;
                }
            }
            else //(pe.y < ps.y)
            {
                if(pe.y > pee.y) //右顶点
                {
                    e.xi = ps.x;
                    e.ymax = ps.y;
                    e.dy = ps.y - (pe.y + 1) + 1;
                }
                else
                {
                    e.xi = ps.x;
                    e.ymax = ps.y;
                    e.dy = ps.y - pe.y + 1;
                }
            }

            InsertEdgeToEdgesTable(e, spET.slEdges);
        }
    }
    spET.first = spET.last = 0;
}

void UpdateEdgesTableActiveRange(SP_EDGES_TABLE& spET, int yScan)
{
    std::vector<EDGE2>& slET = spET.slEdges;
    int edgesCount = static_cast<int>(slET.size());
    while((spET.last < (edgesCount - 1)) && (slET[spET.last + 1].ymax >= yScan))
    {
        spET.last++;
    }

    while(slET[spET.first].dy == 0)
    {
        spET.first++;
    }
}

int GetIntersectionInActiveRange(SP_EDGES_TABLE& spET, int pos)
{
    std::vector<EDGE2>& slET = spET.slEdges;
    assert( (pos >= 0) && (pos < static_cast<int>(slET.size())) );

    int isPos = pos;
    while((isPos <= spET.last) && (slET[isPos].dy == 0))
    {
        isPos++;
    }

    if(isPos <= spET.last)
    {
        return isPos;
    }

    return -1;
}

void FillActiveRangeScanLine(SP_EDGES_TABLE& spET, int yScan, int color)
{
    std::vector<EDGE2>& slET = spET.slEdges;
    int pos = spET.first;

    do
    {
        pos = GetIntersectionInActiveRange(spET, pos);
        if(pos != -1)
        {
            int x1 = ROUND_INT(slET[pos].xi);
            pos = GetIntersectionInActiveRange(spET, pos + 1);
            if(pos != -1)
            {
                int x2 = ROUND_INT(slET[pos].xi);
                pos++;
                DrawHorizontalLine(x1, x2, yScan, color);
            }
            else
            {
                assert(false);
            }
        }
    }while(pos != -1);
}

bool EdgeXiComparator2(EDGE2& e1, EDGE2& e2)
{
    return (e1.xi < e2.xi);
}

void SortActiveRangeByX(SP_EDGES_TABLE& spET)
{
    std::vector<EDGE2>& slET = spET.slEdges;

    sort(slET.begin() + spET.first, 
        slET.begin() + spET.last + 1,
        EdgeXiComparator2);
}

void UpdateActiveRangeIntersection(SP_EDGES_TABLE& spET)
{
    for(int pos = spET.first; pos <= spET.last; pos++)
    {
        if(spET.slEdges[pos].dy > 0)
        {
            spET.slEdges[pos].dy--;
            spET.slEdges[pos].xi -= spET.slEdges[pos].dx;
        }
    }
}

void ProcessScanLineFill2(SP_EDGES_TABLE& spET, 
                          int ymin, int ymax, int color)
{
    for (int yScan = ymax; yScan >= ymin; yScan--)
    {
        UpdateEdgesTableActiveRange(spET, yScan);
        SortActiveRangeByX(spET);
        FillActiveRangeScanLine(spET, yScan, color);
        UpdateActiveRangeIntersection(spET);
    }
}

void ScanLinePolygonFill2(const Polygon& py, int color)
{
    assert(py.IsValid());

    int ymin = 0;
    int ymax = 0;
    GetPolygonMinMax(py, ymin, ymax);
    SP_EDGES_TABLE spET;
    InitScanLineEdgesTable(spET, py);
    HorizonEdgeFill(py, color); //水平边直接画线填充
    ProcessScanLineFill2(spET, ymin, ymax, color);
}
