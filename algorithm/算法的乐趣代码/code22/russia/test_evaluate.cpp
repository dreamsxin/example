#include "stdafx.h"
#include <memory.h>
#include <ctime>
#include <cassert>
#include "type_def.h"
#include "evaluate.h"
#include "test_evaluate.h"

int top_row1 = 20;
int game_board1[GAME_ROW][GAME_COL] = 
{
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
};

int top_row2 = 13;
int game_board2[GAME_ROW][GAME_COL] = 
{
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0,0,0,0,0,0,1,1,0,0 },
    { 0,0,1,1,1,1,1,1,0,0 },
    { 1,1,1,1,1,1,1,1,1,0 },
    { 1,1,1,1,1,0,1,1,1,1 },
    { 1,0,1,0,0,1,1,1,1,0 },
    { 1,0,1,1,1,1,0,1,1,0 },
    { 0,1,1,0,1,1,1,1,0,0 },
};

int top_row3 = 13;
int game_board3[GAME_ROW][GAME_COL] = 
{
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0,0,0,0,0,0,1,1,0,1 },
    { 0,0,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,0,1,1,1,1 },
    { 1,0,1,0,0,1,1,1,1,0 },
    { 1,0,1,1,1,1,0,1,1,0 },
    { 0,1,1,0,1,1,1,1,0,0 },
};

void PrintGme(RUSSIA_GAME *game);

void TestInitGme(RUSSIA_GAME *game, int top_row, int board[GAME_ROW][GAME_COL])
{
    memset(game->board, S_NULL, BOARD_ROW * BOARD_COL * sizeof(int));
    game->top_row = top_row;
    game->old_top_row = -1;
    game->score = 0;
    for(int i = 0; i < BOARD_ROW; i++)
    {
        game->board[i][0] = S_B;
        if((i >= 1) && (i <= GAME_ROW))
        {
            for(int k = 0; k < GAME_COL; k++)
            {
                game->board[i][k + 1] = board[i - 1][k];
            }
        }
        game->board[i][BOARD_COL - 1] = S_B;
    }
    for(int j = 1; j < BOARD_COL - 1; j++)
    {
        game->board[0][j] = S_B;
        game->board[BOARD_ROW - 1][j] = S_B;
    }

    //PrintGme(game);
}

void TestLandingHeight()
{
    RUSSIA_GAME game;

    TestInitGme(&game, top_row1, game_board1);
    int lh = GetLandingHeight(&game, NULL, 0, 0);
    assert(lh == 0);

    TestInitGme(&game, top_row2, game_board2);
    lh = GetLandingHeight(&game, NULL, 0, 0);
    assert(lh == 7);

}

void TestErodedPieceCellsMetric()
{
    RUSSIA_GAME game;

    TestInitGme(&game, top_row1, game_board1);

    B_SHAPE bs = {
                    0,
                    {
                        {0,1,0,0},
                        {1,1,0,0},
                        {0,1,0,0},
                        {0,0,0,0}
                    },
                    2,3
                 };
    int epcm = GetErodedPieceCellsMetric(&game, &bs, 18, 0);
    assert(epcm == 0);


    TestInitGme(&game, top_row3, game_board3);
    epcm = GetErodedPieceCellsMetric(&game, &bs, 13, 8);
    assert(epcm == 1);
}

void TestBoardRowTransitions()
{
    RUSSIA_GAME game;

    TestInitGme(&game, top_row1, game_board1);
    int brt = GetBoardRowTransitions(&game, NULL, 0, 0);
    assert(brt == 0);

    TestInitGme(&game, top_row2, game_board2);
    brt = GetBoardRowTransitions(&game, NULL, 0, 0);
    assert(brt == 30);
}

void TestBoardColTransitions()
{
    RUSSIA_GAME game;

    TestInitGme(&game, top_row1, game_board1);
    int bct = GetBoardColTransitions(&game, NULL, 0, 0);
    assert(bct == 0);

    TestInitGme(&game, top_row2, game_board2);
    bct = GetBoardColTransitions(&game, NULL, 0, 0);
    assert(bct == 26);
}

void TestBoardBuriedHoles()
{
    RUSSIA_GAME game;

    TestInitGme(&game, top_row1, game_board1);
    int bbh = GetBoardBuriedHoles(&game, NULL, 0, 0);
    assert(bbh == 0);

    TestInitGme(&game, top_row2, game_board2);
    bbh = GetBoardBuriedHoles(&game, NULL, 0, 0);
    assert(bbh == 9);
}

