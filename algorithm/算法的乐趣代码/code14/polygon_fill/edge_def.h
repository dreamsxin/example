#ifndef __DEGE_DEF_H__
#define __DEGE_DEF_H__

#include "point.h"


typedef struct tagEDGE
{
    double xi;
    double dx;
    int ymax;
#ifdef _DEBUG
    POINT ps;
    POINT pe;
#endif
}EDGE; 


typedef struct tagEDGE2
{
    double xi;
    double dx;
    int ymax;
    int dy;
#ifdef _DEBUG
    POINT ps;
    POINT pe;
#endif
}EDGE2; 

typedef struct tagEDGE3
{
    double xi;
    double dx;
    int ymax;
    int ymin;
#ifdef _DEBUG
    POINT ps;
    POINT pe;
#endif
}EDGE3; 



#endif /*__DEGE_DEF_H__*/

