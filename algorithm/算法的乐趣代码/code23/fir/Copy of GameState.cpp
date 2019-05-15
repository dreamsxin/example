#include "StdAfx.h"
#include "GameState.h"
#include "Support.h"
#include "ZobristHash.h"

//位置权值表
int posValue[BOARD_CELLS] = 
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,0,1,2,2,2,2,2,2,2,2,2,2,2,1,0,
    0,0,1,2,3,3,3,3,3,3,3,3,3,2,1,0,
    0,0,1,2,3,4,4,4,4,4,4,4,3,2,1,0,
    0,0,1,2,3,4,5,5,5,5,5,4,3,2,1,0,
    0,0,1,2,3,4,5,6,6,6,5,4,3,2,1,0,
    0,0,1,2,3,4,5,6,7,6,5,4,3,2,1,0,
    0,0,1,2,3,4,5,6,6,6,5,4,3,2,1,0,
    0,0,1,2,3,4,5,5,5,5,5,4,3,2,1,0,
    0,0,1,2,3,4,4,4,4,4,4,4,3,2,1,0,
	0,0,1,2,3,3,3,3,3,3,3,3,3,2,1,0,
	0,0,1,2,2,2,2,2,2,2,2,2,2,2,1,0,
	0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

const int DIR_COUNT = 4;
const int DIR_H = 0; // --方向
const int DIR_B = 1; // / 方向
const int DIR_S = 2; // \ 方向
const int DIR_V = 3; // | 方向


const int dir_inc[] = {1, 15, 17, 16, -1, -15, -17,-16, 0};

/* Inside this fast endgame solver, the board is represented by
* a 1D array of 91 uchars board[0..90]:
   dddddddddddddddd
   dxxxxxxxxxxxxxxx  17
   dxxxxxxxxxxxxxxx  33
   dxxxxxxxxxxxxxxx  49
   dxxxxxxxxxxxxxxx  65
   dxxxxxxxxxxxxxxx  81
   dxxxxxxxxxxxxxxx  97
   dxxxxxxxxxxxxxxx  113  
   dxxxxxxxxxxxxxxx  129   
   dxxxxxxxxxxxxxxx  145
   dxxxxxxxxxxxxxxx  161
   dxxxxxxxxxxxxxxx  177
   dxxxxxxxxxxxxxxx  193
   dxxxxxxxxxxxxxxx  209
   dxxxxxxxxxxxxxxx  225
   dxxxxxxxxxxxxxxx  241  
   ddddddddddddddddd   
where A1 is board[17], 
square(a,b) = board[17+a+b*16] for 0<= a,b <=14.
where d (dummy) squares contain DUMMY, x are EMPTY, BLACK, or WHITE: */

int cell2board[GAME_CELLS] = 
{
17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
241,242,243,244,245,246,247,248,249,250,251,252,253,254,255
};

int srh_h_start[] = {17,33,49,65,81,97,113,129,145,161,177,193,209,225,241};
int srh_b_start[] = {21,22,23,24,25,26,27,28,29,30,31,47,63,79,95,111,127,143,159,175,191};
int srh_v_start[] = {17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
int srh_s_start[] = {17,18,19,20,21,22,23,24,25,26,27,33,49,65,81,97,113,129,145,161,177};

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
        if((i % GAME_COL) == 14)
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

    int row = (cell - 17) / 16;
    int col = (cell - 17) % 16;

    int gc = row * GAME_COL + col;
    m_hash ^= zob_hash->key[gc][m_board[cell]];
    m_board[cell] = player_id;
    m_hash ^= zob_hash->key[gc][m_board[cell]];
}

void GameState::UndoPutChess(int cell)
{
    ZOBRIST_HASH *zob_hash = GetZobristHashTbl();

    int row = (cell - 17) / 16;
    int col = (cell - 17) % 16;

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
        if(m_board[cell] == PLAYER_NULL)
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

int GameState::SearchAllHorizon(EvaluatorData &ev_ata)
{
    for(int i = 0; i < COUNT_OF(srh_h_start); i++)
    {
        AnalysisLine(srh_h_start[i], dir_inc[DIR_H], ev_ata);
    }

    return 0;
}

int GameState::SearchAllBais(EvaluatorData &ev_ata)
{
    for(int i = 0; i < COUNT_OF(srh_b_start); i++)
    {
        AnalysisLine(srh_b_start[i], dir_inc[DIR_B], ev_ata);
    }

    return 0;
}

int GameState::SearchAllVertical(EvaluatorData &ev_ata)
{
    for(int i = 0; i < COUNT_OF(srh_v_start); i++)
    {
        AnalysisLine(srh_v_start[i], dir_inc[DIR_V], ev_ata);
    }

    return 0;
}

int GameState::SearchAllSlash(EvaluatorData &ev_ata)
{
    for(int i = 0; i < COUNT_OF(srh_s_start); i++)
    {
        AnalysisLine(srh_s_start[i], dir_inc[DIR_S], ev_ata);
    }
    
    return 0;
}

int GameState::AnalysisLine(int st, int dir_inc, EvaluatorData &ev_ata)
{
    int mark_cell,mark_player_id;
    int ct = st;
    while(m_board[ct] != DUMMY)
    {
        while(m_board[ct] == PLAYER_NULL) 
            ct += dir_inc;

        if(m_board[ct] == DUMMY)
            break;

        mark_cell = ct;
        mark_player_id = m_board[ct];
        //int opp_player_id = GetPeerPlayer(mark_player_id);
        int count = 0;
        while(m_board[ct] == mark_player_id) 
        {
            count++;
            ct += dir_inc;
        }
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
            while(m_board[tmp_t] == PLAYER_NULL)
            {
                pre_space++;
                tmp_t -= dir_inc;
            }
            //向后寻找空位
            while(m_board[ct] == PLAYER_NULL)
            {
                succ_space++;
                ct += dir_inc;
            }
            //除了count个连子之外，还需要5-count个空位，才能构成冲x或活x
            int space_need = 5 - count; 
            //两端都有空位，且任意一端的空位数大于活等于space_need，为活
            if( ((pre_space > 0) && (succ_space > 0)) 
                && ((pre_space >= space_need) || (succ_space >= space_need)) )
            {
                ev_ata.IncreaseCounter(count, mark_player_id, false);
            }
            else
            {
                //两端是否有封闭
                bool preClose = (m_board[mark_cell - dir_inc] != PLAYER_NULL);
                bool succClose = (m_board[ct] != PLAYER_NULL);
                /*空位足够连成5子才统计*/
                if((pre_space + succ_space) >= space_need)
                {
                    ev_ata.IncreaseCounter(count, mark_player_id, preClose||succClose);
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


