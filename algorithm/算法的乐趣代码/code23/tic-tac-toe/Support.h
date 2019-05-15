#pragma once

#include "GameConst.h"

const int LINE_DIRECTION = 8;
const int LINE_CELLS = 3;

extern int line_idx_tbl[LINE_DIRECTION][LINE_CELLS];

inline int GetPeerPlayer(int player_id) { return (player_id == PLAYER_A) ? PLAYER_B : PLAYER_A; };

