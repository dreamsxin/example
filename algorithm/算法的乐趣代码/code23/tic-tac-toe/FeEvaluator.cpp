#include "StdAfx.h"
#include "FeEvaluator.h"
#include "GameState.h"
#include "Support.h"

FeEvaluator::FeEvaluator(void)
{
}

FeEvaluator::~FeEvaluator(void)
{
}

int FeEvaluator::Evaluate(GameState& state, int max_player_id)
{
    int min =  GetPeerPlayer(max_player_id);

    int aOne, aTwo, aThree, bOne, bTwo, bThree;
    CountPlayerChess(state, max_player_id, aOne, aTwo, aThree);
    CountPlayerChess(state, min, bOne, bTwo, bThree);

    if(aThree > 0)
    {
        return INFINITY;
    }
    if(bThree > 0)
    {
        return -INFINITY;
    }

    return (aTwo - bTwo) * DOUBLE_WEIGHT + (aOne - bOne);
}

void FeEvaluator::CountPlayerChess(GameState& state, int player_id, int& countOne, int& countTwo, int& countThree)
{
    countOne = countTwo = countThree = 0;

    int i,j;
    for(i = 0; i < LINE_DIRECTION; i++)
    {
        int sameCount = 0;
        int empty = 0;
        for(j = 0; j < LINE_CELLS; j++)
        {
            if(state.GetGameCell(line_idx_tbl[i][j]) == player_id)
            {
                sameCount++;
            }
            if(state.GetGameCell(line_idx_tbl[i][j]) == PLAYER_NULL)
            {
                empty++;
            }
        }
        if((sameCount == 1) && (empty == 2))
        {
            countOne++;
        }
        if((sameCount == 2) && (empty == 1))
        {
            countTwo++;
        }
        if((sameCount == 3) && (empty == 0))
        {
            countThree++;
        }
    }
}
