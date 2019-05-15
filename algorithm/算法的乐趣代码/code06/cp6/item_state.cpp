// item_state.cpp : implement file for item_state class
//

#include "stdafx.h"
#include "river_def.h"
#include "action_description.h"
#include "item_state.h"

const int monster_count = 3;
const int monk_count    = 3;

ItemState::ItemState()
{
    SetState(monster_count, monk_count, 0, 0, LOCAL);
    curAct = INVALID_ACTION_NAME;
}

ItemState::ItemState(const ItemState& state)
{
    SetState(state.local_monster, state.local_monk, 
             state.remote_monster, state.remote_monk, state.boat);
    curAct = state.curAct;
}

ItemState& ItemState::operator=(const ItemState& state)
{
    SetState(state.local_monster, state.local_monk, 
             state.remote_monster, state.remote_monk, state.boat);
    curAct = state.curAct;
  
    return *this;
}

bool ItemState::IsSameState(const ItemState& state) const
{
    return ( (local_monster == state.local_monster) 
             && (local_monk == state.local_monk) 
             && (remote_monster == state.remote_monster) 
             && (remote_monk == state.remote_monk) 
             && (boat == state.boat) );
}

void ItemState::SetState(int lmonster, int lmonk, int rmonster, int rmonk, BOAT_LOCATION bl)
{
    local_monster  = lmonster;
    local_monk     = lmonk;
    remote_monster = rmonster;
    remote_monk    = rmonk;
    boat           = bl;
}

void ItemState::PrintStates()
{
    std::cout << GetActionDescription(curAct) << std::endl;
}

bool ItemState::IsFinalState()
{
    return ( (local_monster == 0) && (local_monk == 0) 
             && (remote_monster == monster_count) && (remote_monk == monk_count) 
             && (boat == REMOTE) );
}

bool ItemState::CanTakeAction(ACTION_EFFECTION& ae) const
{
    if(boat == ae.boat_to)
        return false;
    if((local_monster + ae.move_monster) < 0 || (local_monster + ae.move_monster) > monster_count)
        return false;
    if((local_monk + ae.move_monk) < 0 || (local_monk + ae.move_monk) > monk_count)
        return false;

    return true;
}

bool ItemState::IsValidState()
{
    if((local_monk > 0) && (local_monster > local_monk))
    {
        return false;
    }
    if((remote_monk > 0) && (remote_monster > remote_monk))
    {
        return false;
    }

    return true;
}

