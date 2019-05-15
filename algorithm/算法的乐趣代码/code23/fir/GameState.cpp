#include "StdAfx.h"
#include "GameState.h"
#include "ZobristHash.h"

//位置权值表
int posValue[BOARD_CELLS] = 
{
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,1,1,1,1,1,1,1,0,
    0,0,1,2,2,2,2,2,1,0,
    0,0,1,2,3,3,3,2,1,0,
    0,0,1,2,3,4,3,2,1,0,
    0,0,1,2,3,3,3,2,1,0,
    0,0,1,2,2,2,2,2,1,0,
    0,0,1,1,1,1,1,1,1,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0

};

const int DIR_COUNT = 4;
const int DIR_H = 0; // --方向
const int DIR_B = 1; // / 方向
const int DIR_S = 2; // \ 方向
const int DIR_V = 3; // | 方向


const int dir_inc[] = {1, 9, 11, 10, -1, -9, -11,-10, 0};

/* Inside this fast endgame solver, the board is represented by
* a 1D array of 91 uchars board[0..90]:
   dddddddddd
   dxxxxxxxxx  11
   dxxxxxxxxx  21
   dxxxxxxxxx  31
   dxxxxxxxxx  41
   dxxxxxxxxx  51
   dxxxxxxxxx  61
   dxxxxxxxxx  71  
   dxxxxxxxxx  81   
   dxxxxxxxxx  91
   ddddddddddd   
where A1 is board[11], 
square(a,b) = board[11+a+b*10] for 0<= a,b <=8.
where d (dummy) squares contain DUMMY, x are EMPTY, BLACK, or WHITE: */

int cell2board[GAME_CELLS] = 
{
11,12,13,14,15,16,17,18,19,
21,22,23,24,25,26,27,28,29,
31,32,33,34,35,36,37,38,39,
41,42,43,44,45,46,47,48,49,
51,52,53,54,55,56,57,58,59,
61,62,63,64,65,66,67,68,69,
71,72,73,74,75,76,77,78,79,
81,82,83,84,85,86,87,88,89,
91,92,93,94,95,96,97,98,99
};

GameState::GameState(void)
{
    m_evaluator = NULL;
    m_gameOver = false;
    m_winnerId = PLAYER_NULL;
    m_hash = 0;
}

GameState::~GameState(void)
{
}

GameState::GameState(const GameState& state)
{
    m_evaluator = state.m_evaluator;
    m_playerId = state.m_playerId;
    m_gameOver = state.m_gameOver;
    m_winnerId = state.m_winnerId;
    m_hash = state.m_hash;
    memmove(m_board, state.m_board, BOARD_CELLS * sizeof(int));
}

const GameState& GameState::operator=(const GameState& state)
{
    if(this != &state)
    {
        m_evaluator = state.m_evaluator;
        m_playerId = state.m_playerId;
        m_gameOver = state.m_gameOver;
        m_winnerId = state.m_winnerId;
        m_hash = state.m_hash;
        memmove(m_board, state.m_board, BOARD_CELLS * sizeof(int));
    }

    return *this;
}

void GameState::PrintGame()
{
    std::cout << "Current game state : " << std::endl;

    std::cout << "   ";
    for(int col = 0; col < GAME_COL; col++)
    {
        char tit = col + 'A';
        std::cout << tit << ' ';
    }
    int row = 0;
    std::cout << std::endl;
    for(int i = 0; i < GAME_CELLS; i++)
    {
        if((i % GAME_COL) == 0)
        {
            row++;
            std::cout << std::setw(2) << row << ' ';
        }
        std::cout << GetCellType(m_board[cell2board[i]]);
        std::cout << ' ';
        if((i % GAME_COL) == (GAME_COL - 1))
        {
            std::cout << std::endl;
        }
   }
}

void GameState::InitGameState(int firstPlayer)
{
    int i;
    for(i = 0; i < BOARD_CELLS; i++)
    {
        m_board[i] = DUMMY;
    }
    for(i = 0; i < GAME_CELLS; i++)
    {
        m_board[cell2board[i]] = PLAYER_NULL;
    }

    m_playerId = firstPlayer;
    m_gameOver = false;
    m_winnerId = PLAYER_NULL;
    m_hash = 0;
}

