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
    int opp_player_id = GetPeerPlayer(player_id);

    std::vector<MOVES_LIST> moves;
    int mc = tryState.FindMoves(player_id, opp_player_id, moves);
    if(mc == 0)
    {
        return -1;
    }
    SortMoves(moves);

    std::vector<int> flips;
    for(int i = 0; i < mc; i++)
    {
        tryState.DoPutChess(moves[i].em, player_id, flips);
        tryState.SwitchPlayer();
        int value = -NegaMax(tryState, depth - 1, -INFINITY, INFINITY, player_id);
        tryState.UndoPutChess(moves[i].em, player_id, flips);
        tryState.SwitchPlayer();
        if(value > bestValue)
        {
            bestValue = value;
            bestCell.clear();
            bestCell.push_back(moves[i].em->cell);
        }
        else if(value == bestValue)
        {
            bestCell.push_back(moves[i].em->cell);
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

    unsigned int state_hash = state.GetZobristHash();

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

    if(state.IsGameOver() || (depth == 0))
    {
#ifdef _DEBUG
        _searcherCounter++;
#endif
        return EvaluateNegaMax(state, max_player_id);
        //return state.Evaluate(max_player_id);
    }
    
    int score = -INFINITY;
    int player_id = state.GetCurrentPlayer();
    int opp_player_id = GetPeerPlayer(player_id);

    std::vector<MOVES_LIST> moves;
    int mc = state.FindMoves(player_id, opp_player_id, moves);
    if(mc != 0)
    {
        SortMoves(moves);

        std::vector<int> flips;
        for(int i = 0; i < mc; i++)
        {
            state.DoPutChess(moves[i].em, player_id, flips);
            state.SwitchPlayer();
            int value = -NegaMax(state, depth - 1, -beta, -alpha, max_player_id);
            state.UndoPutChess(moves[i].em, player_id, flips);
            state.SwitchPlayer();
            score = std::max(score, value);
            alpha = std::max(alpha, value);
            if(beta <= alpha)
                break;
        }
    }
    else
    {
/*
        if(state.GetCurrentPlayer() == max_player_id)
            return -INFINITY;
        else
            return INFINITY;
*/
        state.SwitchPlayer();
        score = -NegaMax(state, depth - 1, -beta, -alpha, max_player_id);
        state.SwitchPlayer();
    }

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

    return score;
}
