// bucket.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

const int PLACE_HOLDER_VALUE = 0;
const int NUM_MIN_VALUE = 1;
const int NUM_MAX_VALUE = 9;
const unsigned int FULL_NINE_BIT_VALUE = 0x000003FE;
const unsigned int FULL_NINE_NUMBER_VALUE = 45;
const unsigned int SKD_ROW_LIMIT = 9;
const unsigned int SKD_COL_LIMIT = 9;
const unsigned int SKD_BLOCK_ROW_LIMIT = 3;
const unsigned int SKD_BLOCK_COL_LIMIT = 3;
const unsigned int SKD_BLOCK_LIMIT = 9;
const unsigned int SKD_CELL_COUNT = SKD_ROW_LIMIT * SKD_COL_LIMIT;
const unsigned int SKD_BLOCK_COUNT = SKD_BLOCK_ROW_LIMIT * SKD_BLOCK_COL_LIMIT;
const int NUM_COUNT = 9;


#if 0
//中等难度，最大递归深度4
int sudoku_numbers1[SKD_CELL_COUNT] = 
{
    0, 0, 0, 0, 9, 0, 6, 0, 0,
    0, 0, 0, 5, 0, 8, 0, 4, 1,
    0, 0, 7, 0, 0, 0, 0, 0, 0,
    5, 1, 0, 4, 0, 0, 0, 6, 0,
    0, 0, 0, 0, 0, 2, 0, 0, 4,
    2, 8, 0, 1, 0, 0, 0, 5, 0,
    9, 4, 0, 8, 2, 0, 1, 7, 0,
    1, 3, 8, 7, 0, 0, 0, 9, 0,
    7, 6, 0, 0, 0, 3, 0, 0, 5
};
/*
8    5    1    2    9    4    6    3    7

3    2    6    5    7    8    9    4    1

4    9    7    6    3    1    5    2    8

5    1    3    4    8    9    7    6    2

6    7    9    3    5    2    8    1    4

2    8    4    1    6    7    3    5    9

9    4    5    8    2    6    1    7    3

1    3    8    7    4    5    2    9    6

7    6    2    9    1    3    4    8    5
*/
#endif

#if 0
//简单，基础排除法搞定
int sudoku_numbers1[SKD_CELL_COUNT] = 
{
    0, 0, 6, 8, 0, 2, 7, 5, 0,
    8, 9, 0, 0, 3, 5, 0, 0, 1,
    2, 0, 0, 9, 0, 0, 0, 0, 6,
    0, 0, 0, 4, 0, 0, 1, 0, 2,
    0, 0, 0, 0, 9, 1, 0, 6, 7,
    1, 0, 0, 0, 6, 3, 0, 4, 0,
    6, 0, 0, 0, 0, 0, 0, 7, 0,
    5, 4, 0, 0, 2, 0, 8, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 4
};
/*
4    3    6    8    1    2    7    5    9

8    9    7    6    3    5    4    2    1

2    5    1    9    7    4    3    8    6

9    6    5    4    8    7    1    3    2

3    8    4    2    9    1    5    6    7

1    7    2    5    6    3    9    4    8

6    1    8    3    4    9    2    7    5

5    4    9    7    2    6    8    1    3

7    2    3    1    5    8    6    9    4
*/
#endif