void GameState::SetGameState(int *state, int firstPlayer)
{
    for(int i = 0; i < GAME_CELLS; i++)
    {
        int cell = cell2board[i];
        m_board[cell] = state[i];
    }

    m_playerId = firstPlayer;
    m_gameOver = false;
    m_winnerId = PLAYER_NULL;
    m_hash = CalcZobristHash();
}

int GameState::Evaluate(int max_player_id)
{
    assert(m_evaluator != NULL);

    return m_evaluator->Evaluate(*this, max_player_id);
}

void GameState::SwitchPlayer()
{
    m_playerId = GetPeerPlayer(m_playerId);
}

bool GameState::IsGameOver()
{
    return m_gameOver;
}

int GameState::GetWinner()
{
    if(IsGameOver())
        return m_winnerId;
    
    return PLAYER_NULL;
}

int GameState::CountEmptyCell()
{
    int count = 0;
    for(int i = 0; i < BOARD_CELLS; i++)
    {
        if(m_board[i] == PLAYER_NULL)
        {
            count++;
        }
    }

    return count;
}

CellType GameState::GetCellType(int player_id) 
{ 
    if(player_id == PLAYER_NULL)
        return CELL_EMPTY;
    else
        return (player_id == PLAYER_B) ? CELL_X : CELL_O;
}

int GameState::CountPosValue(int player_id)
{
    int value = 0;
    for(int i = 0; i < GAME_CELLS; i++)
    {
        int cell = cell2board[i];
        if(m_board[cell] == player_id)
        {
            value += posValue[cell];
        }
    }

    return value;
}

bool GameState::IsValidPosition(int cell, int player_id)
{
    if(m_board[cell] != PLAYER_NULL)
        return false;

    return true;
}

bool GameState::PutChess(int cell, int player_id)
{
    m_board[cell] = player_id;

    if(CheckFiveInRow(cell, player_id))
    {
        m_gameOver = true;
        m_winnerId = player_id;

        return true;
    }

    if(CountEmptyCell() == 0)
    {
        m_gameOver = true;
        m_winnerId = PLAYER_NULL;
    }

    return false;
}

void GameState::DoPutChess(int cell, int player_id)
{
    ZOBRIST_HASH *zob_hash = GetZobristHashTbl();

    int row = (cell - BOARD_START) / BOARD_COL;
    int col = (cell - BOARD_START) % BOARD_COL;

    int gc = row * GAME_COL + col;
    m_hash ^= zob_hash->key[gc][m_board[cell]];
    m_board[cell] = player_id;
    m_hash ^= zob_hash->key[gc][m_board[cell]];
}

void GameState::UndoPutChess(int cell)
{
    ZOBRIST_HASH *zob_hash = GetZobristHashTbl();

    int row = (cell - BOARD_START) / BOARD_COL;
    int col = (cell - BOARD_START) % BOARD_COL;

    int gc = row * GAME_COL + col;
    m_hash ^= zob_hash->key[gc][m_board[cell]];
    m_board[cell] = PLAYER_NULL;
    m_hash ^= zob_hash->key[gc][m_board[cell]];
}

int GameState::FindMoves(int player_id, std::vector<MOVES_LIST>& moves)
{
    std::vector<int> flips;
    MOVES_LIST ml;

    moves.clear();
    for(int i = 0; i < GAME_CELLS; i++)
    {
        int cell = cell2board[i];
        if((m_board[cell] == PLAYER_NULL) && !IsForbidden(cell, player_id))
        {
            ml.cell = cell;
            ml.goodness = posValue[cell];
            moves.push_back(ml);
        }
    }

    return moves.size();
}

unsigned int GameState::GetHash()
{
    return m_hash;
}

int GameState::SearchPatterns(EvaluatorData &ev_ata)
{
    for(int i = 0; i < COUNT_OF(line_cpts); i++)//每个方向
    {
        for(int j = 0; j < MAX_LINE_S; j++)//每个方向9条线
        {
            AnalysisLine(line_cpts[i].line_s[j], line_cpts[i].off_dir, ev_ata);
        }
    }

    return 0;
}

int GameState::SkipEmptyCell(int cs, int dir_inc)
{
    int ct = cs;
    while(m_board[ct] == PLAYER_NULL) 
    {
        ct += dir_inc;
    }

    return ct;
}

