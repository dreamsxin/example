#include "StdAfx.h"
#include "GameState.h"
#include "HumanPlayer.h"

HumanPlayer::HumanPlayer(void)
{
}

HumanPlayer::HumanPlayer(const std::string& name)
{
    SetPlayerName(name);
}

HumanPlayer::~HumanPlayer(void)
{
}

int HumanPlayer::GetNextPosition()
{
    assert(m_state != NULL);

    if(!m_state->TestMoves(GetPlayerId()))
    {
        std::cout << "you have no valid position, skip this step!" << std::endl;
        return -1;
    }
    int np = 0;
    while(true)
    {
        int row, col;
        std::cout << "Please select your position (row = 1-8,col = 1-8): ";
        std::cin >> row >> col;
        np = BOARD_CELL(row, col);
        if( ((np >= 0) && (np < BOARD_CELLS)) && m_state->IsValidPosition(np, GetPlayerId()))
        {
            break;
        }
        else
        {
            std::cout << "Invalid position on (" << row << " , " << col << ")" << std::endl;
            char line[64];
            if(std::cin.fail())
            {
                std::cin.clear();
                std::cin.getline(line, 64);
            }
        }
    }

    return np;
}