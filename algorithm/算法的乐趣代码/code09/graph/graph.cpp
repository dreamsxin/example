// graph.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

const int MAX_VERTEXNODE = 20;

typedef struct tagVertexNode
{
    char *name;   //活动名称
    int days;     //完成活动所需时间
    int sTime;    //活动最早开始时间
    int inCount;  //活动的前驱节点个数
    int adjacentNode[MAX_VERTEXNODE]; //相邻活动列表(节点索引)
    int adjacent; //相邻活动的个数
}VERTEX_NODE;

typedef struct tagGraph
{
    int count; //图的顶点个数
    VERTEX_NODE vertexs[MAX_VERTEXNODE]; //图的顶点列表
}GRAPH;

GRAPH graph = 
{
    9,
    {
        {"P1", 8, 0, 0, {2, 6}, 2},
        {"P2", 5, 0, 0, {2, 4}, 2},
        {"P3", 6, 8, 2, {3}, 1},
        {"P4", 4, 14,2, {5, 8}, 2},
        {"P5", 7, 5, 1, {3, 5}, 2},
        {"P6", 7, 18, 2, {}, 0},
        {"P7", 4, 8, 1, {7}, 1},
        {"P8", 3, 12, 1, {8}, 1},
        {"P9", 4, 18, 2, {}, 0}
    }
};

struct QUEUE_ITEM  
{    
    int node;
    int sTime;
    bool operator < (const QUEUE_ITEM &a) const   
    {    
        return sTime > a.sTime;//最小值优先     
    }    
};   

void EnQueue(std::priority_queue<QUEUE_ITEM>& q, int node, int sTime)
{
    QUEUE_ITEM item = {node, sTime};
    q.push(item);
}

int DeQueue(std::priority_queue<QUEUE_ITEM>& q)
{
    int node = q.top().node;
    q.pop();
    return node;
}

    //std::priority_queue<QUEUE_ITEM, std::vector<QUEUE_ITEM>, PriorityCmp> nodeQueue;
bool TopologicalSorting(GRAPH *g, std::vector<int>& sortedNode)
{
    std::priority_queue<QUEUE_ITEM> nodeQueue;

    for(int i = 0; i < g->count; i++)
    {
        if(g->vertexs[i].inCount == 0)
        {
            EnQueue(nodeQueue, i, g->vertexs[i].sTime);
        }
    }

    while(nodeQueue.size() != 0)
    {
        int node = DeQueue(nodeQueue); //按照开始时间优先级出队
        sortedNode.push_back(node);//输出当前节点
        //遍历节点node的所有邻接点，将表示有向边inCount值减1
        for(int j = 0; j < g->vertexs[node].adjacent; j++)
        {
            int adjNode = g->vertexs[node].adjacentNode[j];
            g->vertexs[adjNode].inCount--;
            //如果inCount值为0，则该节点入队列
            if(g->vertexs[adjNode].inCount == 0)
            {
                EnQueue(nodeQueue, adjNode, g->vertexs[adjNode].sTime);
            }
        }
    }

    return (sortedNode.size() == g->count);
}

void PrintSorting(GRAPH *g, const std::vector<int>& sortedNode)
{
    std::vector<int>::const_iterator cit = sortedNode.begin();
    for(;cit != sortedNode.end(); ++cit)
    {
        std::cout << g->vertexs[*cit].name << std::endl;
    }
}

int main(int argc, char* argv[])
{
    std::vector<int> sortedNode;
    if(!TopologicalSorting(&graph, sortedNode))
    {
        std::cout << "Graph has circle!" << std::endl;
        return -1;
    }
    PrintSorting(&graph, sortedNode);
	return 0;
}