#if 1
//较难，最大递归深度11
int sudoku_numbers1[SKD_CELL_COUNT] = 
{
    0, 0, 0, 0, 0, 6, 0, 0, 1,
    9, 0, 0, 0, 0, 0, 3, 7, 6,
    7, 1, 0, 0, 4, 0, 0, 0, 0,
    1, 7, 0, 8, 0, 0, 0, 0, 3,
    0, 3, 0, 0, 0, 0, 0, 1, 0,
    6, 0, 0, 0, 0, 3, 0, 5, 8,
    0, 0, 0, 0, 3, 0, 0, 6, 5,
    3, 5, 1, 0, 0, 0, 0, 0, 2,
    8, 0, 0, 1, 0, 0, 0, 0, 0
};
/*
5    8    3    9    7    6    4    2    1

9    2    4    5    8    1    3    7    6

7    1    6    3    4    2    5    8    9

1    7    5    8    2    9    6    4    3

2    3    8    6    5    4    9    1    7

6    4    9    7    1    3    2    5    8

4    9    7    2    3    8    1    6    5

3    5    1    4    6    7    8    9    2

8    6    2    1    9    5    7    3    4
*/
#endif
typedef struct 
{
    int num;
    bool fixed;
    std::set<int> candidators;
}SUDOKU_CELL;

typedef struct
{
    SUDOKU_CELL cells[SKD_ROW_LIMIT][SKD_COL_LIMIT];
    int fixedCount;
}SUDOKU_GAME;


static int RoundBlockNumber(int row, int col)
{
    return (row / SKD_BLOCK_COL_LIMIT) * SKD_BLOCK_ROW_LIMIT + (col / SKD_BLOCK_COL_LIMIT);
}

int allNumber[NUM_COUNT] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

static void InitSudokuCell(SUDOKU_CELL *cell, int num)
{
    cell->num = num;
    cell->fixed = (num != PLACE_HOLDER_VALUE) ? true : false;
    cell->candidators.clear();
    if(!cell->fixed)
    {
        copy(allNumber, allNumber + NUM_COUNT, 
             std::insert_iterator<std::set<int> >(cell->candidators, cell->candidators.begin()));
    }
}

bool CheckValidCandidator(SUDOKU_GAME *game, int row, int col, int num)
{
    for(int i = 0; i < SKD_COL_LIMIT; i++)
    {
        if((i != col) && (game->cells[row][i].num == num))
            return false;
    }
    for(int j = 0; j < SKD_ROW_LIMIT; j++)
    {
        if((j != row) && (game->cells[j][col].num == num))
            return false;
    }
    int rs = (row / SKD_BLOCK_ROW_LIMIT) * SKD_BLOCK_ROW_LIMIT;
    int cs = (col / SKD_BLOCK_COL_LIMIT) * SKD_BLOCK_COL_LIMIT;
    for(int r = rs; r < SKD_BLOCK_COL_LIMIT; r++)
    {
        for(int c = cs; c < SKD_BLOCK_ROW_LIMIT; c++)
        {
            if(!((r == row) && (c == col)) && (game->cells[r][c].num == num))
                return false;
        }
    }

    return true;
}

static bool SwitchCellToFixedSC(SUDOKU_GAME *game, int row, int col)
{
    SUDOKU_CELL *cell = &game->cells[row][col];
    assert(cell->candidators.size() == 1);
    if(CheckValidCandidator(game, row, col, *(cell->candidators.begin())))
    {
        cell->num = *(cell->candidators.begin());
        if((row == 1)&&(col==1)&(cell->num==8))
        {
            int sss = 0;
        }
        cell->fixed = true;
        cell->candidators.clear();
        //std::cout << "set[" << row << "][" << col << "] = " << cell->num << std::endl;
        return true;
    }

    return false;
}

static bool SwitchCellToFixed(SUDOKU_GAME *game, int row, int col, int num)
{
    SUDOKU_CELL *cell = &game->cells[row][col];
    if(CheckValidCandidator(game, row, col, num))
    {
        cell->num = num;
        cell->fixed = true;
        cell->candidators.clear();
        //std::cout << "set[" << row << "][" << col << "] = " << num << std::endl;
        return true;
    }

    return false;
}

void InitSudokuGame(SUDOKU_GAME *game, int *numbers)
{
    game->fixedCount = 0;
    for(int i = 0; i < SKD_CELL_COUNT; i++)
    {
        int row = i / SKD_COL_LIMIT;
        int col = i % SKD_COL_LIMIT;
        InitSudokuCell(&game->cells[row][col], numbers[i]);
        if(game->cells[row][col].fixed)
            game->fixedCount++;
    }
}

