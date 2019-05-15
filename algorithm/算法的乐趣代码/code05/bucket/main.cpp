// bucket.cpp 
//

#include "stdafx.h"
#include "bucket_state.h"


bool IsSameBucketState(BucketState state1, BucketState state2)
{
    return state1.IsSameState(state2);
}

bool IsProcessedState(deque<BucketState>& states, const BucketState& newState)
{
    deque<BucketState>::iterator it = states.end();
   
    it = find_if( states.begin(), states.end(), 
                  bind2nd(ptr_fun(IsSameBucketState), newState) );

    //it = find_if(states.begin(), states.end(), bind1st(mem_fun_ref(&BucketState::IsSameState), newState));

    return (it != states.end());
}

void PrintResult(deque<BucketState>& states)
{
    cout << "Find Result : " << endl;
    for_each(states.begin(), states.end(), 
             mem_fun_ref(&BucketState::PrintStates));
    cout << endl << endl;
}

bool IsCurrentActionValid(BucketState& current, int from, int to)
{
    /*不是同一个桶，且from桶中有水，且to桶中不满*/
    if( (from != to) 
        && !current.IsBucketEmpty(from) 
        && !current.IsBucketFull(to) )
    {
        return true;
    }

    return false;
}

void SearchState(deque<BucketState>& states);

void SearchStateOnAction(deque<BucketState>& states, BucketState& current, int from, int to)
{
    if(IsCurrentActionValid(current, from, to))
    {
        BucketState next;
         /*从from到to倒水，如果成功，返回倒水后的状态*/
        bool bDump = current.DumpWater(from, to, next);
        if(bDump && !IsProcessedState(states, next))
        {
            states.push_back(next);
            SearchState(states);
            states.pop_back();
        }
    }
}

void SearchState(deque<BucketState>& states)
{
    BucketState current = states.back(); /*每次都从当前状态开始*/
    if(current.IsFinalState())
    {
        PrintResult(states);
        return;
    }

    /*使用两重循环排列组合6种倒水状态*/
    for(int j = 0; j < buckets_count; ++j)
    {
        for(int i = 0; i < buckets_count; ++i)
        {
            SearchStateOnAction(states, current, i, j);
        }
    }
}


int main(int argc, char* argv[])
{
    deque<BucketState> states;
    BucketState init;

    states.push_back(init);
    SearchState(states);

    assert(states.size() == 1); /*穷举结束后states应该还是只有一个初始状态*/

    return 0;
}
