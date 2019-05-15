#include "stdafx.h"
#include "Dijkstra.h"
#include "Function.h"


//static VECTOR dir[] = { {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1} };
static VECTOR dir[] = { {-1, 0}, {0, -1}, {1, 0}, {0, 1} };

void InitAdjacentMatrix(DIJKSTRA_GRAPH *graph, GRID_CELL *gc)
{
    for(int i = 0; i < N_NODE; i++)
    {
        for(int j = 0; j < N_NODE; j++)
        {
            graph->adj[i][j] = MAX_DISTANCE;
        }
    }

    for(int i = 0; i < N_SCALE; i++)
    {
        for(int j = 0; j < N_SCALE; j++)
        {
            if(gc->cell[i][j].type != CELL_WALL)
            {
                for(int d = 0; d < COUNT_OF(dir); d++)
                {
                    int ni = i + dir[d].y;
                    int nj = j + dir[d].x;
                    if((ni >=0) && (ni < N_SCALE) 
                        && (nj >=0) && (nj < N_SCALE)
                        && gc->cell[ni][nj].type != CELL_WALL)
                    {
                        graph->adj[gc->cell[i][j].node_idx][gc->cell[ni][nj].node_idx] = 1;
                    }
                }            
            }
        }
    }
}

void DijkstraGraphFromGridDef(DIJKSTRA_GRAPH *graph, GRID_CELL *gc, GRID_INIT *gi)
{
    int index = 0;
    for(int i = 0; i < N_SCALE; i++)
    {
        for(int j = 0; j < N_SCALE; j++)
        {
            gc->cell[i][j].processed = false;
            gc->cell[i][j].inPath = false;
            gc->cell[i][j].node_idx = -1;
            gc->cell[i][j].type = gi->grid[i][j];
            if(gi->grid[i][j] != CELL_WALL)
            {
                gc->cell[i][j].node_idx = index;
                GNODE an = {i, j};
                graph->nodes.push_back(an);
                if(gi->grid[i][j] == CELL_SOURCE)
                    graph->source = index;
                if(gi->grid[i][j] == CELL_TARGET)
                    graph->target = index;

                index++;
            }
        }
    }

    InitAdjacentMatrix(graph, gc);
}

static void UpdateCellInfo(DIJKSTRA_GRAPH *graph, std::set<int>& S, GRID_CELL *gc)
{
    for(std::set<int>::iterator it = S.begin(); it != S.end(); ++it)
    {
        GNODE node = graph->nodes[*it];
        gc->cell[node.i][node.j].processed = true;
    }
    int u = graph->target;
    while(u != -1)
    {
        GNODE node = graph->nodes[u];
        gc->cell[node.i][node.j].inPath = true;
        u = graph->prev[u];
    }
}

int Extract_Min(DIJKSTRA_GRAPH *graph, std::set<int>& Q)
{
    int u = 0;
    int mindist = MAX_DISTANCE;
    //std::set<int>::iterator it;
    for(std::set<int>::iterator it = Q.begin(); it != Q.end(); ++it)
    {
        if(graph->dist[*it] < mindist)
        {
            u = *it;
            mindist = graph->dist[u];
        }
    }

    Q.erase(u);
    return u;
}

void Dijkstra(DIJKSTRA_GRAPH *graph, GRID_CELL *gc)
{
    std::set<int> S,Q;

    for(int i = 0; i < graph->nodes.size(); i++)
    {
        graph->dist[i] = graph->adj[graph->source][i];
        graph->prev[i] = (graph->dist[i] == MAX_DISTANCE) ? -1 : graph->source;
        (i == graph->source) ? S.insert(i) : Q.insert(i);
    }
    graph->dist[graph->source] = 0;
    
    while(!Q.empty())
    {
        int u = Extract_Min(graph, Q);
        S.insert(u);
        if(u == graph->target)//已经到达目标，可以结束算法
        {
            UpdateCellInfo(graph, S, gc);
            break;
        }
        for(std::set<int>::iterator it = Q.begin(); it != Q.end(); ++it)
        {
            int j = *it;
            if((graph->adj[u][j] < MAX_DISTANCE) //小于MAX_DISTANCE表示有边相连
                && (graph->dist[u] + graph->adj[u][j] < graph->dist[j]))
            {
                graph->dist[j] = graph->dist[u] + graph->adj[u][j];    //更新dist 
                graph->prev[j] = u;                    //记录前驱顶点 
            }
        }
    }
}
