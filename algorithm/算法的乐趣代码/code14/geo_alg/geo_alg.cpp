// geo_alg.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <algorithm>
#include <vector>
#include <cmath>
#include <cassert>

const double DFP		= 1E-10;
const double INFINITE   = 10000.0;

bool IsZeroFloatValue(double v)
{
    return (std::fabs(v - 0.0) <= DFP);
}

bool IsSameFloatValue(double v1, double v2)
{
    return (std::fabs(v1 - v2) <= DFP);
}


typedef struct tagPOINT
{
    double x;
    double y;
}POINT;

class Point
{
public:
    Point(double x_, double y_) { x = x_; y = y_; };
    Point(const Point& pt) { x = pt.x; y = pt.y; };
    Point() { x = 0; y = 0; };
    Point& operator= (const Point& pt)
    { x = pt.x; y = pt.y; return *this; }

    double x;
    double y;
};

class LineSeg
{
public:
    LineSeg(double x1, double y1, double x2, double y2) : ps(x1, y1), pe(x2, y2)
    {};
    LineSeg(const Point& ps_, const Point& pe_) : ps(ps_.x, ps_.y), pe(pe_.x, pe_.y)
    {};
    LineSeg(const LineSeg& ls) : ps(ls.ps), pe(ls.pe)
    {};
    bool IsHorizontal()
    { return IsSameFloatValue(ps.y, pe.y); };
    bool IsVertical()
    { return IsSameFloatValue(ps.x, pe.x); };

    Point ps;
    Point pe;
};

class Circle
{
public:
    Circle(double x_, double y_, double r_) : p(x_, y_)
    { r = r_; };
    Circle(const Circle& c) : p(c.p.x, c.p.y)
    { r = c.r; };

    Point p;
    double r;
};

class Rect
{
public:
    Rect(double x1, double y1, double x2, double y2) : p1(x1, y1), p2(x2, y2)
    {};
    Rect(const Point& p1_, const Point& p2_) : p1(p1_.x, p1_.y), p2(p2_.x, p2_.y)
    {};
    Rect(const Rect& rc) : p1(rc.p1), p2(rc.p2)
    {};
    Rect() : p1(0.0, 0.0), p2(0.0, 0.0)
    {};

    Point p1;
    Point p2;
};

class Polygon
{
public:
    Polygon(POINT *p, int count)
    {
        for(int i = 0; i < count; i++)
        {
            pts.push_back(Point(p[i].x, p[i].y));
        }
    };
    Polygon(const Polygon& py)
    {
        std::copy(py.pts.begin(), py.pts.end(), back_inserter(pts));
    };

    bool IsValid() const
    { return pts.size() >= 3; };

    int GetPolyCount() const
    { return static_cast<int>(pts.size()); };

    std::vector<Point> pts;
};

//计算欧氏几何空间内平面两点的距离
double PointDistance(const Point& p1, const Point& p2)
{
    return std::sqrt( (p1.x-p2.x)*(p1.x-p2.x)
                        + (p1.y-p2.y)*(p1.y-p2.y) ); 
}

bool IsPointInRect(const Rect& rc, const Point& p)
{
    double xr = (p.x - rc.p1.x) * (p.x - rc.p2.x);
    double yr = (p.y - rc.p1.y) * (p.y - rc.p2.y);

    return ( (xr <= 0.0) && (yr <= 0.0) ); 
}

void GetLineSegmentRect(const LineSeg& ls, Rect& rc)
{
    rc.p1 = ls.pe;
    rc.p2 = ls.ps;
}

double CrossProduct(double x1, double y1, double x2, double y2)
{
    return x1 * y2 - x2 * y1;
}

double DotProduct(double x1, double y1, double x2, double y2)
{
    return x1 * x2 + y1 * y2;
}

bool IsPointOnLineSegment(const LineSeg& ls, const Point& pt)
{
    Rect rc;

    GetLineSegmentRect(ls, rc);
    double cp = CrossProduct(ls.pe.x - ls.ps.x, ls.pe.y - ls.ps.y, 
                             pt.x - ls.ps.x, pt.y - ls.ps.y); //计算叉积

    return ( (IsPointInRect(rc, pt)) //排除实验
             && IsZeroFloatValue(cp) ); //1E-8 精度
}

