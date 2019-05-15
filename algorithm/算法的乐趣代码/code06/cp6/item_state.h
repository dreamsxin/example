// item_state.h : include file for item_state class
//

#pragma once

#include "river_def.h"

struct ItemState
{
    ItemState();
    ItemState(const ItemState& state);
    ItemState& operator=(const ItemState& state);
    bool IsSameState(const ItemState& state) const;
    void SetState(int lmonster, int lmonk, int rmonster, int rmonk, BOAT_LOCATION bl);
    void PrintStates();
    bool IsFinalState();
    bool CanTakeAction(ACTION_EFFECTION& ae) const;
    bool IsValidState();

    int local_monster;
    int local_monk;
    int remote_monster;
    int remote_monk;
    BOAT_LOCATION boat; /*LOCAL or REMOTE*/
    ACTION_NAME curAct;
};
