#pragma once

#include "GameConst.h"

typedef struct tagZobristHash
{
    unsigned int key[GAME_CELLS][PLAYER_TYPE];
}ZOBRIST_HASH;


void InitZobristHashTbl();
ZOBRIST_HASH *GetZobristHashTbl();

