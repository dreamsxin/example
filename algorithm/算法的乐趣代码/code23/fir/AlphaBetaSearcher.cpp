#include "StdAfx.h"
#include "AlphaBetaSearcher.h"

AlphaBetaSearcher::AlphaBetaSearcher(void)
{
    srand((unsigned)time(NULL));
#ifdef _DEBUG
    _searcherCounter = 0;
#endif
}

AlphaBetaSearcher::~AlphaBetaSearcher(void)
{
}

int AlphaBetaSearcher::SearchBestPlay(const GameState& state, int depth)
{
    std::vector<int> bestCell;
    int bestValue = -INFINITY;
    int bestPos = 0;
#ifdef _DEBUG
    _searcherCounter = 0;
#endif

    GameState tryState = state;
    std::vector<POSI> validPos;
    int moves = tryState.Analyse(tryState.GetCurrentPlayer(), validPos);
    if(moves == 0)
    {
        return -1;
    }
    for(int i = 0; i < moves; i++)
    {
        GameState ts = tryState;
        ts.Eat(validPos[i], ts.GetCurrentPlayer());
        ts.SwitchPlayer();
        int value = AlphaBeta(ts, depth - 1, -INFINITY, INFINITY, state.GetCurrentPlayer());
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

    if(bestCell.size() > 0)
        bestPos = rand() % bestCell.size();

#ifdef _DEBUG
    std::cout << "MinimaxSearcher " << _searcherCounter << " (with Alpha-Beta)" << std::endl;
#endif

    return bestCell[bestPos];
}

int AlphaBetaSearcher::AlphaBeta(GameState& state, int depth, int alpha, int beta, int max_player_id)
{
    if(state.IsGameOver() || (depth == 0))
    {
#ifdef _DEBUG
        _searcherCounter++;
#endif
        return state.Evaluate(max_player_id);
    }
    
    int moves = tryState.Analyse(tryState.GetCurrentPlayer(), validPos);
    if(moves == 0)
    {
        return -1;
    }

    if(state.GetCurrentPlayer() == max_player_id) /*极大值节点*/
    {
        std::vector<POSI> validPos;
        int moves = state.Analyse(state.GetCurrentPlayer(), validPos);
        for(int i = 0; i < moves; i++)
        {
            GameState tryState = state;
            tryState.Eat(validPos[i], tryState.GetCurrentPlayer());
            tryState.SwitchPlayer();
            int value = AlphaBeta(tryState, depth - 1, alpha, beta, max_player_id);
            alpha = std::max(alpha, value);
            if(beta <= alpha)/*beta 剪枝*/
                break;
        }

        return alpha;
    }
    else
    {
        for(int i = 0; i < BOARD_CELLS; i++)
        {
            GameState tryState = state;
            if(tryState.IsEmptyCell(i))
            {
                tryState.SetGameCell(i, tryState.GetCurrentPlayer());
                tryState.SwitchPlayer();
                int value = AlphaBeta(tryState, depth - 1, alpha, beta, max_player_id);
                beta = std::min(beta, value);
                if(beta <= alpha)/*alpha 剪枝*/
                    break;
            }
        }
        
        return beta;
    }
}


#if 0
int CAlphaBetaSearcher::AlphaBeta(CGameState& state, int depth, int alpha, int beta)
{
    if(state.IsGameOver() || (depth == 0))
    {
#ifdef _DEBUG
        _searcherCounter++;
#endif
        return state.Evaluate();
    }
    
    if(state.IsComputerPlayer()) /*极大值节点*/
    {
        for(int i = 0; i < BOARD_CELLS; i++)
        {
            CGameState tryState = state;
            if(tryState.IsEmptyCell(i))
            {
                tryState.SetGameCell(i, tryState.GetPlayerRole());
                tryState.SwitchPlayer();
                int value = AlphaBeta(tryState, depth - 1, alpha, beta);
                if(value > alpha)
                {
                    alpha = value;
                    if(alpha >= beta) /*beta 剪枝*/
                    {
                        return beta;
                    }
                }
            }
        }

        return alpha;
    }
    else
    {
        for(int i = 0; i < BOARD_CELLS; i++)
        {
            CGameState tryState = state;
            if(tryState.IsEmptyCell(i))
            {
                tryState.SetGameCell(i, tryState.GetPlayerRole());
                tryState.SwitchPlayer();
                int value = AlphaBeta(tryState, depth - 1, alpha, beta);
                if(value < beta)
                {
                    beta = value;
                    if(alpha >= beta) /*alpha 剪枝*/
                    {
                        return alpha;
                    }
                }
            }
        }
        
        return beta;
    }
}
#endif