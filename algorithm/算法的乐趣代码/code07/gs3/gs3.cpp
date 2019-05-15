// gs3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

const int UNIT_COUNT = 5;

typedef struct tagPartner
{
    char *name;   //名字
    int next;     //下一个邀请对象
    int current;  //当前舞伴，-1表示还没有舞伴
    int pCount; //偏爱列表中舞伴个数
    int perfect[UNIT_COUNT]; //偏爱列表
}PARTNER;

typedef struct tagMaxMatch
{
    int edge[UNIT_COUNT][UNIT_COUNT];
    bool on_path[UNIT_COUNT];
    int path[UNIT_COUNT];
    int max_match;
}GRAPH_MATCH;


PARTNER X[] = 
{
    {"X1", 0, -1, 1, {2} },
    {"X2", 0, -1, 2, {0,1} },
    {"X3", 0, -1, 3, {1,2,3} },
    {"X4", 0, -1, 2, {1,2} },
    {"X5", 0, -1, 3, {2,3,4} }
};
PARTNER Y[] = 
{
    {"Y1", 0, -1, 1, {1} },
    {"Y2", 0, -1, 3, {1,2,3} },
    {"Y3", 0, -1, 4, {0,2,3,4} },
    {"Y4", 0, -1, 2, {2,4} },
    {"Y5", 0, -1, 1, {4} }
};

bool FindAugmentPath(GRAPH_MATCH *match, int xi)
{
    for(int yj = 0; yj < UNIT_COUNT; yj++)
    {
        if((match->edge[xi][yj] == 1) && !match->on_path[yj])
        {
            match->on_path[yj] = true;
            if( (match->path[yj] == -1) 
                || FindAugmentPath(match, match->path[yj]) )
            {
                match->path[yj] = xi;
                return true;
            }
        }
    }

    return false;
}


void ClearOnPathSign(GRAPH_MATCH *match)
{
    for(int i = 0; i < UNIT_COUNT; i++)
    {
        match->on_path[i] = false;
    }
}

bool Hungary_Match(GRAPH_MATCH *match)
{
    for(int xi = 0; xi < UNIT_COUNT; xi++)
    {
        if(FindAugmentPath(match, xi))
        {
            match->max_match++;
        }

        ClearOnPathSign(match);
    }
    return (match->max_match == UNIT_COUNT);
}

void InitGraph(GRAPH_MATCH *match, PARTNER *X, PARTNER *Y)
{
    match->max_match = 0;
    memset(match->edge, 0, UNIT_COUNT * UNIT_COUNT * sizeof(int));
    for(int i = 0; i < UNIT_COUNT; i++)
    {
        match->on_path[i] = false;
        match->path[i] = -1;
        for(int j = 0; j < X[i].pCount; j++)
            match->edge[i][X[i].perfect[j]] = 1;
    }
}

void PrintResult(GRAPH_MATCH *match, PARTNER *X, PARTNER *Y)
{
    for(int i = 0; i < match->max_match; i++)
    {
        std::cout << X[match->path[i]].name << "<--->" << Y[i].name << std::endl;
    }
}

void PrintResult2(GRAPH_MATCH *match, PARTNER *X, PARTNER *Y)
{
    int path[UNIT_COUNT] = { 0 };

    for(int i = 0; i < match->max_match; i++)
    {
        path[match->path[i]] = i;
    }
    for(int i = 0; i < match->max_match; i++)
    {
        std::cout << X[i].name << "<--->" << Y[path[i]].name << std::endl;
    }
}

int main(int argc, char* argv[])
{
    GRAPH_MATCH match = { 0 };

    InitGraph(&match, X, Y);
    if(Hungary_Match(&match))
    {
        PrintResult2(&match, X, Y);
    }

	return 0;
}

