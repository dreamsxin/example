#pragma once

#include "Searcher.h"

class AlphaBetaSearcher : public Searcher
{
public:
    AlphaBetaSearcher(void);
    virtual ~AlphaBetaSearcher(void);

    virtual int SearchBestPlay(const GameState& state, int depth);

#ifdef _DEBUG
    int _searcherCounter;
#endif

protected:
    int AlphaBeta(GameState& state, int depth, int alpha, int beta, int max_player_id);
};
