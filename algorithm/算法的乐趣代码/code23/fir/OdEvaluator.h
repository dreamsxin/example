#pragma once
#include "evaluator.h"

class OdEvaluator : public Evaluator
{
public:
    OdEvaluator(void);
    virtual ~OdEvaluator(void);

    virtual int Evaluate(GameState& state, int max_player_id);

protected:
};
