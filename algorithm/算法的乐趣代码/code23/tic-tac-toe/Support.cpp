#include "StdAfx.h"
#include "Support.h"


int line_idx_tbl[LINE_DIRECTION][LINE_CELLS] = 
{
    {0, 1, 2}, //第一行
    {3, 4, 5}, //第二行
    {6, 7, 8}, //第三行
    {0, 3, 6}, //第一列
    {1, 4, 7}, //第二列
    {2, 5, 8}, //第三列
    {0, 4, 8}, //正交叉线
    {2, 4, 6}, //反交叉线
};