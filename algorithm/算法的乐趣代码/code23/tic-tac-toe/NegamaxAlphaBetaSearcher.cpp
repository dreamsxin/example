#include "StdAfx.h"
#include "NegamaxAlphaBetaSearcher.h"

NegamaxAlphaBetaSearcher::NegamaxAlphaBetaSearcher(void)
{
    srand((unsigned)time(NULL));
#ifdef _DEBUG
    _searcherCounter = 0;
#endif
}

NegamaxAlphaBetaSearcher::~NegamaxAlphaBetaSearcher(void)
{
}

int NegamaxAlphaBetaSearcher::SearchBestPlay(const GameState& state, int depth)
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
            int value = -NegaMax(tryState, depth - 1, -INFINITY, INFINITY, state.GetCurrentPlayer());
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
    std::cout << "NegamaxSearcher " << _searcherCounter << " (with Alpha-Beta)" << std::endl;
#endif

    return bestCell[bestPos];
}

int NegamaxAlphaBetaSearcher::EvaluateNegaMax(GameState& state, int max_player_id)
{
    if(state.GetCurrentPlayer() == max_player_id)
        return state.Evaluate(max_player_id);
    else
        return -state.Evaluate(max_player_id);
}

int NegamaxAlphaBetaSearcher::NegaMax(GameState& state, int depth, int alpha, int beta, int max_player_id)
{
    if(state.IsGameOver() || (depth == 0))
    {
#ifdef _DEBUG
        _searcherCounter++;
#endif
        return EvaluateNegaMax(state, max_player_id);
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
            int value = -NegaMax(tryState, depth - 1, -beta, -alpha, max_player_id);
            score = std::max(score, value);
            alpha = std::max(alpha, value);
            if(beta <= alpha)
                break;
        }
    }
    
    return score;
}
