#include "stdafx.h"
#include <cassert>
#include <algorithm>
#include "A_Star.h"


//static VECTOR dir[] = { {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1} };
static VECTOR dir[] = { {-1, 0}, {0, -1}, {1, 0}, {0, 1} };

bool ExtractMiniFromOpen(ASTAR_GRAPH *graph, ANODE& node)
{
    std::multiset<ANODE, compare>::iterator fin = graph->open.begin();
    if(fin == graph->open.end())
    {
        return false;
    }
    
    node = *fin;
    graph->open.erase(fin);
    return true;
}
#if 0
bool IsSameDistance(ANODE& n1, ANODE& n2)
{
    double f1 = n1.g + n1.h;
    double f2 = n2.g + n2.h;

    return (std::abs(f1 - f2) < 0.00000001);
}

int GetAdjacentNodeD(ANODE& n1, ANODE& n2)
{
    return (std::abs(n1.i - n2.i) + std::abs(n1.j - n2.j));
}

/*当评估距离f相同时，选择距离当前node最近的一个点开始*/
bool ExtractMiniFromOpen_N(ASTAR_GRAPH *graph, ANODE& node)
{
    ANODE last;
    std::vector<ANODE>::reverse_iterator rt = graph->close.rbegin();
    bool need_check = false;
    if(rt != graph->close.rend())
    {
        need_check = true;
        last = *rt;
    }
    std::multiset<ANODE, compare>::iterator fin = graph->open.begin();
    if(fin == graph->open.end())
        return false;

    if(need_check)
    {
        int dd = GetAdjacentNodeD(*fin, last);
        std::multiset<ANODE, compare>::iterator fon = fin++;
        while(fin != graph->open.end() && IsSameDistance(*fin, *fon))
        {
            int ee = GetAdjacentNodeD(*fin, last);
            if(ee < dd)
            {
                fon = fin;
                dd = ee;
            }
            fin++;
        }
        node = *fon;
        graph->open.erase(fon);
        return true;
    }
    else
    {
        node = *fin;
        graph->open.erase(fin);
        return true;
    }
}
#endif
bool IsNodeExistInOpen(std::multiset<ANODE, compare>& open, int i, int j)
{
    std::multiset<ANODE, compare>::iterator it;
    for(it = open.begin(); it != open.end(); ++it)
    {
        if((it->i == i) && (it->j == j))
        {
            return true;
        }
    }

    return false;
}

bool IsNodeExistInClose(std::vector<ANODE>& close, int i, int j)
{
    std::vector<ANODE>::iterator it;
    for(it = close.begin(); it != close.end(); ++it)
    {
        if((it->i == i) && (it->j == j))
        {
            return true;
        }
    }

    return false;
}

ANODE MakeANode(int i, int j, int g, int h)
{
    ANODE node = {i, j, g, h, -1, -1};

    return node;
}

static void UpdateCellInfo(ASTAR_GRAPH *graph, GRID_CELL *gc)
{
    std::vector<ANODE>::reverse_iterator it = graph->close.rbegin();
    int ri = it->i;
    int rj = it->j;
    while((it != graph->close.rend()) && (ri != -1) && (rj != -1))
    {
        if((ri == it->i) && (rj == it->j))
        {
            gc->cell[it->i][it->j].inPath = true;
            ri = it->prev_i;
            rj = it->prev_j;
        }
        
        ++it;
    }
}

void AStarGraphFromGridDef(ASTAR_GRAPH *graph, GRID_CELL *gc, GRID_INIT *gi)
{
    int index = 0;
    for(int i = 0; i < N_SCALE; i++)
    {
        for(int j = 0; j < N_SCALE; j++)
        {
            graph->grid[i][j] = gi->grid[i][j];
            gc->cell[i][j].processed = false;
            gc->cell[i][j].inPath = false;
            gc->cell[i][j].node_idx = -1;
            gc->cell[i][j].type = gi->grid[i][j];
            if(gi->grid[i][j] != CELL_WALL)
            {
                gc->cell[i][j].node_idx = index;
                if(gi->grid[i][j] == CELL_SOURCE)
                    graph->source = MakeANode(i, j, 0, 0);
                if(gi->grid[i][j] == CELL_TARGET)
                    graph->target = MakeANode(i, j, 0, 0);

                index++;
            }
        }
    }
}

void AStar(ASTAR_GRAPH *graph, GRID_CELL *gc, DistanceFuncPtr dist_func)
{
    //步骤(1)
    graph->open.insert(graph->source);

    //步骤(2)
    ANODE cur_node;
    while(ExtractMiniFromOpen(graph, cur_node))
    {
        //graph->open.erase(cur_node);
        graph->close.push_back(cur_node);
        if(cur_node == graph->target)
        {
            UpdateCellInfo(graph, gc);
            break;
        }

        //步骤(3)
        for(int d = 0; d < COUNT_OF(dir); d++)
        {
            ANODE nn = {cur_node.i + dir[d].y, cur_node.j + dir[d].x, 0, 0};
            if((nn.i >=0) && (nn.i < N_SCALE) && (nn.j >=0) && (nn.j < N_SCALE)
                && (gc->cell[nn.i][nn.j].type != CELL_WALL) 
                && !IsNodeExistInClose(graph->close, nn.i, nn.j))
            {
                std::multiset<ANODE, compare>::iterator it;
                it = find(graph->open.begin(), graph->open.end(), nn);
                if(it == graph->open.end()) /*nn不在open列表*/
                {
                    nn.g = cur_node.g + 1; //将g始终赋值为0可得到BFS算法的效果
                    nn.h = dist_func(nn, graph->target);
                    nn.prev_i = cur_node.i;
                    nn.prev_j = cur_node.j;
                    graph->open.insert(nn);
                    gc->cell[nn.i][nn.j].processed = true;
                }
                else  /*nn在open列表中*/
                {
                    if((cur_node.g + 1.0) < it->g)
                    {
                        it->g = cur_node.g + 1.0;
                        it->prev_i = cur_node.i;
                        it->prev_j = cur_node.j;
                    }
                }
            }
        }            
    }

}

