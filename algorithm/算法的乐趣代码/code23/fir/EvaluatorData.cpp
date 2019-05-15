#include "StdAfx.h"
#include "EvaluatorData.h"
#include "Support.h"



EvaluatorData::EvaluatorData(void)
{
    memset(m_counter, 0, sizeof(int) * 2 * 7);
}

EvaluatorData::~EvaluatorData(void)
{
}

void EvaluatorData::IncreaseCounter(int rl_count, int player_id, bool bClose)
{
    int player_idx = player_id - 1;
    int type_base = (rl_count - 2) * 2;
    if(rl_count != 5)
    {
        type_base += (bClose ? 0 : 1);
    }
    m_counter[player_idx][type_base]++;
}

int EvaluatorData::GetCounter(int rl_count, int player_id, bool bClose)
{
    int player_idx = player_id - 1;
    int type_base = (rl_count - 2) * 2;
    if(rl_count != 5)
    {
        type_base += (bClose ? 0 : 1);
    }
    return m_counter[player_idx][type_base];
}