void PrintSudokuGame(SUDOKU_GAME *game)
{
    std::cout << std::endl;
    for(int row = 0; row < SKD_ROW_LIMIT; row++)
    {
        for(int col = 0; col < SKD_COL_LIMIT; col++)
        {
            std::cout << game->cells[row][col].num << "    ";
        }
        std::cout << std::endl << std::endl;
    }
}

void DebugPrintSudokuGame(SUDOKU_GAME *game)
{
    std::cout << std::endl;
    for(int row = 0; row < SKD_ROW_LIMIT; row++)
    {
        for(int col = 0; col < SKD_COL_LIMIT; col++)
        {
            if(!game->cells[row][col].fixed)
            {
                std::cout << "[" << row + 1 << "][" << col + 1 << "] = {";
                std::set<int>::iterator it = game->cells[row][col].candidators.begin();
                while(it != game->cells[row][col].candidators.end())
                {
                    std::cout << *it << ",";
                    it++;
                }
                std::cout << "}" << std::endl;
            }
        }
    }
}

bool IsSudokuOK(SUDOKU_GAME *game)
{
    return (game->fixedCount == SKD_CELL_COUNT);
}

bool SingleRowExclusive(SUDOKU_GAME *game, int row, int num);
bool SingleColumnExclusive(SUDOKU_GAME *game, int col, int num);
bool SingleBlockExclusive(SUDOKU_GAME *game, int block, int num);

bool SinglesCandidature(SUDOKU_GAME *game, int row, int col)
{
    if(!SwitchCellToFixedSC(game, row, col))
        return false;

    if(!SingleRowExclusive(game, row, game->cells[row][col].num))
        return false;
    if(!SingleColumnExclusive(game, col, game->cells[row][col].num))
        return false;
    int block = RoundBlockNumber(row, col);
    if(!SingleBlockExclusive(game, block, game->cells[row][col].num))
        return false;

    game->fixedCount++;
    return true;
}

bool SetCandidatorTofixed(SUDOKU_GAME *game, int row, int col, int num)
{
    if(!SwitchCellToFixed(game, row, col, num))
        return false;

    if(!SingleRowExclusive(game, row, game->cells[row][col].num))
        return false;
    if(!SingleColumnExclusive(game, col, game->cells[row][col].num))
        return false;
    int block = RoundBlockNumber(row, col);
    if(!SingleBlockExclusive(game, block, game->cells[row][col].num))
        return false;
    
    game->fixedCount++;
    return true;
}

bool RemoveCellCandidator(SUDOKU_GAME *game, int row, int col, int num)
{
    if(!game->cells[row][col].fixed)
    {
        game->cells[row][col].candidators.erase(num);
        if(game->cells[row][col].candidators.size() == 1)
        {
            if(!SinglesCandidature(game, row, col))
                return false;
        }
    }

    return true;
}

static bool IsCellHasCandidator(SUDOKU_CELL *cell, int num)
{
    if(cell->fixed)
        return false;

    if(cell->candidators.find(num) != cell->candidators.end())
        return true;

    return false;
}


bool SingleRowExclusive(SUDOKU_GAME *game, int row, int num)
{
    for(int i = 0; i < SKD_COL_LIMIT; i++)
    {
        if(IsCellHasCandidator(&game->cells[row][i], num))
        {
            if(!RemoveCellCandidator(game, row, i, num))
                return false;
        }
    }

    return true;
}

bool SingleColumnExclusive(SUDOKU_GAME *game, int col, int num)
{
    for(int i = 0; i < SKD_ROW_LIMIT; i++)
    {
        if(IsCellHasCandidator(&game->cells[i][col], num))
        {
            if(!RemoveCellCandidator(game, i, col, num))
                return false;
        }
    }

    return true;
}