int GameState::SearchAndCountChess(int cs, int dir_inc, int chess_id, int& count)
{
    int ct = cs;
    while(m_board[ct] == chess_id) 
    {
        count++;
        ct += dir_inc;
    }

    return ct;
}

int GameState::AnalysisLine(int st, int dir_inc, EvaluatorData &ev_ata)
{
    int mark_cell,mark_player_id;
    int ct = st;
    while(m_board[ct] != DUMMY)
    {
        ct = SkipEmptyCell(ct, dir_inc);//向后跳过空位
        if(m_board[ct] == DUMMY) //已经到哨兵位？直接结束
            break;

        mark_cell = ct;
        mark_player_id = m_board[ct];
        int count = 0;
        ct = SearchAndCountChess(ct, dir_inc, mark_player_id, count);
        if(count >= 5)
        {
            ev_ata.IncreaseCounter(5, mark_player_id, false);
        }
        else if(count >= 2)
        {
            int pre_space = 0;
            int succ_space = 0;
            //向前寻找空位
            int tmp_t = mark_cell - dir_inc;
            tmp_t = SearchAndCountChess(tmp_t, -dir_inc, PLAYER_NULL, pre_space);
            //向后寻找空位
            ct = SearchAndCountChess(ct, dir_inc, PLAYER_NULL, succ_space);
            if((m_board[ct] == mark_player_id) && (succ_space == 1)) 
            {
                //处理“跳”的情况
                count++; //多了一个棋子
                int space_need = 5 - count; 
                bool succ_close = (m_board[ct + dir_inc] != PLAYER_NULL);
                if((pre_space + succ_space) >= space_need)
                {
                    ev_ata.IncreaseCounter(count, mark_player_id, succ_close);
                }
            }
            else
            {
                //除了count个连子之外，还需要5-count个空位，才能构成冲x或活x
                int space_need = 5 - count; 
                //两端都有空位，且任意一端的空位数大于等于space_need，直接定为活x
                if( ((pre_space > 0) && (succ_space > 0)) 
                    && ((pre_space >= space_need) || (succ_space >= space_need)) )
                {
                    ev_ata.IncreaseCounter(count, mark_player_id, false);
                }
                else
                {
                    //两端是否有封闭
                    bool pre_close = (m_board[mark_cell - dir_inc] != PLAYER_NULL);
                    bool succ_close = (m_board[ct] != PLAYER_NULL);
                    //空位足够连成5子才统计
                    if((pre_space + succ_space) >= space_need)
                    {
                        ev_ata.IncreaseCounter(count, mark_player_id, pre_close||succ_close);
                    }
                }            
            }
        }
    }

    return 0;
}

bool GameState::CheckFiveInRow(int cell, int player_id)
{
    for(int i = 0; i < DIR_COUNT; i++)
    {
        if(CheckLinefive(cell, dir_inc[i], player_id))
        {
            return true;
        }
    }

    return false;
}

bool GameState::CheckLinefive(int cell, int dir_inc, int player_id)
{
    int count = 1;
    int ct = cell - dir_inc;
    while(m_board[ct] == player_id)
    {
        count++;
        ct -= dir_inc;
    }
    
    ct = cell + dir_inc;
    while(m_board[ct] == player_id)
    {
        count++;
        ct += dir_inc;
    }

    return (count >= 5);
}

unsigned int GameState::CalcZobristHash()
{
    ZOBRIST_HASH *zob_hash = GetZobristHashTbl();

    unsigned int hash = 0;
    for(int i = 0; i < GAME_CELLS; i++)
    {
        int cell = cell2board[i];
        hash ^= zob_hash->key[i][m_board[cell]];
    }

    return hash;
}

bool GameState::IsForbidden(int cell, int player_id)
{
    for(int i = 0; i < COUNT_OF(forbidden_patterns); i++)
    {
        if(IsMatchSingleForbidden(forbidden_patterns[i], cell, player_id))
        {
            return true;
        }
    }

    return false;
}

bool GameState::IsMatchSingleForbidden(FORBIDDEN_ITEM& item, int cell, int player_id)
{
    int match_cnt = 0;
    for(int j = 0; j < item.off_cnt; j++)
    {
        int cf = cell + item.off_inc[j];
        if((cf >= 0) && (cf < BOARD_CELLS))
        {
            match_cnt += ((m_board[cf] == player_id) ? 1 : 0);
        }
    }

    return (match_cnt == item.off_cnt);
}
