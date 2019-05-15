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
    int opp_id =  GetPeerPlayer(max_player_id);

    EvaluatorData ev_data;
    state.SearchPatterns(ev_data);

    if(ev_data.GetCounter(5, opp_id, false) > 0)
        return -10000;
    if( (ev_data.GetCounter(4, opp_id, false) > 0) 
        || (ev_data.GetCounter(4, opp_id, true) > 1)
        || ((ev_data.GetCounter(4, opp_id, true) > 0)&&(ev_data.GetCounter(3, opp_id, false) > 0)) )
    {
        return -9900;
    }
    if(ev_data.GetCounter(3, opp_id, false) > 1) 
    {
        return -9800;
    }
    if( (ev_data.GetCounter(3, opp_id, false) > 0) 
        && (ev_data.GetCounter(3, opp_id, true) > 0) )
    {
        return -9700;
    }
    if(ev_data.GetCounter(4, opp_id, true) > 1) 
    {
        return -9600;
    }
    if( (ev_data.GetCounter(4, opp_id, true) > 0) 
        || (ev_data.GetCounter(3, opp_id, false) > 0) )
    {
        return -9500;
    }
    if(ev_data.GetCounter(5, max_player_id, false) > 0)
        return 10000;
    if( (ev_data.GetCounter(4, max_player_id, false) > 0) 
        || (ev_data.GetCounter(4, max_player_id, true) > 1)
        || ((ev_data.GetCounter(4, max_player_id, true) > 0)&&(ev_data.GetCounter(3, max_player_id, false) > 0)) )
    {
        return 9900;
    }
    if(ev_data.GetCounter(3, max_player_id, false) > 1) 
    {
        return 9800;
    }
    if( (ev_data.GetCounter(3, max_player_id, false) > 0) 
        && (ev_data.GetCounter(3, max_player_id, true) > 0) )
    {
        return 9700;
    }
    if(ev_data.GetCounter(4, max_player_id, true) > 1) 
    {
        return 9600;
    }
    if( (ev_data.GetCounter(4, max_player_id, true) > 0) 
        || (ev_data.GetCounter(3, max_player_id, false) > 0) )
    {
        return 9500;
    }
    int maxev = ev_data.GetCounter(3, max_player_id, true) * 300;
    int minev = ev_data.GetCounter(3, opp_id, true) * 300;
    maxev += ev_data.GetCounter(2, max_player_id, false) * 200;
    minev += ev_data.GetCounter(2, opp_id, false) * 200;
    maxev += ev_data.GetCounter(2, max_player_id, true) * 50;
    minev += ev_data.GetCounter(2, opp_id, true) * 50;
    maxev += state.CountPosValue(max_player_id);
    minev += state.CountPosValue(opp_id);

    return (maxev - minev);
}

