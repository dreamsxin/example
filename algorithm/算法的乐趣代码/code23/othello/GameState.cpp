#include "StdAfx.h"
#include "GameState.h"
#include "Support.h"
#include "ZobristHash.h"

//位置权值表
int posValue[BOARD_CELLS] = 
{
    0,0,0,0,0,0,0,0,0,
    0,100,  -5,  10,   5,   5,  10,  -5, 100,
    0,-5, -45,   1,   1,   1,   1, -45,  -5,
    0,10,   1,   3,   2,   2,   3,   1,  10,
    0,5,   1,   2,   1,   1,   2,   1,   5,
    0,5,   1,   2,   1,   1,   2,   1,   5,
    0,10,   1,   3,   2,   2,   3,   1,  10,
    0,-5, -45,   1,   1,   1,   1, -45,  -5,
    0,100,  -5,  10,   5,   5,  10,  -5, 100,
    0,0,0,0,0,0,0,0,0
};

unsigned char dir_mask[BOARD_CELLS] = 
{
0,0,0,0,0,0,0,0,0,
0,81,81,87,87,87,87,22,22,
0,81,81,87,87,87,87,22,22,
0,121,121,255,255,255,255,182,182,
0,121,121,255,255,255,255,182,182,
0,121,121,255,255,255,255,182,182,
0,121,121,255,255,255,255,182,182,
0,41,41,171,171,171,171,162,162,
0,41,41,171,171,171,171,162,162,
0,0,0,0,0,0,0,0,0,0
};

const int dir_inc[] = {1, -1, 8, -8, 9, -9, 10, -10, 0};

/* Inside this fast endgame solver, the board is represented by
* a 1D array of 91 uchars board[0..90]:
   ddddddddd
   dxxxxxxxx 10
   dxxxxxxxx 19
   dxxxxxxxx 28
   dxxxxxxxx 37
   dxxxxxxxx 46
   dxxxxxxxx 55
   dxxxxxxxx 64    where A1 is board[10], H8 is board[80].
   dxxxxxxxx 73    square(a,b) = board[10+a+b*9] for 0<= a,b <=7.
   dddddddddd   
where d (dummy) squares contain DUMMY, x are EMPTY, BLACK, or WHITE: */

int cell2board[GAME_CELLS] = 
{
10,11,12,13,14,15,16,17,
19,20,21,22,23,24,25,26,
28,29,30,31,32,33,34,35,
37,38,39,40,41,42,43,44,
46,47,48,49,50,51,52,53,
55,56,57,58,59,60,61,62,
64,65,66,67,68,69,70,71,
73,74,75,76,77,78,79,80
};

GameState::GameState(void)
{
    m_evaluator = NULL;
    m_EmHead.pred = NULL;
    m_EmHead.succ = NULL;
}

GameState::~GameState(void)
{
}

GameState::GameState(const GameState& state)
{
    m_evaluator = state.m_evaluator;
    m_playerId = state.m_playerId;
    memmove(m_board, state.m_board, BOARD_CELLS * sizeof(int));
    InitEmptyList();
}

const GameState& GameState::operator=(const GameState& state)
{
    if(this != &state)
    {
        m_evaluator = state.m_evaluator;
        m_playerId = state.m_playerId;
        memmove(m_board, state.m_board, BOARD_CELLS * sizeof(int));
        InitEmptyList();
    }

    return *this;
}

