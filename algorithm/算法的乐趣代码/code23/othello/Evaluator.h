#pragma once

class GameState;

class Evaluator
{
protected:
    Evaluator(void)
    {
    }

public:
    virtual ~Evaluator(void)
    {
    }

    virtual int Evaluate(GameState& state, int max_player_id) = NULL;
};

