#ifndef __A_STAR_H__
#define __A_STAR_H__

#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include <algorithm>
#include "CellDef.h"
#include "Function.h"


struct ANODE
{
    bool operator== (const ANODE& n)
    {
        if((i == n.i) && (j == n.j))
        {
            return true;
        }

        return false;
    }
    int i;
    int j;
    double g;
    double h;
    int prev_i;
    int prev_j;
};

typedef double (*DistanceFuncPtr)(const ANODE& n1, const ANODE& n2);

struct compare
{
    bool operator()(const ANODE& n1,const ANODE& n2) const   
    {
        double f1 = n1.g + n1.h;
        double f2 = n2.g + n2.h;
        
        return (f1 < f2);
    }
};

typedef struct tagAStarGraph
{
    int grid[N_SCALE][N_SCALE];
    std::multiset<ANODE, compare> open;
    std::vector<ANODE> close;
    ANODE source;
    ANODE target;
}ASTAR_GRAPH;


void AStarGraphFromGridDef(ASTAR_GRAPH *graph, GRID_CELL *gc, GRID_INIT *gi);
void AStar(ASTAR_GRAPH *graph, GRID_CELL *gc, DistanceFuncPtr dist_func);

#endif //__A_STAR_H__
