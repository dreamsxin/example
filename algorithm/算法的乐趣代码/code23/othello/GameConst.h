#pragma once

const int GAME_ROW = 8;
const int GAME_COL = 8;
const int GAME_CELLS = 64;


const int BOARD_ROW = 10;
const int BOARD_COL = 9;
const int BOARD_CELLS = 91;

const int INFINITY = 10000;
const int DRAW = 0;

typedef char CellType;

const CellType CELL_EMPTY = '-';
const CellType CELL_O = 'o';
const CellType CELL_X = 'x';


const int PLAYER_TYPE = 3;

const int PLAYER_NULL = 0;
const int PLAYER_A = 1;
const int PLAYER_B = 2;
const int DUMMY = 3;

#define BOARD_CELL(row, col)  ((col - 1) + 10 + (row - 1) * 9)
