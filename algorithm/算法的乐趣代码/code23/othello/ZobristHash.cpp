#include "StdAfx.h"
#include "ZobristHash.h"

static bool zobristHashInit = false;

static ZOBRIST_HASH zob_hash;

void InitZobristHashTbl()
{
    srand((unsigned)time(NULL));

    for(int i = 0; i < GAME_CELLS; i++)
    {
        for(int j = 0; j < PLAYER_TYPE; j++)
        {
            zob_hash.key[i][j] = rand() | ((unsigned int)rand() << 15);
        }
    }
    zobristHashInit = true;
}

ZOBRIST_HASH *GetZobristHashTbl()
{
    assert(zobristHashInit);

    return &zob_hash;
}
