#include "StdAfx.h"
#include "GameState.h"
#include "Support.h"


GameState::GameState(void)
{
    m_evaluator = NULL;
}

GameState::~GameState(void)
{
}

GameState::GameState(const GameState& state)
{
    m_evaluator = state.m_evaluator;
    m_playerId = state.m_playerId;
    memmove(m_board, state.m_board, BOARD_CELLS * sizeof(int));
}

const GameState& GameState::operator=(const GameState& state)
{
    if(this != &state)
    {
        m_evaluator = state.m_evaluator;
        m_playerId = state.m_playerId;
        memmove(m_board, state.m_board, BOARD_CELLS * sizeof(int));
    }

    return *this;
}

void GameState::PrintGame()
{
    std::cout << "Current game state : " << std::endl;

    for(int i = 0; i < BOARD_CELLS; i++)
    {
        std::cout << GetCellType(m_board[i]);
        if((i % BOARD_COL) == 2)
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
        m_board[i] = PLAYER_NULL;
    }
    m_playerId = firstPlayer;
}

void GameState::SetGameCell(int cell, int player_id)
{
    m_board[cell] = player_id;
}

bool GameState::IsEmptyCell(int cell) const
{
    return m_board[cell] == PLAYER_NULL;
}

int GameState::Evaluate(int max_player_id)
{
    assert(m_evaluator != NULL);

    return m_evaluator->Evaluate(*this, max_player_id);
}

#if 0
int GameState::Evaluate(int max_player_id)
{
    int min =  GetPeerPlayer(max_player_id);

    int aOne, aTwo, aThree, bOne, bTwo, bThree;
    CountPlayerChess(max_player_id, aOne, aTwo, aThree);
    CountPlayerChess(min, bOne, bTwo, bThree);

    if(aThree > 0)
    {
        return INFINITY;
    }
    if(bThree > 0)
    {
        return -INFINITY;
    }
    
    if(CountEmptyCell() == 0)
    {
        return DRAW;
    }

    return (aTwo - bTwo) * DOUBLE_WEIGHT + (aOne - bOne);
}

int CGameState::Evaluate()
{
    CellType cth = CellTypeFromPlayer(Human);
    CellType ctc = CellTypeFromPlayer(Computer);

    int cOne, cTwo, cThree, hOne, hTwo, hThree;
    CountPlayerChess(ctc, cOne, cTwo, cThree);
    CountPlayerChess(cth, hOne, hTwo, hThree);

    if(cThree > 0)
    {
        return INFINITY;
    }
    if(hThree > 0)
    {
        return -INFINITY;
    }
    
    if(IsComputerPlayer() && (hTwo > 0))
    {
        return -(WIN_LEVEL + (hTwo - cTwo) * DOUBLE_WEIGHT);
    }
    if(IsHumanPlayer() && (cTwo > 0))
    {
        return WIN_LEVEL + (cTwo - hTwo) * DOUBLE_WEIGHT;
    }
    
    if(CountEmptyCell() == 0)
    {
        return DRAW;
    }

    return cOne - hOne;
}

int CGameState::Evaluate()
{
    CellType cth = CellTypeFromPlayer(Human);
    CellType ctc = CellTypeFromPlayer(Computer);

    int cOne, cTwo, cThree, hOne, hTwo, hThree;
    CountPlayerChess(ctc, cOne, cTwo, cThree);
    CountPlayerChess(cth, hOne, hTwo, hThree);

    if(cThree > 0)
    {
        return INFINITY;
    }
    if(hThree > 0)
    {
        return -INFINITY;
    }
    if(CountEmptyCell() == 0)
    {
        return DRAW;
    }
    
    return (cTwo * DOUBLE_WEIGHT + cOne) - (hTwo * DOUBLE_WEIGHT - hOne);
}
#endif
void GameState::SwitchPlayer()
{
    m_playerId = GetPeerPlayer(m_playerId);
}

bool GameState::IsGameOver()
{
    if(CountEmptyCell() == 0)
        return true;

    int aThree = CountThreeLine(m_playerId);
    if(aThree != 0)
        return true;

    int min = GetPeerPlayer(m_playerId);
    int bThree = CountThreeLine(min);
    if(bThree != 0)
        return true;

    return false;
}

int GameState::GetWinner()
{
    int aThree = CountThreeLine(m_playerId);
    if(aThree != 0)
        return m_playerId;

    int min = GetPeerPlayer(m_playerId);
    int bThree = CountThreeLine(min);
    if(bThree != 0)
        return min;

    return PLAYER_NULL;
}
/*
bool GameState::CountThreeLine(int player_id)
{
    for(int i = 0; i < LINE_DIRECTION; i++)
    {
        if( (m_board[line_idx_tbl[i][0]] == player_id)
            && (m_board[line_idx_tbl[i][1]] == player_id)
            && (m_board[line_idx_tbl[i][2]] == player_id) )
        {
            return true;
        }
    }

    return false;
}
*/
int GameState::CountThreeLine(int player_id)
{
    int lines = 0;
    for(int i = 0; i < LINE_DIRECTION; i++)
    {
        int sameCount = 0;
        for(int j = 0; j < LINE_CELLS; j++)
        {
            if(m_board[line_idx_tbl[i][j]] == player_id)
            {
                sameCount++;
            }
        }
        if(sameCount == 3)
        {
            lines++;
        }
    }

    return lines;
}

int GameState::CountEmptyCell()
{
    int count = 0;
    int i;
    for(i = 0; i < BOARD_CELLS; i++)
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