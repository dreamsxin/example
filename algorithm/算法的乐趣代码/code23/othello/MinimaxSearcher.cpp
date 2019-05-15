#include "StdAfx.h"
#include "MinimaxSearcher.h"

MinimaxSearcher::MinimaxSearcher(void)
{
    srand((unsigned)time(NULL));
#ifdef _DEBUG
    _searcherCounter = 0;
#endif
}

MinimaxSearcher::~MinimaxSearcher(void)
{
}

int MinimaxSearcher::SearchBestPlay(const GameState& state, int depth)
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
            int value = MiniMax(tryState, depth - 1, state.GetCurrentPlayer());
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
    std::cout << "MinimaxSearcher " << _searcherCounter << " (without Alpha-Beta)" << std::endl;
#endif

    return bestCell[bestPos];
}

int MinimaxSearcher::MiniMax(GameState& state, int depth, int max_player_id)
{
    if(state.IsGameOver() || (depth == 0))
    {
#ifdef _DEBUG
        _searcherCounter++;
#endif
        return state.Evaluate(max_player_id);
    }
    
    int score = (state.GetCurrentPlayer() == max_player_id) ? -INFINITY : INFINITY;
    for(int i = 0; i < BOARD_CELLS; i++)
    {
        GameState tryState = state;
        if(tryState.IsEmptyCell(i))/*此位置可以落子*/
        {
            tryState.SetGameCell(i, tryState.GetCurrentPlayer());
            tryState.SwitchPlayer();
            int value = MiniMax(tryState, depth - 1, max_player_id);
            if(state.GetCurrentPlayer() == max_player_id)
            {
                score = std::max(score, value);
                /*
                if(value > score)
                {
                    score = value;
                }*/
            }
            else
            {
                score = std::min(score, value);
                /*
                if(value < score)
                {
                    score = value;
                }*/
            }
        }
    }
    
    return score;
}
#if 0
int CMinimaxSearcher::MiniMax(CGameState& state, int depth)
{
    if(state.IsGameOver())
        return state.Evaluate();
    if(depth == 0)
        return state.Evaluate();
    
    int bestValue[BOARD_CELLS] = { 0 };
    int best = 0;
    if(state.IsComputerPlayer())
        bestValue[best] = -INFINITY;
    else
        bestValue[best] = INFINITY;

    CGameState tryState = state;
    int i;
    for(i = 0; i < BOARD_CELLS; i++)
    {
        if(tryState.IsEmptyCell(i))
        {
            tryState.SetGameCell(i, tryState.GetPlayerRole());
            tryState.SwitchPlayer();
            int value = MiniMax(tryState, depth - 1);
            tryState.SwitchPlayer();
            tryState.SetGameCell(i, CELL_EMPTY);
            
            if(state.IsComputerPlayer())
            {
                if(value > bestValue[best])
                {
                    best = 0;
                    bestValue[best] = value;
                }
                else if(value == bestValue[best])
                {
                    best++;
                    bestValue[best] = value;
                }
            }
            else
            {
                if(value < bestValue[best])
                {
                    best = 0;
                    bestValue[best] = value;
                }
                else if(value == bestValue[best])
                {
                    best++;
                    bestValue[best] = value;
                }
            }
        }
    }
    if(best > 0)
        best = rand() % best;
    
    return bestValue[best];
}
#endif