void TestBoardWells()
{
    RUSSIA_GAME game;

    TestInitGme(&game, top_row1, game_board1);
    int bw = GetBoardWells(&game, NULL, 0, 0);
    assert(bw == 0);

    TestInitGme(&game, top_row2, game_board2);
    bw = GetBoardWells(&game, NULL, 0, 0);
    assert(bw == 11);
}

int top_row4 = 16;
int game_board4[GAME_ROW][GAME_COL] = 
{
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0,0,1,0,0,0,0,0,0,0 },
    { 0,0,1,1,0,0,0,0,0,0 },
    { 0,1,1,1,0,0,0,0,0,0 },
    { 1,1,0,0,0,0,0,0,0,0 },
};
int top_row5 = 18;
int game_board5[GAME_ROW][GAME_COL] = 
{
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0 },
    { 0,1,1,0,1,1,0,0,0,0 },
    { 1,1,0,1,1,0,0,0,0,0 },
};

int top_row6 = 17;
int game_board6[GAME_ROW][GAME_COL] = 
{
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,1,0,0,0,0,0,0 },
    { 0,1,1,1,1,0,0,0,0,0 },
    { 1,1,0,0,1,0,0,0,0,0 },
};

void TestEvaluateFunction()
{
    RUSSIA_GAME game;
    B_SHAPE bs = {
                    0,
                    {
                        {1,0,0,0},
                        {1,1,0,0},
                        {0,1,0,0},
                        {0,0,0,0}
                    },
                    2,3
                 };
    TestInitGme(&game, top_row4, game_board4);
    int bw = EvaluateFunction(&game, &bs, 16, 2);

    B_SHAPE bs1 = {
                    0,
                    {
                        {0,1,1,0},
                        {1,1,0,0},
                        {0,0,0,0},
                        {0,0,0,0}
                    },
                    3,2
                 };
    TestInitGme(&game, top_row5, game_board5);
    bw = EvaluateFunction(&game, &bs1, 18, 3);

    B_SHAPE bs2 = {
                    0,
                    {
                        {1,0,0,0},
                        {1,1,0,0},
                        {0,1,0,0},
                        {0,0,0,0}
                    },
                    2,3
                 };
    TestInitGme(&game, top_row6, game_board6);
    bw = EvaluateFunction(&game, &bs2, 17, 3);
}

int top_row7 = 14;
int game_board7[GAME_ROW][GAME_COL] = 
{
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 1,1,0,0,0,0,0,0,0,0 },
    { 1,1,0,0,0,0,0,0,0,0 },
    { 1,1,1,1,1,1,0,0,0,0 },
    { 1,1,1,0,1,1,1,0,0,0 },
    { 1,1,1,1,1,1,1,1,0,0 },
    { 1,1,1,1,1,1,1,1,0,0 },
};

int top_row8 = 16;
int game_board8[GAME_ROW][GAME_COL] = 
{
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 0,0,0,0,0,0,0,0,0,0 },
    { 0,0,0,0,0,0,0,0,0,0 },
    { 1,1,1,1,1,1,0,0,0,0 },
    { 1,1,1,0,1,1,1,0,0,0 },
    { 1,1,1,1,1,1,1,1,1,1 },
    { 1,1,1,1,1,1,1,1,1,1 },
};

void TestEvaluateFunction2()
{
    RUSSIA_GAME game;
    B_SHAPE bs = {
                    0,
                    {
                        {1,1,0,0},
                        {1,1,0,0},
                        {0,0,0,0},
                        {0,0,0,0}
                    },
                    2,2
                 };
    TestInitGme(&game, top_row7, game_board7);
    int bw = EvaluateFunction(&game, &bs, 14, 0);
    TestInitGme(&game, top_row8, game_board8);
    bw = EvaluateFunction(&game, &bs, 18, 8);
}


void TestPrioritySelection()
{
}



void RunEvaluateTest()
{
    TestLandingHeight();
    TestErodedPieceCellsMetric();
    TestBoardRowTransitions();
    TestBoardColTransitions();
    TestBoardBuriedHoles();
    TestBoardWells();
    TestEvaluateFunction();
    TestEvaluateFunction2();
}