void GameState::PrintGame()
{
    std::cout << "Current game state : " << std::endl;

    std::cout << "  ";
    for(int col = 0; col < GAME_COL; col++)
    {
        std::cout << col + 1 << ' ';
    }
    int row = 0;
    std::cout << std::endl;
    for(int i = 0; i < GAME_CELLS; i++)
    {
        if((i % GAME_COL) == 0)
        {
            row++;
            std::cout << row << ' ';
        }
        std::cout << GetCellType(m_board[cell2board[i]]);
        std::cout << ' ';
        if((i % GAME_COL) == 7)
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

    m_board[BOARD_CELL(4,4)]=PLAYER_A;
    m_board[BOARD_CELL(5,4)]=PLAYER_B;
    m_board[BOARD_CELL(4,5)]=PLAYER_B;
    m_board[BOARD_CELL(5,5)]=PLAYER_A;

    m_playerId = firstPlayer;

    InitEmptyList();
}

void GameState::SetGameState(int *state, int firstPlayer)
{
    for(int i = 0; i < GAME_CELLS; i++)
    {
        int cell = cell2board[i];
        m_board[cell] = state[i];
    }

    m_playerId = firstPlayer;

    InitEmptyList();
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
    if(!TestMoves(PLAYER_A) && !TestMoves(PLAYER_B))
    {
        return true;
    }

    return false;
}

int GameState::GetWinner()
{
    int ca = CountCell(PLAYER_A);
    int cb = CountCell(PLAYER_B);

    if(ca == cb)
        return PLAYER_NULL;
    else
        return (ca > cb) ? PLAYER_A : PLAYER_B;
}

int GameState::CountCell(int player_id)
{
    int count = 0;
    int i;
    for(i = 0; i < BOARD_CELLS; i++)
    {
        if(m_board[i] == player_id)
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

void GameState::InitEmptyList()
{
    int k = 0;
    EMPTY_LIST *pt = &m_EmHead;
    for(int i = 0; i < BOARD_CELLS; i++)
    {
        if(m_board[i] == PLAYER_NULL)
        {
            pt->succ = &(m_Empty_s[k]);
            m_Empty_s[k].pred = pt;
            pt = pt->succ;
            pt->cell = i;
            k++;
        }
    }
    pt->succ = NULL;
}

EMPTY_LIST *GameState::FindEmptyListByCell(int cell)
{
    for (EMPTY_LIST *em = m_EmHead.succ; em != NULL; em = em->succ ) 
    {
        if(em->cell == cell)
        {
            return em;
        }
    }

    return NULL;
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

void GameState::SingleDirFlips(int cell, int dir, int player_id, int opp_player_id, std::vector<int>& flips)
{
    int pt = cell + dir;
    if(m_board[pt] == opp_player_id)
    {
        while(m_board[pt] == opp_player_id)
        {
            pt += dir;
        }
        if(m_board[pt] == player_id)
        {
            pt -= dir;
            do
            {
                m_board[pt] = player_id;
                flips.push_back(pt);
                pt -= dir;
            }while(pt != cell);    
        }
    }
}

int GameState::DoFlips(int cell, int player_id, int opp_player_id, std::vector<int>& flips)
{
    flips.clear();

    for(int i = 0; i < 8; i++) /*在8个方向试探*/
    {
        unsigned char mask = 0x01 << i;
        if(dir_mask[cell] & mask)
        {
            SingleDirFlips(cell, dir_inc[i], player_id, opp_player_id, flips);
        }
    }

    return flips.size();
}

void GameState::UndoFlips(std::vector<int>& flips, int opp_player_id)
{
    std::vector<int>::iterator it; 
    for(it = flips.begin(); it != flips.end(); it++)
    {
        m_board[*it] = opp_player_id;
    }
}

bool GameState::CanSingleDirFlips(int cell, int dir, int player_id, int opp_player_id)
{
    int pt = cell + dir;
    if(m_board[pt] == opp_player_id)
    {
        while(m_board[pt] == opp_player_id)
        {
            pt += dir;
        }

        return (m_board[pt] == player_id) ? true : false;
    }

    return false;
}

bool GameState::CanFlips(int cell, int player_id, int opp_player_id)
{
    /*在8个方向试探，任何一个方向可以翻转对方的棋子就返回true*/
    for(int i = 0; i < 8; i++) 
    {
        unsigned char mask = 0x01 << i;
        if(dir_mask[cell] & mask)
        {
            if(CanSingleDirFlips(cell, dir_inc[i], player_id, opp_player_id))
            {
                return true;
            }
        }
    }

    return false;
}

bool GameState::IsValidPosition(int cell, int player_id)
{
    int opp_player_id = GetPeerPlayer(player_id);

    EMPTY_LIST *em = FindEmptyListByCell(cell);
    if(em != NULL)
    {
        return CanFlips(cell, player_id, opp_player_id);
    }

    return false;
}

int GameState::CountMobility(int player_id)
{
    int opp_player_id = GetPeerPlayer(player_id);
    int mobility = 0;

    for (EMPTY_LIST *em = m_EmHead.succ; em != NULL; em = em->succ ) 
    {
        if(CanFlips(em->cell, player_id, opp_player_id))
        {
            mobility++;
        }
    }

    return mobility;
}

int GameState::PutChess(int cell, int player_id)
{
    EMPTY_LIST *em = FindEmptyListByCell(cell);
    assert(em != NULL);
    if(em == NULL)
    {
        return 0;
    }
    std::vector<int> flips;

    return DoPutChess(em, player_id, flips);
}

int GameState::DoPutChess(EMPTY_LIST *em, int player_id, std::vector<int>& flips)
{
    int opp_player_id = GetPeerPlayer(player_id);

    int j = DoFlips(em->cell, player_id, opp_player_id, flips);
    m_board[em->cell] = player_id;
    em->pred->succ = em->succ;
    if ( em->succ != NULL )
    {
        em->succ->pred = em->pred;
    }

    return j;
}

void GameState::UndoPutChess(EMPTY_LIST *em, int player_id, std::vector<int>& flips)
{
    int opp_player_id = GetPeerPlayer(player_id);

    UndoFlips(flips, opp_player_id);
    m_board[em->cell] = PLAYER_NULL;
    em->pred->succ = em;
    if(em->succ != NULL)
    {
        em->succ->pred = em;
    }
}

bool GameState::TestMoves(int player_id)
{
    int opp_player_id = GetPeerPlayer(player_id);

    for(EMPTY_LIST *em = m_EmHead.succ; em != NULL; em = em->succ) 
    {
        if(CanFlips(em->cell, player_id, opp_player_id))
        {
            return true;
        }
    }

    return false;
}

int GameState::FindMoves(int player_id, int opp_player_id, std::vector<MOVES_LIST>& moves)
{
    std::vector<int> flips;
    MOVES_LIST ml;

    moves.clear();

    for(EMPTY_LIST *em = m_EmHead.succ; em != NULL; em = em->succ) 
    {
        int cell = em->cell;
        int flipped = DoFlips(cell, player_id, opp_player_id, flips);
        if(flipped > 0) 
        {
            m_board[cell] = player_id;
            em->pred->succ = em->succ; //cell链表的succ链暂时跳过em（CountMobility函数会用到这个链表）
            ml.goodness = -CountMobility(opp_player_id);
            em->pred->succ = em; //cell链表的succ链恢复em
            ml.em = em;
            UndoFlips(flips, opp_player_id);
            m_board[cell] = PLAYER_NULL;

            moves.push_back(ml);
        }
    }

    return moves.size();
}

int GameState::CountEmptyCells()
{
    int empty = 0;
    for(EMPTY_LIST *em = m_EmHead.succ; em != NULL; em = em->succ) 
    {
        empty++;
    }

    return empty;
}

unsigned int GameState::GetZobristHash()
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