bool SingleBlockExclusive(SUDOKU_GAME *game, int block, int num)
{
    int r = (block / SKD_BLOCK_ROW_LIMIT) * SKD_BLOCK_COL_LIMIT;
    int c = (block % SKD_BLOCK_ROW_LIMIT) * SKD_BLOCK_ROW_LIMIT;

    for(int i = 0; i < SKD_BLOCK_LIMIT; ++i)
    {
        int row = r + i / SKD_BLOCK_ROW_LIMIT;
        int col = c + i % SKD_BLOCK_ROW_LIMIT;
        if(IsCellHasCandidator(&game->cells[row][col], num))
        {
            if(!RemoveCellCandidator(game, row, col, num))
                return false;
        }
    }

    return true;
}

void CopyGameState(SUDOKU_GAME *game, SUDOKU_GAME *new_game)
{
    new_game->fixedCount = game->fixedCount;
    for(int i = 0; i < SKD_CELL_COUNT; i++)
    {
        int row = i / SKD_COL_LIMIT;
        int col = i % SKD_COL_LIMIT;
        new_game->cells[row][col].fixed = game->cells[row][col].fixed;
        new_game->cells[row][col].num = game->cells[row][col].num;
        new_game->cells[row][col].candidators.clear();
        copy(game->cells[row][col].candidators.begin(), 
             game->cells[row][col].candidators.end(),
             std::insert_iterator<std::set<int> >(new_game->cells[row][col].candidators, 
                                                  new_game->cells[row][col].candidators.begin()));
    }
}

bool SwitchToNextState(SUDOKU_GAME *game, int row, int col, int num, SUDOKU_GAME *new_game)
{
    CopyGameState(game, new_game);
    
    return SetCandidatorTofixed(new_game, row, col, num);
}

static int ddd = 0;

void FindSudokuSolution(SUDOKU_GAME *game, int sp)
{
    static int rc = 0;
    if(game->fixedCount == SKD_CELL_COUNT)
    {
        rc++;
        std::cout << "Find result " << rc << "  (ddd = " << ddd << "):" << std::endl;
        PrintSudokuGame(game);
        return;
    }

    int row = sp / SKD_COL_LIMIT;
    int col = sp % SKD_COL_LIMIT;
    while((sp < SKD_CELL_COUNT) && game->cells[row][col].fixed)
    {
        sp++;
        row = sp / SKD_COL_LIMIT;
        col = sp % SKD_COL_LIMIT;
    }
    if(sp < SKD_CELL_COUNT)
    {
        SUDOKU_CELL *curCell = &game->cells[row][col];
        SUDOKU_GAME new_state;
        std::set<int>::iterator it = curCell->candidators.begin();
        while(it != curCell->candidators.end())
        {
            if(SwitchToNextState(game, row, col, *it, &new_state))
            {
                //PrintSudokuGame(&new_state);
                //DebugPrintSudokuGame(&new_state);
                ddd++;
                FindSudokuSolution(&new_state, sp + 1);
                ddd--;
            }
            ++it;
        }
    }
}

void BaseExclusive(SUDOKU_GAME *game)
{
    for(int i = 0; i < SKD_CELL_COUNT; i++)
    {
        int row = i / SKD_COL_LIMIT;
        int col = i % SKD_COL_LIMIT;
        if(game->cells[row][col].fixed)
        {
            SingleRowExclusive(game, row, game->cells[row][col].num);
            SingleColumnExclusive(game, col, game->cells[row][col].num);
            int block = RoundBlockNumber(row, col);
            SingleBlockExclusive(game, block, game->cells[row][col].num);
        }
    }
}

int main(int argc, char* argv[])
{
    SUDOKU_GAME game;
    InitSudokuGame(&game, sudoku_numbers1);
    PrintSudokuGame(&game);

    BaseExclusive(&game);
    //SinglesCandidature(&game);
    //PrintSudokuGame(&game);
    //DebugPrintSudokuGame(&game);

    FindSudokuSolution(&game, 0);

    return 0;
}


