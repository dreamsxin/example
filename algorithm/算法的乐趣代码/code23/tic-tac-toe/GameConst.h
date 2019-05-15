#pragma once

const int BOARD_ROW = 3;
const int BOARD_COL = 3;
const int BOARD_CELLS = BOARD_ROW * BOARD_COL;

const int INFINITY = 100;
const int WIN_LEVEL = 80;
const int DRAW = 0;
const int DOUBLE_WEIGHT = 10;

typedef char CellType;

const CellType CELL_EMPTY = '-';
const CellType CELL_O = 'o';
const CellType CELL_X = 'x';


const int PLAYER_NULL = 0;
const int PLAYER_A = 1;
const int PLAYER_B = 2;
