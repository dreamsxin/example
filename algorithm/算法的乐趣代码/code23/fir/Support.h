#pragma once

#include "GameConst.h"

#define COUNT_OF(x)   (sizeof(x)/sizeof(x[0]))

const int MAX_FORBIDDEN_PATTERN = 12;

typedef struct tagForbiddenItem
{
    int off_inc[MAX_FORBIDDEN_PATTERN];
    int off_cnt;
}FORBIDDEN_ITEM;

extern FORBIDDEN_ITEM forbidden_patterns[4];

const int MAX_LINE_S = 9;

typedef struct tagLines
{
    int line_s[MAX_LINE_S];
    int off_dir;
}LINES;

extern LINES line_cpts[4];


inline int GetPeerPlayer(int player_id) { return (player_id == PLAYER_A) ? PLAYER_B : PLAYER_A; };

