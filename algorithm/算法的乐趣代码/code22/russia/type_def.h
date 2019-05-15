#ifndef __TYPE_DEF_H__
#define __TYPE_DEF_H__

typedef enum tagShapeT
{
    S_NULL = 0,
    S_S,
    S_Z,
    S_L,
    S_J,
    S_I,
    S_O,
    S_T,
    S_B //边界特殊类型
}SHAPE_T;

const int SHAPE_COUNT = 7;
const int BOARD_ROW = 22;
const int BOARD_COL = 12;
const int GAME_ROW = 20;
const int GAME_COL = 10;
const int SHAPE_BOX = 4;
const int MAX_SHAPE_R = 4;
const int LINE_SCORE = 100;

typedef struct tagRussiaGame
{
    int board[BOARD_ROW][BOARD_COL];
    int top_row;
    int old_top_row; //used only for AddShapeOnGame() and RemoveShapeFromGame() function
    int score;
    int lines;
}RUSSIA_GAME;

typedef struct tagBShape
{
    int r_index;
    int shape[SHAPE_BOX][SHAPE_BOX];
    int width;
    int height;
}B_SHAPE;

typedef struct tagRShape
{
    B_SHAPE shape_r[MAX_SHAPE_R];
    int r_count;
}R_SHAPE;

typedef struct tagEvaluateResult
{
    int r_index;
    int row,col;
    int value;
    int prs;
}EVA_RESULT;

#endif //__TYPE_DEF_H__