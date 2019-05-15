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
    int min =  GetPeerPlayer(max_player_id);
    int empty = state.CountEmptyCells();
    
    int ev = 0;
    if(empty >= 40) /*只考虑行动力*/
    {
        ev += (state.CountMobility(max_player_id) - state.CountMobility(min)) * 7;
    }
    else if((empty >= 18) && (empty < 40))
    {
        ev += (state.CountPosValue(max_player_id) - state.CountPosValue(min)) * 2;
        ev += (state.CountMobility(max_player_id) - state.CountMobility(min)) * 7;
    }
    else
    {
        ev += (state.CountPosValue(max_player_id) - state.CountPosValue(min)) * 2;
        ev += (state.CountMobility(max_player_id) - state.CountMobility(min)) * 7;
        ev += (state.CountCell(max_player_id) - state.CountCell(min)) * 2;
    }
    
    return ev;
}

