// russia.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <ctime>
#include "type_def.h"
#include "data_def.h"
#include "evaluate.h"
#include "test_evaluate.h"

SHAPE_T testShape[] = { S_Z, S_S, S_O };
SHAPE_T testShape2[] = { S_J, S_Z, S_J, S_Z, S_T, S_I, S_J, S_O, S_I, S_Z };

int eliminate_score[] = { 0, 100, 200, 400, 800, 0, 0, 0, 0 };

void GenerateTestShapeList(SHAPE_T *shapes, int count, std::vector<SHAPE_T>& sl)
{
    sl.clear();
    for(int i = 0; i < count; i++)
    {
        sl.push_back(shapes[i]);
    }
}

void GenerateShapeList(int count, std::vector<SHAPE_T>& sl)
{
    SHAPE_T last = S_NULL;
    srand((unsigned)time(NULL));

    sl.clear();
    int i = 0;
    while(i < count)
    {
        SHAPE_T t = static_cast<SHAPE_T>(rand() % SHAPE_COUNT + 1);
        if(t != last)
        {
            sl.push_back(t);
            last = t;
            i++;
        }
    }
}

void ClearRowStatus(RUSSIA_GAME *game, int row)
{
    for(int i = 0; i < GAME_COL; i++)
    {
        game->board[row + 1][i + 1] = S_NULL;
    }
}

void CopyRowStatus(RUSSIA_GAME *game, int from, int to)
{
    for(int i = 0; i < GAME_COL; i++)
    {
        game->board[to + 1][i + 1] = game->board[from + 1][i + 1];
    }
}

void AddShapeOnGame(RUSSIA_GAME *game, B_SHAPE *bs, int row, int col, bool temp)
{
    for(int i = 0; i < bs->height; i++)
    {
        for(int j = 0; j < bs->width; j++)
        {
            game->board[row + i + 1][col + j + 1] += bs->shape[i][j];
        }
    }
    if(temp)
    {
        game->old_top_row = game->top_row;
    }
    else
    {
        game->old_top_row = -1;
    }
    if(game->top_row >= row)
    {
        game->top_row = row;
        //if(temp)
        //std::cout << "AddShapeOnGame change top " << game->old_top_row << " -> " << game->top_row << std::endl;
    }
}

void RemoveShapeFromGame(RUSSIA_GAME *game, B_SHAPE *bs, int row, int col)
{
    for(int i = 0; i < bs->height; i++)
    {
        for(int j = 0; j < bs->width; j++)
        {
            game->board[row + i + 1][col + j + 1] -= bs->shape[i][j];
        }
    }

    if(game->old_top_row != -1)
    {
        //std::cout << "RemoveShapeFromGame change top " << game->top_row << " -> " << game->old_top_row << std::endl;
        game->top_row = game->old_top_row;
    }
}

void EliminateRow(RUSSIA_GAME *game,  int row)
{
    if((row == 0) || (row == game->top_row))
    {
        ClearRowStatus(game, row);
    }
    else
    {
        for(int i = row; i > game->top_row; i--)
        {
            CopyRowStatus(game, i - 1, i);
        }
        ClearRowStatus(game, game->top_row);
    }
}

void PutShapeInPlace(RUSSIA_GAME *game, B_SHAPE *bs, int row, int col)
{
    AddShapeOnGame(game, bs, row, col, false);
    
    bool done = false;
    int max_row = row + bs->height;
    int total = 0;
    do
    {
        done = false;
        for(int i = game->top_row; i < max_row; i++)
        {
            if(IsFullRowStatus(game, i))
            {
                EliminateRow(game, i);
                game->top_row++;
                total++;
                done = true;
                break;
            }
        }
    }while(done);

    game->score += eliminate_score[total];
    game->lines += total;
}

int GetTouchStartRow(RUSSIA_GAME *game, B_SHAPE *bs)
{
    return game->top_row - bs->height;
}

bool CanShapeMoveDown(RUSSIA_GAME *game, B_SHAPE *bs, int row, int col)
{
    int shape[SHAPE_BOX][SHAPE_BOX] = { { 0 } };
    
    if((row + bs->height) >= GAME_ROW) /*已经到底部了*/
        return false;

    row++;
    for(int i = 0; i < bs->height; i++)
    {
        for(int j = 0; j < bs->width; j++)
        {
            shape[i][j] = game->board[row + i + 1][col + j + 1] + bs->shape[i][j];
            if((shape[i][j] > 1) && (shape[i][j] != S_B))
            {
                return false;
            }
        }
    }

    return true;
}