bool IsRectIntersect(const Rect& rc1, const Rect& rc2)
{
    return ( (std::max(rc1.p1.x, rc1.p2.x) >= std::min(rc2.p1.x, rc2.p2.x))
             && (std::max(rc2.p1.x, rc2.p2.x) >= std::min(rc1.p1.x, rc1.p2.x))
             && (std::max(rc1.p1.y, rc1.p2.y) >= std::min(rc2.p1.y, rc2.p2.y))
             && (std::max(rc2.p1.y, rc2.p2.y) >= std::min(rc1.p1.y, rc1.p2.y)) );
}

bool IsLineSegmentExclusive(const LineSeg& ls1, const LineSeg& ls2)
{
    Rect rc1,rc2;

    GetLineSegmentRect(ls1, rc1);
    GetLineSegmentRect(ls2, rc2);

    return !IsRectIntersect(rc1, rc2);
}

bool IsLineSegmentIntersect(const LineSeg& ls1, const LineSeg& ls2)
{
    if(IsLineSegmentExclusive(ls1, ls2)) //排斥实验
    {
        return false;
    }
    //( P1 - Q1 ) × ( Q2 - Q1 )
    double p1xq = CrossProduct(ls1.ps.x - ls2.ps.x, ls1.ps.y - ls2.ps.y, 
                               ls2.pe.x - ls2.ps.x, ls2.pe.y - ls2.ps.y); 
    //( P2 - Q1 ) × ( Q2 - Q1 )
    double p2xq = CrossProduct(ls1.pe.x - ls2.ps.x, ls1.pe.y - ls2.ps.y, 
                               ls2.pe.x - ls2.ps.x, ls2.pe.y - ls2.ps.y); 

    //( Q1 - P1 ) × ( P2 - P1 )
    double q1xp = CrossProduct(ls2.ps.x - ls1.ps.x, ls2.ps.y - ls1.ps.y, 
                               ls1.pe.x - ls1.ps.x, ls1.pe.y - ls1.ps.y); 
    //( Q2 - P1 ) × ( P2 - P1 )
    double q2xp = CrossProduct(ls2.pe.x - ls1.ps.x, ls2.pe.y - ls1.ps.y, 
                               ls1.pe.x - ls1.ps.x, ls1.pe.y - ls1.ps.y); 

    //跨立实验
    return ( (p1xq * p2xq <= 0.0) && (q1xp * q2xp <= 0.0) ); 
}

bool IsPointInPolygon(const Polygon& py, const Point& pt)
{
    assert(py.IsValid()); /*只考虑正常的多边形，边数>=3*/

    int count = 0;
    LineSeg ll = LineSeg(pt, Point(-INFINITE, pt.y)); /*射线L*/
    for(int i = 0; i < py.GetPolyCount(); i++)
    {
        /*当前点和下一个点组成线段P1P2*/
        LineSeg pp = LineSeg(py.pts[i], py.pts[(i + 1) % py.GetPolyCount()]); 
        if(IsPointOnLineSegment(pp, pt))
        {
            return true;
        }

        if(!pp.IsHorizontal())
        {
            if((IsSameFloatValue(pp.ps.y, pt.y)) && (pp.ps.y > pp.pe.y))
            {
                count++;
            }
            else if((IsSameFloatValue(pp.pe.y, pt.y)) && (pp.pe.y > pp.ps.y))
            {
                count++;
            }
            else
            {
                if(IsLineSegmentIntersect(pp, ll))
                {
                    count++;
                }
            }
        }
    }

    return ((count % 2) == 1);
}

void GetPolygonEnvelopRect(const Polygon& py, Rect& rc)
{
    assert(py.IsValid()); /*只考虑正常的多边形，边数>=3*/

    double minx = py.pts[0].x;
    double maxx = -py.pts[0].x;
    double miny = py.pts[0].y;
    double maxy = -py.pts[0].y;
    for(int i = 1; i < py.GetPolyCount(); i++)
    {
        if(py.pts[i].x < minx)
            minx = py.pts[i].x;
        if(py.pts[i].x > maxx)
            maxx = py.pts[i].x;
        if(py.pts[i].y < miny)
            miny = py.pts[i].y;
        if(py.pts[i].y > maxy)
            maxy = py.pts[i].y;
    }

    rc = Rect(minx, miny, maxx, maxy);
}

