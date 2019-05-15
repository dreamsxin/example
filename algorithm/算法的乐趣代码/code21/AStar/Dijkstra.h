#ifndef __DIJKSTRA_H__
#define __DIJKSTRA_H__

#include <iostream>
#include <vector>
#include <set>
#include "CellDef.h"

const int MAX_DISTANCE = 100000;
const int N_NODE = N_SCALE * N_SCALE;

typedef struct tagGNode
{
    int i;
    int j;
}GNODE;

typedef struct tagDijkstraGraph
{
    std::vector<GNODE> nodes;
    int adj[N_NODE][N_NODE];
    int prev[N_NODE];
    int dist[N_NODE];
    int source;
    int target;
}DIJKSTRA_GRAPH;

void DijkstraGraphFromGridDef(DIJKSTRA_GRAPH *graph, GRID_CELL *gc, GRID_INIT *gi);
void Dijkstra(DIJKSTRA_GRAPH *graph, GRID_CELL *gc);

#endif //__DIJKSTRA_H__
