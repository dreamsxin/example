#include "StdAfx.h"
#include "Support.h"
#include "NegamaxAlphaBetaSearcher.h"
#include "TranspositionTable.h"


bool MoveGreater (MOVES_LIST elem1, MOVES_LIST elem2)
{
   return elem1.goodness > elem2.goodness;
}


void SortMoves(std::vector<MOVES_LIST>& moves)
{
    sort(moves.begin(), moves.end(), MoveGreater);
}

NegamaxAlphaBetaSearcher::NegamaxAlphaBetaSearcher(void)
{
    srand((unsigned)time(NULL));
#ifdef _DEBUG
    _searcherCounter = 0;
#endif
    InitTranspositionTable();
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

    ResetTranspositionTable();

    GameState tryState = state;
    int player_id = state.GetCurrentPlayer();

    std::vector<MOVES_LIST> moves;
    int mc = tryState.FindMoves(player_id, moves);
    if(mc == 0) //遇到无路可走的情况，比如被对方逼着走禁手，可放弃一步
    {
        return -1;
    }
    SortMoves(moves);
    for(int i = 0; i < mc; i++)
    {
        tryState.DoPutChess(moves[i].cell, player_id);
        int value = NegaMax(tryState, depth - 1, -INFINITY, INFINITY, player_id);
        tryState.UndoPutChess(moves[i].cell);
        if(value > bestValue)
        {
            bestValue = value;
            bestCell.clear();
            bestCell.push_back(moves[i].cell);
        }
        else if(value == bestValue)
        {
            bestCell.push_back(moves[i].cell);
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
    int alphaOrig = alpha;
#if 0
    unsigned int state_hash = state.GetHash();

    //查询置换表
    TT_ENTRY ttEntry = { 0 };
    if(LookupTranspositionTable(state_hash, ttEntry) && (ttEntry.depth >= depth))
    {
        if(ttEntry.flag == TT_FLAG_EXACT)
            return ttEntry.value;
        else if(ttEntry.flag == TT_FLAG_LOWERBOUND)
            alpha = std::max(alpha, ttEntry.value);
        else// if(ttEntry.flag == TT_FLAG_UPPERBOUND)
            beta = std::min(beta, ttEntry.value);

        if(beta <= alpha)
            return ttEntry.value;
    }
#endif
    if(state.IsGameOver() || (depth == 0))
    {
#ifdef _DEBUG
        _searcherCounter++;
#endif
        return EvaluateNegaMax(state, max_player_id);
    }
    
    state.SwitchPlayer();
    int score = -INFINITY;
    int player_id = state.GetCurrentPlayer();

    std::vector<MOVES_LIST> moves;
    int mc = state.FindMoves(player_id, moves);
    SortMoves(moves);
    for(int i = 0; i < mc; i++)
    {
        state.DoPutChess(moves[i].cell, player_id);
        int value = -NegaMax(state, depth - 1, -beta, -alpha, max_player_id);
        state.UndoPutChess(moves[i].cell);
        score = std::max(score, value);
        alpha = std::max(alpha, value);
        if(beta <= alpha)
            break;
    }

    state.SwitchPlayer();
#if 0
    //写入置换表
    ttEntry.value = score;
    if(score <= alphaOrig)
        ttEntry.flag = TT_FLAG_UPPERBOUND;
    else if(score >= beta)
        ttEntry.flag = TT_FLAG_LOWERBOUND;
    else
        ttEntry.flag = TT_FLAG_EXACT;

    ttEntry.depth = depth;
    StoreTranspositionTable(state_hash, ttEntry);
#endif
    return score;
}