//assert(std::fabs(, ) < DFP)

int main(int argc, char* argv[])
{
    Point p1 = Point(1, 1);
    Point p2 = Point(2, 2);

    double dist = PointDistance(p1, p2);
    assert(std::fabs(PointDistance(p1, p2) - 1.4142135623) < DFP);
    p1 = Point(0, 0);
    dist = PointDistance(p1, p2);
    assert(std::fabs(PointDistance(p1, p2) - 2.8284271247) < DFP);
    p1 = Point(-1, -2);
    dist = PointDistance(p1, p2);
    assert(std::fabs(PointDistance(p1, p2) - 5.0) < DFP);

    assert(IsPointInRect(Rect(-1, -1, 1, 1), Point(0, 0)));
    assert(IsPointInRect(Rect(0, 0, 5, 8), Point(0, 0)));
    assert(IsPointInRect(Rect(0, 0, 5, 8), Point(1, 1)));
    assert(!IsPointInRect(Rect(0, 0, 5, 8), Point(-1, 1)));

    LineSeg ls(0, 0, 3, 5);
    assert(IsPointOnLineSegment(ls, Point(0, 0)));
    assert(IsPointOnLineSegment(ls, Point(3, 5)));

    ls = LineSeg(-1, -1, 9, 9);
    assert(IsPointOnLineSegment(ls, Point(-1, -1)));
    assert(IsPointOnLineSegment(ls, Point(4, 4)));
    assert(IsPointOnLineSegment(ls, Point(9, 9)));

    LineSeg le(0, 0, 3, 5);
    assert(!IsLineSegmentExclusive(ls, le));
    assert(IsLineSegmentIntersect(ls, le));

    le = LineSeg(-1, 10, 6, 8);
    assert(!IsLineSegmentExclusive(ls, le));

    le = LineSeg(-1, 10, 6, 12);
    assert(IsLineSegmentExclusive(ls, le));
    assert(!IsLineSegmentIntersect(ls, le));

    le = LineSeg(-1, 10, 9, 5);
    assert(IsLineSegmentIntersect(ls, le));

    ls = LineSeg(1, 1, 9, 5);
    le = LineSeg(-1, 1, 9, -5);
    assert(!IsLineSegmentIntersect(ls, le));

    ls = LineSeg(1, 1, 9, 5);
    le = LineSeg(2, 2, 5, -1);
    assert(IsLineSegmentIntersect(ls, le));

    ls = LineSeg(1, 1, 9, 9);
    le = LineSeg(3, 2, 5, -1);
    assert(!IsLineSegmentIntersect(ls, le));

    POINT pts[] = { {0, 0}, {0, 5}, {5, 0} };

    Polygon py = Polygon(pts, sizeof(pts)/sizeof(pts[0]));
    assert(IsPointInPolygon(py, Point(0, 0)));
    assert(IsPointInPolygon(py, Point(0, 5)));
    assert(IsPointInPolygon(py, Point(5, 0)));
    assert(IsPointInPolygon(py, Point(4, 1)));
    assert(IsPointInPolygon(py, Point(1, 4)));
    assert(!IsPointInPolygon(py, Point(5, 0.1)));

    POINT pts2[] = { {1, 6}, {-1, 1}, {5, 3}, {7, 2}, {12, 2}, {9, 8}, {4, 4} };
    Polygon py2 = Polygon(pts2, sizeof(pts2)/sizeof(pts2[0]));
    assert(IsPointInPolygon(py2, Point(5, 4)));
    assert(IsPointInPolygon(py2, Point(7, 3)));
    assert(IsPointInPolygon(py2, Point(9, 6)));
    assert(IsPointInPolygon(py2, Point(9, 2)));
    assert(IsPointInPolygon(py2, Point(7, 2)));
    assert(IsPointInPolygon(py2, Point(0, 2)));
    assert(IsPointInPolygon(py2, Point(4, 4)));
    
    assert(!IsPointInPolygon(py2, Point(12, 3)));
    assert(!IsPointInPolygon(py2, Point(4, 1)));
    assert(!IsPointInPolygon(py2, Point(5, 2)));
    assert(!IsPointInPolygon(py2, Point(4, 5)));
    assert(!IsPointInPolygon(py2, Point(-1, 3)));


	return 0;
}

