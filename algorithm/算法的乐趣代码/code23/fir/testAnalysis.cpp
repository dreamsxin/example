#include "StdAfx.h"
#include "testAnalysis.h"
#include "GameState.h"
/*
int test_state1[GAME_CELLS] = 
{
    1,1,2,1,1,1,0,0,2,2,2,0,0,1,1,  //1：一个冲三， 2：一个活三
    0,0,2,1,1,1,1,1,0,0,2,1,1,1,0,  //1: 一个五子
    0,1,2,1,1,1,2,0,1,0,1,1,1,1,0,  //1: 一个活四
    1,1,0,1,1,1,0,2,2,2,2,1,1,1,0,  //1:一个冲三，2：一个冲四
    2,2,2,2,2,1,0,0,0,1,1,1,1,1,0,  //1：一个五子，2：一个五子
    0,2,2,0,0,0,1,1,1,0,0,2,2,1,0,  //1: 一个活三，2：一个活二
    0,0,2,1,1,1,0,0,0,0,2,1,1,1,0,  //1: 一个冲三
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  //1：一个五子
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  //              2:一个五子
    0,1,1,1,1,2,2,2,0,0,2,2,2,2,2,  //1：一个冲四，2：一个冲三，一个五子
    2,1,2,1,2,2,2,0,0,0,2,2,0,1,0,  //               2: 一个冲三，一个活二
    2,1,2,1,1,1,0,0,2,2,2,0,0,1,0,  //1: 一个冲三  2: 一个活三
    2,1,2,1,1,1,2,0,1,0,2,2,1,1,0,
    0,1,2,1,0,1,2,0,0,0,2,1,2,1,2,
    0,0,2,1,1,0,2,0,0,0,2,1,1,1,0
};
*/
int test_state1[GAME_CELLS] = 
{
    1,1,1,0,2,2,2,2,1,  //1:一个冲三，2：一个冲四
    2,2,1,0,0,0,1,1,1,  //1：一个五子，2：一个五子
    0,0,0,1,1,1,0,0,2,  //1: 一个活三，2：一个活二
    1,1,1,0,0,0,0,2,1,  //1: 一个冲三
    1,1,1,1,1,1,1,1,1,  //1：一个五子
    2,2,2,2,2,2,2,2,2,  //              2:一个五子
    1,1,2,2,2,0,0,2,2,  //1：一个冲四，2：一个冲三，一个五子
    1,2,2,2,0,0,0,2,2,  //               2: 一个冲三，一个活二
    1,1,1,0,0,2,2,2,0  //1: 一个冲三  2: 一个活三
};
void TestStateAnalysis()
{
    GameState state1;
    state1.InitGameState(PLAYER_A);
    state1.SetGameState(test_state1, PLAYER_A);

    EvaluatorData ev_data;
    state1.SearchPatterns(ev_data);
    assert(ev_data.GetCounter(3, PLAYER_A, true) == 4);
    assert(ev_data.GetCounter(3, PLAYER_A, false) == 1);
    assert(ev_data.GetCounter(4, PLAYER_A, true) == 1);
    assert(ev_data.GetCounter(4, PLAYER_A, false) == 1);
    assert(ev_data.GetCounter(5, PLAYER_A, false) == 3);


    assert(ev_data.GetCounter(2, PLAYER_B, true) == 0);
    assert(ev_data.GetCounter(2, PLAYER_B, false) == 2);
    assert(ev_data.GetCounter(3, PLAYER_B, true) == 2);
    assert(ev_data.GetCounter(3, PLAYER_B, false) == 2);
    assert(ev_data.GetCounter(4, PLAYER_B, true) == 1);
    assert(ev_data.GetCounter(4, PLAYER_B, false) == 0);
    assert(ev_data.GetCounter(5, PLAYER_B, false) == 3);
}
