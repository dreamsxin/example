#pragma once

#include "GameState.h"

class Searcher
{
public:
    virtual int SearchBestPlay(const GameState& state, int depth) = 0;
};
