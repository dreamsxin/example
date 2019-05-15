#include "StdAfx.h"
#include "WzEvaluator.h"
#include "GameState.h"
#include "Support.h"

WzEvaluator::WzEvaluator(void)
{
}

WzEvaluator::~WzEvaluator(void)
{
}

int WzEvaluator::Evaluate(GameState& state, int max_player_id)
{
    int min = GetPeerPlayer(max_player_id);

    int aThree, aTwo, bThree, bTwo;
    int aValue = CountPlayerChess(state, max_player_id, aThree, aTwo);
    int bValue = CountPlayerChess(state, min, bThree, bTwo);

    if(aThree > 0)
    {
        return INFINITY;
    }
    if((bThree > 0) || (bTwo > 0))
    {
        return -INFINITY;
    }

    return (aThree - bThree) + (aTwo - bTwo);
}

int WzEvaluator::CountPlayerChess(GameState& state, int player_id, int& countThree, int& countTwo)
{
    countThree = 0;
    countTwo = 0;
    int lines = 0;
    for(int i = 0; i < LINE_DIRECTION; i++)
    {
        int sameCount = 0;
        int empty = 0;
        for(int j = 0; j < LINE_CELLS; j++)
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
        if(sameCount == 3)
        {
            countThree++;
        }
        if((sameCount == 2) && (empty == 1))
        {
            countTwo++;
        }
        if((sameCount + empty) == 3)
        {
            lines++;
        }
    }

    return lines;
}
