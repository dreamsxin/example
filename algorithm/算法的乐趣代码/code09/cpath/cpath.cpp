// graph.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

const int MAX_VERTEXNODE = 20;

typedef struct tagEdgeNode
{
    int vertexIndex;    //活动边终点顶点索引
    std::string name;   //活动边的名称
    int duty;           //活动边的时间（权重）
}EDGE_NODE;

typedef struct tagVertexNode
{
    int sTime;    //事件最早开始时间
    int eTime;    //事件最晚开始时间
    int inCount;  //活动的前驱节点个数
    std::vector<EDGE_NODE> edges; //相邻边表
}VERTEX_NODE;

typedef struct tagGraph
{
    int count; //图的顶点的个数
    VERTEX_NODE vertexs[MAX_VERTEXNODE]; //图的顶点列表
}GRAPH;


void InitGraph(GRAPH *g, int vertex)
{
    g->count = vertex;
    for(int i = 0; i < vertex; i++)
    {
        g->vertexs[i].sTime = 0;
        g->vertexs[i].eTime = 0x7FFFFFFF;
        g->vertexs[i].inCount = 0;
    }
}

bool AddGraphEdge(GRAPH *g, const char*name, int u, int v, int weight)
{
    if((u >= g->count) || (v >= g->count))
    {
        return false;
    }
    EDGE_NODE edge = {v, name, weight};

    g->vertexs[u].edges.push_back(edge);
    g->vertexs[v].inCount++;

    return true;
}

bool TopologicalSorting(GRAPH *g, std::vector<int>& sortedNode)
{
    std::priority_queue<int> nodeQueue;

    for(int i = 0; i < g->count; i++)
    {
        if(g->vertexs[i].inCount == 0)
        {
            nodeQueue.push(i);
        }
    }

    while(nodeQueue.size() != 0)
    {
        int u = nodeQueue.top();
        nodeQueue.pop();
        sortedNode.push_back(u);
        for(int j = 0; j < (int)g->vertexs[u].edges.size(); j++)
        {
            int v = g->vertexs[u].edges[j].vertexIndex;
            g->vertexs[v].inCount--;
            if(g->vertexs[v].inCount == 0)
            {
                nodeQueue.push(v);
            }
        }
    }

    return (sortedNode.size() == g->count);
}

void CalcESTime(GRAPH *g, const std::vector<int>& sortedNode)
{
    g->vertexs[0].sTime = 0; //est[0] = 0

    std::vector<int>::const_iterator nit = sortedNode.begin();
    for(; nit != sortedNode.end(); ++nit)
    {
        int u = *nit;
        //遍历u出发的所有有向边
        std::vector<EDGE_NODE>::iterator eit = g->vertexs[u].edges.begin();
        for(; eit != g->vertexs[u].edges.end(); ++eit)
        {
            int v = eit->vertexIndex;
            int uvst = g->vertexs[u].sTime + eit->duty;
            if(uvst > g->vertexs[v].sTime)
            {
                g->vertexs[v].sTime = uvst;
            }
        }
    }
}

void CalcLSTime(GRAPH *g, const std::vector<int>& sortedNode)
{
    //最后一个节点的最晚开始时间等于最早开始时间
    g->vertexs[g->count - 1].eTime = g->vertexs[g->count - 1].sTime;

    std::vector<int>::const_reverse_iterator cit = sortedNode.rbegin();
    for(; cit != sortedNode.rend(); ++cit)
    {
        int u = *cit;
        //遍历u出发的所有有向边
        std::vector<EDGE_NODE>::iterator eit = g->vertexs[u].edges.begin();
        for(; eit != g->vertexs[u].edges.end(); ++eit)
        {
            int v = eit->vertexIndex;
            int uvet = g->vertexs[v].eTime - eit->duty;
            if(uvet < g->vertexs[u].eTime)
            {
                g->vertexs[u].eTime = uvet;
            }
        }
    }
}

void PrintSorting(GRAPH *g, const std::vector<int>& sortedNode)
{
    std::vector<int>::const_iterator cit = sortedNode.begin();
    for(;cit != sortedNode.end(); ++cit)
    {
        std::cout << "e" << *cit << std::endl;
    }
}

bool CriticalPath(GRAPH *g)
{
    std::vector<int> sortedNode;
    if(!TopologicalSorting(g, sortedNode)) //步骤1
    {
        return false;
    }
    CalcESTime(g, sortedNode); //步骤2
    CalcLSTime(g, sortedNode); //步骤4

    //步骤4：输出关键路径上的活动名称
    //for(int u = 0; u < g->count; u++)
    //{

    std::vector<int>::iterator nit = sortedNode.begin();
    for(; nit != sortedNode.end(); ++nit)
    {
        int u = *nit;
        std::vector<EDGE_NODE>::iterator eit = g->vertexs[u].edges.begin();
        for(; eit != g->vertexs[u].edges.end(); ++eit)
        {
            int v = eit->vertexIndex;
            //是否是关键活动？
            if(g->vertexs[u].sTime == g->vertexs[v].eTime - eit->duty)
            {
                if(!eit->name.empty())//过滤连接事件顶点的虚拟活动边
                {
                    std::cout << eit->name << std::endl;
                }
            }
        }
    }

    return true;
}

int main(int argc, char* argv[])
{

    GRAPH graph;
    InitGraph(&graph, 10);
    AddGraphEdge(&graph, "P1", 0, 1, 8);
    AddGraphEdge(&graph, "P2", 0, 2, 5);
    AddGraphEdge(&graph, "", 1, 3, 0);
    AddGraphEdge(&graph, "", 2, 3, 0);
    AddGraphEdge(&graph, "P7", 1, 6, 4);
    AddGraphEdge(&graph, "P5", 2, 5, 7);
    AddGraphEdge(&graph, "P3", 3, 4, 6);
    AddGraphEdge(&graph, "P4", 4, 8, 4);
    AddGraphEdge(&graph, "P8", 6, 7, 3);
    AddGraphEdge(&graph, "", 8, 7, 0);
    AddGraphEdge(&graph, "", 8, 5, 0);
    AddGraphEdge(&graph, "P9", 7, 9, 4);
    AddGraphEdge(&graph, "P6", 5, 9, 7);

    CriticalPath(&graph);
	return 0;
}

