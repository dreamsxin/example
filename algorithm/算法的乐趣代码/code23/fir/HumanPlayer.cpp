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

    int np = 0;
    while(true)
    {
        int row,col;
        char cc;
        std::cout << "Please select your position (row = 1-15,col = A-O): ";
        std::cin >> row >> cc;
        col = cc - 'A' + 1;
        np = BOARD_CELL(row, col);
        if( ((np >= 0) && (np < BOARD_CELLS)) && m_state->IsValidPosition(np, GetPlayerId()))
        {
            break;
        }
        else
        {
            std::cout << "Invalid position on (" << row << " , " << cc << ")" << std::endl;
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