#pragma once

#include "Searcher.h"

class NegamaxAlphaBetaSearcher : public Searcher
{
public:
    NegamaxAlphaBetaSearcher(void);
    virtual ~NegamaxAlphaBetaSearcher(void);

    virtual int SearchBestPlay(const GameState& state, int depth);

#ifdef _DEBUG
    int _searcherCounter;
#endif

protected:
    int EvaluateNegaMax(GameState& state, int max_player_id);
    int NegaMax(GameState& state, int depth, int alpha, int beta, int max_player_id);
};