/*rtn >= 0表示可以放下这个形状，< 0表示无法放下这个形状，可能到顶了*/
int EvaluateShape(RUSSIA_GAME *game, B_SHAPE *bs, EVA_RESULT *result)
{
    int start_row = GetTouchStartRow(game, bs);
    if(start_row < 0)
        return -1;

    for(int col = 0; col < (GAME_COL - bs->width + 1); col++)
    {
        int row = start_row;
        //是否还能向下？如果能就再下降一行，直到停下
        while(CanShapeMoveDown(game, bs, row, col))
        {
            row++;
        }
        AddShapeOnGame(game, bs, row, col, true);
        int values = EvaluateFunction(game, bs, row, col);
        int prs = PrioritySelection(game, bs->r_index, row, col);
        RemoveShapeFromGame(game, bs, row, col);
        //std::cout << "r_index : " << bs->r_index << ", row = " << row << ", col = " << col << ", value = " << values << std::endl;
        if((values > result->value) 
            || ((values == result->value) && (prs > result->prs)))
        {
            result->row = row;
            result->col = col;
            result->value = values;
            result->prs = prs;
        }
    }

    return 1;
}

bool ComputerAIPlayer(RUSSIA_GAME *game, SHAPE_T s)
{
    bool res_find = false;
    EVA_RESULT best_r = { 0, 0, 0, -999999, -999999 };

    R_SHAPE *rs = &g_shapes[s - 1];
    //遍历每个板块的形状，相当于旋转板块
    for(int i = 0; i < rs->r_count; i++)
    {
        B_SHAPE *bs = &rs->shape_r[i];
        EVA_RESULT evr = { i, 0, 0, -999999, -999999 };
        int rtn = EvaluateShape(game, bs, &evr);
        if((evr.value > best_r.value) 
            || ((evr.value == best_r.value) && (evr.prs > best_r.prs)))
        {
            res_find = true;
            best_r = evr;
        }
    }
    if(res_find)
    {
        PutShapeInPlace(game, &rs->shape_r[best_r.r_index], best_r.row, best_r.col);
        //std::cout << "Total : r_index : " << best_r.r_index << ", row = " << best_r.row << ", col = " << best_r.col << ", value = " << best_r.value << "top_row = " << game->top_row << std::endl;
    }
    else
    {
        int sss = 0;
    }

    return res_find;
}

void InitGame(RUSSIA_GAME *game)
{
    memset(game->board, S_NULL, BOARD_ROW * BOARD_COL * sizeof(int));
    game->top_row = GAME_ROW;
    game->old_top_row = -1;
    game->score = 0;
    game->lines = 0;
    for(int i = 0; i < BOARD_ROW; i++)
    {
        game->board[i][0] = S_B;
        game->board[i][BOARD_COL - 1] = S_B;
    }
    for(int j = 1; j < BOARD_COL - 1; j++)
    {
        game->board[0][j] = S_B;
        game->board[BOARD_ROW - 1][j] = S_B;
    }
}

void PrintGame(RUSSIA_GAME *game, SHAPE_T next)
{
    for(int i = 0; i < BOARD_ROW; i++)
    {
        for(int j = 0; j < BOARD_COL; j++)
        {
            std::cout << shape_char[game->board[i][j]] << ' ';
        }

        if((next != S_NULL) && (i <= SHAPE_BOX))
        {
            std::cout << "\t";
            if(i == 0)
            {
                std::cout << "next : ";
            }
            else
            {
                R_SHAPE *rs = &g_shapes[next - 1];
                B_SHAPE *bs = &rs->shape_r[0];
                for(int k = 0; k < SHAPE_BOX; k++)
                {
                    std::cout << shape_char[bs->shape[i - 1][k]] << ' ';
                }

            }
        }
        std::cout << std::endl;
    }
    std::cout << "Total Lines : " << game->lines << ", Total Socre: " << game->score << std::endl;
}

int main(int argc, char* argv[])
{
    std::vector<SHAPE_T> shape_list;
    RUSSIA_GAME game;

    //RunEvaluateTest();

    InitGame(&game);

    GenerateShapeList(100000, shape_list);
    //GenerateTestShapeList(testShape2, 10, shape_list);
    for(int i = 0; i < static_cast<int>(shape_list.size()); i++)
    {
        //PrintGame(&game, shape_list[i]);
        if(!ComputerAIPlayer(&game, shape_list[i]))
        {
            std::cout << "Failed at: " << i + 1 << " pieces!" << std::endl;
            break;
        }
    }
    PrintGame(&game, S_NULL);

    return 0;
}

