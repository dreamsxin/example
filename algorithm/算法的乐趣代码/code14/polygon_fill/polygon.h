#ifndef __POLYGON_H__
#define __POLYGON_H__

#include <vector>
#include <algorithm>
#include "point.h"

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

#endif /*__POLYGON_H__*/

