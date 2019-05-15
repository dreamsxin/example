#pragma once

const int C_TWO = 0;
const int H_TWO = 1;
const int C_THREE = 2;
const int H_THREE = 3;
const int C_FOUR = 4;
const int H_FOUR = 5;
const int S_FIVE = 6;

class EvaluatorData
{
public:
    EvaluatorData(void);
    ~EvaluatorData(void);

    void IncreaseCounter(int rl_count, int player_id, bool bClose);
    int GetCounter(int rl_count, int player_id, bool bClose);
protected:
    int m_counter[2][7];
};
