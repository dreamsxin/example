#pragma once
#include "evaluator.h"

class FeEvaluator : public Evaluator
{
public:
    FeEvaluator(void);
    virtual ~FeEvaluator(void);

    virtual int Evaluate(GameState& state, int max_player_id);

protected:
    void CountPlayerChess(GameState& state, int player_id, int& countOne, int& countTwo, int& countThree);
};
