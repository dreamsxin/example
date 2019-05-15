#ifndef __POINT_H__
#define __POINT_H__

typedef struct tagPOINT
{
    int x;
    int y;
}POINT;

class Point
{
public:
    Point(int x_, int y_)
    { x = x_; y = y_; };
    Point(const Point &pt)
    { x = pt.x; y = pt.y; };
    Point()
    { x = 0; y = 0; };

    int x;
    int y;
};

#endif /*__POINT_H__*/

