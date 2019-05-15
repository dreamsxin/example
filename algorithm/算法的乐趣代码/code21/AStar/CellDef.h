#ifndef __CELL_DEFINE_H__
#define __CELL_DEFINE_H__

const int N_SCALE = 16;

const int CELL_NORMAL = 0;
const int CELL_MARK   = 1;
const int CELL_WALL   = 2;
const int CELL_SOURCE = 3;
const int CELL_TARGET = 4;

typedef struct tagCell
{
    int node_idx;
    int type; //0:normal,1:mark,2:wall,3:source,4:target
    bool inPath;
    bool processed;
}CELL;

typedef struct tagGridCell
{
    CELL cell[N_SCALE][N_SCALE];
}GRID_CELL;

typedef struct tagGridInit
{
    int n;
    int grid[N_SCALE][N_SCALE];
}GRID_INIT;


#endif //__CELL_DEFINE_H__
