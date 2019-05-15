#pragma once

#include "Searcher.h"

class MinimaxSearcher : public Searcher
{
public:
    MinimaxSearcher(void);
    virtual ~MinimaxSearcher(void);

    virtual int SearchBestPlay(const GameState& state, int depth);

#ifdef _DEBUG
    int _searcherCounter;
#endif

protected:
    int MiniMax(GameState& state, int depth, int max_player_id);
};
