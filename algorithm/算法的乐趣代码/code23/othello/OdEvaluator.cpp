#include "StdAfx.h"
#include "OdEvaluator.h"
#include "GameState.h"
#include "Support.h"


OdEvaluator::OdEvaluator(void)
{
}

OdEvaluator::~OdEvaluator(void)
{
}

int OdEvaluator::Evaluate(GameState& state, int max_player_id)
{
    int min =  GetPeerPlayer(max_player_id);

    int ev = (state.CountPosValue(max_player_id) - state.CountPosValue(min)) * 2;
    ev += (state.CountMobility(max_player_id) - state.CountMobility(min)) * 7;
    
    return ev;
}

