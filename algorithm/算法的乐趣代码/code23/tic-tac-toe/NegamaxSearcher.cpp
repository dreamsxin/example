#include "StdAfx.h"
#include "NegamaxSearcher.h"

NegamaxSearcher::NegamaxSearcher(void)
{
    srand((unsigned)time(NULL));
#ifdef _DEBUG
    _searcherCounter = 0;
#endif
}

NegamaxSearcher::~NegamaxSearcher(void)
{
}

int NegamaxSearcher::SearchBestPlay(const GameState& state, int depth)
{
    std::vector<int> bestCell;
    int bestValue = -INFINITY;
    int bestPos = 0;
#ifdef _DEBUG
    _searcherCounter = 0;
#endif

    for(int i = 0; i < BOARD_CELLS; i++)
    {
        GameState tryState = state;
        if(tryState.IsEmptyCell(i))
        {
            tryState.SetGameCell(i, tryState.GetCurrentPlayer());
            tryState.SwitchPlayer();
            int value = -NegaMax(tryState, depth - 1, -1, state.GetCurrentPlayer());
            if(value > bestValue)
            {
                bestValue = value;
                bestCell.clear();
                bestCell.push_back(i);
            }
            else if(value == bestValue)
            {
                bestCell.push_back(i);
            }
        }
    }

    if(bestCell.size() > 0)
        bestPos = rand() % bestCell.size();

#ifdef _DEBUG
    std::cout << "NegamaxSearcher " << _searcherCounter << " (without Alpha-Beta)" << std::endl;
#endif

    return bestCell[bestPos];
}

int NegamaxSearcher::EvaluateNegaMax(GameState& state, int max_player_id)
{
    if(state.GetCurrentPlayer() == max_player_id)
        return state.Evaluate(max_player_id);
    else
        return -state.Evaluate(max_player_id);
}

int NegamaxSearcher::NegaMax(GameState& state, int depth, int color, int max_player_id)
{
    if(state.IsGameOver() || (depth == 0))
    {
#ifdef _DEBUG
        _searcherCounter++;
#endif
        int val = EvaluateNegaMax(state, max_player_id);
        //val = color * state.Evaluate(max_player_id);
        return val;
        //return state.Evaluate(max_player_id);
    }
    
    int score = -INFINITY;
    for(int i = 0; i < BOARD_CELLS; i++)
    {
        GameState tryState = state;
        if(tryState.IsEmptyCell(i))
        {
            tryState.SetGameCell(i, tryState.GetCurrentPlayer());
            tryState.SwitchPlayer();
            int value = -NegaMax(tryState, depth - 1, -color, max_player_id);
            score = std::max(score, value);
        }
    }
    
    return score;
}
#if 0
int CNegamaxSearcher::SearchBestPlay(const CGameState& state, int depth)
{
    int bestValue[BOARD_CELLS] = { 0 };
    int bestCell[BOARD_CELLS] = { 0 };
    int best = 0;

#ifdef _DEBUG
    _searcherCounter = 0;
#endif

    bestValue[best] = -INFINITY;
    int i;
    for(i = 0; i < BOARD_CELLS; i++)
    {
        CGameState tryState = state;
        if(tryState.IsEmptyCell(i))
        {
            tryState.SetGameCell(i, tryState.GetPlayerRole());
            tryState.SwitchPlayer();
            int value = -NegaMax(tryState, depth - 1);
            if(value > bestValue[best])
            {
                best = 0;
                bestValue[best] = value;
                bestCell[best] = i;
            }
            else if(value == bestValue[best])
            {
                bestValue[best] = value;
                bestCell[best] = i;
                best++;
            }
        }
    }

    if(best > 0)
        best = rand() % best;

#ifdef _DEBUG
    std::cout << "NegamaxSearcher " << _searcherCounter << " (without Alpha-Beta)" << std::endl;
#endif

    return bestCell[best];
}

int CNegamaxSearcher::EvaluateNegaMax(CGameState& state)
{
    if(state.IsComputerPlayer())
        return state.Evaluate();
    else
        return -state.Evaluate();
}

int CNegamaxSearcher::NegaMax(CGameState& state, int depth)
{
    if(state.IsGameOver() || (depth == 0))
    {
#ifdef _DEBUG
        _searcherCounter++;
#endif
        return EvaluateNegaMax(state);
    }
    
    int score = -INFINITY;
    for(int i = 0; i < BOARD_CELLS; i++)
    {
        CGameState tryState = state;
        if(tryState.IsEmptyCell(i))
        {
            tryState.SetGameCell(i, tryState.GetPlayerRole());
            tryState.SwitchPlayer();
            int value = -NegaMax(tryState, depth - 1);
            score = std::max(score, value);
            /*
            if(value >= score)
            {
                score = value;
            }*/
        }
    }
    
    return score;
}
#endif